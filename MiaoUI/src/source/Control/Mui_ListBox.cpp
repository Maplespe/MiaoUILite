/**
 * FileName: Mui_ListBox.cpp
 * Note: UI列表框控件实现
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
 * date: 2020-11-28 Create
*/

#include <Control/Mui_ListBox.h>

namespace Mui::Ctrl
{
	using namespace Helper;

#pragma region ListItem

	void ListItem::OnPaintProc(const PaintParam* param)
	{
		ItemStatus state = m_state;
		if (!m_parent->IsEnabled())
			state = UIListItemDisbale;
		if (IsSel())
			state = ItemStatus((int)state + 4);
		if (param->style)
			PaintStyle(param->render, param->style, *param->dstRect, state, param->dstAlpha);
		if (!m_text.empty())
		{
			const auto& font = m_parent->m_fontDef;
			const auto range = std::make_pair(0u, (_m_uint)m_text.length());

			font->SetText(m_text.view());
			font->SetFontStyle(m_style, range);

			const float s = M_MIN(param->dstScale.ws, param->dstScale.hs);
			font->SetFontSize(_scale_to(m_size, s), range);

			m_parent->m_brushDef->SetColor(m_color == 0 ? param->defcolor : m_color);
			m_parent->m_brushDef->SetOpacity(param->dstAlpha);
			_m_rect dst = *param->dstRect;
			Rect::Offset(&dst, param->offset.x, param->offset.y);
			param->render->DrawTextLayout(font, dst, m_parent->m_brushDef, m_parent->m_attrib.Get().fontStyle.textAlign);
		}
	}

	void ListItem::PaintStyle(MRenderCmd* render, UIStyle* style, _m_rect frame,
		ItemStatus state, _m_byte alpha, _m_scale scale)
	{
		style->PaintStyle(render, &frame, alpha, state, 8, scale);
	}

	ListItem::ListItem(std::wstring_view title, _m_color color, UIFontStyle style)
	{
		m_text = title;
		m_color = color;
		m_style = style;
	}

	ListItem::~ListItem() = default;

	void ListItem::SetText(std::wstring_view text, _m_color color, UIFontStyle style, bool updateStyle)
	{
		m_text = text;
		if (color != 0)
			m_color = color;
		if (updateStyle)
			m_style = style;
	}

	void ListItem::SetFontSize(_m_uint size)
	{
		m_size = size;
	}

	std::wstring_view ListItem::GetText()
	{
		return m_text.view();
	}

	bool ListItem::OnMouseMessage(_m_uint message, _m_param wParam, _m_param lParam)
	{
		bool paint = true;
		switch ((_m_msg)message)
		{
		case M_MOUSE_HOVER:
		{
			m_state = UIListItemHover;
			break;
		}
		case M_MOUSE_LEAVE:
		{
			m_state = UIListItemNormal;
			m_isClick = false;
			break;
		}
		case M_MOUSE_RBDOWN:
		case M_MOUSE_LBDOWN:
		{
			m_isClick = true;
			m_state = UIListItemPressed;
			break;
		}
		case M_MOUSE_RBUP:
		case M_MOUSE_LBUP:
		{
			if (m_isClick)
			{
				m_isClick = false;
			}
			m_state = UIListItemHover;
			break;
		}
		default:
			paint = false;
			break;
		}
		return paint;
	}

	bool ListItem::IsSel()
	{
		return m_parent->m_curSelItem == this;
	}

#pragma endregion

#pragma region UIListBox

	UIListBox::UIListBox(UIControl* parent, Attribute attrib, UIScroll::Attribute scrollAttrib)
	: UIListBox(std::move(attrib), std::move(scrollAttrib))
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
	}

	UIListBox::UIListBox(Attribute attrib, UIScroll::Attribute scrollAttrib)
	: UIScroll(std::move(scrollAttrib)), m_attrib(std::move(attrib))
	{
		m_ALLWheel = true;
		SetCallback([this](auto&& PH1, auto&& PH2, auto&& PH3)
		{
			OnScrollView(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
			std::forward<decltype(PH3)>(PH3));
		});
	}

	UIListBox::~UIListBox()
	{
		for (auto& item : m_itemList)
		{
			delete item;
		}
	}

	void UIListBox::Register()
	{
		BindAttribute();

		static auto method = [](UIControl* parent)
		{
			return new UIListBox(parent, Attribute());
		};
		MCTRL_REGISTER(method);
	}

	void UIListBox::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (m_attrib.SetAttribute(attribName, attrib, this))
		{
			if (attribName == L"itemHeight"
				|| attribName == L"lineSpace")
				CalcListView();
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		else
		{
			UIScroll::SetAttribute(attribName, attrib, draw);
			if (attribName == L"inset")
				CalcListView();
		}
	}

	std::wstring UIListBox::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret))
			return ret;
		return UIScroll::GetAttribute(attribName);
	}

	const UIListBox::Attribute& UIListBox::GetAttribute() const
	{
		return m_attrib.Get();
	}

	int UIListBox::AddItem(ListItem* item, int index, bool draw)
	{
		if (item)
		{
			if (index == -1)
			{
				item->m_parent = this;
				m_itemList.push_back(item);
				CalcListView();
				m_cacheUpdate = true;
				if (draw) UpdateDisplay();
			}
			else if (IndexCheck(index))
			{
				item->m_parent = this;
				m_itemList.insert(m_itemList.begin() + index, item);
				CalcListView();
				//列表发生改变 需要更新当前选中项目的索引
				if (index < m_curSelIndex)
					UpdateIndex();
				if (draw)
					UpdateDisplay();
				m_cacheUpdate = true;
			}
			else
			{
				MErrorThrow(MErrorCode::IndexOutOfRange);
				return (int)m_itemList.size();
			}
		}

		return (int)m_itemList.size();
	}

	ListItem* UIListBox::GetItem(int index) const
	{
		if (index == -1)
		{
			if (!m_itemList.empty())
				return m_itemList.back();
		}
		if (IndexCheck(index))
			return m_itemList[index];
		return nullptr;
	}

	bool UIListBox::SetCurSelItem(int index, bool draw)
	{
		if (m_curSelIndex == index)
			return true;

		if (index == -1)
		{
			m_curSelIndex = -1;
			m_curSelItem = nullptr;
		}
		else if (IndexCheck(index))
		{
			m_curSelIndex = index;
			m_curSelItem = m_itemList[index];
		}
		else
			return false;

		m_cacheUpdate = true;
		if (draw) UpdateDisplay();
		return true;
	}

	int UIListBox::GetCurSelItem() const
	{
		return m_curSelIndex;
	}

	int UIListBox::GetItemListCount() const
	{
		return (int)m_itemList.size();
	}

	int UIListBox::DeleteItem(int index, bool draw)
	{
		return DeleteItem(index, false, draw);
	}

	int UIListBox::DeleteItem(int index, bool delItem, bool draw)
	{
		if (index == -1)
		{
			index = (int)m_itemList.size() - 1;
			if (index < 0) index = 0;
		}
		if (IndexCheck(index))
		{
			if (m_curSelIndex == index)
			{
				m_curSelIndex = -1;
				m_curSelItem = nullptr;
			}
			auto iter = m_itemList.begin() + index;
			if (delItem)
				delete* iter;
			m_itemList.erase(iter);
			CalcListView();
			m_cacheUpdate = true;
		}
		else
			MErrorThrow(MErrorCode::IndexOutOfRange);
		if (draw) UpdateDisplay();
		return (int)m_itemList.size();
	}

	void UIListBox::DeleteAllItem(bool delItem, bool draw)
	{
		if (delItem)
		{
			for (auto& item : m_itemList)
			{
				delete item;
			}
		}
		m_curSelIndex = -1;
		m_curSelItem = nullptr;
		m_itemList.clear();
		CalcListView();
		m_cacheUpdate = true;
		if (draw) UpdateDisplay();
	}

	void UIListBox::OnLoadResource(MRenderCmd* render, bool recreate)
	{
		UIScroll::OnLoadResource(render, recreate);

		auto& attrib = m_attrib.Get();

		auto scale = GetRectScale().scale();
		const float fontSize = M_MIN(scale.cx, scale.cy) * (float)attrib.fontStyle.fontSize;

		m_fontDef = render->CreateFonts(L"", attrib.fontStyle.font.cstr(), (_m_uint)fontSize, attrib.fontStyle.fontCustom);
		m_brushDef = render->CreateBrush(attrib.fontStyle.fontColor);
		m_fontDef->SetFontStyle(attrib.fontStyle.fontStyle, std::make_pair(0u, 0u));
	}

	void UIListBox::OnPaintProc(MPCPaintParam param)
	{
		if (auto& attrib = m_attrib.Get(); attrib.style) 
		{
			attrib.style->PaintStyle(param->render, param->destRect, param->blendedAlpha,
				IsEnabled() ? m_state : UIControlStatus_Disable, 4, GetRectScale().scale());
		}
		PaintListView(param->render, param->cacheCanvas ? param->destRect : param->clipRect, param->cacheCanvas);
		UIScroll::OnPaintProc(param);
	}

	bool UIListBox::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		if (!UIScroll::OnMouseMessage(message, wParam, lParam))
		{
			bool paint = true;
			UIPoint point{ (M_LOWORD((_m_long)lParam)), (M_HIWORD((_m_long)lParam)) };

			switch (message)
			{
			case M_MOUSE_HOVER:
				m_state = UIControlStatus_Hover;
				SendEvent(Event_Mouse_Hover, (_m_param)&point);
				break;
			case M_MOUSE_LEAVE:
				m_state = UIControlStatus_Normal;
				SendEvent(Event_Mouse_Exited, (_m_param)&point);
				break;
			case M_MOUSE_LBDOWN:
			case M_MOUSE_RBDOWN:
				if (message == M_MOUSE_RBDOWN && !m_attrib.Get().allowRightSel)
					break;

				m_state = UIControlStatus_Pressed;
				m_downViewY = m_view.top - point.y;
				SendEvent(message == M_MOUSE_RBDOWN ? Event_Mouse_RDown : Event_Mouse_LDown, (_m_ptrv)&point);
				//移动端可能直接触发LBDOWN消息而不先触发MOVE 所以LBDOWN的时候查找一下hover控件
				paint = DispatchItemMsg(point, message, wParam, lParam);
				goto End;
			case M_MOUSE_LBUP:
			case M_MOUSE_RBUP:
			{
				if (message == M_MOUSE_RBUP && !m_attrib.Get().allowRightSel)
					break;

				m_state = UIControlStatus_Normal;
				if (m_hoverIndex && m_hoverIndex->m_isClick)
					SetSelItem(m_hoverIndex, point);
				SendEvent(message == M_MOUSE_RBUP ? Event_Mouse_RUp : Event_Mouse_LUp, (_m_param)&point);
			}
				break;

			case M_MOUSE_MOVE:
				
				paint = DispatchItemMsg(point, message, wParam, lParam);
				SendEvent(Event_Mouse_Move, (_m_param)&point);
				break;
			default:
				paint = false;
				break;
			}

			if (m_hoverIndex)
			{
				bool ret = m_hoverIndex->OnMouseMessage(message, wParam, lParam);
				if (!paint && ret)
					paint = true;
				if (message == M_MOUSE_LEAVE)
					m_hoverIndex = nullptr;
			}

		End:
			if (paint)
			{
				m_cacheUpdate = true;
				UpdateDisplay();
			}
			return paint;
		}
		return true;
	}

	void UIListBox::OnScrollView(UIScroll* ptr, int dragValue, bool horizontal)
	{
		if (!horizontal)
		{
			int value = CalcOffsetDragValue(horizontal, dragValue, m_view.viewHeight);
			SetViewTop(-value);
		}
	}

	bool UIListBox::DispatchItemMsg(UIPoint& point, _m_uint msg, _m_param wParam, _m_param lParam)
	{
		//如果当前有焦点 而在区域内
		if (m_hoverIndex && Rect::IsPtInside(m_curItemRect, point))
			return m_hoverIndex->OnMouseMessage(msg, wParam, lParam);

		//鼠标位置移动到了别的控件 重新查找当前焦点
		for (int i = 0; i < m_view.indexCount; ++i)
		{
			auto index = m_view.beginIndex + i;
			if (index == (int)m_itemList.size()) break;

			auto item = m_itemList[index];
			auto& itemRect = m_view.viewItemRect[i];

			if (Rect::IsPtInside(itemRect, point))
			{
				if (m_hoverIndex != nullptr && m_hoverIndex != item)
				{
					m_hoverIndex->OnMouseMessage(M_MOUSE_LEAVE, wParam, lParam);
					item->OnMouseMessage(M_MOUSE_HOVER, wParam, lParam);
				}
				else if (m_hoverIndex == nullptr)
				{
					item->OnMouseMessage(M_MOUSE_HOVER, wParam, lParam);
				}
				m_hoverIndex = item;
				m_curItemRect = itemRect;
				return true;
			}
		}
		//如果没找到焦点 移动到了空白区域 给当前焦点发送离开通知
		if (m_hoverIndex)
		{
			bool ret = m_hoverIndex->OnMouseMessage(M_MOUSE_LEAVE, wParam, lParam);
			m_hoverIndex = nullptr;
			return ret;
		}
		return false;
	}

	void UIListBox::SetViewTop(int top)
	{
		m_view.top = top;
		//一个项目+一个间距为一组
		int height = m_view.itemHeight + m_view.lineSpace;
		int beginIndex = 0;
		UIPoint offsetPos;

		//计算当前开始索引
		if (top != 0) 
		{
			top = -top;
			int offset = top / height;

			beginIndex = offset;

			offsetPos.y = top % height;//取余数
			offsetPos.y = -offsetPos.y;//还原成负数
		}
		m_view.beginIndex = beginIndex;
		m_view.offsetPos = offsetPos;
	}

	bool UIListBox::IndexCheck(int index) const
	{
		if (index < (int)m_itemList.size() && index >= 0)
			return true;
		return false;
	}

	void UIListBox::CalcListView()
	{
		auto& attrib = m_attrib.Get();
		auto& scroll = UIScroll::GetAttribute();
		//DPI
		_m_rcscale scale = GetRectScale();
		auto lineSpace = _scale_to(attrib.lineSpace, scale.hs);
		auto itemHeight = _scale_to(attrib.itemHeight, scale.hs);
		UIRect boxRect = Frame();
		const auto& inset = scroll.inset;
		boxRect.left += _scale_to(inset.left, scale.ws);
		boxRect.top += _scale_to(inset.top, scale.hs);
		boxRect.right -= _scale_to(inset.right, scale.ws);
		boxRect.bottom -= _scale_to(inset.bottom, scale.hs);

		m_view.scale = scale;
		m_view.viewHeight = boxRect.GetHeight();
		m_view.boxRect = boxRect;
		m_view.itemHeight = itemHeight;
		m_view.lineSpace = lineSpace;

		//计算视图位置
		itemHeight += lineSpace;
		m_view.bottom = (_m_long)(m_itemList.size() * itemHeight) - lineSpace;
		//没有项目
		if (m_view.bottom < 0) m_view.bottom = 0;

		//设置滚动条属性
		if (scroll.range.height != m_view.bottom)
		{
			bool vertical;

			if (m_view.viewHeight < m_view.bottom)
				vertical = true;
			else
				vertical = false;

			UIScroll::SetAttributeSrc(L"rangeV", m_view.bottom, false);
			UIScroll::SetAttributeSrc(L"vertical", vertical, true);
		}
		else
			UIScroll::SetAttributeSrc(L"vertical", m_view.viewHeight < m_view.bottom, true);

		if (m_view.viewHeight > m_view.bottom)
		{
			SetViewTop(0);
			SetDragValueNoAni(false, 0, false);
		}
		//计算可视项目数量
		m_view.indexCount = Helper::M_MAX((int)ceil((float)m_view.viewHeight / (float)itemHeight) + 1, 0);
		m_view.viewItemRect.resize(m_view.indexCount);
	}

	void UIListBox::PaintListView(MRenderCmd* render, MPCRect destRect, bool cache)
	{
		UIRect boxRect = m_view.boxRect;
		if (cache)
			boxRect.Offset(-(int)UINodeBase::m_data.Frame.left, -(int)UINodeBase::m_data.Frame.top);
		UIRect itemRect = UIRect(boxRect.left, boxRect.top + m_view.offsetPos.y, boxRect.GetWidth(), m_view.itemHeight);
		if(cache)
			itemRect.Offset(destRect->left, destRect->top);

		//重设裁剪区
		render->PopClipRect();
		_m_rect clip = boxRect;
		Rect::Intersect(&clip, destRect, &clip);
		render->PushClipRect(clip);

		auto& attrib = m_attrib.Get();
		UIPoint _offset = { _scale_to(attrib.drawOffset.x, m_view.scale.ws), _scale_to(attrib.drawOffset.y, m_view.scale.hs) };

		ListItem::PaintParam param;
		param.render = render;
		param.dstAlpha = cache ? 255 : UINodeBase::m_data.AlphaDst;
		param.style = m_attrib.Get().itemStyle.get();
		param.dstScale = m_view.scale;
		param.defcolor = m_attrib.Get().fontStyle.fontColor;
		for (int i = 0; i < m_view.indexCount; ++i)
		{
			if (m_view.beginIndex + i >= (int)m_itemList.size()) break;

			auto item = m_itemList[m_view.beginIndex + i];
			m_view.viewItemRect[i] = itemRect;
			if (cache)
			{
				m_view.viewItemRect[i].Offset((int)UINodeBase::m_data.Frame.left, (int)UINodeBase::m_data.Frame.top);
				m_view.viewItemRect[i].Offset(-destRect->left, -destRect->top);
			}
			auto dstRect = itemRect;
			param.dstRect = &dstRect;
			param.offset = _offset;
			item->OnPaintProc(&param);

			if (auto&& dbgframe = GetDbgFramePen())
				render->DrawRectangle(dstRect, dbgframe);

			itemRect.Offset(0, m_view.itemHeight + m_view.lineSpace);
		}
	}

	void UIListBox::SetSelItem(ListItem* item, UIPoint pt)
	{
		if (m_curSelItem != item) 
		{
			int oldIndex = m_curSelIndex;
			auto oldItem = m_curSelItem;

			m_curSelItem = item;
			UpdateIndex();
			SendEvent(Event_ListBox_ItemLClick, (_m_param)m_curSelIndex);
			mslot.itemClicked.Emit(pt, m_curSelIndex);

			bool change = true;
			SendEvent(Event_ListBox_ItemChanging, (_m_param)&change);
			mslot.itemChanging.Emit(m_curSelIndex, std::ref(change));

			//如果用户指定不更改 还原
			if (!change)
			{
				m_curSelIndex = oldIndex;
				m_curSelItem = oldItem;
				return;
			}
			SendEvent(Event_ListBox_ItemChanged, (_m_param)m_curSelIndex);
			mslot.itemChanged.Emit(m_curSelIndex);
		}
		else
		{
			SendEvent(Event_ListBox_ItemLClick, (_m_param)m_curSelIndex);
			mslot.itemClicked.Emit(pt, m_curSelIndex);
		}
	}

	void UIListBox::UpdateIndex()
	{
		//查找当前选中项目的索引值
		auto iter = std::find(m_itemList.begin(), m_itemList.end(), m_curSelItem);
		if (iter == m_itemList.end())
		{
			m_curSelIndex = -1;
			m_curSelItem = nullptr;
		}
		else
		{
			m_curSelIndex = (int)std::distance(m_itemList.begin(), iter);
		}
	}

	void UIListBox::BindAttribute()
	{
		using Type = decltype(m_attrib);

		const auto fontStyleOffset = Type::MOffsetOf(&ItemFont::fontStyle) + Type::MOffsetOf(&Attribute::fontStyle);

		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::style, L"style"),
			MakeUIAttrib(m_attrib, &Attribute::itemStyle, L"itemStyle"),
			MakeUIAttrib(m_attrib, &Attribute::itemHeight, L"itemHeight"),
			MakeUIAttrib(m_attrib, &Attribute::allowRightSel, L"allowRightSel"),
			MakeUIAttrib(m_attrib, &Attribute::lineSpace, L"lineSpace"),
			MakeUIAttrib(m_attrib, &Attribute::fontStyle, L"iFontStyle",
			[](Type::SetData param)
			{
				auto style = CtrlMgr::ConvertAttribPtr<ItemFont>(param.attribValue);
				if (!style) return false;

				const auto range = std::make_pair(0u, 0u);
				const auto _this = param.param;

				if (style->fontCustom != param.data->fontStyle.fontCustom)
					_this->m_fontDef = _this->m_render->CreateFonts(L"", style->font.cstr(), style->fontSize, style->fontCustom);
				else
				{
					_this->m_fontDef->SetText(L"");
					_this->m_fontDef->SetFontName(style->font.cstr());
					_this->m_fontDef->SetFontSize(style->fontSize, range);
				}
				_this->m_fontDef->SetFontStyle(style->fontStyle, range);

				param.data->fontStyle = *style;
				return true;
			},
			[](Type::GetData param)
			{
				return CtrlMgr::ConvertAttribPtr(&param.data->fontStyle);
			}),
			MakeUIAttribEx(m_attrib, &ItemFont::font, &Attribute::fontStyle, L"iFont",
			[](Type::SetData param)
			{
				auto font = param.GetValue();
				if (param.data->fontStyle.font == font) return true;

				param.data->fontStyle.font = font;
				param.param->m_fontDef->SetFontName(font);

				return true;
			}),
			MakeUIAttribEx(m_attrib, &ItemFont::fontSize, &Attribute::fontStyle, L"iFontSize",
			[](Type::SetData param)
			{
				auto fontSize = param.GetValue<_m_ushort>();
				if (param.data->fontStyle.fontSize == fontSize) return true;

				param.data->fontStyle.fontSize = fontSize;
				//dpi
				auto scale = param.param->GetRectScale().scale();
				fontSize = (_m_ushort)(M_MIN(scale.cx, scale.cy) * (float)fontSize);

				param.param->m_fontDef->SetFontSize(fontSize, std::make_pair(0u, (_m_uint)param.param->m_fontDef->GetText().length()));

				return true;
			}),
			Type::MakeAttribAtOffset(&UIFontStyle::bold, fontStyleOffset, L"iFontBold",
			[](Type::SetData param)
			{
				bool bold = param.GetValue<bool>();
				if (param.data->fontStyle.fontStyle.bold == bold) return true;

				param.data->fontStyle.fontStyle.bold = bold;
				param.param->updateStyle(param.data->fontStyle.fontStyle);
				return true;
			}),
			Type::MakeAttribAtOffset(&UIFontStyle::italics, fontStyleOffset, L"iFontItalics",
			[](Type::SetData param)
			{
				bool italics = param.GetValue<bool>();
				if (param.data->fontStyle.fontStyle.italics == italics) return true;

				param.data->fontStyle.fontStyle.italics = italics;
				param.param->updateStyle(param.data->fontStyle.fontStyle);
				return true;
			}),
			Type::MakeAttribAtOffset(&UIFontStyle::underline, fontStyleOffset, L"iFontUnderline",
			[](Type::SetData param)
			{
				bool underline = param.GetValue<bool>();
				if (param.data->fontStyle.fontStyle.underline == underline) return true;

				param.data->fontStyle.fontStyle.underline = underline;
				param.param->updateStyle(param.data->fontStyle.fontStyle);
				return true;
			}),
			Type::MakeAttribAtOffset(&UIFontStyle::strikeout, fontStyleOffset, L"iFontStrikeout",
			[](Type::SetData param)
			{
				bool strikeout = param.GetValue<bool>();
				if (param.data->fontStyle.fontStyle.strikeout == strikeout) return true;

				param.data->fontStyle.fontStyle.strikeout = strikeout;
				param.param->updateStyle(param.data->fontStyle.fontStyle);
				return true;
			}),
			MakeUIAttribEx(m_attrib, &ItemFont::fontColor, &Attribute::fontStyle, L"iFontColor"),
			MakeUIAttribEx(m_attrib, &ItemFont::textAlign, &Attribute::fontStyle, L"iTextAlign"),
			MakeUIAttribEx(m_attrib, &ItemFont::fontCustom, &Attribute::fontStyle, L"iFontCustom",
			[](Type::SetData param)
			{
				auto fontc = param.GetValue<_m_ptrv>();
				if (param.data->fontStyle.fontCustom == fontc) return true;

				param.data->fontStyle.fontCustom = fontc;

				auto _this = param.param;
				auto scale = _this->GetRectScale().scale();
				_m_uint fontSize = (_m_uint)(M_MIN(scale.cx, scale.cy) * (float)param.data->fontStyle.fontSize);
				_this->m_fontDef = _this->m_render->CreateFonts(L"", param.data->fontStyle.font.cstr(), fontSize, fontc);
				_this->m_fontDef->SetFontStyle(param.data->fontStyle.fontStyle, std::make_pair(0u, 0u));

				return true;
			}),
			MakeUIAttrib(m_attrib, &Attribute::drawOffset, L"drawOffset")
		};

		Type::RegisterAttrib(list);
	}

	void UIListBox::updateStyle(UIFontStyle& style)
	{
		m_fontDef->SetFontStyle(style, std::make_pair(0, (_m_uint)m_fontDef->GetText().length()));
	}
#pragma endregion
}