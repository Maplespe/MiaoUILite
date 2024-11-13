/**
 * FileName: Mui_Scroll.cpp
 * Note: UI滚动条控件实现
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
 * date: 2020-11-15 Create
*/

#include <Control/Mui_Scroll.h>

namespace Mui::Ctrl
{
	using namespace Helper;

	UIScroll::UIScroll(UIControl* parent, Attribute attrib) : UIScroll(std::move(attrib))
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
	}

	UIScroll::UIScroll(Attribute attrib) : m_attrib(std::move(attrib))
	{
		m_cacheSupport = true;
	}

	int UIScroll::CalcOffsetDragValue(bool horizontal, int dragValue, int offset)
	{
		int value = dragValue;
		int range = GetRange(horizontal);
		if (offset > range)
			range = offset;
		float percentage = (float)value / (float)range;
		range -= offset;
		value = int((float)range * percentage);
		return value;
	}

	void UIScroll::Register()
	{
		BindAttribute();

		static auto method = [](UIControl* parent)
		{
			return new UIScroll(parent, Attribute());
		};
		MCTRL_REGISTER(method);
	}

	void UIScroll::OnPaintProc(MPCPaintParam param)
	{
		UISize size = { param->destRect->GetWidth(), param->destRect->GetHeight() };
		if (size.width != m_size.width
			|| size.height != m_size.height)
		{
			m_size = size;
			CalcControlRect();
		}
		//控件是否可用
		_m_byte alpha = UINodeBase::m_data.AlphaDst;
		if (param->cacheCanvas) alpha = 255;
		auto scale = GetRectScale().scale();
		auto Paint = [&](UIStyle* style, UIRect rc, ScrollButtonType type, bool horizontal)
		{
			rc.Offset(param->destRect->left, param->destRect->top);
			PaintControl(param->render, style, m_btnType == type && m_btnTypeH == horizontal
				                                   ? m_status : UIControlStatus_Normal, type, rc, alpha, scale);
		};
		auto& attrib = m_attrib.Get();
		//垂直滚动条
		if (attrib.vertical && attrib.styleV)
		{
			Paint(attrib.styleV.get(), m_scroll[0].track, Track, false);
			if (attrib.button)
			{
				Paint(attrib.styleV.get(), m_scroll[0].scrollBtn[0], TopButton, false);
				Paint(attrib.styleV.get(), m_scroll[0].scrollBtn[1], BottomButton, false);
			}
			Paint(attrib.styleV.get(), m_scroll[0].thumbButton, ThumbButton, false);
		}
		//水平滚动条
		if (attrib.horizontal && attrib.styleH)
		{
			Paint(attrib.styleH.get(), m_scroll[1].track, Track, true);
			if (attrib.button)
			{
				Paint(attrib.styleH.get(), m_scroll[1].scrollBtn[0], TopButton, true);
				Paint(attrib.styleH.get(), m_scroll[1].scrollBtn[1], BottomButton, true);
			}
			Paint(attrib.styleH.get(), m_scroll[1].thumbButton, ThumbButton, true);
		}
		//交汇点
		if (attrib.vertical && attrib.styleV)
		{
			Paint(attrib.styleV.get(), m_intersect, Intersect, false);
		}
	}

	int UIScroll::GetRange(bool horizontal) const
	{
		if(horizontal)
			return m_attrib.Get().range.width;
		return m_attrib.Get().range.height;
	}

	UISize UIScroll::GetRange() const
	{
		return m_attrib.Get().range;
	}

	void UIScroll::SetRange(bool horizontal, int range, bool draw)
	{
		m_attrib.SetAttribute(horizontal ? L"rangeH" : L"rangeV", range, this);
		CalcControlRect();
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	void UIScroll::SetRange(UISize range, bool draw)
	{
		m_attrib.SetAttribute(L"range", range, this);
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	int UIScroll::GetDragValue(bool horizontal) const
	{
		if (horizontal)
			return m_attrib.Get().dragValue.width;
		return m_attrib.Get().dragValue.height;
	}

	UISize UIScroll::GetDragValue() const
	{
		return m_attrib.Get().dragValue;
	}

	void UIScroll::SetDragValue(bool horizontal, int value, bool draw)
	{
		SetDragValueNoAni(horizontal, value, draw);
	}

	void UIScroll::CalcControlRect()
	{
		auto& attrib = m_attrib.Get();
		int barWidth = attrib.barWidth;
		auto inset = attrib.inset;
		int btnHeight = attrib.btnHeight;

		//dpi
		auto [ws, hs] = GetRectScale().scale();
		float s = M_MIN(ws, hs);
		barWidth = int(s * (float)barWidth);
		btnHeight = int(s * (float)btnHeight);
		inset.left = _scale_to(inset.left, ws);
		inset.right = _scale_to(inset.right, ws);
		inset.top = _scale_to(inset.top, hs);
		inset.bottom = _scale_to(inset.bottom, hs);

		UIRect controlRc = UIRect(0, 0, (int)UINodeBase::m_data.Frame.GetWidth(), (int)UINodeBase::m_data.Frame.GetHeight());

		//垂直滚动条
		if (attrib.vertical)
		{
			UIRect VscrollRc = controlRc;

			VscrollRc.left = VscrollRc.right - barWidth - inset.right;
			VscrollRc.top += inset.top;
			VscrollRc.right -= inset.right;
			VscrollRc.bottom -= inset.bottom;

			//如果有水平滚动条 计算出底部交汇区域
			if (attrib.horizontal)
			{
				VscrollRc.bottom -= barWidth;//减去水平滚动条的宽度(高度)

				m_intersect.left = VscrollRc.left;
				m_intersect.top = VscrollRc.bottom + 1;
				m_intersect.right = VscrollRc.right;
				m_intersect.bottom = controlRc.bottom - inset.bottom;
			}

			UIRect Track = VscrollRc;

			//计算轨道和滚动调节按钮区域
			if (attrib.button)
			{
				UIRect topBtn, BottomBtn;
				topBtn = BottomBtn = Track;
				topBtn.bottom = topBtn.top + btnHeight;

				BottomBtn.top = BottomBtn.bottom - btnHeight;

				Track.top = topBtn.bottom + 1;
				Track.bottom = BottomBtn.top - 1;

				m_scroll[0].scrollBtn[0] = topBtn;
				m_scroll[0].scrollBtn[1] = BottomBtn;
			}
			m_scroll[0].track = Track;

			UIRect Thumb = Track;

			//计算拖拽按钮矩形
			int TrackH = m_scroll[0].track.GetHeight();
			if (attrib.range.height > TrackH)
			{
				//计算滑块按钮高度
				float viewRatio = (float)TrackH / (float)attrib.range.height;
				int _btnHeight = int((float)TrackH * viewRatio);
				if (_btnHeight < attrib.barMinHeight)
					_btnHeight = attrib.barMinHeight;
				Thumb.bottom = Thumb.top + _btnHeight;
			}
			m_scroll[0].thumbButton = Thumb;
			CalcThumbBtnPos(false);
		}
		//水平滚动条
		if (attrib.horizontal)
		{
			UIRect HscrollRc = controlRc;

			HscrollRc.left += inset.left;
			HscrollRc.top = HscrollRc.bottom - inset.bottom - barWidth;
			HscrollRc.bottom -= inset.bottom;
			if (attrib.vertical)
				HscrollRc.right = m_intersect.left - 1;
			else
				HscrollRc.right -= inset.right;

			UIRect Track = HscrollRc;

			if (attrib.button)
			{
				UIRect leftBtn, RightBtn;
				leftBtn = RightBtn = Track;
				leftBtn.right = leftBtn.left + btnHeight;

				RightBtn.left = RightBtn.right - btnHeight;

				Track.left = leftBtn.right + 1;
				Track.right = RightBtn.left - 1;

				m_scroll[1].scrollBtn[0] = leftBtn;
				m_scroll[1].scrollBtn[1] = RightBtn;
			}
			m_scroll[1].track = Track;

			UIRect Thumb = Track;

			//计算拖拽按钮矩形
			int TrackW = m_scroll[1].track.GetWidth();
			if (attrib.range.width > TrackW)
			{
				//计算滑块按钮宽度
				float viewRatio = (float)TrackW / (float)attrib.range.width;
				int _btnWidth = int((float)TrackW * viewRatio);
				if (_btnWidth < attrib.barMinHeight)
					_btnWidth = attrib.barMinHeight;
				Thumb.right = Thumb.left + _btnWidth;
			}
			m_scroll[1].thumbButton = Thumb;
			CalcThumbBtnPos(true);
		}
	}

	void UIScroll::CalcThumbBtnPos(bool Horizontal)
	{
		auto& attrib = m_attrib.Get();
		if (!Horizontal)
		{
			float TrackValue = (float)m_scroll[0].track.GetHeight();
			float ThumbBtnH = (float)m_scroll[0].thumbButton.GetHeight();
			TrackValue -= ThumbBtnH;
			if (TrackValue != 0.f && attrib.dragValue.height <= attrib.range.height)
			{
				//根据Drag值计算占用空间
				TrackValue /= 100.f;//化成小数运算 不然要用long long
				TrackValue = (float)attrib.dragValue.height * TrackValue / (float)attrib.range.height;
				UIRect btnRc = m_scroll[0].thumbButton;
				btnRc.top = m_scroll[0].track.top + int(TrackValue * 100.f);
				btnRc.bottom = btnRc.top + (int)ThumbBtnH;
				m_scroll[0].thumbButton = btnRc;
			}
			else
				m_scroll[0].thumbButton = m_scroll[0].track;
		}
		else
		{
			float TrackValue = (float)m_scroll[1].track.GetWidth();
			float ThumbBtnW = (float)m_scroll[1].thumbButton.GetWidth();
			TrackValue -= ThumbBtnW;
			if (TrackValue != 0.f && attrib.dragValue.width <= attrib.range.width)
			{
				TrackValue /= 100.f;
				TrackValue = (float)attrib.dragValue.width * TrackValue / (float)attrib.range.width;
				UIRect btnRc = m_scroll[1].thumbButton;
				btnRc.left = m_scroll[1].track.left + int(TrackValue * 100.f);
				btnRc.right = btnRc.left + (int)ThumbBtnW;
				m_scroll[1].thumbButton = btnRc;
			}
			else
				m_scroll[1].thumbButton = m_scroll[1].track;
		}
	}

	void UIScroll::CalcDragValue(bool horizontal)
	{
		auto& data = m_attrib.Set();
		if (!horizontal)
		{
			//根据裁剪按钮空间反推Drag值
			int TrackValue = m_scroll[0].thumbButton.top - m_scroll[0].track.top;
			int height = m_scroll[0].track.GetHeight() - m_scroll[0].thumbButton.GetHeight();
			float value = 0;
			if(TrackValue != 0)
				value = (float)TrackValue / (float)height * (float)data.range.height;
			data.dragValue.height = (int)round(value);
		}
		else
		{
			int TrackValue = m_scroll[1].thumbButton.left - m_scroll[1].track.left;
			int width = m_scroll[1].track.GetWidth() - m_scroll[1].thumbButton.GetWidth();
			float value = 0;
			if(TrackValue != 0)
				value = (float)TrackValue / (float)width * (float)data.range.width;
			data.dragValue.width = (int)round(value);
		}
	}

	void UIScroll::PaintControl(MRenderCmd* render, UIStyle* style, UIControlStatus status,
	                            ScrollButtonType type, UIRect& drawRect, _m_byte& alpha, _m_scale scale)
	{
		if (!IsEnabled())
			status = UIControlStatus_Disable;

		int rectCout = -1;
		switch (type)
		{
		//按钮有4种状态 依次排列+4
		case TopButton:
			rectCout = (int)status;
			break;
		case BottomButton:
			rectCout = (int)status + 4;
			break;
		case ThumbButton:
			{
				rectCout = (int)status + 8;
				break;
			}
		//轨道和交汇点的皮肤只有2种状态 普通和禁用
		case Track:
			if (status != UIControlStatus_Disable)
				status = UIControlStatus_Normal;
			else
				status = UIControlStatus_Hover;//Disable
			rectCout = (int)status + 12;
			break;
		case Intersect:
			if (status != UIControlStatus_Disable)
				status = UIControlStatus_Normal;
			else
				status = UIControlStatus_Hover;//Disable
			rectCout = (int)status + 14;
			break;
		case ButtonNull:
			break;
		}

		if (rectCout != -1 && style->GetPartCount() == 16)
			style->PaintStyle(render, &drawRect, alpha, rectCout, 16, scale);
	}

	void UIScroll::SetDragValueNoAni(bool horizontal, int value, bool draw)
	{
		if (horizontal)
			m_attrib.Set().dragValue.width = value;
		else
			m_attrib.Set().dragValue.height = value;
		CalcThumbBtnPos(horizontal);
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	int UIScroll::GetScrollPage(bool up, bool horz, float count)
	{
		auto& attrib = m_attrib.Get();
		const int value = horz ? attrib.dragValue.width : attrib.dragValue.height;
		const int range = horz ? attrib.range.width : attrib.range.height;
		int pageSize;
		if (horz)
		{
			pageSize = int(float(UINodeBase::m_data.Frame.GetWidth()) * count);
			if (!up)
				pageSize += value;
			else
				pageSize = value - pageSize;
		}
		else
		{
			pageSize = int(float(UINodeBase::m_data.Frame.GetHeight()) * count);
			if (!up)
				pageSize += value;
			else
				pageSize = value - pageSize;
		}
		if (pageSize < 0)
			pageSize = 0;
		else if (pageSize > range)
			pageSize = range;

		return pageSize;
	}

	void UIScroll::BindAttribute()
	{
		using Type = decltype(m_attrib);

		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::styleV, L"styleV"),
			MakeUIAttrib(m_attrib, &Attribute::styleH, L"styleH"),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::vertical, L"vertical", param,
			{
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::horizontal, L"horizontal", param,
			{
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttrib(m_attrib, &Attribute::active, L"active"),
			MakeUIAttrib(m_attrib, &Attribute::animate, L"animate"),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::button, L"button", param,
			{
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttrib(m_attrib, &Attribute::range, L"range",
			[](Type::SetData param)
			{
				param.Assign(&Attribute::range);
				if (param.data->range.width < param.data->dragValue.width)
						param.data->dragValue.width = param.data->range.width;
				if (param.data->range.height < param.data->dragValue.height)
						param.data->dragValue.height = param.data->range.height;
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttribEx(m_attrib, &UISize::height, &Attribute::range, L"rangeV",
			[](Type::SetData param)
			{
				param.data->range.height = param.GetValue<int>();
				if (param.data->range.height < param.data->dragValue.height)
					param.data->dragValue.height = param.data->range.height;
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttribEx(m_attrib, &UISize::width, &Attribute::range, L"rangeH",
			[](Type::SetData param)
			{
				param.data->range.width = param.GetValue<int>();
				if (param.data->range.width < param.data->dragValue.width)
					param.data->dragValue.width = param.data->range.width;
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttrib(m_attrib, &Attribute::dragValue, L"dragValue",
			[](Type::SetData param)
			{
				if (param.data->range.width < param.data->dragValue.width)
					param.data->dragValue.width = param.data->range.width;
				if (param.data->range.height < param.data->dragValue.height)
					param.data->dragValue.height = param.data->range.height;
				param.param->CalcThumbBtnPos(false);
				param.param->CalcThumbBtnPos(true);
				return true;
			}),
			MakeUIAttribEx(m_attrib, &UISize::height, &Attribute::dragValue, L"dragValueV",
			[](Type::SetData param)
			{
				param.data->dragValue.height = param.GetValue<int>();
				if (param.data->range.height < param.data->dragValue.height)
					param.data->dragValue.height = param.data->range.height;
				param.param->CalcThumbBtnPos(false);
				return true;
			}),
			MakeUIAttribEx(m_attrib, &UISize::width, &Attribute::dragValue, L"dragValueH",
			[](Type::SetData param)
			{
				param.data->dragValue.width = param.GetValue<int>();
				if (param.data->range.width < param.data->dragValue.width)
					param.data->dragValue.width = param.data->range.width;
				param.param->CalcThumbBtnPos(true);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::barWidth, L"barWidth", param,
			{
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::barMinHeight, L"barMinHeight", param,
			{
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::btnHeight, L"btnHeight", param,
			{
				param.param->CalcControlRect();
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::inset, L"inset", param,
			{
				param.param->CalcControlRect();
				return true;
			}),
		};

		Type::RegisterAttrib(list);
	}

	UIScroll::ScrollButtonType UIScroll::CalcPointIn(const UIPoint& pt)
	{
		UIPoint offset = pt;
		offset.x -= (int)UINodeBase::m_data.Frame.left;
		offset.y -= (int)UINodeBase::m_data.Frame.top;

		bool TypeH = false;
		auto ret = ButtonNull;
		auto& attrib = m_attrib.Get();
		if (attrib.vertical) 
		{
			if (Rect::IsPtInside(m_scroll[0].thumbButton, offset)) 
			{
				ret = ThumbButton;
				TypeH = false;
				goto End;
			}
			if (attrib.button)
			{
				if (Rect::IsPtInside(m_scroll[0].scrollBtn[0], offset)) 
				{
					ret = TopButton;
					TypeH = false;
					goto End;
				}
				if (Rect::IsPtInside(m_scroll[0].scrollBtn[1], offset)) 
				{
					ret = BottomButton;
					TypeH = false;
					goto End;
				}
			}
			if (Rect::IsPtInside(m_scroll[0].track, offset))
			{
				ret = Track;
				TypeH = false;
				goto End;
			}
		}
		if (attrib.horizontal) 
		{
			if (Rect::IsPtInside(m_scroll[1].thumbButton, offset)) 
			{
				ret = ThumbButton;
				TypeH = true;
				goto End;
			}
			if (attrib.button)
			{
				if (Rect::IsPtInside(m_scroll[1].scrollBtn[0], offset)) 
				{
					ret = TopButton;
					TypeH = true;
					goto End;
				}
				if (Rect::IsPtInside(m_scroll[1].scrollBtn[1], offset)) 
				{
					ret = BottomButton;
					TypeH = true;
					goto End;
				}
			}
			if (Rect::IsPtInside(m_scroll[1].track, offset)) 
			{
				ret = Track;
				TypeH = true;
			}
		}

	End:
		if (m_btnTypeH != TypeH && !isCapture()) 
		{
			m_isClick = TypeH;//已经改变了位置 按下状态无效
			m_btnTypeH = TypeH;
		}

		return ret;
	}

	bool UIScroll::OnMouseMove(_m_uint flag, const UIPoint& point)
	{
		auto type = CalcPointIn(point);
		if (m_btnType != type && !isCapture()) 
		{
			m_btnType = type;
			m_isClick = false;
			m_status = UIControlStatus_Hover;
			m_cacheUpdate = true;
			UpdateDisplay();
		} //拖动滑块
		else if (auto& attrib = m_attrib.Get(); m_btnType == ThumbButton && m_isClick)
		{
			UIPoint offset = { (int)UINodeBase::m_data.Frame.left, (int)UINodeBase::m_data.Frame.top };

			if (!m_btnTypeH)
			{
				int height = m_scroll[0].thumbButton.GetHeight();
				int top = point.y - offset.y;//让按钮顶边等于鼠标点击的位置
				top -= m_clickPos.y;//减去点击位置的距离

				//不能拖动超过滚动条轨道范围
				if (top < m_scroll[0].track.top)
					top = m_scroll[0].track.top;
				int bottom = top + height;
				if (bottom > m_scroll[0].track.bottom)
				{
					bottom = m_scroll[0].track.bottom;
					top = bottom - height;
				}
				m_scroll[0].thumbButton.top = top;
				m_scroll[0].thumbButton.bottom = bottom;

				CalcDragValue(false);
				if (attrib.callback)
					attrib.callback(this, attrib.dragValue.height, false);
			}
			else
			{
				int width = m_scroll[1].thumbButton.GetWidth();
				int left = point.x - offset.x;
				left -= m_clickPos.x;

				//不能拖动超过滚动条轨道范围
				if (left < m_scroll[1].track.left)
					left = m_scroll[1].track.left;
				int right = left + width;
				if (right > m_scroll[1].track.right)
				{
					right = m_scroll[1].track.right;
					left = right - width;
				}
				m_scroll[1].thumbButton.left = left;
				m_scroll[1].thumbButton.right = right;

				CalcDragValue(true);
				if (attrib.callback)
					attrib.callback(this, attrib.dragValue.width, true);
			}
			m_cacheUpdate = true;
			UpdateDisplay();
		}
		SendEvent(Event_Mouse_Move, (_m_param)&point);
		return m_btnType != ButtonNull;
	}

	bool UIScroll::OnMouseExited(_m_uint flag, const UIPoint& point)
	{
		if (!isCapture()) 
		{
			m_status = UIControlStatus_Normal;
			m_btnType = ButtonNull;
			m_cacheUpdate = true;
			UpdateDisplay();
		}
		SendEvent(Event_Mouse_Exited, (_m_param)&point);
		return false;
	}

	bool UIScroll::OnLButtonDown(_m_uint flag, const UIPoint& point)
	{
		auto type = CalcPointIn(point);
		m_btnType = type;
		m_isClick = true;
		m_status = UIControlStatus_Pressed;
		if (type != ButtonNull) 
		{
			if (type == ThumbButton)
			{
				SetCapture();
				UIPoint offset = { (int)UINodeBase::m_data.Frame.left, (int)UINodeBase::m_data.Frame.top };
				offset.x = point.x - offset.x;
				offset.y = point.y - offset.y;
				//计算出鼠标点击位置到按钮顶边的距离
				offset.y -= m_scroll[0].thumbButton.top;
				offset.x -= m_scroll[1].thumbButton.left;
				m_clickPos = offset;
			}
			m_cacheUpdate = true;
			UpdateDisplay();
		}
		SendEvent(Event_Mouse_LDown, (_m_param)&point);
		return m_btnType != ButtonNull;
	}

	bool UIScroll::OnLButtonUp(_m_uint flag, const UIPoint& point)
	{
		auto type = CalcPointIn(point);
		m_btnType = type;
		m_status = UIControlStatus_Hover;
		if (auto& attrib = m_attrib.Get(); m_isClick)
		{
			if (type != ButtonNull && (type == TopButton || type == BottomButton || type == Track)) 
			{
				//每次递增或递减半个page的内容
				int value = m_btnTypeH ? attrib.dragValue.width : attrib.dragValue.height;
				if (type == TopButton)
				{
					value = GetScrollPage(true, m_btnTypeH, 0.5f);
				}
				else if (type == BottomButton)
				{
					value = GetScrollPage(false, m_btnTypeH, 0.5f);
				}
				//每次递增或递减一个page的内容
				else if (type == Track)
				{
					UIPoint offset = point;
					if (m_btnTypeH)
					{
						offset.x -= (int)UINodeBase::m_data.Frame.left;
						value = GetScrollPage(offset.x < m_scroll[1].thumbButton.right, true, 1.f);
					}
					else
					{
						offset.y -= (int)UINodeBase::m_data.Frame.top;
						value = GetScrollPage(offset.y < m_scroll[0].thumbButton.bottom, false, 1.f);
					}
				}
				SetDragValue(m_btnTypeH, value, true);
				if (attrib.callback)
						attrib.callback(this, m_btnTypeH ? attrib.dragValue.width : attrib.dragValue.height, m_btnTypeH);
			}
			ReleaseCapture();
			m_isClick = false;
		}
		SendEvent(Event_Mouse_LUp, (_m_param)&point);
		return m_btnType != ButtonNull;
	}

	bool UIScroll::OnMouseWheel(_m_uint flag, short delta, const UIPoint& point)
	{
		if (m_btnType != ButtonNull || m_isClick || m_ALLWheel) 
		{
			auto& attrib = m_attrib.Get();
			//每次递增或递减1/2的内容
			int value;
			if (delta > 0)
				value = GetScrollPage(true, m_btnTypeH, 0.5f * 1.f);
			else
				value = GetScrollPage(false, m_btnTypeH, 0.5f * 1.f);

			SetDragValue(m_btnTypeH, value, true);
			if (attrib.callback)
				attrib.callback(this, m_btnTypeH ? attrib.dragValue.width : attrib.dragValue.height, m_btnTypeH);
		}
		UIControl::OnMouseWheel(flag, delta, point);
		return m_btnType != ButtonNull;
	}

	void UIScroll::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (m_attrib.SetAttribute(attribName, attrib, this))
		{
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		else
			UIControl::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UIScroll::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret))
			return ret;
		return UIControl::GetAttribute(attribName);
	}
}
