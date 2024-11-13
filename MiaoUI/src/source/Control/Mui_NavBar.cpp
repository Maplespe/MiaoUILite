/**
 * FileName: Mui_NavBar.cpp
 * Note: UI导航栏声明
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
 * date: 2022-9-7 Create
*/
#include <Control/Mui_NavBar.h>

namespace Mui::Ctrl
{
	using namespace Helper;

	void UINavBar::Register()
	{
		BindAttribute();

		static auto method = [](UIControl* parent)
			{
				return new UINavBar(parent, Attribute());
			};
		MCTRL_REGISTER(method);
	}

	UINavBar::UINavBar(UIControl* parent, Attribute attrib) : UINavBar(std::move(attrib))
	{
		M_ASSERT(parent)
			parent->AddChildren(this);
	}

	UINavBar::UINavBar(Attribute attrib) : m_attrib(std::move(attrib))
	{
		m_cacheSupport = true;
		UINodeBase::m_data.AutoSize = true;
	}

	void UINavBar::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (!m_attrib.SetAttribute(attribName, attrib, { this, draw }))
			UIControl::SetAttribute(attribName, attrib, draw);
		else
			m_cacheUpdate = true;
	}

	std::wstring UINavBar::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret))
			return ret;
		return UIControl::GetAttribute(attribName);
	}

	void UINavBar::SetFontStyle(UILabel::Attribute& attrib, bool draw)
	{
		m_attrib.SetAttribute(L"fontStyle", &attrib, { this, draw });
	}

	int UINavBar::AddItem(std::wstring_view title, int index, bool draw)
	{
		if (index == -1)
		{
			m_itemList.push_back(InitItemRect(title, index));
			if (draw)
				UpdateDisplay();
			m_cacheUpdate = true;
		}
		else if (IndexCheck(index))
		{
			m_font->SetText(title.data());
			m_itemList.insert(m_itemList.begin() + index, InitItemRect(title, index));
			//列表发生改变 需要更新当前选中项目的索引
			if (index < m_curSelIndex)
				m_curSelIndex++;
			if (draw)
				UpdateDisplay();
			m_cacheUpdate = true;
		}
		else
		{
			_M_OutErrorDbg_(L"AddItem failed!", false);
		}
		return (int)m_itemList.size();
	}

	void UINavBar::SetItemTitle(int index, std::wstring_view title, bool draw)
	{
		if (auto _size = m_itemList.size(); _size != 0)
		{
			if (index == -1)
				index = int(_size - 1);
			if (IndexCheck(index))
			{
				m_itemList[index] = std::make_pair((std::wstring)title, UIRect());
				CalcItemRect();
				m_cacheUpdate = true;
				if (draw)
					UpdateDisplay();
			}
		}
	}

	std::wstring UINavBar::GetItemTitle(int index) const
	{
		if (index == -1 && !m_itemList.empty())
		{
			return m_itemList.back().first;
		}
		if (IndexCheck(index))
		{
			return m_itemList[index].first;
		}
		return L"";
	}

	bool UINavBar::SetCurSelItem(int index, bool draw)
	{
		if (IndexCheck(index))
		{
			m_curSelIndex = index;
			auto& item = m_itemList[index];
			m_baroffset = item.second.left;
			m_baroffset = item.second.GetWidth();
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
			return true;
		}
		return false;
	}

	int UINavBar::GetCurSelItem() const
	{
		return m_curSelIndex;
	}

	int UINavBar::GetItemListCount() const
	{
		return (int)m_itemList.size();
	}

	int UINavBar::DeleteItem(int index, bool draw)
	{
		if (!m_itemList.empty())
		{
			if (index == -1)
			{
				if (int(m_itemList.size() - 1) == m_curSelIndex)
					m_curSelIndex = 0;
				m_itemList.pop_back();
			}
			else if (IndexCheck(index))
			{
				if (index == m_curSelIndex)
					m_curSelIndex = 0;
				m_itemList.erase(m_itemList.begin() + index);
				CalcItemRect();
			}
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		return (int)m_itemList.size();
	}

	void UINavBar::DeleteAllItem(bool draw)
	{
		if (!m_itemList.empty())
		{
			m_curSelIndex = 0;
			m_itemList.clear();
			m_cacheUpdate = true;
			if (draw) UpdateDisplay();
		}
	}

	void UINavBar::OnLoadResource(MRenderCmd* render, bool recreate)
	{
		UINodeBase::OnLoadResource(render, recreate);

		auto& attrib = m_attrib.Get();

		m_effect = nullptr;

		auto scale = GetRectScale().scale();
		const float fontSize = M_MIN(scale.cx, scale.cy) * (float)attrib.fontSize;

		m_font = render->CreateFonts(L"", attrib.font.view(), (_m_uint)fontSize, attrib.fontCustom);
		m_brush = render->CreateBrush(attrib.fontColor);
		m_font->SetFontStyle(attrib.fontStyle, std::make_pair(0u, 0u));
	}

	void UINavBar::OnPaintProc(MPCPaintParam param)
	{
		auto& attrib = m_attrib.Get();
		//尺寸更改 重新计算
		if (m_size != UINodeBase::m_data.Size)
		{
			m_size = UINodeBase::m_data.Size;
			CalcItemRect();
		}
		if (attrib.shadowUse && !attrib.shadowLow)
		{
			if (!m_effect)
				m_effect = m_render->CreateEffects(MEffects::GaussianBlur, attrib.shadowBlur);
			else
				m_effect->SetEffectValue(attrib.shadowBlur);
		}

		m_brush->SetOpacity(param->cacheCanvas ? 255 : UINodeBase::m_data.AlphaDst);
		int index = 0;
		for (auto& item : m_itemList)
		{
			m_font->SetText(item.first);
			UIRect dst = item.second;
			dst.Offset(param->destRect->left, param->destRect->top);

			_m_scale scale = GetRectScale().scale();

			//绘制字体阴影
			if (attrib.shadowUse)
			{
				const UIPoint offset{ _scale_to(attrib.shadowOffset.x, scale.cx), _scale_to(attrib.shadowOffset.y, scale.cy) };
				m_brush->SetColor(attrib.shadowColor);
				UIRect offsetrc = dst;
				offsetrc.Offset(offset.x, offset.y);
				param->render->DrawTextLayout(m_font, offsetrc, m_brush, TextAlign_Left);
			}

			//项目标题状态
			if (m_curHoverIndex == index && m_state != UIControlStatus_Normal)
			{
				if (m_state == UIControlStatus_Hover)
					m_brush->SetColor(attrib.fontHoverColor);
				else if (m_state == UIControlStatus_Pressed)
					m_brush->SetColor(attrib.fontPressColor);
				else
					m_brush->SetColor(attrib.fontColor);
			}
			else if (m_curSelIndex == index)
				m_brush->SetColor(attrib.fontHoverColor);
			else
				m_brush->SetColor(attrib.fontColor);

			param->render->DrawTextLayout(m_font, dst, m_brush, TextAlign_Left);

			//绘制横条
			if (m_curSelIndex == index && attrib.barHeight > 0)
			{
				dst = UIRect(param->destRect->left + m_baroffset, dst.bottom + _scale_to(attrib.barSpace, scale.cy),
					m_barwidth, _scale_to(attrib.barHeight, scale.cy));

				m_brush->SetColor(attrib.barColor);

				if (attrib.barRound != 0.f)
					param->render->FillRoundedRect(dst, M_MAX(scale.cx, scale.cy) * (float)attrib.barRound, m_brush);
				else
					param->render->FillRectangle(dst, m_brush);
			}

			index++;
		}
	}

	bool UINavBar::OnMouseEntered(_m_uint flag, const UIPoint& point)
	{
		if (IsHitItem(point))
		{
			m_state = UIControlStatus_Hover;
			m_cacheUpdate = true;
			UpdateDisplay();
		}
		return UIControl::OnMouseEntered(flag, point);
	}

	bool UINavBar::OnMouseExited(_m_uint flag, const UIPoint& point)
	{
		if (m_curHoverIndex != -1)
		{
			m_curHoverIndex = -1;
			m_state = UIControlStatus_Normal;
			m_cacheUpdate = true;
			m_down = false;
			UpdateDisplay();
		}
		return UIControl::OnMouseExited(flag, point);
	}

	bool UINavBar::OnLButtonDown(_m_uint flag, const UIPoint& point)
	{
		m_down = true;
		if (m_curHoverIndex != -1)
		{
			m_state = UIControlStatus_Pressed;
			m_cacheUpdate = true;
			UpdateDisplay();
		}
		return UIControl::OnLButtonDown(flag, point);
	}

	bool UINavBar::OnLButtonUp(_m_uint flag, const UIPoint& point)
	{
		if (!m_down)
			return UIControl::OnLButtonUp(flag, point);

		m_down = false;

		if (!IsHitItem(point))
			return UIControl::OnLButtonUp(flag, point);

		if (m_curSelIndex != m_curHoverIndex)
		{
			//项目横条动画过渡
			auto& beginItem = m_itemList[m_curSelIndex];
			auto& endItem = m_itemList[m_curHoverIndex];

			int end_left = endItem.second.left;
			int begin_width = beginItem.second.GetWidth();
			int end_width = endItem.second.GetWidth();

			if (begin_width == end_width)
				m_barwidth = begin_width;

			m_baroffset = end_left;
			m_barwidth = end_width;

			m_curSelIndex = m_curHoverIndex;
			SendEvent(Event_NavBar_ItemChange, m_curSelIndex);
		}
		m_state = UIControlStatus_Hover;
		m_cacheUpdate = true;
		UpdateDisplay();
		return SendEvent(Event_Mouse_LClick, (_m_param)&point);
	}

	bool UINavBar::OnMouseMove(_m_uint flag, const UIPoint& point)
	{
		if (IsHitItem(point) && m_curHoverIndex != m_lastHover)
		{
			m_lastHover = m_curHoverIndex;
			m_state = UIControlStatus_Hover;
			m_cacheUpdate = true;
			UpdateDisplay();
		}
		else
		{
			if (m_curHoverIndex != m_lastHover)
			{
				m_lastHover = -1;
				m_cacheUpdate = true;
				UpdateDisplay();
			}
		}
		return UIControl::OnMouseMove(flag, point);
	}

	void UINavBar::OnScale(_m_scale scale)
	{
		UIControl::OnScale(scale);
		auto& attrib = m_attrib.Get();

		scale = GetRectScale().scale();
		const float fontSize = M_MIN(scale.cx, scale.cy) * (float)attrib.fontSize;
		if (m_font)
			m_font->SetFontSize((_m_uint)fontSize, std::make_pair(0, (_m_uint)m_font->GetText().length()));
		CalcItemRect();
	}

	_m_sizef UINavBar::GetContentSize()
	{
		_m_sizef size = UINodeBase::GetContentSize();
		if (UINodeBase::m_data.AutoSize)
		{
			CalcItemRect();
			int max_right = 0;
			int max_bottom = 0;
			for (auto& item : m_itemList)
			{
				max_right = M_MAX(max_right, item.second.right);
				max_bottom = M_MAX(max_bottom, item.second.bottom);
			}
			auto scale = GetRectScale().scale();
			auto& attrib = m_attrib.Get();
			max_bottom += _scale_to(attrib.barSpace, scale.cy);
			max_bottom += _scale_to(attrib.barHeight, scale.cy);
			return { (float)max_right, (float)max_bottom };
		}
		return size;
	}

	void UINavBar::OnLayoutCalced()
	{
		//CalcItemRect();
		//m_cacheUpdate = true;
		//UpdateDisplay();
	}

	bool UINavBar::IndexCheck(int index) const
	{
		if (index < (int)m_itemList.size() && index >= 0)
			return true;
		return false;
	}

	std::pair<std::wstring, UIRect> UINavBar::InitItemRect(std::wstring_view title, int index)
	{
		_m_scale scale = GetRectScale().scale();
		int space = _scale_to(m_attrib.Get().itemSpace, scale.cx);
		int offset = 0;
		if (!m_itemList.empty() && index != 0)
		{
			if (index == -1)
				offset = m_itemList.back().second.right + space;
			else
				offset = m_itemList[size_t(index)].second.left + space;
		}
		m_font->SetText(title.data());
		auto ret = std::make_pair((std::wstring)title, m_font->GetMetrics());
		ret.second.left += offset;
		ret.second.right += offset + 1;
		return ret;
	}

	void UINavBar::CalcItemRect()
	{
		_m_scale scale = GetRectScale().scale();
		int offset = 0;
		int index = 0;
		for (auto& [text, rect] : m_itemList)
		{
			//计算项目尺寸
			m_font->SetText(text);
			rect = m_font->GetMetrics();
			rect.left += offset;
			rect.right += offset + 1;
			offset += rect.GetWidth() + _scale_to(m_attrib.Get().itemSpace, scale.cx);

			//横条位置和尺寸
			if (index == m_curSelIndex)
			{
				m_baroffset = rect.left;
				m_barwidth = rect.GetWidth();
			}

			index++;
		}
	}

	bool UINavBar::IsHitItem(const UIPoint& pt)
	{
		int index = 0;
		for (auto& item : m_itemList)
		{
			UIRect hover = item.second;
			hover.Offset((int)UINodeBase::m_data.Frame.left, (int)UINodeBase::m_data.Frame.top);
			if (Rect::IsPtInside(hover, pt))
			{
				m_curHoverIndex = index;
				return true;
			}
			hover.ResetOffset();
			index++;
		}
		m_curHoverIndex = -1;
		return false;
	}

	void UINavBar::BindAttribute()
	{
		using Type = decltype(m_attrib);

		auto list =
		{
			Type::MakeAttrib(&Attribute::fontStyle, L"fontStyle",
			[](Type::SetData param)
			{
				UILabel::Attribute defaults;
				auto style = param.GetValue<UILabel::Attribute*>();
				if (!style) style = &defaults;

				const auto range = std::make_pair(0u, (_m_uint)style->text.length());
				const auto _this = param.param.first;

				if (style->fontCustom != param.data->fontCustom)
					_this->m_font = _this->m_render->CreateFonts(style->text.cstr(), style->font.cstr(), style->fontSize, style->fontCustom);
				else
				{
					_this->m_font->SetFontName(style->font.cstr());
					_this->m_font->SetFontSize(style->fontSize, range);
				}
				_this->m_font->SetFontStyle(style->fontStyle, range);

				param.data->font = style->font;
				param.data->fontColor = style->fontColor;
				param.data->fontSize = style->fontSize;
				param.data->fontStyle = style->fontStyle;
				param.data->fontCustom = style->fontCustom;
				param.data->shadowUse = style->shadowUse;
				param.data->shadowLow = style->shadowLow;
				param.data->shadowBlur = style->shadowBlur;
				param.data->shadowOffset = style->shadowOffset;
				param.data->shadowColor = style->shadowColor;
				return _this->updateRender(param.param.second, true);
			},
			[](Type::GetData param)
			{
				return CtrlMgr::ConvertAttribPtr(&param.data->fontStyle);
			}),

			MakeUIAttrib(m_attrib, &Attribute::font, L"font",
			[](Type::SetData param)
			{
				if (param.data->font == param.attribValue)
					return true;

				param.data->font = param.attribValue;
				param.param.first->m_font->SetFontName(param.data->font.cstr());

				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttrib(m_attrib, &Attribute::fontColor, L"fontColor"),
			MakeUIAttrib(m_attrib, &Attribute::fontHoverColor, L"fontHoverColor"),
			MakeUIAttrib(m_attrib, &Attribute::fontPressColor, L"fontPressColor"),

			MakeUIAttrib(m_attrib, &Attribute::fontSize, L"fontSize",
			[](Type::SetData param)
			{
				auto fontSize = param.GetValue<_m_ushort>();
				if (param.data->fontSize == fontSize)
					return true;

				param.data->fontSize = fontSize;
				const auto _this = param.param.first;

				//dpi
				auto scale = _this->GetRectScale().scale();
				fontSize = (_m_ushort)(M_MIN(scale.cx, scale.cy) * (float)fontSize);
				param.param.first->m_font->SetFontSize(fontSize, { 0u, (_m_uint)param.param.first->m_font->GetText().length()});

				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttribEx(m_attrib, &UIFontStyle::bold, &Attribute::fontStyle, L"fontBold",
			[](Type::SetData param)
			{
				bool value = param.GetValue<bool>();
				if (param.data->fontStyle.bold == value)
					return true;

				param.data->fontStyle.bold = value;
				param.param.first->updateStyle(param.data->fontStyle);

				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttribEx(m_attrib, &UIFontStyle::italics, &Attribute::fontStyle, L"fontItalics",
			[](decltype(m_attrib)::SetData param)
			{
				bool value = param.GetValue<bool>();
				if (param.data->fontStyle.italics == value)
					return true;

				param.data->fontStyle.italics = value;
				param.param.first->updateStyle(param.data->fontStyle);

				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttribEx(m_attrib, &UIFontStyle::underline, &Attribute::fontStyle, L"fontUnderline",
			[](decltype(m_attrib)::SetData param)
			{
				bool value = param.GetValue<bool>();
				if (param.data->fontStyle.underline == value)
					return true;

				param.data->fontStyle.underline = value;
				param.param.first->updateStyle(param.data->fontStyle);

				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttribEx(m_attrib, &UIFontStyle::strikeout, &Attribute::fontStyle, L"fontStrikeout",
			[](decltype(m_attrib)::SetData param)
			{
				bool value = param.GetValue<bool>();
				if (param.data->fontStyle.strikeout == value)
					return true;

				param.data->fontStyle.strikeout = value;
				param.param.first->updateStyle(param.data->fontStyle);

				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttrib(m_attrib, &Attribute::fontCustom, L"fontCustom",
			[](decltype(m_attrib)::SetData param)
			{
				auto fontc = param.GetValue<_m_ptrv>();
				if (param.data->fontCustom == fontc) return true;

				param.data->fontCustom = fontc;
				const auto _this = param.param.first;

				auto scale = _this->GetRectScale().scale();
				_m_uint fontSize = (_m_uint)(M_MIN(scale.cx, scale.cy) * (float)param.data->fontSize);
				_this->m_font = _this->m_render->CreateFonts(L"", param.data->font.cstr(), fontSize, fontc);
				_this->m_font->SetFontStyle(param.data->fontStyle, std::make_pair(0u, 0u));

				return _this->updateRender(param.param.second, true);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::shadowUse, L"shadowUse", param,
			{
				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::shadowBlur, L"shadowBlur", param,
			{
				return param.param.first->updateRender(param.param.second);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::shadowColor, L"shadowColor", param,
			{
				return param.param.first->updateRender(param.param.second);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::shadowOffset, L"shadowOffset", param,
			{
				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::shadowLow, L"shadowLow", param,
			{
				return param.param.first->updateRender(param.param.second);
			}),

			MakeUIAttrib(m_attrib, &Attribute::itemSpace, L"itemSpace"),
			MakeUIAttrib(m_attrib, &Attribute::barSpace, L"barSpace"),
			MakeUIAttrib(m_attrib, &Attribute::barHeight, L"barHeight"),
			MakeUIAttrib(m_attrib, &Attribute::barAnitime, L"barAnitime"),
			MakeUIAttrib(m_attrib, &Attribute::barRound, L"barRound"),
			MakeUIAttrib(m_attrib, &Attribute::barColor, L"barColor"),
		};

		Type::RegisterAttrib(list);
	}

	void UINavBar::updateStyle(UIFontStyle& style)
	{
		m_font->SetFontStyle(style, std::make_pair(0, (_m_uint)m_font->GetText().length()));
	}

	bool UINavBar::updateRender(bool draw, bool layout)
	{
		m_cacheUpdate = true;
		if (!draw) return true;
		if (layout)
			CalcItemRect();
		UpdateDisplay();
		return true;
	}
}