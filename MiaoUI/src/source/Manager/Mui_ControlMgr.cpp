/**
 * FileName: Mui_ControlMgr.cpp
 * Note: UI控件管理器实现
 *
 * Copyright (C) 2022 Maplespe (mapleshr@icloud.com)
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
 * date: 2022-3-8 Create
*/

#include <Manager/Mui_ControlMgr.h>
//MiaoUI 控件
#include <Control/Mui_Button.h>
#include <Control/Mui_CheckBox.h>
#include <Control/Mui_Combox.h>
#include <Control/Mui_EditBox.h>
#include <Control/Mui_ImgBox.h>
#include <Control/Mui_Label.h>
#include <Control/Mui_ListBox.h>
#include <Control/Mui_Progress.h>
#include <Control/Mui_Scroll.h>
#include <Control/Mui_Slider.h>
#include <Control/Mui_ColorPicker.h>
#include <Control/Mui_NavBar.h>

namespace Mui::CtrlMgr
{

	using namespace Ctrl;

	struct RegData
	{
		RegMethod method;
		std::vector<AttribType> resAttrib;
	};

	bool g_isRegMui = false;
	std::unordered_map<std::wstring, RegData> g_regList;

	bool RegisterControl(std::wstring_view name, RegMethod&& method, const std::vector<AttribType>& ptrAttrib)
	{
		auto iter = g_regList.find(name.data());
		if (iter != g_regList.end())
			return false;
		g_regList.insert(std::make_pair(name, RegData{ method, ptrAttrib }));
		return true;
	}

	void RegisterMuiControl()
	{
		if (!g_isRegMui)
		{
			UIButton::Register();
			UICheckBox::Register();
			UIComBox::Register();
			UIControl::Register();
			UIEditBox::Register();
			UIImgBox::Register();
			UILabel::Register();
			UIListBox::Register();
			UIProgressBar::Register();
			UIScroll::Register();
			UISlider::Register();
			UIColorPicker::Register();
			UINavBar::Register();
		}
	}

	UIControl* CreateControl(std::wstring_view name, UIControl* parent)
	{
		UIControl* ret = nullptr;
		auto iter = g_regList.find(name.data());
		if (iter != g_regList.end() && iter->second.method)
		{
			ret = iter->second.method(parent);
		}
		return ret;
	}

	AttribType::type GetAttributeType(std::wstring_view name, std::wstring_view attribName)
	{
		auto iter = g_regList.find(name.data());
		if (iter != g_regList.end())
		{
			for (auto& type : iter->second.resAttrib)
			{
				if (type.attribName == attribName)
					return type.attribType;
			}
		}
		return AttribType::defaults;
	}

#define DEF_ConverGet [](std::wstring_view value) -> std::any
#define DEF_ConverSet [](std::any value) -> std::wstring
#define DEF_NumberConver(type, func) AddMethod<type>( \
	DEF_ConverGet{ return std::make_any<type>(func(value.data())); }, \
		DEF_ConverSet{ return std::to_wstring(std::any_cast<type>(value)); } \
	)
#define DEF_MPtrConver(type) AddMethod<type>( \
	DEF_ConverGet{ return std::make_any<type>(type((type::Type*)std::stoull(value.data()))); }, \
		DEF_ConverSet{ return std::to_wstring((_m_ptrv)std::any_cast<type>(value).get()); } \
	)

	std::unordered_map<std::string, AttribConverter::data> AttribConverter::m_methodList;

	template<typename T, int _count>
	std::any AttribInt_ConvGetMethod(std::wstring_view value)
	{
		std::vector<int> dstValue;
		Helper::M_GetAttribValueInt(value, dstValue, _count);
		if constexpr (_count == 4)
		{
			return std::make_any<T>(T{ dstValue[0], dstValue[1], dstValue[2], dstValue[3] });
		}
		else
			return std::make_any<T>(T{ dstValue[0], dstValue[1] });
	}

	template<typename T>
	std::wstring RectInt_ConvSetMethod(std::any value)
	{
		auto rect = std::any_cast<T>(value);
		return Attrib::Value_Make4x(rect.left, rect.top, rect.right, rect.bottom);
	}

	AttribConverter::AttribConverter()
	{
		//bool
		AddMethod<bool>(
			DEF_ConverGet{ return std::make_any<bool>(value == L"true"); },
			DEF_ConverSet{ return std::any_cast<bool>(value) ? L"true" : L"false"; }
		);
		//float
		DEF_NumberConver(float, std::stof);
		//double
		DEF_NumberConver(double, std::stod);
		//wstring
		AddMethod<std::wstring>(
			DEF_ConverGet{ return std::make_any<std::wstring>(value); },
			DEF_ConverSet{ return std::any_cast<std::wstring>(value); }
		);
		//wstring_view
		AddMethod<std::wstring_view>(
			DEF_ConverGet{ return std::make_any<std::wstring_view>(value); },
			DEF_ConverSet{ return std::any_cast<std::wstring_view>(value).data(); }
		);
		//wchar_t
		AddMethod<wchar_t>(
			DEF_ConverGet{ return std::make_any<wchar_t>(value[0]);},
			DEF_ConverSet{ std::wstring str; str = std::any_cast<wchar_t>(value); return str; }
		);

		//_m_uchar, _m_byte, uint_8
		DEF_NumberConver(_m_uchar, (_m_uchar)std::stoi);
		//_m_short, int_16
		DEF_NumberConver(_m_short, (_m_short)std::stoi);
		//_m_ushort
		DEF_NumberConver(_m_ushort, (_m_ushort)std::stoi);
		//_m_int, _m_long, int_32
		DEF_NumberConver(_m_int, (_m_int)std::stoi);
		//_m_uint, _m_ulong, uint_32
		DEF_NumberConver(_m_uint, (_m_uint)std::stoul);
		//_m_param, _m_long64, int_64
		DEF_NumberConver(_m_param, std::stoll);
		//_m_ulong64, _m_size, _m_ptrv, uint_64
		DEF_NumberConver(_m_ulong64, std::stoull);

		//void*, _m_ptr
		AddMethod<void*>(
			DEF_ConverGet{ return std::make_any<void*>((void*)std::stoull(value.data())); },
			DEF_ConverSet{ return std::to_wstring((_m_ptrv)std::any_cast<void*>(value)); }
		);

		//UIStyle*
		AddMethod<UIStyle*>(
			DEF_ConverGet{ return std::make_any<UIStyle*>((UIStyle*)std::stoull(value.data())); },
			DEF_ConverSet{ return std::to_wstring((_m_ptrv)std::any_cast<UIStyle*>(value)); }
		);

		//UIBitmap*
		AddMethod<UIBitmap*>(
			DEF_ConverGet{ return std::make_any<UIBitmap*>((UIBitmap*)std::stoull(value.data())); },
			DEF_ConverSet{ return std::to_wstring((_m_ptrv)std::any_cast<UIBitmap*>(value)); }
		);

		//_m_color
		AddMethod<_m_color>(
			DEF_ConverGet{ return std::make_any<_m_color>(Helper::M_GetAttribValueColor(value)); },
			DEF_ConverSet
			{
				_m_color color = std::any_cast<_m_color>(value);
				return Color::M_RGBA_STR(color.r, color.g, color.b, color.a);
			}
		);

		//_m_rect
		AddMethod<_m_rect>(AttribInt_ConvGetMethod<_m_rect, 4>, RectInt_ConvSetMethod<_m_rect>);
		//_m_rect_t<int>
		AddMethod<_m_rect_t<int>>(AttribInt_ConvGetMethod<_m_rect_t<int>, 4>, RectInt_ConvSetMethod<_m_rect_t<int>>);
		//UIRect
		AddMethod<UIRect>(AttribInt_ConvGetMethod<UIRect, 4>, RectInt_ConvSetMethod<UIRect>);

		//UIPoint
		AddMethod<UIPoint>(
			AttribInt_ConvGetMethod<UIPoint, 2>,
			DEF_ConverSet
			{
				auto pt = std::any_cast<UIPoint>(value);
				return Attrib::Value_Make2x(pt.x, pt.y);
			}
		);
		//UISize
		AddMethod<UISize>(
			AttribInt_ConvGetMethod<UISize, 2>,
			DEF_ConverSet
			{
				auto size = std::any_cast<UISize>(value);
				return Attrib::Value_Make2x(size.width, size.height);
			}
		);

		//UIStylePtr
		DEF_MPtrConver(UIStylePtr);
		//UIBitmapPtr
		DEF_MPtrConver(UIBitmapPtr);
	}
 }
