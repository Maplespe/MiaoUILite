/**
 * FileName: Mui_CheckBox.cpp
 * Note: UI选择框控件实现
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
 * date: 2020-11-26 Create
*/

#include <Control/Mui_CheckBox.h>

namespace Mui::Ctrl
{
	using namespace Helper;

	UICheckBox::UICheckBox(UIControl* parent, Attribute attrib, UILabel::Attribute fontStyle)
	: UICheckBox(std::move(attrib), std::move(fontStyle))
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
	}

	UICheckBox::UICheckBox(Attribute attrib, UILabel::Attribute fontStyle)
	: UILabel(std::move(fontStyle)), m_attrib(std::move(attrib))
	{
		UILabel::OffsetDraw = true;
	}

	void UICheckBox::Register()
	{
		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::style, L"style"),
			MakeUIAttrib(m_attrib, &Attribute::isSel, L"isSel"),
			MakeUIAttrib(m_attrib, &Attribute::allowClick, L"allowClick"),
			MakeUIAttrib(m_attrib, &Attribute::animate, L"animate"),
			MakeUIAttrib(m_attrib, &Attribute::aniAlphaType, L"aniAlphaType"),
			MakeUIAttrib(m_attrib, &Attribute::textOffset, L"textOffset")
		};
		decltype(m_attrib)::RegisterAttrib(list);

		static auto method = [](UIControl* parent)
		{
			return new UICheckBox(parent, Attribute());
		};
		MCTRL_REGISTER(method);
	}

	void UICheckBox::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (m_attrib.SetAttribute(attribName, attrib, draw))
		{
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		else
			UILabel::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UICheckBox::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret))
			return ret;
		return UILabel::GetAttribute(attribName);
	}

	const UICheckBox::Attribute& UICheckBox::GetAttribute() const
	{
		return m_attrib.Get();
	}

	void UICheckBox::SetSel(bool sel, bool draw)
	{
		m_attrib.SetAttribute(L"isSel", sel, false);

		switch (m_state)
		{
		case UICheckBoxHoverSel:
			if (!sel) m_state = UICheckBoxHover;
			break;
		case UICheckBoxHover:
			if (sel) m_state = UICheckBoxHoverSel;
			break;
		case UICheckBoxNormalSel:
			if (!sel) m_state = UICheckBoxNormal;
			break;
		case UICheckBoxNormal:
			if (sel) m_state = UICheckBoxNormalSel;
			break;
		case UICheckBoxDisbaleSel:
			if (!sel) m_state = UICheckBoxDisbale;
			break;
		case UICheckBoxDisbale:
			if (sel) m_state = UICheckBoxDisbaleSel;
			break;
		}

		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	bool UICheckBox::GetSel() const
	{
		return m_attrib.Get().isSel;
	}

	void UICheckBox::OnPaintProc(MPCPaintParam param)
	{
		UIRect frame = *param->destRect;

		auto& attrib = m_attrib.Get();

		auto scale = GetRectScale().scale();
		int offset = attrib.textOffset;
		offset = _scale_to(offset, scale.cx);

		int height = frame.GetHeight();
		if (UINodeBase::m_data.AutoSize)
			frame.right += height + offset;
		OffsetDrawRc = { frame.left + height + offset, frame.top, frame.right, frame.bottom };

		Status tmpStatus = m_state;
		if (tmpStatus == UICheckBoxNormal && attrib.isSel)
			tmpStatus = UICheckBoxNormalSel;

		if (!IsEnabled())
		{
			if (attrib.isSel)
				tmpStatus = UICheckBoxDisbaleSel;
			else
				tmpStatus = UICheckBoxDisbale;
		}

		if (attrib.style)
		{
			UIRect dest = *param->destRect;
			dest.right = dest.left + height;
			attrib.style->PaintStyle(param->render, &dest, param->blendedAlpha, tmpStatus, 8, scale);
		}
		UILabel::OnPaintProc(param);
	}

	bool UICheckBox::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		auto& attrib = m_attrib.Get();
		switch ((_m_msg)message)
		{
		case M_MOUSE_HOVER:
			ChangeStatus(attrib.isSel ? UICheckBoxHoverSel : UICheckBoxHover,
			             attrib.isSel ? UICheckBoxNormalSel : UICheckBoxNormal);
			break;
		case M_MOUSE_LEAVE:
			ChangeStatus(attrib.isSel ? UICheckBoxNormalSel : UICheckBoxNormal,
			             attrib.isSel ? UICheckBoxHoverSel : UICheckBoxHover);
			break;
		case M_MOUSE_LBDOWN:
			m_isClick = true;
			ChangeStatus(attrib.isSel ? UICheckBoxPressedSel : UICheckBoxPressed,
			             attrib.isSel ? UICheckBoxHoverSel : UICheckBoxHover);
			break;
		case M_MOUSE_LBUP:
			if (m_isClick && attrib.allowClick)
			{
				m_attrib.SetAttribute(L"isSel", !attrib.isSel, false);
			}
			ChangeStatus(attrib.isSel ? UICheckBoxHoverSel : UICheckBoxHover,
			             attrib.isSel ? UICheckBoxPressedSel : UICheckBoxPressed);
			break;
		}
		return UILabel::OnMouseMessage(message, wParam, lParam);
	}

	_m_sizef UICheckBox::GetContentSize()
	{
		_m_sizef size = UINodeBase::GetContentSize();
		if (UINodeBase::m_data.AutoSize)
		{
			//加上选择区域和文本距离
			int offset = m_attrib.Get().textOffset;
			SizeScale(offset, true, true);

			const UISize textSize = CalcSize();
			size = { (float)(textSize.width + textSize.height + offset), (float)textSize.height };

			size.width = M_MAX(size.width, (float)textSize.width);
			size.height = M_MAX(size.height, (float)textSize.height);
		}
		return size;
	}

	void UICheckBox::ChangeStatus(Status&& state, Status&& oldState)
	{
		m_state = state;
		m_cacheUpdate = true;
		UpdateDisplay();
	}
}
