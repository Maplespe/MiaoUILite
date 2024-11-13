/**
 * FileName: Mui_Button.cpp
 * Note: UI按钮控件实现
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
 * date: 2020-10-19 Create
*/

#include <Control/Mui_Button.h>

namespace Mui::Ctrl
{
	UIButton::UIButton(UIControl* parent, Attribute attrib, UILabel::Attribute fontStyle)
	: UIButton(std::move(attrib), std::move(fontStyle))
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
	}

	UIButton::UIButton(Attribute attrib, UILabel::Attribute fontStyle)
	: UILabel(std::move(fontStyle)), m_attrib(std::move(attrib))
	{
		UILabel::OffsetDraw = true;
	}

	void UIButton::Register()
	{
		using namespace CtrlMgr;
		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::style, L"style"),
			MakeUIAttrib(m_attrib, &Attribute::animate, L"animate"),
			MakeUIAttrib(m_attrib, &Attribute::aniAlphaType, L"aniAlphaType"),
			MakeUIAttrib(m_attrib, &Attribute::inset, L"inset")
		};
		decltype(m_attrib)::RegisterAttrib(list);

		static auto method = [](UIControl* parent)
		{
			return new UIButton(parent, Attribute());
		};
		MCTRL_REGISTER(method);
	}

	void UIButton::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (m_attrib.SetAttribute(attribName, attrib, draw))
		{
			//属性已更改 更新缓存
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		else
			UILabel::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UIButton::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret))
			return ret;
		return UILabel::GetAttribute(attribName);
	}

	const UIButton::Attribute& UIButton::GetAttribute() const
	{
		return m_attrib.Get();
	}

	void UIButton::OnPaintProc(MPCPaintParam param)
	{
		auto& attrib = m_attrib.Get();
		auto scale = GetRectScale().scale();
		if (attrib.style) 
		{
			attrib.style->PaintStyle(param->render, param->destRect,
					param->blendedAlpha,
					IsEnabled() ? m_state : UIControlStatus_Disable, 4, scale);
		}
		UIRect frame = *param->destRect;
		const auto& inset = attrib.inset;
		if(!(UILabel::GetAttribute().textAlign & TextAlign_Center))
			frame.left += _scale_to(inset.left, scale.cx);
		if (!(UILabel::GetAttribute().textAlign & TextAlign_VCenter))
			frame.top += _scale_to(inset.top, scale.cy);
		//frame.right -= _scale_to(inset.right, scale.cx);
		//frame.bottom -= _scale_to(inset.bottom, scale.cy);
		OffsetDrawRc = frame.ToRect();
		UILabel::OnPaintProc(param);
	}

	bool UIButton::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		switch (message)
		{
		case M_MOUSE_HOVER:
			ChangeStatus(UIControlStatus_Hover, UIControlStatus_Normal);
			break;
		case M_MOUSE_LEAVE:
			ChangeStatus(UIControlStatus_Normal, UIControlStatus_Hover);
			break;
		case M_MOUSE_LBDOWN:
			ChangeStatus(UIControlStatus_Pressed, UIControlStatus_Hover);
			break;
		case M_MOUSE_LBUP:
			ChangeStatus(UIControlStatus_Hover, UIControlStatus_Pressed);
			break;
		}
		return UILabel::OnMouseMessage(message, wParam, lParam);
	}

	void UIButton::ChangeStatus(UIControlStatus&& state, UIControlStatus&& oldState)
	{
		m_state = state;
		m_cacheUpdate = true;
		UpdateDisplay();
	}

	_m_sizef UIButton::GetContentSize()
	{
		_m_sizef size = UILabel::GetContentSize();
		if (UINodeBase::m_data.AutoSize)
		{
			auto scale = GetRectScale().scale();
			const auto& inset = m_attrib.Get().inset;
			size.width += _scale_to((float)(inset.left + inset.right), scale.cx);
			size.height += _scale_to((float)(inset.top + inset.bottom), scale.cy);
		}
		return size;
	}
}