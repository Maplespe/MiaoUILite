/**
 * FileName: Mui_Progress.h
 * Note: UI进度条控件声明
 *
 * Copyright (C) 2020-2023 Maplespe (mapleshr@icloud.com)
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
 * date: 2020-11-24 Create
*/

#pragma once
#include <Control/Mui_Control.h>

namespace Mui::Ctrl
{
	/* UIProgBar(UIProgressBar) 控件
	* 属性列表:
	* style (UIStyle*)
	* maxValue (int)					  - 最大值
	* value (int)						  - 当前值
	* vertical (bool)					  - 是否垂直
	* leftShow (bool)					  - 如果非垂直 进度从左往右显示 (默认true)
	* bottomShow (bool)					  - 如果非水平 进度从下往上显示 (默认true)
	*/
	class UIProgressBar : public UIControl
	{
	public:
		MCTRL_DEFINE(L"UIProgBar", { CtrlMgr::AttribType::UIStyle, L"style" });

		struct Attribute
		{
			UIStylePtr style = nullptr;
			int maxValue = 100;
			int value = 0;
			bool vertical = false;
			bool leftShow = true;
			bool bottomShow = true;
		};

		UIProgressBar(UIControl* parent, Attribute attrib);

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

		//设置最大值
		void SetMaxValue(int value, bool draw = true);
		//获取最大值
		int GetMaxValue() const;

		//设置当前值
		void SetCurValue(int value, bool draw = true);
		//获取当前值
		int GetCurValue() const;

	protected:
		explicit UIProgressBar(Attribute attrib);

		void OnPaintProc(MPCPaintParam param) override;

		//计算百分比值
		int Percentage(int value1, int percentValue, int percent);
		//计算块矩形
		void CalcDrawRect(MPCRect dest);

	private:
		CtrlMgr::UIAttribute<Attribute> m_attrib;
		static void BindAttribute();

		UIRect m_drawRect;
	};

}
