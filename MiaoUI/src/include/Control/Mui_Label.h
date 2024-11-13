/**
 * FileName: Mui_Label.h
 * Note: UI标签控件声明
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
 * date: 2020-10-23 Create
*/

#pragma once
#include <Control/Mui_Control.h>

namespace Mui::Ctrl
{
	/* UI标签控件
	* 属性列表:
	* fontStyle
	* text	(std::wstring_view）			- 文本
	* font	(std::wstring_view)			- 字体名称
	* fontSize (_m_ushort)				- 字体尺寸 默认12px
	* fontBold (bool)					- 是否粗体
	* fontItalics (bool)				- 是否斜体
	* fontUnderline (bool)				- 是否下划线
	* fontStrikeout (bool)				- 是否删除线
	* fontColor 4x						- 字体颜色 默认RGBA(0,0,0,255)
	* hyperlink (bool)					- 是否为超链接
	* url (std::wstring_view)			- 超链接网址
	* urlColor 4x						- 超链接热点颜色 默认RGBA(167,226,54,255)
	* textAlign							- 文本对齐方式 仅数字 为TextAlignment枚举值 默认LeftTop
	* fontCustom (_m_ptrv)				- 自定义字体集对象参数	Lite版本无效
	* shadowUse (bool)					- 使用文本阴影效果
	* shadowBlur (float)				- 文本阴影模糊度 默认1.f
	* shadowColor (_m_color) 4x			- 文本阴影颜色 默认RGBA(0,0,0,255)
	* shadowOffset (UIPoint) 2x			- 文本阴影偏移位置 x,y 默认 1,1
	* shadowLow (bool)					- 是否使用低质量阴影 如果为true=没有模糊效果 blur属性无效
	*/
	class UILabel : public UIControl
	{
	public:
		MCTRL_DEFINE
		(
			L"UILabel",
			{ CtrlMgr::AttribType::labelStyle, L"fontStyle" }
		);
		MCTRL_DEFSLOT
		(
			CtrlMgr::EventSlot<UIPoint> clicked;//鼠标左键单击
		);

		struct Attribute
		{
			UIString text;
			UIString font = M_DEF_SYSTEM_FONTNAME;
			UIString url;

			_m_color fontColor = Color::M_Black;
			_m_color urlColor = 0xffa7e236;
			_m_color shadowColor = Color::M_Black;

			_m_ushort fontSize = 12;
			_m_ptrv fontCustom = 0;

			bool hyperlink = false;
			bool shadowUse = false;
			bool shadowLow = false;

			float shadowBlur = 1.f;

			UIPoint shadowOffset = { 1,1 };
			UIFontStyle fontStyle;
			TextAlign textAlign = TextAlign_Top;
		};

		UILabel(UIControl* parent, Attribute attrib);

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		template<typename T>
		bool SetAttributeSrc(std::wstring_view attribName, T&& value, bool draw = true)
		{
			return m_attrib.SetAttribute<T>(attribName, std::forward<T>(value), { this, draw });
		}
		[[nodiscard]] const Attribute& GetAttribute() const { return m_attrib.Get(); }

		//取文本尺寸
		[[nodiscard]] UISize GetTextMetric(bool dpi = true) const;

	protected:
		explicit UILabel(Attribute attrib);

		UISize CalcSize();

		void OnLoadResource(MRenderCmd* render, bool recreate) override;

		void OnPaintProc(MPCPaintParam param) override;

		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;

		bool OnSetCursor(_m_param hCur, _m_param lParam) override;

		void OnScale(_m_scale scale) override;

		_m_sizef GetContentSize() override;

		//偏移绘制
		bool OffsetDraw = false;
		_m_rect OffsetDrawRc = { 0,0,0,0 };

		MFontPtr m_font = nullptr;
		MBrushPtr m_brush = nullptr;
		MEffectPtr m_effect = nullptr;

		bool m_mouseIn = false;
		bool m_isClick = false;

		CtrlMgr::UIAttribute<Attribute, std::pair<UILabel*, bool>> m_attrib;
		static void BindAttribute();
		void updateStyle(UIFontStyle& style);
		bool updateRender(bool draw, bool layout = false);
	};
}
