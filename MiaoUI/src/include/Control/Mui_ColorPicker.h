/**
 * FileName: Mui_ColorPicker.h
 * Note: UI颜色选择器声明
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
 * date: 2022-9-5 Create
*/
#pragma once
#include <Control/Mui_Control.h>

namespace Mui::Ctrl
{
	/* UIColorPicker 控件
	* 属性列表:
	* color 4x - RGBA颜色
	* hsv 3x   - HSV颜色
	*/
	class UIColorPicker : public UIControl
	{
	public:
		MCTRL_DEFNAME(L"UIColorPicker");
		MCTRL_DEFSLOT
		(
			CtrlMgr::EventSlot<_m_color> changed;//颜色已更改
		);

		UIColorPicker(UIControl* parent);

		struct HSV
		{
			_m_ushort hue = 0;
			_m_ushort sat = 0;
			_m_ushort val = 0;
		};

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		void SetHSVColor(HSV hsv, bool draw = true);
		HSV GetHSVColor();

		void SetRGBAColor(_m_color color, bool draw = true);
		_m_color GetRGBAColor();

	protected:
		UIColorPicker() { m_cacheSupport = true; }

		void OnLoadResource(MRenderCmd* render, bool recreate) override;

		void OnPaintProc(MPCPaintParam param) override;
		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;
		bool OnMouseMove(_m_uint flag, const UIPoint& point) override;
		bool OnLButtonUp(_m_uint flag, const UIPoint& point) override;
		bool OnSetCursor(_m_param hCur, _m_param lParam) override;

	private:
		float m_hue = 359.f;
		float m_sat = 0.f;
		float m_val = 0.f;

		float m_hue_old = m_hue;

		_m_color m_rgb = 0;
		_m_color m_rgb_old = 0;
		_m_byte m_alpha = 255;

		void UpdateRGB();
		void UpdateHSV();

		MGradientBrushPtr m_colorBrush = nullptr;
		MGradientBrushPtr m_bottomBrush = nullptr;

		bool m_down = false;
	};
}
