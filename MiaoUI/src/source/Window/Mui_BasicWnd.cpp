/**
 * FileName: Mui_BasicWnd.cpp
 * Note: UI窗口容器实现
 *
 * Copyright (C) 2021-2024 Maplespe (mapleshr@icloud.com)
 *
 * This file is part of MiaoUI library.
 * MiaoUI library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by the Free Software Foundation, either version 3
 * of the License, or any later version.
 *
 * MiaoUI library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * date: 2021-4-28 Create
*/
#include <Window/Mui_BasicWnd.h>
#include <Control/Mui_Control.h>
#include <Mui_XML.h>
#include <Mui_Settings.h>

namespace Mui::Window
{
	UIWindowBasic::UIWindowBasic(Render::Def::MRender* render)
		: MThreadT([this] { ThreadProc(); }), m_rootBox(new Ctrl::UIControl()), m_drawCmdList(255)
	{
		m_render = render;
		m_renderCmd = new Render::MRenderCmd(render);
		m_resourceMgr = new UIResourceMgr(m_renderCmd);

		m_timer = new MTimers();

		m_rootBox->m_render = m_renderCmd;
		m_rootBox->SetAlignType(UIAlignment_Absolute, false);
		m_rootBox->UINodeBase::m_data.ParentWnd = this;

		m_renderRoot = new Render::MNodeRoot(m_rootBox);

		Render::UINodeBase::PosSizeUnit unit;
		unit.x_w = Render::UINodeBase::Percentage;
		unit.y_h = unit.x_w;

		m_root = new Ctrl::UIControl();
		m_rootBox->AddChildren(m_root);
		m_root->SetAlignType(UIAlignment_Absolute, false);
		m_root->SetSize(100, 100, false);
		m_root->SetSizeUnit(unit, false);
		m_root->AutoSize(false, false);

		m_toplay = new Ctrl::UIControl();
		m_rootBox->AddChildren(m_toplay);
		m_toplay->SetAlignType(UIAlignment_Absolute, false);
		m_toplay->SetSize(100, 100, false);
		m_toplay->SetSizeUnit(unit, false);
		m_toplay->SetMsgIgnore(true, false);
		m_toplay->AutoSize(false, false);

		m_xmlUI = new XML::MuiXML(this);

		Start(true);
	}

	UIWindowBasic::~UIWindowBasic()
	{
		Stop();
		m_dbgFrame = nullptr;
		delete m_xmlUI;
		delete m_rootBox;
		delete m_renderRoot;
		delete m_timer;
		delete m_resourceMgr;
		m_render->Release();
		delete m_renderCmd;
	}

	Ctrl::UIControl* UIWindowBasic::GetRootControl() const
	{
		return m_root;
	}

	void UIWindowBasic::UpdateLayout(MPCRect rect)
	{
		m_renderCmd->RunTask([this]
		{
			const UIRect&& rcClient = GetWindowRect(true);
			auto ctrl = m_rootBox;
			ctrl->SetSize(rcClient.GetWidth(), rcClient.GetHeight(), false);
			ctrl->UINodeBase::m_data.Frame = rcClient.ToRectT<float>();
			ctrl->UINodeBase::m_data.ClipFrame = rcClient.ToRectT<float>();
			ctrl->UINodeBase::m_data.Align.Layout(ctrl, 0);
			ctrl->UpdateDisplay();
		});
	}

	void UIWindowBasic::UpdateDisplay(MPCRect rect)
	{
		if (m_renderMode) return;
		_m_rect updateRect;
		if (rect)
			updateRect = *rect;
		if(!m_drawCmdList.isFull())
			m_drawCmdList.push({ updateRect.left, updateRect.top, updateRect.right, updateRect.bottom });
		ResumeThread();
	}

	void UIWindowBasic::SetRenderMode(bool active)
	{
		m_renderMode = active;
		ResumeThread();
	}

	bool UIWindowBasic::GetRenderMode() const
	{
		return m_renderMode;
	}

	void UIWindowBasic::ScaleWindow(_m_uint newWidth, _m_uint newHeight)
	{
		_m_scale newSize;
		UISize src = GetWindowSrcSize();
		newSize.cx = (float)newWidth / (float)src.width;
		newSize.cy = (float)newHeight / (float)src.height;
		ScaleWindow(newSize);
	}

	void UIWindowBasic::ScaleWindow(_m_scale scale)
	{
		UISize src = GetWindowSrcSize();
		UIRect rect = GetWindowRect();
		rect.right = rect.left + int((float)src.width * scale.cx);
		rect.bottom = rect.top + int((float)src.height * scale.cy);
		SetWindowRect(rect);
		m_scale = scale;
		UpdateScale();
	}

	_m_scale UIWindowBasic::GetWindowScale() const
	{
		return m_scale;
	}

	MTimers::ID UIWindowBasic::SetTimer(_m_uint elapse, const MTimerCallback& callback)
	{
		auto task = [callback, this](_m_ptrv id, _m_ulong time)
		{
			if (callback) callback((_m_ptrv)this, id, time);
			EventSource(M_WND_TIMER, (_m_param)id);
		};
		return m_timer->AddTimer(elapse, task);
	}

	void UIWindowBasic::KillTimer(MTimers::ID id)
	{
		m_timer->DelTimer(id);
	}

	void UIWindowBasic::SetMaxFPSLimit(int fps)
	{
		m_fpsCounter.SetMaxFPS((float)fps);
	}

	_m_uint UIWindowBasic::GetLastFPS() const
	{
		return m_fpsCache;
	}

	void UIWindowBasic::SetMainWindow(bool main)
	{
		m_isMainWnd = main;
	}

	bool UIWindowBasic::IsMainWindow() const
	{
		return m_isMainWnd;
	}

	void UIWindowBasic::SetCacheMode(bool cache)
	{
		m_cacheRes = cache;
		UpdateDisplay(nullptr);
	}

	bool UIWindowBasic::GetResMode()
	{
		return m_cacheRes;
	}

	Render::MRenderCmd* UIWindowBasic::GetRender() const
	{
		return m_renderCmd;
	}

	UIResourceMgr* UIWindowBasic::GetResourceMgr() const
	{
		return m_resourceMgr;
	}

	UIFocus UIWindowBasic::GetFocusControl() const
	{
		return m_focus;
	}

	Ctrl::UIControl* UIWindowBasic::FindTopLevelControl(std::wstring_view name) const
	{
		return m_toplay->FindChildren<Ctrl::UIControl>(name);
	}

	void UIWindowBasic::ShowDebugRect(bool show)
	{
		ExecuteThreadTask([&]
		{
			if (!show)
				m_dbgFrame = nullptr;
			else if (!m_dbgFrame)
			{
				m_dbgFrame = m_renderCmd->CreatePen(1, Color::M_RED);
			}

			if (!m_renderMode)
				RenderControlTree(nullptr);
		});
	}

	void UIWindowBasic::ShowHighlight(bool show)
	{
		m_highlight = show;
		UpdateDisplay(nullptr);
	}

	void UIWindowBasic::UpdateCache()
	{
		m_updateCache = true;
		UpdateLayout(nullptr);
	}

	void UIWindowBasic::SetFocusControl(Ctrl::UIControl* control)
	{
		m_focus = { control };
		if (m_dbgFrame && m_highlight)
			control->UpdateDisplay();
	}

	void UIWindowBasic::SetCapture(Ctrl::UIControl* control)
	{
		m_capture = control;
#ifdef _WIN32
		::SetCapture((HWND)GetWindowHandle());
#endif // _WIN32

	}

	void UIWindowBasic::ReleaseCapture()
	{
		m_capture = nullptr;
#ifdef _WIN32
		::ReleaseCapture();
#endif // _WIN32

	}

	_m_result UIWindowBasic::EventSource(MEventCodeEnum code, _m_param param)
	{
		bool result = false;
		switch (code)
		{
		case M_WND_CLOSE:
		{
			SetRenderMode(false);
			MThreadT::Stop();
			break;
		}
		case M_WND_PAINT:
		{
			UpdateDisplay(nullptr);
			result = true;
			break;
		}
		case M_WND_SIZE:
		{
			m_renderCmd->RunTask([this]
			{
				auto rc = GetWindowRect(true);
				m_render->Resize(rc.GetWidth(), rc.GetHeight());
				//更新布局
				UpdateLayout(nullptr);
				m_updateCache = true;
			});
		}
		break;
		case M_MOUSE_LEAVE:
		case M_MOUSE_MOVE:
		case M_MOUSE_LBDOWN:
		case M_MOUSE_LBUP:
		case M_MOUSE_RBDOWN:
		case M_MOUSE_RBUP:
		case M_MOUSE_MBDOWN:
		case M_MOUSE_MBUP:
		case M_MOUSE_LBDBCLICK:
		case M_MOUSE_RBDBCLICK:
		case M_MOUSE_MBDBCLICK:
		case M_MOUSE_WHEEL:
		case M_SETCURSOR:
		{
			if (m_capture)
				result = DispatchControlMessage(m_capture, code, param, true);
			else
				result = EventMouseProc(code, param);
		}
		break;
		default:
		{
			const auto focus = m_focus.curFocus;
			if (code != M_WND_SETFOCUS && code != M_WND_KILLFOCUS)
			{
				if (m_capture)
					result = DispatchControlMessage(m_capture, code, param, false);
				else if (focus)
				{
					DispatchControlMessage(focus, code, param, false);
				}
				break;
			}
			if (code == M_WND_KILLFOCUS)
			{
				if (focus != nullptr)
					focus->SetFocus(false);
				m_focus = { nullptr };
			}
		}
		break;
		}

		return result;
	}

	bool UIWindowBasic::DispatchControlMessage(Ctrl::UIControl* control, MEventCodeEnum code, _m_param param, bool mouse)
	{
		const auto pm = (std::pair<_m_param, _m_param>*)param;
		if (control && pm)
		{
			if (mouse)
				return control->DispatchMouseMessage(code, pm->first, pm->second);
			return control->DispatchWindowMessage(code, pm->first, pm->second);
		}
		return false;
	}

	bool UIWindowBasic::EventMouseProc(MEventCodeEnum code, _m_param param)
	{
		const auto pm = (std::pair<_m_param, _m_param>*)param;

		//设置当前鼠标控件
		auto SetMouseControl = [this, param](Ctrl::UIControl* ctrl)
		{
			if (m_mouseCur == ctrl) return;

			//对旧控件分发离开消息
			if (m_mouseCur)
				DispatchControlMessage(m_mouseCur, M_MOUSE_LEAVE, param, true);

			//对新控件分发进入消息
			if (ctrl)
				DispatchControlMessage(ctrl, M_MOUSE_HOVER, param, true);

			m_mouseCur = ctrl;
		};

		switch (code)
		{
		case M_MOUSE_LEAVE:
			SetMouseControl(nullptr);
			m_mouseIn = false;
			return true;
		case M_MOUSE_LBDOWN:
#ifdef _WIN32
			::SetFocus((HWND)GetWindowHandle());
#else
				EventSource(M_WND_SETFOCUS, param);
#endif // _WIN32
			break;
		default:
			break;
		}

		if (m_rootBox)
		{
			const UIPoint point = { (int)(short)Helper::M_LOWORD((_m_long)pm->second), (int)(short)Helper::M_HIWORD((_m_long)pm->second) };
			Ctrl::UIControl* curControl = m_rootBox->FindMouseControl(point);

			if (code == M_MOUSE_MOVE)
			{
#ifdef _WIN32
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = (HWND)GetWindowHandle();
				TrackMouseEvent(&tme);
#endif

				SetMouseControl(curControl);
			}
			else if (code == M_SETCURSOR)
			{
				curControl = m_mouseCur;
				if (!curControl) return false;
			}

			return DispatchControlMessage(curControl, code, param, true);
		}

		return false;
	}

	void UIWindowBasic::SetInited(bool inited)
	{
		m_inited = inited;
		auto&& ctrl = m_rootBox;
		ctrl->UINodeBase::m_data.Frame = GetWindowRect(true).ToRectT<float>();
	}

	void UIWindowBasic::UpdateScale()
	{
		if (m_rootBox)
		{
			auto ctrl = m_rootBox;
			ctrl->OnScale(m_scale);
		}
		UpdateLayout(nullptr);
	}

	void UIWindowBasic::LoadNodeResource(bool recreate)
	{
		std::function<void(Ctrl::UINodeBase*)> loadResource
			= [&](Ctrl::UINodeBase* node)
		{
			if (!recreate)
				node->InitDeviceResource();
			else
				node->OnLoadResource(m_renderCmd, true);
			std::vector<Ctrl::UINodeBase*> nodeList;
			node->GetChildrenList(nodeList);
			for (const auto& _node : nodeList)
			{
				loadResource(_node);
			}
		};
		loadResource(m_rootBox);
	}

	void UIWindowBasic::RenderControlTree(const _m_rect_t<int>* dirtyArea)
	{
		if (!m_inited || IsMinimize()) return;

		m_fpsCounter.LimitFPS();

		m_renderCmd->RunTask([&]
		{
			const UIRect&& rcClient = GetWindowRect(true);
			const int cvWidth = rcClient.GetWidth();
			const int cvHeight = rcClient.GetHeight();

			if (cvWidth == 0 || cvHeight == 0)
			{
				return;
			}

			_m_rect dirtyAreaRect;
			if (dirtyArea)
			{
				dirtyAreaRect = *dirtyArea;
				//限定更新区域不能超过画布尺寸
				if (dirtyArea->left < 0) dirtyAreaRect.left = 0;
				if (dirtyArea->top < 0) dirtyAreaRect.top = 0;
				if (dirtyArea->right > cvWidth || dirtyArea->right < 0)
					dirtyAreaRect.right = cvWidth;
				if (dirtyArea->bottom > cvHeight || dirtyArea->bottom < 0)
					dirtyAreaRect.bottom = cvHeight;
			}
			m_renderCmd->SetCanvas(m_renderCmd->GetRenderCanvas());
			m_renderCmd->BeginDraw();
			//清空脏区域
			if (!dirtyArea && !m_renderMode)
			{
				m_renderCmd->PushClipRect(dirtyAreaRect);
				m_renderCmd->Clear();
				m_renderCmd->PopClipRect();
			}
			else
				m_renderCmd->Clear();

			//绘制子控件
			renderData _param;
			try 
			{
				m_renderRoot->RenderTree(&_param);
			}
			catch(...)
			{
				m_renderCmd->EndDraw();
				std::rethrow_exception(std::current_exception());
			}
			m_updateCache = false;

			//由平台接口实现呈现
			Present(m_renderCmd, &dirtyAreaRect);
		});

		//记录当前FPS
		m_fpsCache = m_fpsCounter.CalcFPS();
	}

	void UIWindowBasic::ThreadProc() try
	{
		//绘制任务
		_m_rect_t dirtyArea { 0 };
		if (!m_drawCmdList.isEmpty() && !m_renderMode)
			m_drawCmdList.pop(dirtyArea);
		RenderControlTree(&dirtyArea);

		using namespace std::chrono;
		std::unique_lock lock = GetLock();
		if (!m_renderMode && m_drawCmdList.isEmpty() && m_threadTaskList.empty())
		{
			MThreadT::Pause(lock);
		}

		//线程任务
		for (auto& [task, notification] : m_threadTaskList)
		{
			try
			{
				task();
				notification.set_value();
			}
			catch (...) { notification.set_exception(std::current_exception()); }
		}
		m_threadTaskList.clear();
	}
	catch(...)
	{
		M_ThreadPostException(std::current_exception());
	}

	void UIWindowBasic::FreeCurMouseCtrl()
	{
		m_mouseCur = nullptr;
		m_mouseIn = false;
	}

	void UIWindowBasic::ExecuteThreadTask(const std::function<void()>& task)
	{
		if (MThreadT::GetID() == std::this_thread::get_id())
		{
			task();
			return;
		}

		std::unique_lock lock = GetLock();

		std::promise<void> send_notification;
		auto wait_notification = send_notification.get_future();
		m_threadTaskList.push_back({ task, send_notification });
		lock.unlock();

		ResumeThread();
		wait_notification.wait();
	}

	void UIWindowBasic::ResumeThread()
	{
		if (!m_renderCmd->IsTaskThread())
			Resume();
	}
}
