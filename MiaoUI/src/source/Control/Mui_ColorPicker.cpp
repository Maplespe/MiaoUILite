/**
 * FileName: Mui_ColorPicker.cpp
 * Note: UI颜色选择器实现
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
#include <Control/Mui_ColorPicker.h>

#define MAX(a, b, c) ((a)>(b) ? ((a)>(c) ? (a):(c)) : ((b)>(c) ? (b):(c)))
#define MIN(a, b, c) ((a)<(b) ? ((a)<(c) ? (a):(c)) : ((b)<(c) ? (b):(c)))

namespace Mui::Ctrl
{
	using namespace Color;

	void UIColorPicker::Register()
	{
		static auto method = [](UIControl* parent)
		{
			return new UIColorPicker(parent);
		};
		CtrlMgr::RegisterControl(ClassName, method);
	}

	UIColorPicker::UIColorPicker(UIControl* parent) : UIColorPicker()
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
	}

	void UIColorPicker::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (attribName == L"color")
		{
			SetRGBAColor(Helper::M_GetAttribValueColor(attrib), draw);
		}
		else if (attribName == L"hsv")
		{
			std::vector<int> dst;
			Helper::M_GetAttribValueInt(attrib, dst, 3);
			HSV hsv;
			hsv.hue = (_m_ushort)dst[0]; hsv.sat = (_m_ushort)dst[1]; hsv.val = (_m_ushort)dst[2];
			SetHSVColor(hsv, draw);
		}
		else
			UIControl::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UIColorPicker::GetAttribute(std::wstring_view attribName)
	{
		if (attribName == L"color")
		{
			return M_RGBA_STR(GetRGBAColor());
		}
		if (attribName == L"hsv")
		{
			HSV hsv = GetHSVColor();
			std::wstring ret = std::to_wstring(hsv.hue);
			ret += L"," + std::to_wstring(hsv.sat);
			ret += L"," + std::to_wstring(hsv.val);
			return ret;
		}
		return UIControl::GetAttribute(attribName);
	}

	void UIColorPicker::SetHSVColor(HSV hsv, bool draw)
	{
		m_hue = hsv.hue;
		m_sat = hsv.sat;
		m_val = hsv.val;

		UpdateRGB();

		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	UIColorPicker::HSV UIColorPicker::GetHSVColor()
	{
		HSV hsv;
		hsv.hue = (_m_ushort)round(m_hue);
		hsv.sat = (_m_ushort)round(m_sat);
		hsv.val = (_m_ushort)round(m_val);
		return hsv;
	}

	void UIColorPicker::SetRGBAColor(_m_color color, bool draw)
	{
		m_rgb = M_RGB(M_GetRValue(color), M_GetGValue(color), M_GetBValue(color));
		m_alpha = M_GetAValue(color);

		UpdateHSV();

		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	_m_color UIColorPicker::GetRGBAColor()
	{
		return m_rgb | (m_alpha << 24);
	}

	void UIColorPicker::OnLoadResource(MRenderCmd* render, bool recreate)
	{
		UINodeBase::OnLoadResource(render, recreate);

		m_colorBrush = nullptr;
		m_bottomBrush = nullptr;
		std::pair<_m_color, float> vertex[2];
		vertex[0] = std::make_pair(0, 0.f);
		vertex[1] = std::make_pair(M_Black, 1.f);
		m_bottomBrush = render->CreateGradientBrush(vertex, 2, { 0, 0 }, { 0, 0 });
	}

	void UIColorPicker::OnPaintProc(MPCPaintParam param)
	{
		if (!m_bottomBrush)
			return;
		if ((_m_ushort)m_hue != (_m_ushort)m_hue_old) 
		{
			m_colorBrush = nullptr;
			m_hue_old = m_hue;
		}
		if (!m_colorBrush)
		{
			float sat = m_sat;
			float val = m_val;
			_m_color rgb = m_rgb;
					
			m_sat = 100.f;
			m_val = 100.f;

			UpdateRGB();
			_m_color vertcolor = m_rgb | (param->cacheCanvas ? 255 << 24 : UINodeBase::m_data.AlphaDst << 24);

			m_sat = sat;
			m_val = val;
			m_rgb = rgb;

			std::pair<_m_color, float> vertex[2];
			vertex[0] = std::make_pair(M_White, 0.f);
			vertex[1] = std::make_pair(vertcolor, 1.f);
			m_colorBrush = param->render->CreateGradientBrush(vertex, 2, { 0, 0 }, { 0, 0 });
		}

		const int width = param->destRect->GetWidth();
		const int height = param->destRect->GetHeight();

		m_colorBrush->SetStartPoint({ 0, width });
		m_bottomBrush->SetStartPoint({ width, 0 });
		m_colorBrush->SetEndPoint({ height, width });
		m_bottomBrush->SetEndPoint({ width, height });

		param->render->FillRectangle(*param->destRect, m_colorBrush);
		param->render->FillRectangle(*param->destRect, m_bottomBrush);
	}

	bool UIColorPicker::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		switch (message)
		{
		case M_MOUSE_HOVER:
#ifdef _WIN32
			SetCursor(IDC_CROSS);
#endif // _WIN32
			break;
		case M_MOUSE_LEAVE:
#ifdef _WIN32
			SetCursor(IDC_ARROW);
#endif // _WIN32
			break;
		case M_MOUSE_LBDOWN:
			m_down = true;
			SetCapture();
			break;
		}
		return UIControl::OnMouseMessage(message, wParam, lParam);
	}

	bool UIColorPicker::OnMouseMove(_m_uint flag, const UIPoint& point)
	{
		if (m_down)
		{
			auto&& frame = Frame();
			float x = float(point.x - frame.left);
			float y = float(point.y - frame.top);
			float width = (float)frame.GetWidth();
			float height = (float)frame.GetHeight();

			bool setpos = false;
			if (x < 0.f) {
				x = 0.f;
				setpos = true;
			}
			else if (x > width) {
				x = width;
				setpos = true;
			}
			if (y < 0.f) {
				y = 0.f;
				setpos = true;
			}
			else if (y > height) {
				y = height;
				setpos = true;
			}
			if (setpos) {
#ifdef _WIN32
				if (auto parent = GetParentWin())
				{
					POINT pt = { (int)x + frame.left, (int)y + frame.top };
					ClientToScreen((HWND)parent->GetWindowHandle(), &pt);
					SetCursorPos(pt.x, pt.y);
				}
#endif // _WIN32
			}
			m_sat = x / width * 100.f;
			m_val = (height - y) / height * 100.f;
			UpdateRGB();
			if (m_rgb_old != m_rgb)
			{
				m_rgb_old = m_rgb;
				SendEvent(Event_ColorPicker_Change, m_rgb | (m_alpha << 24));
				mslot.changed.Emit(m_rgb | (m_alpha << 24));
			}
		}
		return UIControl::OnMouseMove(flag, point);
	}

	bool UIColorPicker::OnLButtonUp(_m_uint flag, const UIPoint& point)
	{
		if (m_down)
			OnMouseMove(flag, point);
		m_down = false;
		ReleaseCapture();
		return UIControl::OnLButtonUp(flag, point);
	}

	bool UIColorPicker::OnSetCursor(_m_param hCur, _m_param lParam)
	{
#ifdef _WIN32
		::SetCursor((HCURSOR)hCur);
#endif // _WIN32
		return true;
	}

	void UIColorPicker::UpdateRGB()
	{
		float sat = m_sat / 100.f;
		float val = m_val / 100.f;

		bool zero = false;
		if (m_hue == 0.f)
		{
			zero = true;
			m_hue = 359.f;
		}

		float base = 255.f * (1.f - sat) * val;

		float r, g, b;
		r = g = b = 0;

		switch (_m_ushort(m_hue / 60.f))
		{
		case 0:
			r = 255.f * val;
			g = (255.f * val - base) * (m_hue / 60.f) + base;
			b = base;
			break;

		case 1:
			r = (255.f * val - base) * (1.f - (((_m_ushort)m_hue % 60) / 60.f)) + base;
			g = 255.f * val;
			b = base;
			break;

		case 2:
			r = base;
			g = 255.f * val;
			b = (255.f * val - base) * (((_m_ushort)m_hue % 60) / 60.f) + base;
			break;

		case 3:
			r = base;
			g = (255.f * val - base) * (1.f - (((_m_ushort)m_hue % 60) / 60.f)) + base;
			b = 255.f * val;
			break;

		case 4:
			r = (255.f * val - base) * (((_m_ushort)m_hue % 60) / 60.f) + base;
			g = base;
			b = 255.f * val;
			break;

		case 5:
			r = 255.f * val;
			g = base;
			b = (255.f * val - base) * (1.f - (((_m_ushort)m_hue % 60) / 60.f)) + base;
			break;
		}
		if (zero)
			m_hue = 0.f;
		m_rgb = M_RGB((_m_byte)r, (_m_byte)g, (_m_byte)b);
	}

	void UIColorPicker::UpdateHSV()
	{
		_m_byte r = M_GetRValue(m_rgb);
		_m_byte g = M_GetGValue(m_rgb);
		_m_byte b = M_GetBValue(m_rgb);
		short temp;
		_m_ushort max = MAX(r, g, b);
		_m_ushort min = MIN(r, g, b);
		_m_ushort delta = max - min;

		if (max == 0)
		{
			m_hue = m_sat = m_val = 0;
			return;
		}

		m_val = (float)max / 255.f * 100.f;
		m_sat = ((float)delta / max) * 100.f;

		if (r == max)
			temp = (short)(60 * ((g - b) / (float)delta));
		else if (g == max)
			temp = (short)(60 * (2.0 + (b - r) / (float)delta));
		else
			temp = (short)(60 * (4.0 + (r - g) / (float)delta));

		if (temp < 0)
			m_hue = float(temp + 360);
		else
			m_hue = (float)temp;
	}

}
