/**
 * FileName: Mui_RenderMgr.cpp
 * Note: 渲染命令管理器实现
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
 * date: 2021-12-25 Create
*/

#include <Render/Mui_RenderMgr.h>

namespace Mui::Render
{
	using namespace RAII;

	MRenderCmd::MRenderCmd(MRender* base)
	{
		m_stop = false;
		m_begindraw = false;
		m_base = base;
		m_base->m_base = this;
		m_thread = std::thread(&MRenderCmd::ThreadProc, this);
	}

	MRenderCmd::~MRenderCmd()
	{
		m_stop = true;
		m_signal.notify_one();
		m_thread.join();
	}

	_m_lpcwstr MRenderCmd::GetRenderName()
	{
		return m_base->GetRenderName();
	}

	bool MRenderCmd::InitRender(_m_uint width, _m_uint height)
	{
		bool ret = false;
		Task([&]
		{
			ret = m_base->InitRender(width, height);
		});
		return ret;
	}

	bool MRenderCmd::Resize(_m_uint width, _m_uint height)
	{
		bool ret = false;
		Task([&]
		{
			ret = m_base->Resize(width, height);
		});
		return ret;
	}

	MCanvasPtr MRenderCmd::CreateCanvas(_m_uint width, _m_uint height, _m_param param)
	{
		MCanvasPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateCanvas(width, height, param);
			if(ret) ret->m_base = this;
		});
		return ret;
	}

	MBitmapPtr MRenderCmd::CreateBitmap(std::wstring_view path, _m_param param)
	{
		MBitmapPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateBitmap(path, param);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MBitmapPtr MRenderCmd::CreateBitmap(UIResource resource, _m_param param)
	{
		MBitmapPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateBitmap(resource, param);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MBitmapPtr MRenderCmd::CreateBitmap(_m_uint width, _m_uint height, void* bit, _m_uint len, _m_uint stride)
	{
		MBitmapPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateBitmap(width, height, bit, len, stride);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MBitmapPtr MRenderCmd::CreateSVGBitmap(std::wstring_view path, _m_uint width, _m_uint height, bool repColor, _m_color color)
	{
		MBitmapPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateSVGBitmap(path, width, height, repColor, color);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MBitmapPtr MRenderCmd::CreateSVGBitmapFromXML(std::wstring_view xml, _m_uint width, _m_uint height, bool repColor,
		_m_color color)
	{
		MBitmapPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateSVGBitmapFromXML(xml, width, height, repColor, color);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MPenPtr MRenderCmd::CreatePen(_m_uint width, _m_color color)
	{
		MPenPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreatePen(width, color);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MBrushPtr MRenderCmd::CreateBrush(_m_color color)
	{
		MBrushPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateBrush(color);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MGradientBrushPtr MRenderCmd::CreateGradientBrush(const std::pair<_m_color, float>* vertex, _m_ushort count, UIPoint start, UIPoint end)
	{
		MGradientBrushPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateGradientBrush(vertex, count, start, end);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MFontPtr MRenderCmd::CreateFonts(std::wstring_view text, std::wstring_view fontName, _m_uint fontSize, _m_ptrv fontCollection)
	{
		MFontPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateFonts(text, fontName, fontSize, fontCollection);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MEffectPtr MRenderCmd::CreateEffects(MEffects::Types effect, float value)
	{
		MEffectPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateEffects(effect, value);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MGeometryPtr MRenderCmd::CreateRoundGeometry(_m_rect dest, float round)
	{
		MGeometryPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateRoundGeometry(dest, round);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MGeometryPtr MRenderCmd::CreateEllipseGeometry(_m_rect dest)
	{
		MGeometryPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateEllipseGeometry(dest);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MCanvasPtr MRenderCmd::CreateSubAtlasCanvas(_m_uint width, _m_uint height)
	{
		MCanvasPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateSubAtlasCanvas(width, height);
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	MBatchBitmapPtr MRenderCmd::CreateBatchBitmap()
	{
		MBatchBitmapPtr ret = nullptr;
		Task([&]
		{
			ret = m_base->CreateBatchBitmap();
			if (ret) ret->m_base = this;
		});
		return ret;
	}

	bool MRenderCmd::CopyBitmapContent(MBitmap* dst, MBitmap* src, UIPoint dstPt, _m_rect srcRect)
	{
		bool ret = false;
		Task([&]
		{
			ret = m_base->CopyBitmapContent(dst, src, dstPt, srcRect);
		});
		return ret;
	}

	bool MRenderCmd::CopyBitmapContent(MCanvas* dst, MCanvas* src, UIPoint dstPt, _m_rect srcRect)
	{
		bool ret = false;
		Task([&]
		{
			ret = m_base->CopyBitmapContent(dst, src, dstPt, srcRect);
		});
		return ret;
	}

	void MRenderCmd::BeginDraw()
	{
		if (m_begindraw)
			_M_OutErrorDbg_(L"status error", true);
		else
		{
			m_begindraw = true;
			Task([=]
			{
				m_base->BeginDraw();
			});
		}
	}

	void MRenderCmd::SetCanvas(MCanvasPtr canvas)
	{
		Task([&]
		{
			m_base->SetCanvas(canvas.get());
		});
	}

	void MRenderCmd::ResetCanvas()
	{
		Task([this]
		{
			m_base->ResetCanvas();
		});
	}

	void MRenderCmd::DrawBitmap(MBitmapPtr img, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality)
	{
		Task([&]
		{
			m_base->DrawBitmap(img.get(), alpha, dest, src, highQuality);
		});
	}

	void MRenderCmd::DrawBitmap(MCanvasPtr canvas, _m_byte alpha, _m_rect dest, _m_rect src, bool highQuality)
	{
		Task([&]
		{
			m_base->DrawBitmap(canvas.get(), alpha, dest, src, highQuality);
		});
	}

	void MRenderCmd::DrawBatchBitmap(MBatchBitmapPtr bmp, MBitmapPtr input, bool highQuality)
	{
		Task([&]
		{
			m_base->DrawBatchBitmap(bmp.get(), input.get(), highQuality);
		});
	}

	void MRenderCmd::DrawBatchBitmap(MBatchBitmapPtr bmp, MCanvasPtr input, bool highQuality)
	{
		Task([&]
		{
			m_base->DrawBatchBitmap(bmp.get(), input.get(), highQuality);
		});
	}

	void MRenderCmd::DrawNinePalacesImg(MBitmapPtr img, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin, bool highQuality)
	{
		Task([&]
		{
			m_base->DrawNinePalacesImg(img.get(), alpha, dest, src, margin, highQuality);
		});
	}

	void MRenderCmd::DrawNinePalacesImg(MCanvasPtr canvas, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin, bool highQuality)
	{
		Task([&]
		{
			m_base->DrawNinePalacesImg(canvas.get(), alpha, dest, src, margin, highQuality);
		});
	}

	void MRenderCmd::DrawRectangle(_m_rect dest, MPenPtr pen)
	{
		Task([&]
		{
			m_base->DrawRectangle(dest, pen.get());
		});
	}

	void MRenderCmd::DrawRoundedRect(_m_rect dest, float round, MPenPtr pen)
	{
		Task([&]
		{
			m_base->DrawRoundedRect(dest, round, pen.get());
		});
	}

	void MRenderCmd::FillRectangle(_m_rect dest, MBrushPtr brush)
	{
		Task([&]
		{
			m_base->FillRectangle(dest, brush.get());
		});
	}

	void MRenderCmd::FillRectangle(_m_rect dest, MGradientBrushPtr brush)
	{
		Task([&]
		{
			m_base->FillRectangle(dest, brush.get());
		});
	}

	void MRenderCmd::FillRoundedRect(_m_rect dest, float round, MBrushPtr brush)
	{
		Task([&]
		{
			m_base->FillRoundedRect(dest, round, brush.get());
		});
	}

	void MRenderCmd::DrawTextLayout(MFontPtr font, _m_rect dest, MBrushPtr brush, TextAlign alignment)
	{
		Task([&]
		{
			m_base->DrawTextLayout(font.get(), dest, brush.get(), alignment);
		});
	}

	void MRenderCmd::DrawBitmapEffects(MBitmapPtr img, MEffectPtr effect, _m_byte alpha, _m_rect dest, _m_rect src)
	{
		Task([&]
		{
			m_base->DrawBitmapEffects(img.get(), effect.get(), alpha, dest, src);
		});
	}

	void MRenderCmd::DrawBitmapEffects(MCanvasPtr canvas, MEffectPtr effect, _m_byte alpha, _m_rect dest, _m_rect src)
	{
		Task([&]
		{
			m_base->DrawBitmapEffects(canvas.get(), effect.get(), alpha, dest, src);
		});
	}

	void MRenderCmd::DrawLine(UIPoint x, UIPoint y, MPenPtr pen)
	{
		Task([&]
		{
			m_base->DrawLine(x, y, pen.get());
		});
	}

	void MRenderCmd::DrawEllipse(_m_rect dest, MPenPtr pen)
	{
		Task([&]
		{
			m_base->DrawEllipse(dest, pen.get());
		});
	}

	void MRenderCmd::FillEllipse(_m_rect dest, MBrushPtr brush)
	{
		Task([&]
		{
			m_base->FillEllipse(dest, brush.get());
		});
	}

	void MRenderCmd::PushClipRect(_m_rect rect)
	{
		Task([&]
		{
			m_base->PushClipRect(rect);
		});
	}

	void MRenderCmd::PopClipRect()
	{
		Task([this]
		{
			m_base->PopClipRect();
		});
	}

	void MRenderCmd::PushClipGeometry(MGeometryPtr geometry)
	{
		Task([&]
		{
			m_base->PushClipGeometry(geometry.get());
		});
	}

	void MRenderCmd::PopClipGeometry()
	{
		Task([&]
		{
			m_base->PopClipGeometry();
		});
	}

	void MRenderCmd::Clear(_m_color color)
	{
		Task([&]
		{
			m_base->Clear(color);
		});
	}

	_m_result MRenderCmd::EndDraw()
	{
		_m_result ret = 0;
		Task([&]
		{
			ret = m_base->EndDraw();
		});
		m_begindraw = false;
		return ret;
	}

	MCanvas* MRenderCmd::GetCanvas()
	{
		MCanvas* ret = nullptr;
		Task([&]
		{
			ret = m_base->GetCanvas();
		});
		return ret;
	}

	MCanvas* MRenderCmd::GetRenderCanvas()
	{
		MCanvas* ret = nullptr;
		Task([&]
		{
			ret = m_base->GetRenderCanvas();
		});
		return ret;
	}

	void* MRenderCmd::Get()
	{
		void* ret = nullptr;
		Task([&]
		{
			ret = m_base->Get();
		});
		return ret;
	}

	void MRenderCmd::Flush()
	{
		Task([&]
		{
			m_base->Flush();
		});
	}

	MCanvas* MRenderCmd::GetSharedCanvas()
	{
		MCanvas* ret = nullptr;
		Task([&]
		{
			ret = m_base->GetSharedCanvas();
		});
		return ret;
	}

	bool MRenderCmd::SaveMBitmap(MBitmapPtr bitmap, std::wstring path, MImgFormat format)
	{
		bool ret = false;
		Task([&]
		{
			ret = m_base->SaveMBitmap(bitmap.get(), path, format);
		});
		return ret;
	}

	bool MRenderCmd::SaveMCanvas(MCanvasPtr canvas, std::wstring path, MImgFormat format)
	{
		bool ret = false;
		Task([&]
		{
			ret = m_base->SaveMCanvas(canvas.get(), path, format);
		});
		return ret;
	}

	UIResource MRenderCmd::SaveMBitmap(MBitmapPtr bitmap, MImgFormat format)
	{
		UIResource ret;
		Task([&]
		{
			ret = m_base->SaveMBitmap(bitmap.get(), format);
		});
		return ret;
	}

	UIResource MRenderCmd::SaveMCanvas(MCanvasPtr canvas, MImgFormat format)
	{
		UIResource ret;
		Task([&]
		{
			ret = m_base->SaveMCanvas(canvas.get(), format);
		});
		return ret;
	}

	bool MRenderCmd::CheckSubAtlasSource(MCanvasPtr canvas, MCanvasPtr canvas1)
	{
		bool ret = false;
		Task([&]
		{
			ret = m_base->CheckSubAtlasSource(canvas.get(), canvas1.get());
		});
		return ret;
	}

	void MRenderCmd::ThreadProc()
	{
		while (!m_stop)
		{
			std::unique_lock lock(m_tasklock);
			while (m_taskList.empty() && !m_stop)
				m_signal.wait(lock);

			for (auto& task_item : m_taskList)
			{
				//传递异常到目标等待线程
				try
				{
					task_item.task();
					task_item.notification.set_value();
				}
				catch(...)
				{
					task_item.notification.set_exception(std::current_exception());
				}
			}
			m_taskList.clear();
		}
	}

	void MRenderCmd::Task(std::function<void()>&& task)
	{
		if (IsTaskThread())
		{
			task();
			return;
		}
		std::exception_ptr ex = nullptr;
		std::promise<void> send_notification;
		auto wait_notification = send_notification.get_future();
		{
			std::unique_lock lock(m_tasklock);
			m_taskList.emplace_back(task, send_notification);
		}
		m_signal.notify_one();
		wait_notification.get();
	}

	bool MRenderCmd::IsTaskThread()
	{
		return m_thread.get_id() == std::this_thread::get_id();
	}
}
