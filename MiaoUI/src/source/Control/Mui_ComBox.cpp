/**
 * FileName: Mui_Combox.cpp
 * Note: UI组合框控件实现
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
 * date: 2021-4-2 Create
*/

#include <Control/Mui_ComBox.h>

namespace Mui::Ctrl
{
	UIComBox::UIComBox(UIControl* parent, Attribute attrib, UIListBox::Attribute listAttrib,
		UIScroll::Attribute scrollAttrib, UILabel::Attribute fontStyle)
	: UIComBox(attrib, std::move(listAttrib), std::move(scrollAttrib), std::move(fontStyle))
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
		m_root = GetWindowTopCtrl();
		m_root->AddChildren(m_popList);

		m_attrib.SetAttribute(L"listShadowBlur", attrib.listShadowBlur, this);
		m_attrib.SetAttribute(L"listShadowColor", attrib.listShadowColor, this);
		m_attrib.SetAttribute(L"listShadowOffset", attrib.listShadowOffset, this);
		m_attrib.SetAttribute(L"listShadowExtend", attrib.listShadowExtend, this);
	}

	UIComBox::UIComBox(Attribute attrib, UIListBox::Attribute listAttrib, UIScroll::Attribute scrollAttrib,
		UILabel::Attribute fontStyle)
	: UILabel(std::move(fontStyle)), m_attrib(std::move(attrib))
	{
		m_popList = new UIListBoxCom(std::move(listAttrib), std::move(scrollAttrib));
		m_popList->SetVisible(false);
		m_popList->AutoSize(false, false);
		m_popList->combox = this;
	}

	UIComBox::~UIComBox()
	{
		if (m_popList && m_root)
		{
			m_root->RemoveChildren(m_popList);
			delete m_popList;
		}
	}

	void UIComBox::Register()
	{
		BindAttribute();

		static auto method = [](UIControl* parent)
		{
			return new UIComBox(parent, Attribute());
		};
		MCTRL_REGISTER(method);
	}

	void UIComBox::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (attribName == L"listStyle")
			m_popList->SetAttribute(L"style", attrib, draw);
		else if (m_attrib.SetAttribute(attribName, attrib, this))
		{
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		else if (IsListAttrib(attribName))
			m_popList->SetAttribute(attribName, attrib, draw);
		else
			UILabel::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UIComBox::GetAttribute(std::wstring_view attribName)
	{
		if (attribName == L"listStyle")
			return m_popList->GetAttribute(L"style");

		std::wstring dst;
		if (!m_attrib.GetAttribute(attribName, dst) && IsListAttrib(attribName))
			return m_popList->GetAttribute(attribName);
		return UILabel::GetAttribute(attribName);
	}

	const UIComBox::Attribute& UIComBox::GetAttribute() const
	{
		return m_attrib.Get();
	}

	int UIComBox::AddItem(ListItem* item, int index, bool draw)
	{
		return m_popList->AddItem(item, index, draw);
	}

	ListItem* UIComBox::GetItem(int index) const
	{
		return m_popList->GetItem(index);
	}

	bool UIComBox::SetCurSelItem(int index, bool draw)
	{
		const bool ret = m_popList->SetCurSelItem(index, draw);
		if (ret) 
		{
			index = m_popList->GetCurSelItem();
			std::wstring title;
			if (index != -1)
				title = m_popList->GetItem(index)->GetText();
			UILabel::SetAttribute(L"text", title, draw);
		}
		return ret;
	}

	int UIComBox::GetCurSelItem() const
	{
		return m_popList->GetCurSelItem();
	}

	int UIComBox::GetItemListCount() const
	{
		return m_popList->GetItemListCount();
	}

	int UIComBox::DeleteItem(int index, bool draw)
	{
		return m_popList->DeleteItem(index, false, draw);
	}

	int UIComBox::DeleteItem(int index, bool delItem, bool draw)
	{
		return m_popList->DeleteItem(index, delItem, draw);
	}

	void UIComBox::DeleteAllItem(bool delItem, bool draw)
	{
		m_popList->DeleteAllItem(delItem, draw);
	}

	void UIComBox::OnPaintProc(MPCPaintParam param)
	{
		auto& attrib = m_attrib.Get();
		auto scale = GetRectScale().scale();
		if (attrib.style)
		{
			attrib.style->PaintStyle(param->render, param->destRect, param->blendedAlpha,
				IsEnabled() ? m_state : UIControlStatus_Disable, 4, scale);
		}
		UILabel::OnPaintProc(param);
		//图标绘制
		if (attrib.dropIcon)
		{
			UIRect dragIcoPos = *param->destRect;
			int frameHeight = dragIcoPos.GetHeight();
			if (!attrib.dropIconAutoPos)
			{
				int xOffset = _scale_to(attrib.dropIconXPos, scale.cx);
				dragIcoPos.left = xOffset;
				dragIcoPos.right = dragIcoPos.left + frameHeight;
			}
			else
			{
				dragIcoPos.left = dragIcoPos.right - frameHeight;
			}
			param->render->DrawBitmap(attrib.dropIcon->GetBitmap(), param->blendedAlpha, dragIcoPos);
		}
	}

	bool UIComBox::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		bool update = true;
		switch (message)
		{
		case M_MOUSE_HOVER:
			m_state = UIControlStatus_Hover;
			m_isHover = true;
			break;
		case M_MOUSE_LEAVE:
			m_state = UIControlStatus_Normal;
			m_isHover = false;
			break;
		case M_MOUSE_LBDOWN:
			m_state = UIControlStatus_Pressed;
			m_isClick = true;
			break;
		case M_MOUSE_LBUP:
			if (m_isClick)
			{
				ShowMenu(!m_popList->IsVisible());
				m_isClick = false;
			}
			m_state = UIControlStatus_Normal;
			break;
		default:
			update = false;
			break;
		}
		if (update)
		{
			m_cacheUpdate = true;
			UpdateDisplay();
		}
		return UILabel::OnMouseMessage(message, wParam, lParam);
	}

	bool UIComBox::OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam)
	{
		//如果焦点为弹出的列表框 则不发送失去焦点消息
		if (code == M_WND_KILLFOCUS && reinterpret_cast<UIListBoxCom*>(wParam) == m_popList)
			return true;
		return UIControl::OnWindowMessage(code, wParam, lParam);
	}

	void UIComBox::OnScale(_m_scale scale)
	{
		UILabel::OnScale(scale);
		m_popList->OnScale(scale);
		CalcMenuFrame();
	}

	void UIComBox::OnLayoutCalced()
	{
		CalcMenuFrame();
	}

	void UIComBox::ShowMenu(bool show)
	{
		m_popList->SetFocus(true);
		if (show)
		{
			CalcMenuFrame();
			m_popList->UpdateLayout();
		}

		m_popList->SetVisible(show, true);
	}

	void UIComBox::CalcMenuFrame()
	{
		auto& attrib = m_attrib.Get();
		int height = _scale_to(attrib.menuHeight, UINodeBase::m_data.RectScale.hs);

		UIRect rect;
		const auto& frame = Frame();
		rect.left = frame.left;

		m_popList->UINodeBase::m_data.EnableDPIScale = UINodeBase::m_data.EnableDPIScale;
		m_popList->SetScale({ 1.f, 1.f, UINodeBase::m_data.RectScale.ws, UINodeBase::m_data.RectScale.hs }, false);

		_m_scale scale = { 1.f, 1.f };
		if (UINodeBase::m_data.EnableDPIScale)
		{
			scale = UINodeBase::m_data.DPIScale;
		}
		rect.left = int((float)rect.left / (scale.cx / 1.f));

		if (attrib.popTop)
		{
			rect.top = frame.top - _scale_to(height, scale.cy);
		}
		else
			rect.top = frame.bottom;

		rect.top = int((float)rect.top / (scale.cy / 1.f));
		m_popList->SetPos(rect.left, rect.top, true);
		int width = int((float)frame.GetWidth() / (scale.cx / 1.f));
		m_popList->SetSize(width, height, false);
		//m_popList->UpdateLayout();
		m_popList->OnLayoutCalced();
	}

	void UIComBox::SetText(std::wstring_view text)
	{
		UILabel::SetAttribute(L"text", text);
	}

	bool UIComBox::IsListAttrib(std::wstring_view name)
	{
		if (name == L"itemStyle"
			|| name == L"itemHeight"
			|| name == L"lineSpace"
			|| name == L"iFontStyle"
			|| name == L"iFont"
			|| name == L"iFontSize"
			|| name == L"iFontBold"
			|| name == L"iFontItalics"
			|| name == L"iFontUnderline"
			|| name == L"iFontStrikeout"
			|| name == L"iFontColor"
			|| name == L"iTextAlign"
			|| name == L"iFontCustom"
			|| name == L"drawOffset"
			|| name == L"styleV"
			|| name == L"animate"
			|| name == L"button"
			|| name == L"dragValueV"
			|| name == L"barWidth"
			|| name == L"barMinHeight"
			|| name == L"btnHeight"
			|| name == L"inset")
			return true;
		return false;
	}

	bool UIComBox::UIListBoxCom::OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam)
	{
		if (code == M_WND_KILLFOCUS && !combox->m_isHover)
			combox->ShowMenu(false);
		return UIControl::OnWindowMessage(code, wParam, lParam);
	}

	bool UIComBox::UIListBoxCom::SendEvent(UINotifyEvent event, _m_param param)
	{
		if (event == Event_ListBox_ItemChanged
			|| event == Event_ListBox_ItemLClick
			|| event == Event_ListBox_ItemLDBClick) 
		{
			if (event == Event_ListBox_ItemChanged)
			{
				combox->SetText(this->GetItem((int)param)->GetText());
				combox->mslot.itemChanged.Emit((int)param);
			}
			else if (event == Event_ListBox_ItemLClick)
				combox->mslot.itemClicked.Emit((int)param);
			else if (event == Event_ListBox_ItemLDBClick)
				combox->mslot.itemDBClicked.Emit((int)param);
			combox->ShowMenu(false);
		}
		else if(event == Event_ListBox_ItemChanging)
		{
			combox->mslot.itemChanging.Emit(m_curSelIndex, std::ref(*(bool*)param));
		}
		//将列表框消息转到组合框控件发送
		return combox->SendEvent(event, param);
	}

	void UIComBox::BindAttribute()
	{
		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::style, L"style"),
			MakeUIAttrib(m_attrib, &Attribute::dropIcon, L"dropIcon"),
			MakeUIAttrib(m_attrib, &Attribute::dropIconXPos, L"dropIconXPos"),
			MakeUIAttrib(m_attrib, &Attribute::dropIconAutoPos, L"dropIconAutoPos"),
			MakeUIAttrib(m_attrib, &Attribute::popTop, L"popTop"),
			MakeUIAttrib(m_attrib, &Attribute::menuHeight, L"menuHeight"),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::listShadowBlur, L"listShadowBlur", param,
			{
				param.param->m_popList->SetAttribute(L"shadowBlur", param.attribValue, false);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::listShadowColor, L"listShadowColor", param,
			{
				param.param->m_popList->SetAttribute(L"shadowColor", param.attribValue, false);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::listShadowOffset, L"listShadowOffset", param,
			{
				param.param->m_popList->SetAttribute(L"shadowOffset", param.attribValue, false);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::listShadowExtend, L"listShadowExtend", param,
			{
				param.param->m_popList->SetAttribute(L"shadowExtend", param.attribValue, false);
				return true;
			})
		};

		decltype(m_attrib)::RegisterAttrib(list);
	}
}
