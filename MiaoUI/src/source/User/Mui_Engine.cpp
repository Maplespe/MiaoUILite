/**
 * FileName: Mui_Engine.cpp
 * Note: 引擎主类封装实现
 *
 * Copyright (C) 2022-2023 Maplespe (mapleshr@icloud.com)
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
 * date: 2022-12-28 Create
*/
#include <User/Mui_Engine.h>
#include <Window/Mui_Windows.h>
#include <Mui_Settings.h>
#include <Render/Graphs/Mui_GdipRender.h>

#ifdef _WIN32
#include <ShellScalingApi.h>
#endif

UINT_PTR g_gdiplusToken = 0;

namespace Mui
{
#ifdef _WIN32

	bool GetWndMonitorDPI(HWND hWnd, UINT& dpiX, UINT& dpiY)
	{
		//Windows 8.1及以上可用
		static HINSTANCE _dll = LoadLibraryW(L"SHCore.dll");
		if (_dll)
		{
			typedef HRESULT(WINAPI* _fun)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
			if (_fun GetDpiForMonitor_ = (_fun)GetProcAddress(_dll, "GetDpiForMonitor"))
			{
				if (FAILED(GetDpiForMonitor_(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
					MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
					goto gdiget;
				return true;
			}
		}

		//旧版系统 使用gdi获取主桌面DPI
	gdiget:
		HDC hdc = GetDC(NULL);
		if (hdc)
		{
			dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
			dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
			ReleaseDC(NULL, hdc);
			return true;
		}
		return false;
	}

	class WindowsCtx final : public Window::UIWindowsWnd
	{
	public:
		explicit WindowsCtx(Render::MRender* render) : UIWindowsWnd(render)
		{
			m_defsource = [this](auto&& PH1, auto&& PH2) -> _m_result
			{
				if((DWORD)PH1 == WM_DPICHANGED && m_enabledpi)
				{
					thread_local bool flag = false;
					if (flag)
						return 0;
					flag = true;
					const auto pm = (std::pair<_m_param, _m_param>*)PH2;
					const UINT dpiX = Helper::M_LOWORD((_m_long)pm->first);
					float scale = (float)dpiX / 96.f;
					ScaleWindow({ scale, scale });
					flag = false;
					return 0;
				}
				if ((DWORD)PH1 == WM_DESTROY)
				{
					delete m_src;
					return 0;
				}

				return UIWindowBasic::EventSource(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
			};
		}

	private:
		bool EventProc(UINotifyEvent event, Ctrl::UIControl* control, _m_param param) override
		{
			if (m_event)
			{
				if ( m_event(m_src, event, control, param))
					return true;
			}
			return false;
		}

		_m_result EventSource(MEventCodeEnum code, _m_param param) override
		{
			if (m_source)
				return m_source(m_src, m_defsource, code, param);
			return m_defsource(code, param);
		}

		MWndDefEventSource m_defsource = nullptr;
		MWndEventCallback m_event = nullptr;
		MWndSourceCallback m_source = nullptr;

		bool m_enabledpi = false;
		UIRect m_srcRect;
		MWindowCtx* m_src = nullptr;

		friend class MiaoUI;
		friend class MWindowCtx;
	};
#endif

	MiaoUI::~MiaoUI()
	{
		if(m_isinit)
			UnInitEngine();
	}

	bool MiaoUI::InitEngine(std::wstring& error, Render render, _m_ptrv param)
	{
		if (m_isinit) return false;

		CtrlMgr::RegisterMuiControl();
		m_isinit = true;
		m_renderType = render;
		if (render == Render::Auto || render == Render::Gdiplus)
		{
			CoInitialize(nullptr);
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
		}
		else if(render == Render::Custom)
		{
			if (!param)
			{
				error = L"param error";
				return false;
			}
			m_customRender = param;
		}
		return true;
	}

	void MiaoUI::UnInitEngine()
	{
		if (!m_isinit) return;
		m_isinit = false;
		for (size_t i = 0; i < m_windowList.size(); i++)
		{
			delete m_windowList[i];
		}
		for (size_t i = 0; i < m_res_memList.size(); i++)
		{
			m_res_memList[i].first.Release();
		}
		if (m_renderType == Render::Gdiplus)
			Gdiplus::GdiplusShutdown(g_gdiplusToken);
	}

	bool MiaoUI::AddResourcePath(std::wstring_view path, std::wstring_view key)
	{
		DMResources res;
		if(!res.LoadResource(path.data(), false))
		{
			res.CloseResource();
			return false;
		}
		res.CloseResource();
		for (auto& wnd : m_windowList)
			wnd->Base()->GetResourceMgr()->AddResourcePath(path, key);
		return m_res_pathList.insert(std::make_pair(path, key)).second;
	}

	bool MiaoUI::AddResourceMem(UIResource memfile, std::wstring_view key)
	{
		for(auto& mem : m_res_memList)
		{
			if (mem.first.size == memfile.size && 
				memcmp(mem.first.data, memfile.data, memfile.size) != 0)
				return false;
		}
		DMResources res;
		if(!res.LoadResource(memfile))
		{
			res.CloseResource();
			return false;
		}
		res.CloseResource();

		UIResource dst(new(std::nothrow)_m_byte[memfile.size], memfile.size);
		if (!dst.data)
			return false;

		memcpy(dst.data, memfile.data, memfile.size);

		for (auto& wnd : m_windowList)
			wnd->Base()->GetResourceMgr()->AddResourceMem(dst, key);

		m_res_memList.emplace_back(dst, key);
		return true;
	}

	bool MiaoUI::RemoveResource(std::wstring_view path)
	{
		bool del = false;
		for(auto& wnd : m_windowList)
		{
			bool success = wnd->Base()->GetResourceMgr()->RemoveResource(path);
			if (!del && success)
				del = true;
		}
		for (auto iter = m_res_pathList.begin(); iter != m_res_pathList.end(); ++iter)
		{
			if (iter->first == path)
			{
				m_res_pathList.erase(iter);
				break;
			}
		}
		return del;
	}

	MWindowCtx* MiaoUI::CreateWindowCtx(UIRect rect, MWindowType type, std::wstring_view title,
		bool main, bool dpi, _m_param parent, _m_ptrv exparam)
	{
		auto ret = new MWindowCtx();
#ifdef _WIN32
		const auto wnd = new WindowsCtx(CreateRender());
		if(!wnd->Create(parent, title, rect, [] { return true; }, (int)type, (_m_param)exparam))
		{
			delete ret;
			delete wnd;
			return nullptr;
		}
		ret->m_base = wnd;
		ret->m_base->SetInited(false);

		auto mgr = wnd->GetResourceMgr();
		for (auto& path : m_res_pathList)
			mgr->AddResourcePath(path.first, path.second);
		for (auto& mem : m_res_memList)
			mgr->AddResourceMem(mem.first, mem.second);

		wnd->m_enabledpi = dpi;
		wnd->m_srcRect = rect;
		wnd->m_src = ret;
		ret->m_engine = this;

		m_windowList.push_back(ret);

		if (main)
			wnd->SetMainWindow(true);

		return ret;
#else
#error __TODO__
#endif
	}

	Render::MRender* MiaoUI::CreateRender()
	{
#ifdef _WIN32
		if (m_renderType == Render::Gdiplus || m_renderType == Render::Auto)
			return new Mui::Render::MRender_GDIP();
#if MUI_CFG_ENABLE_OPENGL
		if (m_renderType == Render::OpenGL_Core)
			return new Mui::Render::GL::MRender_GL();
#endif
#elif __ANDROID__
		if(m_renderType == Render::OpenGL_ES)
			return new MRender_GL();
#else
#error __TODO__
#endif
		if (m_renderType == Render::Custom && m_customRender)
			return (Mui::Render::MRender*)m_customRender;

		MErrorThrow(L"没有可用的渲染器(There is no renderer available.)");
		return nullptr;
	}

	MWindowCtx::~MWindowCtx()
	{
		delete m_base;
		for (_m_long64 i = 0; i < (_m_long64)m_engine->m_windowList.size(); ++i)
		{
			if(m_engine->m_windowList[i] == this)
			{
				m_engine->m_windowList.erase(m_engine->m_windowList.begin() + i);
				return;
			}
		}
	}

	Window::UIWindowBasic* MWindowCtx::Base() const
	{
		return m_base;
	}

	XML::MuiXML* MWindowCtx::XML() const
	{
		return m_base->XMLUI();
	}

	bool MWindowCtx::InitWindow(const MWndInitUICallback& callback, bool visible)
	{
		if(!m_isInit)
		{
			m_isInit = callback(this, m_base->GetRootControl(), m_base->XMLUI());
			if (m_isInit)
			{
#ifdef _WIN32
				if (auto p = static_cast<WindowsCtx*>(m_base); p->m_enabledpi)
				{
					//DPI缩放
					UINT dpiX, dpiY = 0;
					if (GetWndMonitorDPI((HWND)m_base->GetWindowHandle(), dpiX, dpiY))
					{
						float scaleX = (float)dpiX / 96.f;
						float scaleY = (float)dpiY / 96.f;

						m_base->ScaleWindow({ scaleX, scaleY });
					}
				}
#else
#error __TODO__
#endif
				m_base->ShowWindow(visible);
			}
			m_base->SetInited(m_isInit);
			m_base->m_updateCache = true;
			m_base->UpdateLayout(nullptr);
			return m_isInit;
		}
		return false;
	}

	void MWindowCtx::SetEventCallback(MWndEventCallback callback)
	{
		dynamic_cast<WindowsCtx*>(m_base)->m_event = std::move(callback);
	}

	void MWindowCtx::SetEventSourceCallback(MWndSourceCallback callback)
	{
		dynamic_cast<WindowsCtx*>(m_base)->m_source = std::move(callback);
	}

	void MWindowCtx::EventLoop()
	{
		Settings::UIMessageLoop();
	}

	MWindowCtx::MWindowCtx() = default;
}
