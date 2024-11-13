/**
 * FileName: Mui_Slider.h
 * Note: UI滑块条控件声明
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

#pragma once
#include <Control/Mui_Control.h>

namespace Mui::Ctrl
{
	/* UISlider 控件
	* 属性列表:
	* style (UIStyle*)
	* btnStyle (UIStyle*)				  - 滑块按钮样式
	* maxValue (int)					  - 最大值
	* minValue (int)					  - 最小值
	* value (int)						  - 当前值
	* vertical (bool)					  - 是否垂直
	* leftShow (bool)					  - 如果非垂直 进度从左往右显示 (默认true)
	* bottomShow (bool)					  - 如果非水平 进度从下往上显示 (默认true)
	* trackInset (_m_rect_t<int>) 4x	  - 轨道内边距
	* btnSize (UISize) 2x				  - 拖拽按钮尺寸 0=自动计算
	*/
	class UISlider : public UIControl
	{
	public:
		MCTRL_DEFINE
		(
			L"UISlider",
			{ CtrlMgr::AttribType::UIStyle, L"style" },
			{ CtrlMgr::AttribType::UIStyle, L"btnStyle" }
		);
		MCTRL_DEFSLOT
		(
			CtrlMgr::EventSlot<int> valueChanged;//滑块值已更改
		);

		struct Attribute
		{
			UIStylePtr style = nullptr;
			UIStylePtr btnStyle = nullptr;

			int maxValue = 100;
			int minValue = 0;
			int value = 0;
			bool vertical = false;
			bool leftShow = true;
			bool bottomShow = true;
			_m_rect_t<int> trackInset = { 0 };
			UISize btnSize;
		};

		UISlider(UIControl* parent, Attribute attrib);

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		template<typename T>
		bool SetAttributeSrc(std::wstring_view attribName, T&& value, bool draw = true)
		{
			if (m_attrib.SetAttribute(attribName, std::forward<T>(value), draw))
			{
				m_cacheUpdate = true;
				if (draw) UpdateDisplay();
				return true;
			}
			return false;
		}
		[[nodiscard]] const Attribute& GetAttribute() const { return m_attrib.Get(); }

		//设置当前值
		void SetCurValue(int value, bool draw = true);
		//获取当前值
		int GetCurValue() const;
		//设置最大值
		void SetMaxValue(int max, bool draw = true);
		//获取最大值
		int GetMaxValue() const;
		//设置最小值
		void SetMinValue(int min, bool draw = true);
		//获取最小值
		int GetMinValue() const;

	protected:
		enum UISliderStatus
		{
			UISliderTrackNormal,		//轨道背景 普通
			UISliderTrackDisable = 2,	//轨道背景 禁止
			UISliderBlockNormal = 1,	//块前景 普通
			UISliderBlockDisable = 3,	//块前景 禁止
			UISliderBtnNormal = 0,		//拖拽按钮 普通
			UISliderBtnHover,			//拖拽按钮 热点
			UISliderBtnPressed,			//拖拽按钮 按下
			UISliderBtnDisable			//拖拽按钮 禁止
		};

		explicit UISlider(Attribute attrib);

		void OnPaintProc(MPCPaintParam param) override;

		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;

		//计算百分比值
		int Percentage(int value1, int percentValue, int percent);
		//计算矩形
		void CalcDrawRect();
		//计算值
		int CalcPtValue(UIPoint pt);

	private:
		CtrlMgr::UIAttribute<Attribute> m_attrib;
		static void BindAttribute();

		UISliderStatus btnStatus = UISliderBtnNormal;

		UIRect m_drawRect;
		UIRect m_drawBtnRect;
		UIRect m_drawBgRect;
		bool m_isClick = false;
		int m_oldValue = 0;
		UIPoint m_downpos;
		bool m_hitbtn = false;
	};

}
