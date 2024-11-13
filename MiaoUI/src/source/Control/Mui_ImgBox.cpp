/**
 * FileName: Mui_ImgBox.cpp
 * Note: UI图片框控件实现
 *
 * Copyright (C) 2020-2023 Maplespe (mapleshr@icloud.com)
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

#include <Control/Mui_ImgBox.h>

#include <utility>

namespace Mui::Ctrl
{
	UIImgBox::UIImgBox(UIControl* parent) : UIImgBox()
	{
		M_ASSERT(parent)
		UINodeBase::m_data.AutoSize = true;
		parent->AddChildren(this);
	}

	void UIImgBox::Register()
	{
		static auto method = [](UIControl* parent)
		{
			return new UIImgBox(parent);
		};
		MCTRL_REGISTER(method);
	}

	UISize UIImgBox::GetImageSize() const
	{
		UISize size;
		if (m_image)
			size = m_image->GetBitmap()->GetSize();
		return size;
	}

	void UIImgBox::SetImage(UIBitmapPtr img, bool lowQuality, bool draw)
	{
		m_isLowQuality = lowQuality;
		m_image = std::move(img);

		if (draw)
			UpdateDisplay(true);
		else
			m_cacheUpdate = true;
	}

	void UIImgBox::SetImageStyle(ImgStyle style, bool draw)
	{
		m_style = style;
		if (draw)
			UpdateDisplay(true);
		else
			m_cacheUpdate = true;
	}

	void UIImgBox::SetQuality(bool lowQuality, bool draw)
	{
		m_isLowQuality = lowQuality;
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	void UIImgBox::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (attribName == L"img")
		{
			UIBitmapPtr img = nullptr;
			if (auto pImg = (UIBitmap*)Helper::M_StoLong64(attrib))
				img = pImg;
			SetImage(img, false, false);
		}
		else if (attribName == L"imgStyle")
		{
			ImgStyle imgStyle = (ImgStyle)Helper::M_StoInt(attrib);
			SetImageStyle(imgStyle, false);
		}
		else if(attribName == L"lowQuality")
		{
			m_isLowQuality = attrib == L"true";
		}
		else 
		{
			UIControl::SetAttribute(attribName, attrib, draw);
			return;
		}
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	std::wstring UIImgBox::GetAttribute(std::wstring_view attribName)
	{
		if (attribName == L"img")
			return std::to_wstring((_m_param)m_image.get());
		if (attribName == L"imgStyle")
			return std::to_wstring(m_style);
		if (attribName == L"lowQuality")
			return m_isLowQuality ? L"true" : L"false";
		return UIControl::GetAttribute(attribName);
	}

	//取等比
	inline UISize _GetScale(int size1, int size2, int newSize1)
	{
		float newSize[2] = { (float)size1,(float)size2 };

		newSize[1] = (float)newSize1;
		newSize[0] = newSize[1] / size1 * size2;
		return { (int)newSize[0], (int)newSize[1] };
	}

	void UIImgBox::OnPaintProc(MPCPaintParam param)
	{
		if (!m_image)
			return;

		auto image = m_image->GetBitmap();

		UIRect m_DrawRect = *param->destRect;
		const UISize m_Size = image->GetSize();
		const UIRect m_DrawSrc = { 0, 0, m_Size.width, m_Size.height };

		switch (m_style)
		{
		case ImageBoxStyle_Original:
			m_DrawRect.right = m_DrawRect.left + m_Size.width;
			m_DrawRect.bottom = m_DrawRect.top + m_Size.height;
			break;
		case ImageBoxStyle_Center:
		{
			//如果图片大于图片框区域 等比例缩放
			UISize imgSize_new;

			int DrawRcHeight = m_DrawRect.GetHeight();
			int DrawRcWidth = m_DrawRect.GetWidth();

			//默认按图片框宽度计算
			UISize newSize = _GetScale(m_DrawSrc.right, m_DrawSrc.bottom, DrawRcWidth);
			//计算出来的宽度和图片框一样了 但是高度超出了 就按高度来算
			if (newSize.width > DrawRcHeight) 
			{
				newSize = _GetScale(m_DrawSrc.bottom, m_DrawSrc.right, DrawRcHeight);
				imgSize_new = newSize;
			}
			else //如果按宽度算 结果是 高 宽 把它改成 宽 高
				imgSize_new = { newSize.height, newSize.width };

			//计算居中位置
			const int pos[2] = { (DrawRcWidth - imgSize_new.width) / 2,(DrawRcHeight - imgSize_new.height) / 2 };

			m_DrawRect = { 0, 0, imgSize_new.width, imgSize_new.height };
			m_DrawRect.Offset(param->destRect->left + pos[0], param->destRect->top + pos[1]);
		}
		break;
		case ImageBoxStyle_ZoomFill:
		{
			//按高等比例拉伸
			int DrawRcHeight = m_DrawRect.GetHeight();
			int DrawRcWidth = m_DrawRect.GetWidth();
			UIPoint pos;

			int newWidth = Helper::M_CalcAspectRatio(m_Size.width, m_Size.height, DrawRcHeight, false);
			int newHeight = DrawRcHeight;

			pos.x = newWidth - DrawRcWidth;
			pos.x /= 2;//居中
			if (pos.x != 0) pos.x = -pos.x;
			pos.y = 0;

			//按高不足以填充宽 按宽
			if (newWidth < DrawRcWidth)
			{
				newWidth = DrawRcWidth;
				newHeight = Helper::M_CalcAspectRatio(m_Size.width, m_Size.height, DrawRcWidth, true);
				pos.x = 0;
				pos.y = newHeight - DrawRcHeight;
				pos.y /= 2;//居中
				if (pos.y != 0) pos.y = -pos.y;
			}
			m_DrawRect.left = m_DrawRect.left + pos.x;
			m_DrawRect.top = m_DrawRect.top + pos.y;
			m_DrawRect.right = m_DrawRect.left + newWidth;
			m_DrawRect.bottom = m_DrawRect.top + newHeight;
		}
		break;
		}
		param->render->DrawBitmap(image, param->blendedAlpha, m_DrawRect, m_DrawSrc, !m_isLowQuality);
	}

	_m_sizef UIImgBox::GetContentSize()
	{
		auto size = UINodeBase::GetContentSize();
		if (UINodeBase::m_data.AutoSize && m_image)
			size = { (float)GetImageSize().width, (float)GetImageSize().height };
		return size;
	}
}
