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
#pragma once
#include <Render/Graphs/Mui_GdipBaseObj.h>

namespace Mui::Render
{
	struct ALPHAINFO
	{
		BITMAP bm{};
		LPBYTE lpBuf = nullptr;
		RECT rc{};
	};
	class GdiAlpha
	{
		static BYTE s_byAlphaBack[65536];

		static LPBYTE ALPHABACKUP(BITMAP* pBitmap, int x, int y, int cx, int cy)
		{
			LPBYTE lpAlpha = s_byAlphaBack;
			if (x + cx >= pBitmap->bmWidth) cx = pBitmap->bmWidth - x;
			if (y + cy >= pBitmap->bmHeight) cy = pBitmap->bmHeight - y;
			if (cx < 0 || cy < 0 || pBitmap->bmBits == NULL) return NULL;

			if (cx * cy > 65536) lpAlpha = (LPBYTE)malloc(cx * cy);
			LPBYTE lpBits = NULL;
			for (int iRow = 0; iRow < cy; iRow++)
			{
				lpBits = (LPBYTE)pBitmap->bmBits + (y + iRow) * pBitmap->bmWidth * 4 + x * 4;
				lpBits += 3;
				for (int iCol = 0; iCol < cx; iCol++)
				{
					lpAlpha[iRow * cx + iCol] = *lpBits;
					lpBits += 4;
				}
			}
			return lpAlpha;
		}

		static void ALPHARESTORE(BITMAP* pBitmap, int x, int y, int cx, int cy, LPBYTE lpAlpha)
		{
			if (x + cx >= pBitmap->bmWidth) cx = pBitmap->bmWidth - x;
			if (y + cy >= pBitmap->bmHeight) cy = pBitmap->bmHeight - y;
			if (cx < 0 || cy < 0) return;
			LPBYTE lpBits = NULL;
			for (int iRow = 0; iRow < cy; iRow++)
			{
				lpBits = (LPBYTE)pBitmap->bmBits + (y + iRow) * pBitmap->bmWidth * 4 + x * 4;
				lpBits += 3;
				for (int iCol = 0; iCol < cx; iCol++)
				{
					*lpBits = lpAlpha[iRow * cx + iCol];
					lpBits += 4;
				}
			}
			if (lpAlpha != s_byAlphaBack)
				free(lpAlpha);
		}

	public:
		static BOOL AlphaBackup(HDC hdc, LPCRECT pRect, ALPHAINFO& alphaInfo)
		{
			alphaInfo.lpBuf = NULL;
			HBITMAP hBmp = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
			GetObject(hBmp, sizeof(BITMAP), &alphaInfo.bm);

			if (alphaInfo.bm.bmBitsPixel != 32) return FALSE;
			alphaInfo.rc = *pRect;
			alphaInfo.rc.right++;
			alphaInfo.rc.bottom++;
			POINT pt;
			GetViewportOrgEx(hdc, &pt);
			RECT rcImg = { 0,0,alphaInfo.bm.bmWidth,alphaInfo.bm.bmHeight };
			OffsetRect(&alphaInfo.rc, pt.x, pt.y);
			IntersectRect(&alphaInfo.rc, &alphaInfo.rc, &rcImg);
			alphaInfo.lpBuf = ALPHABACKUP(&alphaInfo.bm, alphaInfo.rc.left, alphaInfo.rc.top, alphaInfo.rc.right - alphaInfo.rc.left, alphaInfo.rc.bottom - alphaInfo.rc.top);
			return TRUE;
		}

		static void AlphaRestore(ALPHAINFO& alphaInfo)
		{
			if (!alphaInfo.lpBuf) return;
			ALPHARESTORE(&alphaInfo.bm, alphaInfo.rc.left, alphaInfo.rc.top, alphaInfo.rc.right - alphaInfo.rc.left, alphaInfo.rc.bottom - alphaInfo.rc.top, alphaInfo.lpBuf);
		}
	};

	class MRender_GDIP : public MRender
	{
	public:
		MRender_GDIP() = default;

		_m_lpcwstr GetRenderName() override;

		bool InitRender(_m_uint width, _m_uint height) override;

		bool Resize(_m_uint width, _m_uint height) override;

		MCanvas* CreateCanvas(_m_uint width, _m_uint height, _m_param param) override;

		MBitmap* CreateBitmap(std::wstring_view path, _m_param param = 0U) override;

		MBitmap* CreateBitmap(UIResource resource, _m_param param) override;

		MBitmap* CreateBitmap(_m_uint width, _m_uint height, void* bit, _m_uint len, _m_uint stride) override;

		MBitmap* CreateSVGBitmap(std::wstring_view path, _m_uint width, _m_uint height, bool repColor = false, _m_color color = 0) override;

		MBitmap* CreateSVGBitmapFromXML(std::wstring_view xml, _m_uint width, _m_uint height, bool repColor = false, _m_color color = 0) override;

		MPen* CreatePen(_m_uint width, _m_color color) override;

		MBrush* CreateBrush(_m_color color) override;

		MGradientBrush* CreateGradientBrush(const std::pair<_m_color, float>* vertex, _m_ushort count, UIPoint start, UIPoint end) override;

		MFont* CreateFonts(std::wstring_view text, std::wstring_view fontName, _m_uint fontSize, _m_ptrv fontCollection = 0) override;

		MEffects* CreateEffects(MEffects::Types effect, float value) override;

		MGeometry* CreateRoundGeometry(_m_rect dest, float round) override;

		MGeometry* CreateEllipseGeometry(_m_rect dest) override;

		MCanvas* CreateSubAtlasCanvas(_m_uint width, _m_uint height) override;

		MBatchBitmap* CreateBatchBitmap() override;

		bool CopyBitmapContent(MBitmap* dst, MBitmap* src, UIPoint dstPt, _m_rect srcRect) override;

		bool CopyBitmapContent(MCanvas* dst, MCanvas* src, UIPoint dstPt, _m_rect srcRect) override;

		void BeginDraw() override;

		void SetCanvas(MCanvas* canvas) override;

		void ResetCanvas() override;

		void DrawBitmap(MBitmap* img, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality) override;

		void DrawBitmap(MCanvas* canvas, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality) override;

		void DrawBatchBitmap(MBatchBitmap* bmp, MBitmap* input, bool highQuality = true) override;

		void DrawBatchBitmap(MBatchBitmap* bmp, MCanvas* input, bool highQuality = true) override;

		void DrawNinePalacesImg(MBitmap* img, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin, bool highQuality) override;

		void DrawNinePalacesImg(MCanvas* canvas, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin, bool highQuality) override;

		void DrawRectangle(_m_rect dest, MPen* pen) override;

		void DrawRoundedRect(_m_rect dest, float round, MPen* pen) override;

		void FillRectangle(_m_rect dest, MBrush* brush) override;

		void FillRectangle(_m_rect dest, MGradientBrush* brush) override;

		void FillRoundedRect(_m_rect dest, float round, MBrush* brush) override;

		void DrawTextLayout(MFont* font, _m_rect dest, MBrush* brush, TextAlign alignment) override;

		void DrawBitmapEffects(MBitmap* img, MEffects* effect, _m_byte alpha, _m_rect dest, _m_rect src) override;

		void DrawBitmapEffects(MCanvas* canvas, MEffects* effect, _m_byte alpha, _m_rect dest, _m_rect src) override;

		void DrawLine(UIPoint x, UIPoint y, MPen* pen) override;

		void DrawEllipse(_m_rect dest, MPen* pen) override;

		void FillEllipse(_m_rect dest, MBrush* brush) override;

		void PushClipRect(_m_rect rect) override;

		void PopClipRect() override;

		void PushClipGeometry(MGeometry* geometry) override;

		void PopClipGeometry() override;

		void Clear(_m_color color = 0) override;

		virtual _m_param GetDC();

		virtual void ReleaseDC();

		_m_result EndDraw() override;

		MCanvas* GetCanvas() override;

		MCanvas* GetRenderCanvas() override;

		void* Get() override;

		void Flush() override;

		MCanvas* GetSharedCanvas() override;

		bool SaveMBitmap(MBitmap* bitmap, std::wstring_view path, MImgFormat format) override;

		bool SaveMCanvas(MCanvas* canvas, std::wstring_view path, MImgFormat format) override;

		UIResource SaveMBitmap(MBitmap* bitmap, MImgFormat format) override;

		UIResource SaveMCanvas(MCanvas* canvas, MImgFormat format) override;

		bool CheckSubAtlasSource(MCanvas* canvas, MCanvas* canvas1) override;

		//仅32位 位图支持
		static BOOL MAlphaBlend(HDC hdcDest, int xDest, int yDest, int wDest, int hDest,
			HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, BYTE alpha, MRgn_GDIP* mRgn = nullptr);

		MRgn_GDIP* GetMRgn();

	protected:
		void ReleaseThis() override;

		void drawBitmap(HDC hdc, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality);
		UIResource saveBitmap(HBITMAP hbitmap, MImgFormat format);

		std::unique_ptr<Gdiplus::Graphics> m_Graphics = nullptr;

		RAII::Mui_Ptr<MCanvas_GDIP> m_Canvas = nullptr;
		RAII::Mui_Ptr<MCanvas_GDIP> m_CanvasTmp = nullptr;
		RAII::Mui_Ptr<MCanvas_GDIP> m_CanvasDef = nullptr;
		RAII::Mui_Ptr<MRgn_GDIP> m_Rgn = nullptr;

		Gdiplus::GraphicsState m_state = 0;
	};
}