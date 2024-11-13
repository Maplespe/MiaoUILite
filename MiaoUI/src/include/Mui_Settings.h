/**
 * FileName: Mui_Settings.h
 * Note: MiaoUI 附加设置声明
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
 * date: 2022-1-23 Create
*/
#pragma once
#include <Mui_Base.h>

namespace Mui::Settings
{
	//MiaoUI Logo字符画
	extern const wchar_t* MuiLogoText;

	//MiaoUI 界面库版本
	extern const wchar_t* MuiEngineVer;

	//窗口消息循环
	extern void UIMessageLoop();

	//扩展flag
	enum ExSection
	{
		//Lite版本不支持任何扩展flag

		//Windows Only
		Render_D2D_Factory_Flag_Options,
		Render_D2D_D3DCreateDeviceFlag,
		Render_D2D_D3DCreateDeviceDriverType,
		Render_D2D_RenderNoDXGI,				//不使用DXGI渲染表面 使用DC渲染
		//通用
		Graphic_Atlas_Size_Width,				//Atlas纹理宽度 默认1024
		Graphic_Atlas_Size_Height,				//Atlas纹理高度 默认1024
		Graphic_Atlas_MaxCachePixel,			//Atlas最大缓存像素总数 默认2048*2048
		MXML_SVG_Rasterized_Width,				//MXMLUI 栅格化SVG图标默认宽度 默认64
		MXML_SVG_Rasterized_Height,				//MXMLUI 栅格化SVG图标默认宽度 默认64
		//测试
		Test_DeviceLost
	};

	//ExtensionFlag系函数均为线程安全 但不能递归调用

	/* 设置扩展flag 这些参数通常只适用于特定平台
	* @param ExSection - 扩展部分
	* @param flag - 附加参数
	* @param override - 是否要求直接覆盖原参数 否则追加flag 这里不是覆盖追加的扩展flag 而是要求实际获取flag的函数怎么使用这些flag
	*/
	extern void SetExtensionFlag(ExSection section, _m_param flag, bool override);

	//还原默认扩展flag
	extern void ResExtensionFlag(ExSection section);

	//获取扩展flag
	using ExFlag = std::pair<_m_param, bool>;
	extern bool GetExtensionFlag(ExSection section, ExFlag& param);
}