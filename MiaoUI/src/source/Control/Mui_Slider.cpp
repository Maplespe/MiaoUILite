/**
 * FileName: Mui_Slider.cpp
 * Note: UI滑块条控件实现
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
 * date: 2020-12-20 Create
*/

#include <Control/Mui_Slider.h>

namespace Mui::Ctrl
{
	UISlider::UISlider(UIControl* parent, Attribute attrib) : UISlider(std::move(attrib))
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
	}

	UISlider::UISlider(Attribute attrib) : m_attrib(std::move(attrib))
	{
		m_cacheSupport = true;
	}

	void UISlider::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (m_attrib.SetAttribute(attribName, attrib))
		{
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		else
			UIControl::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UISlider::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret))
			return ret;
		return UIControl::GetAttribute(attribName);
	}

	void UISlider::Register()
	{
		BindAttribute();

		static auto method = [](UIControl* parent)
		{
			return new UISlider(parent, Attribute());
		};
		MCTRL_REGISTER(method);
	}

	void UISlider::SetCurValue(int value, bool draw)
	{
		auto ptr = m_attrib.GetPtr();
		if (value > ptr->maxValue)
			value = ptr->maxValue;
		if (value < 0)
			value = 0;
		ptr->value = value;
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	int UISlider::GetCurValue() const
	{
		return m_attrib.Get().value;
	}

	void UISlider::SetMaxValue(int max, bool draw)
	{
		if (max < 0)
			max = 0;
		m_attrib.Set().maxValue = max;
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	int UISlider::GetMaxValue() const
	{
		return m_attrib.Get().maxValue;
	}

	void UISlider::SetMinValue(int min, bool draw)
	{
		if (min < 0)
			min = 0;
		m_attrib.Set().minValue = min;
		m_cacheUpdate = true;
		if (draw)
			UpdateDisplay();
	}

	int UISlider::GetMinValue() const
	{
		return m_attrib.Get().minValue;
	}

	void UISlider::OnPaintProc(MPCPaintParam param)
	{
		CalcDrawRect();
		UIRect dstDraw = m_drawRect;
		UIRect dstBg = m_drawBgRect;
		UIRect dstBtn = m_drawBtnRect;
		if (param->cacheCanvas)
		{
			int x = -(int)UINodeBase::m_data.Frame.left;
			x += param->destRect->left;
			int y = -(int)UINodeBase::m_data.Frame.top;
			y += param->destRect->top;
			dstDraw.Offset(x, y);
			dstBg.Offset(x, y);
			dstBtn.Offset(x, y);
		}
		_m_byte alpha = param->blendedAlpha;

		auto scale = GetRectScale().scale();
		if (const auto style = m_attrib.Get().style)
		{
			style->PaintStyle(param->render, &dstBg, alpha, IsEnabled() ? UISliderTrackNormal : UISliderTrackDisable, 4, scale);
			style->PaintStyle(param->render, &dstDraw, alpha, IsEnabled() ? UISliderBlockNormal : UISliderBlockDisable, 4, scale);
		}
		if(const auto style = m_attrib.Get().btnStyle)
			style->PaintStyle(param->render, &dstBtn, alpha, IsEnabled() ? btnStatus : UISliderBtnDisable, 4, scale);

		if (auto&& dbgframe = GetDbgFramePen())
			param->render->DrawRectangle(dstBtn, dbgframe);
	}

	bool UISlider::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		using namespace Helper;
		const UIPoint pt = M_GetMouseEventPt(lParam);
		switch (message)
		{
		case M_MOUSE_MOVE:
			{
				if (Rect::IsPtInside(m_drawBtnRect, pt) &&
					btnStatus != UISliderBtnHover && btnStatus != UISliderBtnPressed)
				{
					btnStatus = UISliderBtnHover;
					m_cacheUpdate = true;
					UpdateDisplay();
				}
				else if (!m_isClick && !Rect::IsPtInside(m_drawBtnRect, pt) &&
					btnStatus == UISliderBtnHover)
				{
					btnStatus = UISliderBtnNormal;
					m_cacheUpdate = true;
					UpdateDisplay();
				}
				if (m_isClick) 
				{
					SetCurValue(CalcPtValue(pt));
					if (int value = m_attrib.Get().value; m_oldValue != value)
					{
						m_oldValue = value;
						SendEvent(Event_Slider_Change, value);
						mslot.valueChanged.Emit(value);
					}
				}
				break;
			}
		case M_MOUSE_LBDOWN:
			{
				m_isClick = true;
				m_hitbtn = Rect::IsPtInside(m_drawBtnRect, pt);
				m_downpos = pt;
				m_downpos.x = pt.x - m_drawBtnRect.left;
				m_downpos.y = pt.y - m_drawBtnRect.top;

				if(!m_hitbtn)
					SetCurValue(CalcPtValue(pt));
				if (int value = m_attrib.Get().value; m_oldValue != value)
				{
					m_oldValue = value;
					SendEvent(Event_Slider_Change, value);
					mslot.valueChanged.Emit(value);
				}
				SetCapture();
				break;
			}
		case M_MOUSE_LBUP:
			{
				if (m_isClick)
				{
					ReleaseCapture();
					m_isClick = false;
					m_hitbtn = false;
					SendEvent(Event_Mouse_LClick);
				}
				break;
			}
		case M_MOUSE_LEAVE:
			{
				btnStatus = UISliderBtnNormal;
				m_cacheUpdate = true;
				UpdateDisplay();
				break;
			}
		default:
			break;
		}
		return UIControl::OnMouseMessage(message, wParam, lParam);
	}

	int UISlider::Percentage(int value1, int percentValue, int percent)
	{
		//化小数运算 避免乘法溢出
		const float v1 = (float)value1 / 100.f;
		const float v2 = (float)percentValue / 100.f;
		const float v3 = (float)percent / 100.f;
		const float ret = v1 * v2 / v3;
		return (int)(ret * 100.f);
	}

	void UISlider::CalcDrawRect()
	{
		m_drawRect = Frame();

		auto& attrib = m_attrib.Get();

		_m_scale scale = GetRectScale().scale();

		//track内边距
		auto trackInset = attrib.trackInset;
		m_drawRect.left += _scale_to(trackInset.left, scale.cx);
		m_drawRect.top +=  _scale_to(trackInset.top, scale.cy);
		m_drawRect.right -= _scale_to(trackInset.right, scale.cx);
		m_drawRect.bottom -= _scale_to(trackInset.bottom, scale.cy);

		m_drawBgRect = m_drawRect;

		int frameHeight = (int)UINodeBase::m_data.Frame.GetHeight();
		int frameWidth = (int)UINodeBase::m_data.Frame.GetWidth();

		//按钮矩形计算 如果=0自动计算
		UISize btnSize = attrib.btnSize;
		btnSize.width = _scale_to(btnSize.width, scale.cx);
		btnSize.height = _scale_to(btnSize.height, scale.cy);

		if (attrib.vertical)
		{
			//防止按钮大小超出frame
			if (btnSize.width > frameWidth || btnSize.width == 0)
				btnSize.width = frameWidth;
			if (btnSize.height == 0)
				btnSize.height = frameWidth;

			//track值和按钮矩形计算
			int btnOffset = btnSize.height / 2;
			int trackHeight = m_drawRect.GetHeight();
			int calcValue = Percentage(attrib.value, trackHeight - btnSize.height, attrib.maxValue);

			if (attrib.bottomShow)
			{
				m_drawRect.top += trackHeight - calcValue - btnOffset;
				m_drawBtnRect.top = m_drawRect.top - btnOffset;
			}
			else
			{
				m_drawRect.bottom -= trackHeight - calcValue - btnOffset;

				m_drawBtnRect.top = m_drawRect.bottom - btnOffset;
			}
			m_drawBtnRect.bottom = m_drawBtnRect.top + btnSize.height;
			//如果按钮小于frame 居中显示
			m_drawBtnRect.left = (int)UINodeBase::m_data.Frame.left + ((frameWidth - btnSize.width) / 2);
			m_drawBtnRect.right = m_drawBtnRect.left + btnSize.width;
		}
		else
		{
			if (btnSize.height > frameHeight || btnSize.height == 0)
				btnSize.height = frameHeight;
			if (btnSize.width == 0)
				btnSize.width = frameHeight;

			int btnOffset = btnSize.width / 2;
			int trackWidth = m_drawRect.GetWidth();
			int calcValue = Percentage(attrib.value, trackWidth - btnSize.width, attrib.maxValue);
			if (!attrib.leftShow)
			{
				m_drawRect.left += trackWidth - calcValue - btnOffset;
				m_drawBtnRect.left = m_drawRect.left - btnOffset;
			}
			else
			{
				m_drawRect.right -= trackWidth - calcValue - btnOffset;
				m_drawBtnRect.left = m_drawRect.right - btnOffset;
			}
			m_drawBtnRect.right = m_drawBtnRect.left + btnSize.width;
			m_drawBtnRect.top = (int)UINodeBase::m_data.Frame.top + ((frameHeight - btnSize.height) / 2);
			m_drawBtnRect.bottom = m_drawBtnRect.top + btnSize.height;
		}
	}

	int UISlider::CalcPtValue(UIPoint pt)
	{
		//鼠标在控件内的位置
		const UIPoint curPt =
		{
			pt.x - m_drawBgRect.left,
			pt.y - m_drawBgRect.top
		};

		auto& attrib = m_attrib.Get();
		int ret;
		if (attrib.vertical)
		{
			int height[2] = { m_drawBtnRect.GetHeight(), m_drawBgRect.GetHeight() };
			int offset = height[0] / 2;
			if (m_hitbtn)
				offset = m_downpos.y;
			int calcValue = Helper::M_MIN(height[1], curPt.y - offset);
			ret = Percentage(calcValue, attrib.maxValue, height[1] - height[0]);
			//反向
			if (attrib.bottomShow)
				ret = (int)attrib.maxValue - ret;
		}
		else
		{
			int width[2] = { m_drawBtnRect.GetWidth(), m_drawBgRect.GetWidth() };
			int offset = width[0] / 2;
			if (m_hitbtn)
				offset = m_downpos.x;
			int calcValue = Helper::M_MIN(width[1], curPt.x - offset + 1);
			ret = Percentage(calcValue, attrib.maxValue, width[1] - width[0]);

			if (!attrib.leftShow)
				ret = (int)attrib.maxValue - ret;

			m_cacheUpdate = true;
			UpdateDisplay();
		}
		if (ret < 0)
			ret = 0;
		if (ret < attrib.minValue)
			ret = attrib.minValue;

		return ret;
	}

	void UISlider::BindAttribute()
	{
		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::style, L"style"),
			MakeUIAttrib(m_attrib, &Attribute::btnStyle, L"btnStyle"),
			MakeUIAttrib(m_attrib, &Attribute::maxValue, L"maxValue"),
			MakeUIAttrib(m_attrib, &Attribute::minValue, L"minValue"),
			MakeUIAttrib(m_attrib, &Attribute::value, L"value"),
			MakeUIAttrib(m_attrib, &Attribute::vertical, L"vertical"),
			MakeUIAttrib(m_attrib, &Attribute::leftShow, L"leftShow"),
			MakeUIAttrib(m_attrib, &Attribute::bottomShow, L"bottomShow"),
			MakeUIAttrib(m_attrib, &Attribute::trackInset, L"trackInset"),
			MakeUIAttrib(m_attrib, &Attribute::btnSize, L"btnSize")
		};

		decltype(m_attrib)::RegisterAttrib(list);
	}
}
