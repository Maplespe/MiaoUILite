/**
 * FileName: Mui_Label.cpp
 * Note: UI标签控件实现
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
 * date: 2020-10-23 Create
*/

#include <Control/Mui_Label.h>
#ifdef __ANDROID__
#include <Mui_JNIHelper.h>
#endif

namespace Mui::Ctrl
{
	using namespace Helper;

	UILabel::UILabel(UIControl* parent, Attribute attrib) : UILabel(std::move(attrib))
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
	}

	UILabel::UILabel(Attribute attrib) : m_attrib(std::move(attrib))
	{
		m_cacheSupport = true;
		UINodeBase::m_data.AutoSize = true;
	}

	void UILabel::Register()
	{
		BindAttribute();

		static auto method = [](UIControl* parent)
		{
			return new UILabel(parent, Attribute());
		};
		MCTRL_REGISTER(method);
	}

	void UILabel::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if(!m_attrib.SetAttribute(attribName, attrib, std::make_pair(this, draw)))
			UIControl::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UILabel::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret))
			return ret;
		return UIControl::GetAttribute(attribName);
	}

	UISize UILabel::GetTextMetric(bool dpi) const
	{
		auto attrib = m_attrib.Get();
		if(!dpi && IsDPIScaleEnabled())
		{
			m_font->SetFontSize(attrib.fontSize, std::make_pair(0u, (_m_uint)attrib.text.length()));
		}
		const auto rect = m_font->GetMetrics();
		if(!dpi && IsDPIScaleEnabled())
		{
			auto scale = GetRectScale().scale();
			auto fontSize = (_m_ushort)(M_MIN(scale.cx, scale.cy) * (float)attrib.fontSize);
			m_font->SetFontSize(fontSize, std::make_pair(0u, (_m_uint)attrib.text.length()));
		}
		return { rect.GetWidth(), rect.GetHeight() };
	}

	UISize UILabel::CalcSize()
	{
		const UIRect rect = m_font->GetMetrics();
		UIPoint offset;
		if (auto& attrib = m_attrib.Get(); attrib.shadowUse)
		{
			offset = attrib.shadowOffset;
			auto scale = GetRectScale().scale();
			offset.x = _scale_to(offset.x, scale.cx);
			offset.y = _scale_to(offset.y, scale.cy);
		}
		return { rect.GetWidth() + offset.x, rect.GetHeight() + offset.y };
	}

	void UILabel::OnLoadResource(MRenderCmd* render, bool recreate)
	{
		UINodeBase::OnLoadResource(render, recreate);

		auto& attrib = m_attrib.Get();

		m_effect = nullptr;

		auto scale = GetRectScale().scale();
		const float fontSize = M_MIN(scale.cx, scale.cy) * (float)attrib.fontSize;

		m_font = render->CreateFonts(attrib.text.cstr(), attrib.font.cstr(), (_m_uint)fontSize, attrib.fontCustom);
		m_brush = render->CreateBrush(attrib.fontColor);
		m_font->SetFontStyle(attrib.fontStyle, std::make_pair(0u, (_m_uint)attrib.text.length()));

	}

	void UILabel::OnPaintProc(MPCPaintParam param)
	{
		auto& attrib = m_attrib.Get();
		if (attrib.text.empty())
			return;

		UIRect drawRect = *param->destRect;
		if (OffsetDraw)
			drawRect = OffsetDrawRc;

		if (!m_brush) return;

		m_brush->SetOpacity(param->blendedAlpha);

		//如果绘制字体阴影
		if (attrib.shadowUse)
		{
			//如果没有效果 创建一个效果
			if (!m_effect)
				m_effect = param->render->CreateEffects(MEffects::GaussianBlur, attrib.shadowBlur);
			else
				m_effect->SetEffectValue(attrib.shadowBlur);

			m_brush->SetColor(attrib.shadowColor);

			//借用共享画布渲染透明背景文字
			MCanvas* canvasTmp = param->render->GetSharedCanvas();
			//设置渲染对象为临时画布
			param->render->SetCanvas(canvasTmp);
			//设置裁剪区
			param->render->PushClipRect(drawRect);
			param->render->Clear();//清空临时画布的旧内容
			param->render->DrawTextLayout(m_font, drawRect, m_brush, attrib.textAlign);//绘制字体
			param->render->PopClipRect();

			//将临时画布上的文字 作为效果输入 渲染到主画布
			if (!param->cacheCanvas)
				param->render->SetCanvas(param->render->GetRenderCanvas());
			else
				param->render->SetCanvas(UINodeBase::m_data.SubAtlas.get());

			if (attrib.shadowLow)
			{

				drawRect.Offset(attrib.shadowOffset.x, attrib.shadowOffset.y);
				param->render->DrawBitmap(canvasTmp, 255, drawRect, drawRect);
				drawRect.ResetOffset();
			}
			else 
			{
				UIRect _draw_ = drawRect;
				_draw_.Offset(attrib.shadowOffset.x, attrib.shadowOffset.y);
				param->render->DrawBitmapEffects(canvasTmp, m_effect, 255, _draw_, drawRect);
			}
		}

		//设置热点颜色
		if (attrib.hyperlink && m_mouseIn)
			m_brush->SetColor(attrib.urlColor);
		else
			m_brush->SetColor(attrib.fontColor);

		param->render->DrawTextLayout(m_font, drawRect, m_brush, attrib.textAlign);
	}

	bool UILabel::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		bool ret = false;

		auto& attrib = m_attrib.Get();

		switch (message)
		{
		case M_SETCURSOR:
			if (attrib.hyperlink)
				ret = true;
			break;
		case M_MOUSE_HOVER:
			m_mouseIn = true;
			if (attrib.hyperlink)
			{
#ifdef _WIN32
				SetCursor(IDC_HAND);
#endif
				m_cacheUpdate = true;
				UpdateDisplay();
				ret = true;
			}
			break;
		case M_MOUSE_LEAVE:
			m_mouseIn = false;
			if (attrib.hyperlink)
			{
#ifdef _WIN32
				SetCursor(IDC_ARROW);
#endif
				m_cacheUpdate = true;
				UpdateDisplay();
				ret = true;
			}
			break;
		case M_MOUSE_LBDOWN:
			m_isClick = true;
			if (attrib.hyperlink) 
			{
				m_cacheUpdate = true;
				UpdateDisplay();
				ret = true;
			}
			break;
		case M_MOUSE_LBUP:
			if (m_isClick)
			{
				if (attrib.hyperlink)
				{
#ifdef _WIN32
					SetCursor(IDC_HAND);
					if (!attrib.url.empty())
					{
						ShellExecuteW(nullptr, L"Open", attrib.url.cstr(), nullptr, nullptr, SW_SHOWNORMAL);
					}
#elif __ANDROID__
						if (m_attrib.Url != L"")
							JNI::OpenURI(attrib.url);
#else
#error __TODO__
#endif
				}
				UIPoint point{ (int)M_LOWORD((_m_long)lParam), (int)M_HIWORD((_m_long)lParam) };
				SendEvent(Event_Mouse_LClick, (_m_param)&point);

				mslot.clicked.Emit(point);

				m_cacheUpdate = true;
				UpdateDisplay();
				ret = true;
			}
			m_isClick = false;
			break;
		default: 
			break;
		}
		UIControl::OnMouseMessage(message, wParam, lParam);
		return ret;
	}

	void UILabel::OnScale(_m_scale scale)
	{
		UIControl::OnScale(scale);
		auto& attrib = m_attrib.Get();

		scale = GetRectScale().scale();
		const float fontSize = M_MIN(scale.cx, scale.cy) * (float)attrib.fontSize;
		if (m_font)
			m_font->SetFontSize((_m_uint)fontSize, std::make_pair(0, (_m_uint)attrib.text.length()));
	}

	_m_sizef UILabel::GetContentSize()
	{
		_m_sizef size = UINodeBase::GetContentSize();
		if (UINodeBase::m_data.AutoSize)
		{
			const UISize textSize = CalcSize();

			size.width = M_MAX(size.width, (float)textSize.width);
			size.height = M_MAX(size.height, (float)textSize.height);
		}
		return size;
	}

	void UILabel::BindAttribute()
	{
		using namespace CtrlMgr;
		using Type = decltype(m_attrib);

		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::fontStyle, L"fontStyle", 
			[](Type::SetData param)
			{
				Attribute defaults;
				auto style = param.GetValue<Attribute*>();
				if (!style) style = &defaults;

				const auto range = std::make_pair(0u, (_m_uint)style->text.length());
				const auto _this = param.param.first;

				if (style->fontCustom != param.data->fontCustom)
					_this->m_font = _this->m_render->CreateFonts(style->text.cstr(), style->font.cstr(), style->fontSize, style->fontCustom);
				else
				{
					_this->m_font->SetText(style->text.cstr());
					_this->m_font->SetFontName(style->font.cstr());
					_this->m_font->SetFontSize(style->fontSize, range);
				}
				_this->m_font->SetFontStyle(style->fontStyle, range);

				*param.data = *style;
				return _this->updateRender(param.param.second, true);
			},
			[](Type::GetData param)
			{
				return CtrlMgr::ConvertAttribPtr(&param.data->fontStyle);
			}),

			decltype(m_attrib)::MakeAttrib(&Attribute::text, L"text", [](Type::SetData param)
			{
				if (param.data->text.view() == param.attribValue) 
					return true;
				param.Assign(&Attribute::text);
				param.param.first->m_font->SetText(param.data->text.cstr());
				return param.param.first->updateRender(param.param.second, true);
			}),

			MakeUIAttrib(m_attrib, &Attribute::font, L"font",
			[](Type::SetData param)
			{
				if (param.data->font.view() == param.attribValue)
					return true;

				param.data->font = param.attribValue;
				param.param.first->m_font->SetFontName(param.data->font.cstr());

				return param.param.first->updateRender(param.param.second, true);
			}),

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
				param.param.first->m_font->SetFontSize(fontSize, { 0u, (_m_uint)param.data->text.length() });

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

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::fontColor, L"fontColor", param,
			{
				return param.param.first->updateRender(param.param.second);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::hyperlink, L"hyperlink", param,
			{
				return param.param.first->updateRender(param.param.second);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::url, L"url", param,
			{
				return param.param.first->updateRender(param.param.second);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::urlColor, L"urlColor", param,
			{
				return param.param.first->updateRender(param.param.second);
			}),

			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::textAlign, L"textAlign", param,
			{
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
				_this->m_font = _this->m_render->CreateFonts(param.data->text.cstr(), param.data->font.cstr(), fontSize, fontc);
				_this->m_font->SetFontStyle(param.data->fontStyle, std::make_pair(0u, (_m_uint)param.data->text.length()));

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
		};

		decltype(m_attrib)::RegisterAttrib(list);
	}

	void UILabel::updateStyle(UIFontStyle& style)
	{
		m_font->SetFontStyle(style, std::make_pair(0u, (_m_uint)m_attrib.Get().text.length()));
	}

	bool UILabel::updateRender(bool draw, bool layout)
	{
		m_cacheUpdate = true;
		if (!draw) return true;
		if (layout && UINodeBase::m_data.AutoSize) UpdateLayout();
		else UpdateDisplay();
		return true;
	}

	bool UILabel::OnSetCursor(_m_param hCur, _m_param lParam)
	{
#ifdef _WIN32
		::SetCursor((HCURSOR)hCur);
#endif
		return true;
	}
}
