/**
 * FileName: Mui_Control.cpp
 * Note: UI控件基本类型实现
 *
 * Copyright (C) 2020-2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2020-10-17 Create
*/

#include <Control/Mui_Control.h>
#include <Render/Graphs/Mui_Render.h>

namespace Mui
{
	using namespace Render;
	using namespace Color;
	using namespace Window;

	namespace Ctrl
	{

		UIControl::UIControl() = default;

		UIControl::~UIControl() = default;

		void UIControl::Register()
		{
			static auto method = [](UIControl* parent)
			{
				UIControl* ret = new UIControl();
				parent->AddChildren(ret);
				return ret;
			};
			CtrlMgr::RegisterControl(ClassName, method);
		}

		void UIControl::SetUserData(_m_param data)
		{
			m_data.UserData = data;
		}

		_m_param UIControl::GetUserData() const
		{
			return m_data.UserData;
		}

		void UIControl::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
		{
			using namespace Helper;

			if (attribName == L"pos")
			{
				std::vector<int> dest;
				M_GetAttribValueInt(attrib, dest, 2);
				SetPos(dest[0], dest[1], draw);
			}
			else if (attribName == L"size")
			{
				std::vector<int> dest;
				M_GetAttribValueInt(attrib, dest, 2);
				SetSize(dest[0], dest[1], draw);
			}
			else if (attribName == L"minSize")
			{
				std::vector<int> dest;
				M_GetAttribValueInt(attrib, dest, 2);
				SetMinSize({ dest[0], dest[1] }, draw);
			}
			else if (attribName == L"maxSize")
			{
				std::vector<int> dest;
				M_GetAttribValueInt(attrib, dest, 2);
				SetMaxSize({ dest[0], dest[1] }, draw);
			}
			else if (attribName == L"frame")
			{
				std::vector<int> dest;
				M_GetAttribValueInt(attrib, dest, 4);
				SetPos(dest[0], dest[1], false);
				SetSize(dest[2], dest[3], draw);
			}
			else if (attribName == L"padding")
			{
				std::vector<int> dest;
				M_GetAttribValueInt(attrib, dest, 4);
				SetPadding(UIRect(_m_rect(dest[0], dest[1], dest[2], dest[3])), draw);
			}
			else if (attribName == L"name")
			{
				SetName(attrib.data());
			}
			else if (attribName == L"data")
			{
				SetUserData(M_StoLong64(attrib));
			}
			else if (attribName == L"visible")
			{
				SetVisible(attrib == L"false" ? false : true, draw);
			}
			else if (attribName == L"enable")
			{
				SetEnabled(attrib == L"false" ? false : true, draw);
			}
			else if (attribName == L"alpha")
			{
				SetAlpha((_m_byte)M_StoInt(attrib), draw);
			}
			else if (attribName == L"enableFocus")
			{
				SetEnableFocus(attrib == L"false" ? false : true);
			}
			else if (attribName == L"msgFilter")
			{
				SetMsgFilter(attrib == L"true" ? true : false);
			}
			else if (attribName == L"msgLgnore")
			{
				SetMsgIgnore(attrib == L"true" ? true : false, m_data.MsgIgnoreChild);
			}
			else if (attribName == L"msgLgnoreChild")
			{
				SetMsgIgnore(m_data.MsgIgnore, attrib == L"true" ? true : false);
			}
			else if (attribName == L"bgColor")
			{
				UIBkgndStyle style = m_bgStyle;
				style.bkgndColor = M_GetAttribValueColor(attrib);
				SetBackground(style);
			}
			else if (attribName == L"frameColor")
			{
				UIBkgndStyle style = m_bgStyle;
				style.FrameColor = M_GetAttribValueColor(attrib);
				SetBackground(style);
			}
			else if (attribName == L"frameWidth")
			{
				UIBkgndStyle style = m_bgStyle;
				style.FrameWidth = (_m_ushort)M_StoInt(attrib);
				SetBackground(style);
			}
			else if (attribName == L"frameRound")
			{
				UIBkgndStyle style = m_bgStyle;
				style.RoundValue = M_StoFloat(attrib);
				SetBackground(style);
			}
			else if(attribName == L"shadowColor")
			{
				UIBkgndStyle style = m_bgStyle;
				style.ShadowColor = M_GetAttribValueColor(attrib);
				SetBackground(style);
			}
			else if (attribName == L"shadowOffset")
			{
				std::vector<int> dest;
				M_GetAttribValueInt(attrib, dest, 2);
				UIBkgndStyle style = m_bgStyle;
				style.ShadowOffset = { dest[0], dest[1] };
				SetBackground(style);
			}
			else if (attribName == L"shadowExtend")
			{
				UIBkgndStyle style = m_bgStyle;
				style.ShadowExtend = M_StoInt(attrib);
				SetBackground(style);
			}
			else if (attribName == L"shadowRadius")
			{
			UIBkgndStyle style = m_bgStyle;
			style.ShadowRadius = M_StoFloat(attrib);
			SetBackground(style);
			}
			else if (attribName == L"align")
			{
				SetAlignType((UIAlignment)M_StoInt(attrib), draw);
			}
			else if(attribName == L"autoSize")
			{
				AutoSize(attrib == L"false" ? false : true, draw);
			}
			else if(attribName == L"dpiScale")
			{
				EnableDPIScale(attrib == L"false" ? false : true);
			}
			else if(attribName == L"scale")
			{
				std::vector<std::wstring> dst;
				M_GetAttribValue(attrib, dst, 4);
				_m_rcscale scale;
				scale.xs = M_StoFloat(dst[0]);
				scale.ys = M_StoFloat(dst[1]);
				scale.ws = M_StoFloat(dst[2]);
				scale.hs = M_StoFloat(dst[3]);
				SetScale(scale, draw);
			}
		}

		std::wstring UIControl::GetAttribute(std::wstring_view attribName)
		{
			using namespace CtrlMgr::Attrib;
			if (attribName == L"pos")
			{
				auto pos = GetPos();
				std::wstring value = std::to_wstring(pos.x);
				value += L"," + std::to_wstring(pos.y);
				return value;
			}
			if (attribName == L"size")
			{
				return Value_Make2x(GetSize());
			}
			if (attribName == L"minSize")
			{
				return Value_Make2x(GetMinSize());
			}
			if (attribName == L"maxSize")
			{
				return Value_Make2x(GetMaxSize());
			}
			if (attribName == L"frame")
			{
				auto pos = GetPos();
				auto size = GetSize();
				return Value_Make4x(pos.x, pos.y, size.width, size.height);
			}
			if (attribName == L"padding")
			{
				return Value_Make4x(GetPadding());
			}
			if (attribName == L"name")
			{
				return GetName().cstr();
			}
			if (attribName == L"data")
			{
				return std::to_wstring(GetUserData());
			}
			if (attribName == L"visible")
			{
				return IsVisible() ? L"true" : L"false";
			}
			if (attribName == L"enable")
			{
				return IsEnabled() ? L"true" : L"false";
			}
			if (attribName == L"alpha")
			{
				return std::to_wstring(GetAlpha());
			}
			if (attribName == L"enableFocus")
			{
				return m_data.IsFocus ? L"true" : L"false";
			}
			if (attribName == L"msgFilter")
			{
				return m_data.MsgFilter ? L"true" : L"false";
			}
			if (attribName == L"msgLgnore")
			{
				return m_data.MsgIgnore ? L"true" : L"false";
			}
			if (attribName == L"msgLgnoreChild")
			{
				return m_data.MsgIgnoreChild ? L"true" : L"false";
			}
			if (attribName == L"bgColor")
			{
				return M_RGBA_STR(m_bgStyle.bkgndColor);
			}
			if (attribName == L"frameColor")
			{
				return M_RGBA_STR(m_bgStyle.FrameColor);
			}
			if (attribName == L"frameWidth")
			{
				return std::to_wstring(m_bgStyle.FrameWidth);
			}
			if (attribName == L"frameRound")
			{
				return std::to_wstring(m_bgStyle.RoundValue);
			}
			if (attribName == L"align")
			{
				return std::to_wstring(GetAlignType());
			}
			if (attribName == L"autoSize")
			{
				return AutoSize() ? L"true" : L"false";
			}
			if(attribName == L"dpiScale")
			{
				return IsDPIScaleEnabled() ? L"true" : L"false";
			}
			if(attribName == L"scale")
			{
				auto scale = GetScale();
				return Value_Make4x(scale.xs, scale.ys, scale.ws, scale.hs);
			}
			return {};
		}

		void UIControl::SetEnabled(bool enabled, bool draw)
		{
			m_data.Enabled = enabled;

			std::function<void(UIControl*, bool)> setchild = [&setchild, this](UIControl* ctrl, bool enable)
			{
				ctrl->m_cacheUpdate = true;
				ctrl->m_data.ParentEnabled = enable;

				for (auto& child : ctrl->GetNodeList())
				{
					if(auto _ctrl = dynamic_cast<UIControl*>((UINodeBase*)child)) 
						setchild(_ctrl, enable);
				}
			};

			for (auto& child : GetNodeList())
			{
				if (auto ctrl = dynamic_cast<UIControl*>((UINodeBase*)child))
					setchild(ctrl, enabled);
			}
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}

		bool UIControl::IsEnabled() const
		{
			return m_data.Enabled && m_data.ParentEnabled;
		}

		UIWindowBasic* UIControl::GetParentWin() const
		{
			return UINodeBase::m_data.ParentWnd;
		}

		void UIControl::AddChildren(UINodeBase* UINode)
		{
			UINodeBase::AddChildren(UINode);
			if (auto ctrl = dynamic_cast<UIControl*>(UINode))
				ctrl->m_data.ParentEnabled = IsEnabled();
		}

		void UIControl::SetFocus(bool focus)
		{
			if (!UINodeBase::m_data.ParentWnd)
				return;
			if (UINodeBase::m_data.ParentWnd->GetFocusControl().curFocus == this && focus)
				return;
			if (m_data.IsFocus)
			{
				UIControl* oldFocus = UINodeBase::m_data.ParentWnd->GetFocusControl().curFocus;
				if (oldFocus && oldFocus != this)
					oldFocus->OnWindowMessage(M_WND_KILLFOCUS, (_m_param)this, 0);
				if (focus) {
					UINodeBase::m_data.ParentWnd->SetFocusControl(this);
					OnWindowMessage(M_WND_SETFOCUS, (_m_param)this, 0);
				}
				else {
					OnWindowMessage(M_WND_KILLFOCUS, (_m_param)this, 0);
				}
			}
		}

		void UIControl::SetEnableFocus(bool enable)
		{
			m_data.IsFocus = enable;
		}

		void UIControl::SetMsgFilter(bool filter)
		{
			m_data.MsgFilter = filter;
		}

		void UIControl::SetMsgIgnore(bool ignore, bool child)
		{
			m_data.MsgIgnore = ignore;
			m_data.MsgIgnoreChild = child;
		}

		void UIControl::ScaleControl(_m_uint width, _m_uint height, bool child)
		{
			_m_scale newSize;
			newSize.cx = (float)width / (float)UINodeBase::m_data.Size.width;
			newSize.cy = (float)height / (float)UINodeBase::m_data.Size.height;
			ScaleControl({ newSize.cx, newSize.cy }, child);
		}

		void UIControl::ScaleControl(_m_scale scale, bool child)
		{
			class Node : public UINodeBase
			{
			public:
				void SetScaleInternal(_m_rcscale scale)
				{
					m_data.RectScale = scale;
				}
			};
			std::function<void(Node*, _m_rcscale)> setChild = [this, &setChild](Node* node, _m_rcscale scale)
			{
				node->SetScaleInternal(scale);
				std::vector<UINodeBase*> nodeList;
				node->GetChildrenList(nodeList);

				for (auto& child : nodeList)
					setChild((Node*)child, scale);
			};
			UINodeBase::m_data.RectScale = { 1.f, 1.f, scale.cx, scale.cy };
			if (child)
			{
				std::vector<UINodeBase*> nodeList;
				GetChildrenList(nodeList);

				_m_rcscale _scale;
				_scale.xs = scale.cx;
				_scale.ys = scale.cy;
				_scale.ws = scale.cx;
				_scale.hs = scale.cy;
				for (auto& _child : nodeList)
					setChild((Node*)_child, _scale);
			}
			UpdateScale();
			UpdateLayout();
		}

		void UIControl::SetCapture()
		{
			UINodeBase::m_data.ParentWnd->SetCapture(this);
		}

		void UIControl::ReleaseCapture()
		{
			UINodeBase::m_data.ParentWnd->ReleaseCapture();
		}

		bool UIControl::isCapture()
		{
			return UINodeBase::m_data.ParentWnd->m_capture == this;
		}

		MTimers::ID UIControl::SetTimer(_m_uint uTimeout)
		{
			auto task = [this](_m_param wnd, _m_param id, _m_param time)
			{
				OnTimer(id);
			};
			return UINodeBase::m_data.ParentWnd->SetTimer(uTimeout, task);
		}

		void UIControl::KillTimer(MTimers::ID idTimer)
		{
			return UINodeBase::m_data.ParentWnd->KillTimer(idTimer);
		}

		UIControl* UIControl::FindMouseControl(const UIPoint& point)
		{
			UIControl* ret = this;

			auto& list = GetNodeList();
			for (size_t i = list.size(); i > 0; --i)
			{
				//类型检查
				UIControl* control = dynamic_cast<UIControl*>((UINodeBase*)list[i - 1]);
				if (!control || !control->IsVisible() || !control->IsEnabled()
					|| (control->m_data.MsgIgnore && control->m_data.MsgIgnoreChild)
					|| !Helper::Rect::IsPtInside(control->Frame(), point))
					continue;

				//忽略消息但不忽略子控件消息
				if(control->m_data.MsgIgnore && !control->m_data.MsgIgnoreChild)
				{
					ret = control->FindMouseControl(point);
					if (ret == control)
					{
						ret = this;
						continue;
					}
				}
				else
					ret = control->FindMouseControl(point);
				break;
			}
			return ret;
		}

		UIControl* UIControl::GetWindowTopCtrl() const
		{
			return GetWindowTopCtrl(UINodeBase::m_data.ParentWnd);
		}

		UIControl* UIControl::GetWindowTopCtrl(UIWindowBasic* wnd) const
		{
			if (wnd)
				return wnd->m_toplay;
			return nullptr;
		}

		void UIControl::SetCursor(_m_lpcwstr cursor_name)
		{
#ifdef _WIN32
			//::ShowCursor(FALSE);
			::SetCursor(LoadCursorW(NULL, cursor_name));
			//::ShowCursor(TRUE);
#endif // _WIN32
		}

		bool UIControl::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
		{
			UIPoint point = Helper::M_GetMouseEventPt(lParam);
			_m_uint flag = (_m_uint)wParam;
			bool ret = false;

			switch ((_m_msg)message)
			{
			case M_MOUSE_HOVER:
				ret = OnMouseEntered(flag, point);
				break;
			case M_MOUSE_LEAVE:
				ret = OnMouseExited(flag, point);
				break;
			case M_MOUSE_WHEEL:
				ret = OnMouseWheel(Helper::M_LOWORD((_m_long)wParam), (short)Helper::M_HIWORD((_m_long)wParam), point);
				break;
			case M_MOUSE_MOVE:
				ret = OnMouseMove(flag, point);
				break;
			case M_MOUSE_LBDOWN:
				SetFocus(true);
				ret = OnLButtonDown(flag, point);
				break;
			case M_MOUSE_LBUP:
				ret = OnLButtonUp(flag, point);
				break;
			case M_MOUSE_LBDBCLICK:
				SetFocus(true);
				ret = OnLButtonDoubleClicked(flag, point);
				break;
			case M_MOUSE_RBDOWN:
				SetFocus(true);
				ret = OnRButtonDown(flag, point);
				break;
			case M_MOUSE_RBUP:
				ret = OnRButtonUp(flag, point);
				break;
			case M_MOUSE_RBDBCLICK:
				SetFocus(true);
				ret = OnRButtonDoubleClicked(flag, point);
				break;
			case M_MOUSE_MBDOWN:
				SetFocus(true);
				ret = OnMButtonDown(flag, point);
				break;
			case M_MOUSE_MBUP:
				ret = OnMButtonUp(flag, point);
				break;
			case M_MOUSE_MBDBCLICK:
				SetFocus(true);
				ret = OnMButtonDoubleClicked(flag, point);
				break;
			case M_SETCURSOR:
				ret = OnSetCursor(wParam, lParam);
				break;
			}

			return ret;
		}

		bool UIControl::OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam)
		{
			switch (code)
			{
			case M_WND_SETFOCUS:
				return SendEvent(Event_Focus_True, wParam);
			case M_WND_KILLFOCUS:
				return SendEvent(Event_Focus_False, wParam);
			case M_WND_KEYDOWN:
				return SendEvent(Event_Key_Down, wParam);
			case M_WND_KEYUP:
				return SendEvent(Event_Key_Up, wParam);
			default:
				break;
			}
			return false;
		}

		bool UIControl::OnMouseEntered(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_Hover, (_m_param)&point);
		}

		bool UIControl::OnMouseExited(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_Exited, (_m_param)&point);
		}

		bool UIControl::OnLButtonDown(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_LDown, (_m_param)&point);
		}

		bool UIControl::OnLButtonUp(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_LUp, (_m_param)&point);
		}

		bool UIControl::OnLButtonDoubleClicked(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_LDoubleClicked, (_m_param)&point);
		}

		bool UIControl::OnRButtonDown(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_RDown, (_m_param)&point);
		}

		bool UIControl::OnRButtonUp(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_RUp, (_m_param)&point);
		}

		bool UIControl::OnRButtonDoubleClicked(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_RDoubleClicked, (_m_param)&point);
		}

		bool UIControl::OnMButtonDown(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_MDown, (_m_param)&point);
		}

		bool UIControl::OnMButtonUp(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_MUp, (_m_param)&point);
		}

		bool UIControl::OnMButtonDoubleClicked(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_MDoubleClicked, (_m_param)&point);
		}

		bool UIControl::OnMouseWheel(_m_uint flag, short delta, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_Wheel, (_m_param)delta);
		}

		bool UIControl::OnMouseMove(_m_uint flag, const UIPoint& point)
		{
			return SendEvent(Event_Mouse_Move, (_m_param)&point);
		}

		bool UIControl::OnSetCursor(_m_param hCur, _m_param lParam)
		{
			return SendEvent(Event_Control_SetCursor);
		}

		void UIControl::OnTimer(MTimers::ID idTimer)
		{
			SendEvent(Event_Control_OnTimer, (_m_param)idTimer);
		}

		bool UIControl::SendEvent(UINotifyEvent event, _m_param param)
		{
			if (UINodeBase::m_data.ParentWnd)
				return UINodeBase::m_data.ParentWnd->EventProc(event, this, param);
			return false;
		}

		MPenPtr UIControl::GetDbgFramePen() const
		{
			if (UINodeBase::m_data.ParentWnd)
				return UINodeBase::m_data.ParentWnd->m_dbgFrame;
			return nullptr;
		}

		bool UIControl::DispatchMouseMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam)
		{
			bool ret = OnMouseMessage(code, wParam, lParam);
			if (m_data.MsgFilter)
			{
				if (auto control = dynamic_cast<UIControl*>(GetParent()))
				{
					control->DispatchMouseMessage(code, wParam, lParam);
				}
			}
			return ret;
		}

		bool UIControl::DispatchWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam)
		{
			bool ret = OnWindowMessage(code, wParam, lParam);
			if (m_data.MsgFilter)
			{
				for (auto& child : GetNodeList())
				{
					if (dynamic_cast<UIControl*>((UINodeBase*)child))
						static_cast<UIControl*>(child)->DispatchWindowMessage(code, wParam, lParam);
				}
			}
			return ret;
		}
	}
}