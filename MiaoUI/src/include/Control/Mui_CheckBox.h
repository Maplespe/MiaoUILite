/**
 * FileName: Mui_CheckBox.h
 * Note: UI选择框控件声明
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

#pragma once
#include <Control/Mui_Label.h>

namespace Mui::Ctrl
{
	/* UICheckBox 控件
	* 属性列表:
	* style (UIStyle*)
	* isSel (bool)			- 是否被选中
	* allowClick (bool)		- 是否可通过点击改变选中状态(否则只能手动设置isSel来控制是否选中)
	* animate (bool)		- 是否启用动画效果 Lite版本无效
	* aniAlphaType (bool)	- 动画效果方式 混合或覆盖 Lite版本无效
	* textOffset (int)		- 文本和图标之间的距离
	* ...
	* 还继承UILabel的属性...
	*/
	class UICheckBox : public UILabel
	{
	public:
		MCTRL_DEFINE_EX
		(
			L"UICheckBox",
			MCTRL_MAKE_ATTRIB({ CtrlMgr::AttribType::UIStyle, L"style" }),
			MCTRL_BASE_ATTRIB(UILabel)
		);

		struct Attribute
		{
			UIStylePtr style = nullptr;
			bool isSel = false;
			bool allowClick = true;
			bool animate = false; //Lite版本无效
			bool aniAlphaType = false; //Lite版本无效
			int textOffset = 10;
		};

		UICheckBox(UIControl* parent, Attribute attrib, UILabel::Attribute fontStyle = {});

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		template<typename T>
		bool SetAttributeSrc(std::wstring_view attribName, T&& value, bool draw = true)
		{
			if (!m_attrib.SetAttribute<T>(attribName, std::forward<T>(value), draw))
				return UILabel::SetAttributeSrc<T>(attribName, std::forward<T>(value), draw);
			m_cacheUpdate = true;
			if (draw) UpdateDisplay();
			return true;
		}
		[[nodiscard]] const Attribute& GetAttribute() const;

		virtual void SetSel(bool sel, bool draw = true);
		virtual bool GetSel() const;

	protected:
		explicit UICheckBox(Attribute attrib, UILabel::Attribute fontStyle = {});

		enum Status
		{
			UICheckBoxNormal,		//普通
			UICheckBoxHover,		//热点
			UICheckBoxPressed,		//按下
			UICheckBoxDisbale,		//禁止
			UICheckBoxNormalSel,	//普通选中
			UICheckBoxHoverSel,		//热点选中
			UICheckBoxPressedSel,	//按下选中
			UICheckBoxDisbaleSel	//禁止选中
		};

		void OnPaintProc(MPCPaintParam param) override;
		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;

		_m_sizef GetContentSize() override;

		void ChangeStatus(Status&& state, Status&& oldState);

	private:
		CtrlMgr::UIAttribute<Attribute> m_attrib;

		Status m_state = UICheckBoxNormal;
	};
}
