/**
 * FileName: Mui_Windows.h
 * Note: UI Windows平台窗口声明
 *
 * Copyright (C) 2021-2023 Maplespe (mapleshr@icloud.com)
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
 * date: 2021-4-22 Create
*/
#pragma once
#ifdef _WIN32
#include <Window/Mui_BasicWnd.h>

namespace Mui::Window
{
	class UIWindowsWnd : public UIWindowBasic
	{
	public:
		UIWindowsWnd(Render::Def::MRender* render);
		UIWindowsWnd(UIWindowsWnd&&) = delete;
		UIWindowsWnd(const UIWindowsWnd&) = delete;
		UIWindowsWnd& operator=(const UIWindowsWnd&) = delete;
		UIWindowsWnd& operator=(UIWindowsWnd&&) = delete;
		~UIWindowsWnd() override;

		/*创建窗口
		* @param parent - 父窗口
		* @param title - 窗口标题
		* @param rect - 窗口矩形
		* @param afterCreated - 窗口创建后回调函数
		* @param style - 窗口样式 WS_ 开头的Windows样式
		* @param exStyle - 扩展窗口样式 WS_EX_ 开头的Windows样式
		* 
		* @return 创建失败或重复创建或afterCreated返回false 则返回false
		*/
		bool Create(_m_param parent, std::wstring_view title, UIRect rect, std::function<bool()>&& afterCreated, _m_param style, _m_param exStyle);

		/*创建窗口
		* @param parent - 父窗口
		* @param title - 窗口标题
		* @param rect - 窗口矩形
		* @param afterCreated - 窗口创建后回调函数
		* @param wndType - 窗口类型 0=默认带系统边框的窗口 1=不带标题栏的普通窗口 2=弹出式无边框窗口(WS_POPUP) 3=分层窗口(WS_EX_LAYERED)
		* @param exStyle - 自定义扩展样式附加参数
		*
		* @return 创建失败或重复创建或afterCreated返回false 则返回false
		*/
		bool Create(_m_param parent, std::wstring_view title, UIRect rect, std::function<bool()>&& afterCreated, int wndType, _m_param exStyle = 0);

		void SetWindowTitle(std::wstring_view title) override;

		std::wstring GetWindowTitle() override;

		void ShowWindow(bool show, bool focus = true) override;

		bool IsShowWindow() override;

		void EnableWindow(bool enable) override;

		bool IsEnableWindow() override;

		bool IsMinimize() override;

		bool IsMaximize() override;

		void CenterWindow() override;

		void CloseWindow() override;

		void SetWindowRect(UIRect rect) override;

		UIRect GetWindowRect(bool client = false) override;

		void SetWindowAlpha(_m_byte alpha, bool draw = true) override;

		_m_byte GetWindowAlpha() override;

		_m_ptrv GetWindowHandle() override;

		void SetMinimSize(UISize min) override;

		void SetVerticalSync(bool sync);

		//将Windows消息码转换为MEventCodeEnum 例如 WM_SIZE -> M_WND_SIZE
		MEventCodeEnum ConvertEventCode(_m_uint src);
		//将MEventCodeEnum转换为Windows消息码 例如 M_WND_SIZE -> WM_SIZE
		_m_uint ConvertEventCode(MEventCodeEnum src);

	protected:
		UISize GetWindowSrcSize() override;

		void Present(Render::MRenderCmd* render, const _m_rect_t<int>* rcPaint) override;

		bool InitRender(Render::MRenderCmd* render) override;

	private:
		std::wstring_view M_DEF_CLSNAME = L"MiaoUI_Windows";

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		BOOL RegisterWindowClass();

		HWND m_hWnd = nullptr;
		_m_byte m_alpha = 255;
		std::wstring m_title;
		UISize m_srcSize;
		UISize m_minSize;
		bool m_layerWnd = false;
		bool m_notitleWnd = false;
		std::atomic_bool m_VSync = false;
		_m_param m_cachePos = 0;
	};
}
#endif // _WIN32