/**
 * FileName: Mui_Engine.h
 * Note: 引擎主类封装声明
 *
 * Copyright (C) 2022-2024 Maplespe (mapleshr@icloud.com)
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
#pragma once
#include <Window/Mui_BasicWnd.h>
#include <Mui_XML.h>

namespace Mui
{
	class MWindowCtx;

	class MiaoUI final
	{
	public:
		MiaoUI(MiaoUI&&) = delete;
		MiaoUI(const MiaoUI&) = delete;
		MiaoUI& operator=(const MiaoUI&) = delete;
		MiaoUI& operator=(MiaoUI&&) = delete;
		MiaoUI() = default;
		~MiaoUI();

		enum class Render
		{
			Auto,			//根据平台自动选择
			Direct2D_3D,	//仅Windows可用		Lite版本不可用
			OpenGL_Core,	//仅桌面端平台可用		Lite版本不可用
			OpenGL_ES,		//仅Android可用		Lite版本不可用
			Custom,			//自定义渲染器

			Gdiplus			//仅Windows可用 仅Lite版本可用
		};

		enum class MWindowType
		{
			Normal,		//普通默认系统窗口
			NoTitleBar,	//无标题栏窗口 带系统边框和动画
			Popup,		//弹出式无边框窗口
			Layer,		//分层窗口
		};

		/*初始化
		* @param error - 输出错误信息
		* @param render - 渲染器类型
		* @param param - 渲染器附加参数
		* @return bool 如果失败返回false并将原因写入error文本
		*/
		bool InitEngine(std::wstring& error, Render render = Render::Auto, _m_ptrv param = 0);

		/*反初始化
		* 释放所用的资源和所有窗口
		*/
		void UnInitEngine();

		//添加资源文件
		bool AddResourcePath(std::wstring_view path, std::wstring_view key);
		bool AddResourceMem(UIResource memfile, std::wstring_view key);


		//移除资源文件引用 仅路径添加的有效
		bool RemoveResource(std::wstring_view path);

		/*创建窗口上下文
		* @param rect - 窗口矩形
		* @param type - 窗口类型
		* @param title - 窗口标题
		* @param visible - 是否可视
		* @param main - 是否为主窗口
		* @param dpi - 启用dpi自动缩放
		* @param parent - 父窗口
		* @param exparam - 扩展参数
		*
		* @return 窗口上下文指针
		*/
		MWindowCtx* CreateWindowCtx(UIRect rect, MWindowType type, std::wstring_view title,
			bool main, bool dpi = true, _m_param parent = 0, _m_ptrv exparam = 0);

	private:
		std::vector<MWindowCtx*> m_windowList{};
		std::unordered_map<std::wstring, std::wstring> m_res_pathList{};
		std::vector<std::pair<UIResource, std::wstring>> m_res_memList{};

		bool m_isinit = false;
		Render m_renderType = Render::Auto;
		_m_ptrv m_customRender = 0;

		Mui::Render::MRender* CreateRender();

		friend class MWindowCtx;
	};

	using MWndEventCallback = std::function<bool(MWindowCtx*, UINotifyEvent, Ctrl::UIControl*, _m_param)>;
	using MWndDefEventSource = std::function<_m_result(MEventCodeEnum, _m_param)>;
	using MWndSourceCallback = std::function<_m_result(MWindowCtx*, MWndDefEventSource, MEventCodeEnum, _m_param)>;
	using MWndInitUICallback = std::function<bool(MWindowCtx*, Ctrl::UIControl*, XML::MuiXML*)>;

	class MWindowCtx
	{
	public:
		MWindowCtx(MWindowCtx&&) = delete;
		MWindowCtx(const MWindowCtx&) = delete;
		MWindowCtx& operator=(const MWindowCtx&) = delete;
		MWindowCtx& operator=(MWindowCtx&&) = delete;
		virtual ~MWindowCtx();

		[[nodiscard]] Window::UIWindowBasic* Base() const;

		[[nodiscard]] XML::MuiXML* XML() const;

		bool InitWindow(const MWndInitUICallback& callback, bool visible);

		void SetEventCallback(MWndEventCallback callback);

		void SetEventSourceCallback(MWndSourceCallback callback);

		void EventLoop();

	protected:
		MWindowCtx();
		Window::UIWindowBasic* m_base = nullptr;
		MiaoUI* m_engine = nullptr;

		bool m_isInit = false;

		friend class MiaoUI;
	};
}