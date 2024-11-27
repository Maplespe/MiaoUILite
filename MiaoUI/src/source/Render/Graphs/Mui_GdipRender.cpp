/**
 * FileName: Mui_GdipRender.cpp
 * Note: GdiPlus 渲染器
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

#include <Render/Graphs/Mui_GdipRender.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

namespace Mui::Render
{
	using namespace Gdiplus;

	BYTE GdiAlpha::s_byAlphaBack[65536];

	const CLSID* GetEncoderClsid(MImgFormat format)
	{
		static std::map<MImgFormat, CLSID> encoderClsidMap;

		if (encoderClsidMap.empty())
		{
			UINT numEncoders = 0, size = 0;
			GetImageEncodersSize(&numEncoders, &size);

			if (size == 0) return nullptr; // No encoders found

			std::unique_ptr<BYTE[]> buffer(new BYTE[size]);
			ImageCodecInfo* encoders = reinterpret_cast<ImageCodecInfo*>(buffer.get());

			GetImageEncoders(numEncoders, size, encoders);

			for (UINT i = 0; i < numEncoders; ++i)
			{
				std::wstring mimeType(encoders[i].MimeType);
				if (mimeType == L"image/png") encoderClsidMap[MImgFormat::PNG] = encoders[i].Clsid;
				else if (mimeType == L"image/jpeg") encoderClsidMap[MImgFormat::JPG] = encoders[i].Clsid;
				else if (mimeType == L"image/bmp") encoderClsidMap[MImgFormat::BMP] = encoders[i].Clsid;
			}
		}

		auto it = encoderClsidMap.find(format);
		return it != encoderClsidMap.end() ? &it->second : nullptr;
	}

	_m_lpcwstr MRender_GDIP::GetRenderName()
	{
		return L"GDIPlus";
	}

	bool MRender_GDIP::InitRender(_m_uint width, _m_uint height)
	{
		if (m_Graphics)
			return true;
		Resize(width, height);
		return true;
	}

	bool MRender_GDIP::Resize(_m_uint width, _m_uint height)
	{
		m_CanvasDef = (MCanvas_GDIP*)CreateCanvas(width, height, 0);
		m_CanvasTmp = (MCanvas_GDIP*)CreateCanvas(width, height, 0);
		m_Canvas = m_CanvasDef;
		m_Rgn = new MRgn_GDIP((int)width, (int)height);
		ResetCanvas();
		return true;
	}

	MCanvas* MRender_GDIP::CreateCanvas(_m_uint width, _m_uint height, _m_param param)
	{
		Bitmap bitmap((int)width, (int)height, PixelFormat32bppARGB);
		HBITMAP hbitmap = nullptr;
		if(bitmap.GetHBITMAP(Gdiplus::Color::Transparent, &hbitmap) != Gdiplus::Ok)
			return nullptr;

		auto ret = new MCanvas_GDIP();
		ret->m_base = m_base;
		ret->m_size = { (int)width, (int)height };
		ret->m_bitmap = hbitmap;
		ret->m_hdc = CreateCompatibleDC(nullptr);
		SelectObject(ret->m_hdc, hbitmap);
		return ret;
	}

	MBitmap* MRender_GDIP::CreateBitmap(std::wstring_view path, _m_param param)
	{
		auto bitmap = Bitmap::FromFile(path.data());
		auto clean = RAII::scope_exit([&] { delete bitmap; });
		HBITMAP hbitmap = nullptr;
		if (!bitmap || bitmap->GetHBITMAP(Gdiplus::Color::Transparent, &hbitmap) != Gdiplus::Ok)
			return nullptr;

		auto ret = new MBitmap_GDIP();
		ret->m_base = m_base;
		ret->m_bitmap = hbitmap;
		ret->m_hdc = CreateCompatibleDC(nullptr);
		ret->m_size = { (int)bitmap->GetWidth(), (int)bitmap->GetHeight() };
		SelectObject(ret->m_hdc, hbitmap);
		return ret;
	}

	MBitmap* MRender_GDIP::CreateBitmap(UIResource resource, _m_param param)
	{
		RAII::Mui_Ptr stream = SHCreateMemStream(resource.data, (UINT)resource.size);
		if (!stream)
			return nullptr;

		auto bitmap = Bitmap::FromStream(stream.get());
		auto clean = RAII::scope_exit([&] { delete bitmap; });
		stream = nullptr;
		HBITMAP hbitmap = nullptr;
		if (!bitmap || bitmap->GetHBITMAP(Gdiplus::Color::Transparent, &hbitmap) != Gdiplus::Ok)
			return nullptr;

		auto ret = new MBitmap_GDIP();
		ret->m_base = m_base;
		ret->m_bitmap = hbitmap;
		ret->m_hdc = CreateCompatibleDC(nullptr);
		ret->m_size = { (int)bitmap->GetWidth(), (int)bitmap->GetHeight() };
		SelectObject(ret->m_hdc, hbitmap);
		return ret;
	}

	MBitmap* MRender_GDIP::CreateBitmap(_m_uint width, _m_uint height, void* bit, _m_uint len, _m_uint stride)
	{
		Bitmap bitmap((int)width, (int)height, (int)stride, PixelFormat32bppARGB, (BYTE*)bit);
		HBITMAP hbitmap = nullptr;
		if (bitmap.GetHBITMAP(Gdiplus::Color::Transparent, &hbitmap) != Gdiplus::Ok)
			return nullptr;

		auto ret = new MBitmap_GDIP();
		ret->m_base = m_base;
		ret->m_bitmap = hbitmap;
		ret->m_hdc = CreateCompatibleDC(nullptr);
		ret->m_size = { (int)width, (int)height };
		SelectObject(ret->m_hdc, hbitmap);
		return ret;
	}

	MBitmap* MRender_GDIP::CreateSVGBitmap(std::wstring_view path, _m_uint width, _m_uint height, bool repColor,
		_m_color color)
	{
		return nullptr;
	}

	MBitmap* MRender_GDIP::CreateSVGBitmapFromXML(std::wstring_view xml, _m_uint width, _m_uint height, bool repColor,
		_m_color color)
	{
		return nullptr;
	}

	MPen* MRender_GDIP::CreatePen(_m_uint width, _m_color color)
	{
		auto ret = new MPen_GDIP();
		ret->m_base = m_base;
		ret->m_pen = new Pen(Gdiplus::Color());
		ret->SetWidthAndColor(width, color);
		return ret;
	}

	MBrush* MRender_GDIP::CreateBrush(_m_color color)
	{
		auto ret = new MBrush_GDIP();
		ret->m_base = m_base;
		ret->m_brush = new SolidBrush(Gdiplus::Color());
		ret->SetColor(color);
		return ret;
	}

	MGradientBrush* MRender_GDIP::CreateGradientBrush(const std::pair<_m_color, float>* vertex, _m_ushort count,
		UIPoint start, UIPoint end)
	{
		if (count < 2)
			return nullptr;
		auto ret = new MGradientBrush_GDIP();
		ret->m_base = m_base;
		ret->m_brush = new LinearGradientBrush(Point(start.x, start.y), Point(end.x, end.y),
			Gdiplus::Color::Transparent, Gdiplus::Color::Transparent);
		ret->m_vertex.resize(count);
		ret->m_start = start;
		ret->m_end = end;
		for (_m_ushort i = 0; i < count; ++i)
		{
			ret->m_vertex[i] = vertex[i];
		}
		ret->SetOpacity(255);
		return ret;
	}

	MFont* MRender_GDIP::CreateFonts(std::wstring_view text, std::wstring_view fontName, _m_uint fontSize,
		_m_ptrv fontCollection)
	{
		auto ret = new MFont_GDIP();
		ret->m_base = m_base;
		ret->m_font = fontName;
		ret->m_text = text;
		ret->m_fontsize = fontSize;
		ret->UpdateFont();
		return ret;
	}

	MEffects* MRender_GDIP::CreateEffects(MEffects::Types effect, float value)
	{
		return nullptr;
	}

	MGeometry* MRender_GDIP::CreateRoundGeometry(_m_rect dest, float round)
	{
		round *= 2;
		auto ret = new MGeometry_GDIP();
		ret->m_base = m_base;
		ret->m_geometry = new GraphicsPath();
		ret->m_geometry->AddArc((float)dest.left, (float)dest.top, round, round, 180, 90);
		ret->m_geometry->AddArc((float)dest.right - round, (float)dest.top, round, round, 270, 90);
		ret->m_geometry->AddArc((float)dest.right - round, (float)dest.bottom - round, round, round, 0, 90);
		ret->m_geometry->AddArc((float)dest.left, (float)dest.bottom - round, round, round, 90, 90);
		ret->m_geometry->CloseFigure();
		return ret;
	}

	MGeometry* MRender_GDIP::CreateEllipseGeometry(_m_rect dest)
	{
		auto ret = new MGeometry_GDIP();
		ret->m_base = m_base;
		ret->m_geometry = new GraphicsPath();
		ret->m_geometry->AddEllipse((float)dest.left, (float)dest.top, (float)dest.GetWidth(), (float)dest.GetHeight());
		return ret;
	}

	MCanvas* MRender_GDIP::CreateSubAtlasCanvas(_m_uint width, _m_uint height)
	{
		return nullptr;
	}

	MBatchBitmap* MRender_GDIP::CreateBatchBitmap()
	{
		return nullptr;
	}

	bool MRender_GDIP::CopyBitmapContent(MBitmap* dst, MBitmap* src, UIPoint dstPt, _m_rect srcRect)
	{
		if (!dst || !src) return false;

		return BitBlt(static_cast<MBitmap_GDIP*>(dst)->m_hdc, dstPt.x, dstPt.y, srcRect.GetWidth(), srcRect.GetHeight(),
						static_cast<MBitmap_GDIP*>(src)->m_hdc, srcRect.left, srcRect.top, SRCCOPY);
	}

	bool MRender_GDIP::CopyBitmapContent(MCanvas* dst, MCanvas* src, UIPoint dstPt, _m_rect srcRect)
	{
		if (!dst || !src) return false;

		bool ret = BitBlt(static_cast<MCanvas_GDIP*>(dst)->m_hdc, dstPt.x, dstPt.y, srcRect.GetWidth(), srcRect.GetHeight(),
			static_cast<MCanvas_GDIP*>(src)->m_hdc, srcRect.left, srcRect.top, SRCCOPY);
		return ret;
	}

	void MRender_GDIP::BeginDraw()
	{
	}

	void MRender_GDIP::SetCanvas(MCanvas* canvas)
	{
		m_Canvas = (MCanvas_GDIP*)canvas;
		m_Graphics = std::make_unique<Graphics>(m_Canvas->m_hdc);
	}

	void MRender_GDIP::ResetCanvas()
	{
		SetCanvas(m_CanvasDef.get());
	}

	void MRender_GDIP::DrawBitmap(MBitmap* img, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality)
	{
		if (dest.IsEmpty())
			dest = { 0, 0, int(img->GetWidth()), int(img->GetHeight()) };

		if (src.IsEmpty())
			src = { 0, 0, int(img->GetWidth()), int(img->GetHeight()) };

		drawBitmap(static_cast<MBitmap_GDIP*>(img)->m_hdc, alpha, dest, src, highQuality);
	}

	void MRender_GDIP::DrawBitmap(MCanvas* canvas, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality)
	{
		if (dest.IsEmpty())
			dest = { 0, 0, int(canvas->GetWidth()), int(canvas->GetHeight()) };

		if (src.IsEmpty())
			src = { 0, 0, int(canvas->GetWidth()), int(canvas->GetHeight()) };

		drawBitmap(static_cast<MCanvas_GDIP*>(canvas)->m_hdc, alpha, dest, src, highQuality);
	}

	void MRender_GDIP::DrawBatchBitmap(MBatchBitmap* bmp, MBitmap* input, bool highQuality)
	{
	}

	void MRender_GDIP::DrawBatchBitmap(MBatchBitmap* bmp, MCanvas* input, bool highQuality)
	{
	}

	void MRender_GDIP::DrawNinePalacesImg(MBitmap* img, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin,
		bool highQuality)
	{
		if (dest.IsEmpty())
			dest = { 0, 0, int(img->GetWidth()), int(img->GetHeight()) };

		if (src.IsEmpty())
			src = { 0, 0, int(img->GetWidth()), int(img->GetHeight()) };

		NinePalaceDraw([&](_m_rect _dst, _m_rect _src)
		{
			drawBitmap(static_cast<MBitmap_GDIP*>(img)->m_hdc, alpha, _dst, _src, highQuality);
		}, dest, src, margin);
	}

	void MRender_GDIP::DrawNinePalacesImg(MCanvas* canvas, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin,
		bool highQuality)
	{
		if (dest.IsEmpty())
			dest = { 0, 0, int(canvas->GetWidth()), int(canvas->GetHeight()) };

		if (src.IsEmpty())

			src = { 0, 0, int(canvas->GetWidth()), int(canvas->GetHeight()) };

		NinePalaceDraw([&](_m_rect _dst, _m_rect _src)
		{
			drawBitmap(static_cast<MCanvas_GDIP*>(canvas)->m_hdc, alpha, _dst, _src, highQuality);
		}, dest, src, margin);
	}

	void MRender_GDIP::DrawRectangle(_m_rect dest, MPen* pen)
	{
		int penWidth = (int)static_cast<MPen_GDIP*>(pen)->m_pen->GetWidth();
		dest.left += penWidth / 2;
		dest.bottom += penWidth / 2;
		dest.right -= penWidth;
		dest.bottom -= penWidth;

		m_Graphics->SetSmoothingMode(SmoothingModeHighSpeed);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeNone);
		m_Graphics->DrawRectangle(static_cast<MPen_GDIP*>(pen)->m_pen, dest.left, dest.top, dest.GetWidth(), dest.GetHeight());
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeDefault);
	}

	void MRender_GDIP::DrawRoundedRect(_m_rect dest, float round, MPen* pen)
	{
		RAII::Mui_Ptr geometry = CreateRoundGeometry(dest, round);
		m_Graphics->SetSmoothingMode(SmoothingModeHighQuality);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);
		m_Graphics->DrawPath(static_cast<MPen_GDIP*>(pen)->m_pen, geometry.cast<MGeometry_GDIP>()->m_geometry);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeDefault);
	}

	void MRender_GDIP::FillRectangle(_m_rect dest, MBrush* brush)
	{
		m_Graphics->SetSmoothingMode(SmoothingModeHighSpeed);
		m_Graphics->FillRectangle(static_cast<MBrush_GDIP*>(brush)->m_brush, dest.left, dest.top, dest.GetWidth(), dest.GetHeight());
	}

	void MRender_GDIP::FillRectangle(_m_rect dest, MGradientBrush* brush)
	{
		m_Graphics->SetSmoothingMode(SmoothingModeHighSpeed);
		auto gbrush = static_cast<MGradientBrush_GDIP*>(brush)->m_brush;
		gbrush->TranslateTransform((float)dest.left, (float)dest.top, MatrixOrderAppend);
		m_Graphics->FillRectangle(gbrush, dest.left, dest.top, dest.GetWidth(), dest.GetHeight());
		gbrush->ResetTransform();
	}

	void MRender_GDIP::FillRoundedRect(_m_rect dest, float round, MBrush* brush)
	{
		RAII::Mui_Ptr geometry = CreateRoundGeometry(dest, round);
		m_Graphics->SetSmoothingMode(SmoothingModeHighQuality);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);
		m_Graphics->FillPath(static_cast<MBrush_GDIP*>(brush)->m_brush, geometry.cast<MGeometry_GDIP>()->m_geometry);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeDefault);
		m_Graphics->SetSmoothingMode(SmoothingModeHighSpeed);
	}

	void MRender_GDIP::DrawTextLayout(MFont* font, _m_rect dest, MBrush* brush, TextAlign alignment)
	{
		//m_Graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
		m_Graphics->SetSmoothingMode(SmoothingModeHighQuality);
		m_Graphics->SetInterpolationMode(InterpolationModeHighQuality);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeHalf);

		StringFormat format;
		if (alignment & TextAlign_Top)
			format.SetLineAlignment(StringAlignmentNear);
		if (alignment & TextAlign_Left)
			format.SetAlignment(StringAlignmentNear);
		if (alignment & TextAlign_Center)
			format.SetAlignment(StringAlignmentCenter);
		if (alignment & TextAlign_Right)
			format.SetAlignment(StringAlignmentFar);
		if (alignment & TextAlign_VCenter)
			format.SetLineAlignment(StringAlignmentCenter);
		if (alignment & TextAlign_Bottom)
		{
			format.SetAlignment(StringAlignmentFar);
			format.SetLineAlignment(StringAlignmentFar);
		}
		format.SetFormatFlags(StringFormatFlagsNoWrap);
		format.SetTrimming(StringTrimmingNone);

		auto fontObj = static_cast<MFont_GDIP*>(font);
		m_Graphics->DrawString(fontObj->m_text.cstr(), -1,
			fontObj->m_fontObj, RectF((float)dest.left, (float)dest.top - fontObj->m_baseOffset, (float)dest.GetWidth(), (float)dest.GetHeight()),
			&format, static_cast<MBrush_GDIP*>(brush)->m_brush);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeDefault);
	}

	void MRender_GDIP::DrawBitmapEffects(MBitmap* img, MEffects* effect, _m_byte alpha, _m_rect dest, _m_rect src)
	{
	}

	void MRender_GDIP::DrawBitmapEffects(MCanvas* canvas, MEffects* effect, _m_byte alpha, _m_rect dest, _m_rect src)
	{
	}

	void MRender_GDIP::DrawLine(UIPoint x, UIPoint y, MPen* pen)
	{
		m_Graphics->SetSmoothingMode(SmoothingModeAntiAlias);
		m_Graphics->DrawLine(static_cast<MPen_GDIP*>(pen)->m_pen, x.x, x.y, y.x, y.y);
	}

	void MRender_GDIP::DrawEllipse(_m_rect dest, MPen* pen)
	{
		m_Graphics->SetSmoothingMode(SmoothingModeAntiAlias);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);
		m_Graphics->DrawEllipse(static_cast<MPen_GDIP*>(pen)->m_pen,
			RectF((float)dest.left, (float)dest.top, (float)dest.GetWidth(), (float)dest.GetHeight()));
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeDefault);
	}

	void MRender_GDIP::FillEllipse(_m_rect dest, MBrush* brush)
	{
		m_Graphics->SetSmoothingMode(SmoothingModeAntiAlias);
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeHighQuality);
		m_Graphics->FillEllipse(static_cast<MBrush_GDIP*>(brush)->m_brush,
			RectF((float)dest.left, (float)dest.top, (float)dest.GetWidth(), (float)dest.GetHeight()));
		m_Graphics->SetPixelOffsetMode(PixelOffsetModeDefault);
		m_Graphics->SetSmoothingMode(SmoothingModeHighSpeed);
	}

	void MRender_GDIP::PushClipRect(_m_rect rect)
	{
		m_Graphics->SetClip(Rect(rect.left, rect.top, rect.GetWidth(), rect.GetHeight()));
	}

	void MRender_GDIP::PopClipRect()
	{
		m_Graphics->ResetClip();
	}

	void MRender_GDIP::PushClipGeometry(MGeometry* geometry)
	{
		m_state = m_Graphics->Save();
		m_Graphics->SetClip(static_cast<MGeometry_GDIP*>(geometry)->m_geometry);
	}

	void MRender_GDIP::PopClipGeometry()
	{
		m_Graphics->Restore(m_state);
	}

	void MRender_GDIP::Clear(_m_color color)
	{
		m_Graphics->Clear(Gdiplus::Color(color));
	}

	_m_param MRender_GDIP::GetDC()
	{
		//m_hdc = m_Graphics->GetHDC();
		return (_m_param)m_Canvas->m_hdc;
	}

	void MRender_GDIP::ReleaseDC()
	{
		/*m_Graphics->ReleaseHDC(m_hdc);
		m_hdc = nullptr;*/
	}

	_m_result MRender_GDIP::EndDraw()
	{
		return 0;
	}

	MCanvas* MRender_GDIP::GetCanvas()
	{
		return m_Canvas.get();
	}

	MCanvas* MRender_GDIP::GetRenderCanvas()
	{
		return m_CanvasDef.get();
	}

	void* MRender_GDIP::Get()
	{
		return m_Graphics.get();
	}

	void MRender_GDIP::Flush()
	{
	}

	MCanvas* MRender_GDIP::GetSharedCanvas()
	{
		return m_CanvasTmp.get();
	}

	bool MRender_GDIP::SaveMBitmap(MBitmap* bitmap, std::wstring_view path, MImgFormat format)
	{
		if (!bitmap) return false;

		Gdiplus::Bitmap* gbitmap = Gdiplus::Bitmap::FromHBITMAP(static_cast<MBitmap_GDIP*>(bitmap)->m_bitmap, nullptr);
		auto clean = RAII::scope_exit([&] { delete gbitmap; });

		const CLSID* clsid = GetEncoderClsid(format);
		if (!clsid) return false;

		return gbitmap->Save(path.data(), clsid, nullptr) == Ok;
	}

	bool MRender_GDIP::SaveMCanvas(MCanvas* canvas, std::wstring_view path, MImgFormat format)
	{
		if (!canvas) return false;

		Gdiplus::Bitmap* gbitmap = Gdiplus::Bitmap::FromHBITMAP(static_cast<MCanvas_GDIP*>(canvas)->m_bitmap, nullptr);
		auto clean = RAII::scope_exit([&] { delete gbitmap; });


		const CLSID* clsid = GetEncoderClsid(format);
		if (!clsid) return false;

		return gbitmap->Save(path.data(), clsid, nullptr) == Ok;
	}

	UIResource MRender_GDIP::SaveMBitmap(MBitmap* bitmap, MImgFormat format)
	{
		return saveBitmap(static_cast<MBitmap_GDIP*>(bitmap)->m_bitmap, format);
	}

	UIResource MRender_GDIP::SaveMCanvas(MCanvas* canvas, MImgFormat format)
	{
		return saveBitmap(static_cast<MCanvas_GDIP*>(canvas)->m_bitmap, format);
	}

	bool MRender_GDIP::CheckSubAtlasSource(MCanvas* canvas, MCanvas* canvas1)
	{
		return false;
	}

	BOOL MRender_GDIP::MAlphaBlend(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc,
		int ySrc, int wSrc, int hSrc, BYTE alpha, MRgn_GDIP* mRgn, bool useLinearInterpolation)
	{
		if(!hdcDest || !hdcSrc)
			return FALSE;

		HBITMAP dstBitmap = (HBITMAP)GetCurrentObject(hdcDest, OBJ_BITMAP);
		BITMAP dstBmpInfo;
		GetObjectW(dstBitmap, sizeof(BITMAP), &dstBmpInfo);
		auto* dstPixels = static_cast<BYTE*>(dstBmpInfo.bmBits);

		if(dstBmpInfo.bmBitsPixel != 32)
			return FALSE;

		HBITMAP srcBitmap = (HBITMAP)GetCurrentObject(hdcSrc, OBJ_BITMAP);
		BITMAP srcBmpInfo;
		GetObjectW(srcBitmap, sizeof(BITMAP), &srcBmpInfo);
		auto* srcPixels = static_cast<BYTE*>(srcBmpInfo.bmBits);

		if(srcBmpInfo.bmBitsPixel != 32)
			return FALSE;

		ySrc = srcBmpInfo.bmHeight - ySrc - hSrc;
		int yDestInv = dstBmpInfo.bmHeight - yDest - hDest;

		float scaleX = static_cast<float>(wSrc) / (float)wDest;
		float scaleY = static_cast<float>(hSrc) / (float)hDest;

		for (int y = 0; y < hDest; ++y)
		{
			for (int x = 0; x < wDest; ++x)
			{
				int srcX = static_cast<int>((float)x * scaleX);
				int srcY = static_cast<int>((float)y * scaleY);

				if(xDest + x < 0 || xDest + x >= dstBmpInfo.bmWidth || yDestInv + y < 0 || yDestInv + y >= dstBmpInfo.bmHeight)
					continue;

				if(mRgn)
				{
					_m_byte maskBit = mRgn->GetBit(xDest + x, yDestInv + y + mRgn->m_align);
					if(maskBit == 0)
						continue;
				}

				BYTE* dstPixel = dstPixels + (y + yDestInv) * dstBmpInfo.bmWidthBytes + (x + xDest) * 4;
				BYTE* srcPixel = srcPixels + (srcY + ySrc) * srcBmpInfo.bmWidthBytes + (srcX + xSrc) * 4;
				if (useLinearInterpolation)
				{
					float srcXF = (float)x * scaleX;
					float srcYF = (float)y * scaleY;
					int srcX = static_cast<int>(srcXF);
					int srcY = static_cast<int>(srcYF);
					float u = srcXF - srcX;
					float v = srcYF - srcY;

					BYTE* srcPixelTR = srcPixel;
					if (xSrc + srcX + 1 >= 0 && xSrc + srcX + 1 < srcBmpInfo.bmWidth)
						srcPixelTR += 4;

					BYTE* srcPixelBL = srcPixel;
					if (ySrc + srcY + 1 >= 0 && ySrc + srcY + 1 < srcBmpInfo.bmHeight)
						srcPixelBL += srcBmpInfo.bmWidthBytes;

					BYTE* srcPixelBR = srcPixelBL;
					if (xSrc + srcX + 1 >= 0 && xSrc + srcX + 1 < srcBmpInfo.bmWidth)
						srcPixelBR += 4;

					BYTE interpolatedPixel[4] = { 0 };
					for (int c = 0; c < 4; ++c)
					{
						float top = srcPixel[c] * (1 - u) + srcPixelTR[c] * u;
						float bottom = srcPixelBL[c] * (1 - u) + srcPixelBR[c] * u;
						interpolatedPixel[c] = static_cast<BYTE>(top * (1 - v) + bottom * v);
					}

					BYTE srcAlpha = (interpolatedPixel[3] * alpha) / 255;
					BYTE invAlpha = 255 - srcAlpha;

					for (int c = 0; c < 3; ++c)
					{
						dstPixel[c] = (interpolatedPixel[c] * srcAlpha + dstPixel[c] * invAlpha) / 255;
					}
					dstPixel[3] = (srcAlpha + dstPixel[3] * invAlpha / 255);
				}
				else
				{
					BYTE srcAlpha = (srcPixel[3] * alpha) / 255;
					BYTE invAlpha = 255 - srcAlpha;
					for (int c = 0; c < 3; ++c)
					{
						dstPixel[c] = (srcPixel[c] * srcAlpha + dstPixel[c] * invAlpha) / 255;
					}
					dstPixel[3] = (srcAlpha + dstPixel[3] * invAlpha / 255);
				}
			}
		}
		return TRUE;
	}

	MRgn_GDIP* MRender_GDIP::GetMRgn()
	{
		Region region;
		m_Graphics->GetClip(&region);
		HRGN hRGN = region.GetHRGN(m_Graphics.get());
		m_Rgn->Set(hRGN);
		DeleteObject(hRGN);
		return m_Rgn.get();
	}

	void MRender_GDIP::ReleaseThis()
	{
	}
	
	void MRender_GDIP::drawBitmap(HDC hdc, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality)
	{
		//SelectClipRgn(m_Canvas->m_hdc, hRGN);

		auto rgn = GetMRgn();
		MAlphaBlend(m_Canvas->m_hdc, dest.left, dest.top, dest.GetWidth(), dest.GetHeight(),
			hdc, src.left, src.top, src.GetWidth(), src.GetHeight(), alpha, rgn, highQuality);

		//SelectClipRgn(m_Canvas->m_hdc, nullptr);

		/*Bitmap img(hbitmap, nullptr);

		if (dest.IsEmpty())
			dest = { 0, 0, int(img.GetWidth()), int(img.GetHeight()) };

		if (src.IsEmpty())
			src = { 0, 0, int(img.GetWidth()), int(img.GetHeight()) };

		Rect destGdiRect(dest.left, dest.top, dest.GetWidth(), dest.GetHeight());
		Rect srcGdiRect(src.left, src.top, src.GetWidth(), src.GetHeight());

		m_Graphics->SetSmoothingMode(highQuality ? SmoothingModeHighQuality : SmoothingModeHighSpeed);

		if (alpha == 255)
		{
			m_Graphics->DrawImage(&img, destGdiRect, srcGdiRect.X, srcGdiRect.Y,
				srcGdiRect.Width, srcGdiRect.Height, UnitPixel);
		}
		else
		{
			ImageAttributes imageAttributes;
			ColorMatrix colorMatrix =
			{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, (float)alpha / 255.f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f
			};
			imageAttributes.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
			m_Graphics->DrawImage(&img, destGdiRect, srcGdiRect.X, srcGdiRect.Y,
				srcGdiRect.Width, srcGdiRect.Height, UnitPixel, &imageAttributes);
		}*/
	}

	UIResource MRender_GDIP::saveBitmap(HBITMAP hbitmap, MImgFormat format)
	{
		Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(hbitmap, nullptr);
		auto clean = RAII::scope_exit([&] { delete bitmap; });

		if (!bitmap) return { nullptr, 0 };

		const CLSID* clsid = GetEncoderClsid(format);
		if (!clsid) return { nullptr, 0 };

		IStream* stream = SHCreateMemStream(nullptr, 0);
		if (!stream) return { nullptr, 0 };

		Status status = bitmap->Save(stream, clsid, nullptr);
		if (status != Ok)
		{
			stream->Release();
			return { nullptr, 0 };
		}

		ULARGE_INTEGER size;
		LARGE_INTEGER zero = { 0 };
		stream->Seek(zero, STREAM_SEEK_END, &size);
		stream->Seek(zero, STREAM_SEEK_SET, nullptr);

		std::vector<_m_byte> data(size.LowPart);
		ULONG read;
		stream->Read(data.data(), size.LowPart, &read);
		stream->Release();

		return { data.data(), size.LowPart };
	}
}
