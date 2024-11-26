/**
 * FileName: Mui_GdipBaseObj.cpp
 * Note: GdiPlus 基本对象实现
 *
 * Copyright (C) 2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2024-11-6 Create
*/

#include <Render/Graphs/Mui_GdipBaseObj.h>

namespace Mui::Render
{
	using namespace Color;
	using namespace Gdiplus;

	void MBitmap_GDIP::ReleaseThis()
	{
		if(m_bitmap)
			DeleteObject(m_bitmap);
		if(m_hdc)
			DeleteDC(m_hdc);
	}

	_m_rect MCanvas_GDIP::GetSubRect()
	{
		return { 0, 0, m_size.width, m_size.height };
	}

	void MCanvas_GDIP::ReleaseThis()
	{
		if(m_bitmap)
			DeleteObject(m_bitmap);
		if (m_hdc)
			DeleteDC(m_hdc);
	}

	void MPen_GDIP::SetColor(_m_color color)
	{
		m_colorSrc = color;
		SetOpacity(m_alpha);
	}

	void MPen_GDIP::SetWidth(_m_uint width)
	{
		m_pen->SetWidth((Gdiplus::REAL)width);
	}

	void MPen_GDIP::SetOpacity(_m_byte alpha)
	{
		m_alpha = alpha;
		_m_byte tmp = M_GetAValue(m_colorSrc);
		m_pen->SetColor(Gdiplus::Color(_m_byte((float)alpha * (float)tmp / 255.f), 
			M_GetRValue(m_colorSrc), M_GetGValue(m_colorSrc), M_GetBValue(m_colorSrc)));
	}

	void MPen_GDIP::SetWidthAndColor(_m_uint width, _m_color color)
	{
		SetWidth(width);
		SetColor(color);
	}

	_m_color MPen_GDIP::GetColor()
	{
		return m_colorSrc;
	}

	_m_uint MPen_GDIP::GetWidth()
	{
		return (int)m_pen->GetWidth();
	}

	_m_byte MPen_GDIP::GetOpacity()
	{
		return m_alpha;
	}

	void MPen_GDIP::ReleaseThis()
	{
		delete m_pen;
	}

	void MBrush_GDIP::SetColor(_m_color color)
	{
		m_colorSrc = color;
		SetOpacity(m_alpha);
	}

	void MBrush_GDIP::SetOpacity(_m_byte alpha)
	{
		m_alpha = alpha;
		_m_byte tmp = M_GetAValue(m_colorSrc);
		m_brush->SetColor(Gdiplus::Color(_m_byte((float)alpha * (float)tmp / 255.f),
			M_GetRValue(m_colorSrc), M_GetGValue(m_colorSrc), M_GetBValue(m_colorSrc)));
	}

	_m_color MBrush_GDIP::GetColor()
	{
		return m_colorSrc;
	}

	_m_byte MBrush_GDIP::GetOpacity()
	{
		return m_alpha;
	}

	void MBrush_GDIP::ReleaseThis()
	{
		delete m_brush;
	}

	_m_uint MGradientBrush_GDIP::GetColorPosCount()
	{
		return (_m_uint)m_vertex.size();
	}

	_m_color MGradientBrush_GDIP::GetPosColor(_m_uint index)
	{
		return m_vertex[index].first;
	}

	void MGradientBrush_GDIP::SetOpacity(_m_byte alpha)
	{
		m_alpha = alpha;
		auto color = new Gdiplus::Color[m_vertex.size()];
		auto pt = new Gdiplus::REAL[m_vertex.size()];
		for (size_t i = 0; i < m_vertex.size(); ++i)
		{
			_m_byte tmp = M_GetAValue(m_vertex[i].first);
			color[i] = Gdiplus::Color(_m_byte((float)alpha * (float)tmp / 255.f),
			M_GetRValue(m_vertex[i].first), M_GetGValue(m_vertex[i].first), M_GetBValue(m_vertex[i].first));
			pt[i] = m_vertex[i].second;
		}
		if(m_vertex.size() > 2)
			m_brush->SetInterpolationColors(color + 2, pt, (int)m_vertex.size() - 2);
		else
			m_brush->SetLinearColors(color[0], color[1]);
		delete[] color;
		delete[] pt;
	}

	_m_byte MGradientBrush_GDIP::GetOpacity()
	{
		return m_alpha;
	}

	UIPoint MGradientBrush_GDIP::GetStartPoint()
	{
		return m_start;
	}

	void MGradientBrush_GDIP::SetStartPoint(UIPoint start)
	{
		Gdiplus::PointF pt = { (Gdiplus::REAL)start.x, (Gdiplus::REAL)start.y };
		m_start = start;
		delete m_brush;
		m_brush = new Gdiplus::LinearGradientBrush(pt, { (float)m_end.x, (float)m_end.y },
			Gdiplus::Color(0, 0, 0, 0), Gdiplus::Color(0, 0, 0, 0));
		SetOpacity(m_alpha);
	}

	UIPoint MGradientBrush_GDIP::GetEndPoint()
	{
		return m_end;
	}

	void MGradientBrush_GDIP::SetEndPoint(UIPoint end)
	{
		Gdiplus::PointF pt = { (Gdiplus::REAL)end.x, (Gdiplus::REAL)end.y };
		m_end = end;
		delete m_brush;
		m_brush = new Gdiplus::LinearGradientBrush({ (float)m_start.x, (float)m_start.y }, pt,
			Gdiplus::Color(0, 0, 0, 0), Gdiplus::Color(0, 0, 0, 0));
		SetOpacity(m_alpha);
	}

	void MGradientBrush_GDIP::ReleaseThis()
	{
		delete m_brush;
	}

	void MFont_GDIP::SetFontName(std::wstring_view name)
	{
		m_font = name;
		UpdateFont();
	}

	void MFont_GDIP::SetFontSize(_m_uint size, std::pair<_m_uint, _m_uint> range)
	{
		m_fontsize = size;
		UpdateFont();
	}

	void MFont_GDIP::SetFontStyle(UIFontStyle style, std::pair<_m_uint, _m_uint> range)
	{
		m_style = style;
		UpdateFont();
	}

	void MFont_GDIP::SetFontColor(MBrush* brush, std::pair<_m_uint, _m_uint> range)
	{
		Gdiplus::Color cr;
		static_cast<MBrush_GDIP*>(brush)->m_brush->GetColor(&cr);
		m_color = cr.GetValue();
	}

	void MFont_GDIP::SetText(std::wstring_view text)
	{
		m_text = text;
		CalcMetrics();
	}

	UIRect MFont_GDIP::GetMetrics()
	{
		return m_metrics;
	}

	const UIString& MFont_GDIP::GetFontName()
	{
		return m_font;
	}

	_m_uint MFont_GDIP::GetFontSize()
	{
		return m_fontsize;
	}

	UIFontStyle MFont_GDIP::GetFontStyle()
	{
		return m_style;
	}

	_m_color MFont_GDIP::GetFontColor()
	{
		return m_color;
	}

	const UIString& MFont_GDIP::GetText()
	{
		return m_text;
	}

	void MFont_GDIP::ReleaseThis()
	{
		delete m_fontObj;
	}

	void MFont_GDIP::CalcMetrics()
	{
		GraphicsPath graphicsPathObj;
		FontFamily fontfamily;
		m_fontObj->GetFamily(&fontfamily);
		StringFormat f;
		f.SetAlignment(StringAlignmentNear);
		f.SetLineAlignment(StringAlignmentNear);
		graphicsPathObj.AddString(m_text.cstr(), -1, &fontfamily, m_fontObj->GetStyle(), m_fontObj->GetSize(),Point(1,1), &f);
		RectF rcBound;
		graphicsPathObj.GetBounds(&rcBound);

		m_metrics = { 0, 0, (int)round(rcBound.X + rcBound.Width),
			(int)round(rcBound.Y + rcBound.Height) };
	}

	void MFont_GDIP::UpdateFont()
	{
		Gdiplus::FontFamily family(m_font.cstr());

		int fontStyle = FontStyleRegular;
		if (m_style.bold) fontStyle |= FontStyleBold;
		if (m_style.italics) fontStyle |= FontStyleItalic;
		if (m_style.underline) fontStyle |= FontStyleUnderline;
		if (m_style.strikeout) fontStyle |= FontStyleStrikeout;

		delete m_fontObj;
		m_fontObj = new Gdiplus::Font(&family, (REAL)m_fontsize, fontStyle, UnitPixel);
		CalcMetrics();
	}

	MGeometry::MGeometryTypes MGeometry_GDIP::GetGeometryType()
	{
		return m_type;
	}

	void MGeometry_GDIP::ReleaseThis()
	{
		delete m_geometry;
	}

	MRgn_GDIP::MRgn_GDIP(int width, int height)
	{
		width = (width + 3) / 4 * 4;
		int nheight = (height + 3) / 4 * 4;

		m_align = nheight - height;
		height = nheight;

		m_hdc = CreateCompatibleDC(nullptr);

		auto* pbmi = (BITMAPINFO*)new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
		BITMAPINFO& bmi = *pbmi;
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 8;
		bmi.bmiHeader.biCompression = BI_RGB;

		for (USHORT i = 0; i < 256; ++i)
		{
			bmi.bmiColors[i].rgbRed = (BYTE)i;
			bmi.bmiColors[i].rgbGreen = (BYTE)i;
			bmi.bmiColors[i].rgbBlue = (BYTE)i;
			bmi.bmiColors[i].rgbReserved = 0;
		}
		m_bitmap = CreateDIBSection(m_hdc, &bmi, DIB_RGB_COLORS, &m_bits, nullptr, 0);
		delete[] pbmi;

		SelectObject(m_hdc, m_bitmap);

		m_size = { width, height };
	}

	void MRgn_GDIP::Set(HRGN hRgn)
	{
		RECT rect{ 0,0, m_size.width, m_size.height };
		FillRect(m_hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		SelectClipRgn(m_hdc, hRgn);
		FillRect(m_hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		SelectClipRgn(m_hdc, nullptr);
	}

	void MRgn_GDIP::ReleaseProc()
	{
		if(m_bitmap)
			DeleteObject(m_bitmap);
		if(m_hdc)
			DeleteDC(m_hdc);
	}

	_m_byte MRgn_GDIP::GetBit(int x, int y)
	{
		if (x < 0 || x >= m_size.width || y < 0 || y >= m_size.height)
			return 0;
		return *((_m_lpbyte)m_bits + y * m_size.width + x);
	}
}
