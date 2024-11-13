/**
 * FileName: Mui_Button.h
 * Note: UI按钮控件声明
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
 * date: 2020-10-19 Create
*/

#pragma once
#include <Control/Mui_Label.h>

namespace Mui::Ctrl
{
	/* UIButton 控件
	* 属性列表:
	* style (UIStyle*)
	* animate (bool)		 - 是否启用动画效果 Lite版本无效
	* aniAlphaType (bool)	 - 动画效果方式 混合或覆盖 Lite版本无效
	* inset (_m_rect_t<int>) - 内边距 仅autoSize=true 时才有效
	* ...
	* 还继承UILabel的属性...
	*/
	class UIButton : public UILabel
	{
	public:
		MCTRL_DEFINE_EX
		(
			L"UIButton",
			MCTRL_MAKE_ATTRIB({ CtrlMgr::AttribType::UIStyle, L"style" }),
			MCTRL_BASE_ATTRIB(UILabel)
		);

		struct Attribute
		{
			UIStylePtr style = nullptr;
			bool animate = false;	//Lite版本无效
			bool aniAlphaType = false; //Lite版本无效
			_m_rect_t<int> inset = { 2,2,2,2 };
		};

		UIButton(UIControl* parent, Attribute attrib, UILabel::Attribute fontStyle = {});

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

	protected:
		explicit UIButton(Attribute attrib, UILabel::Attribute fontStyle = {});

		void OnPaintProc(MPCPaintParam param) override;
		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;

		void ChangeStatus(UIControlStatus&& state, UIControlStatus&& oldState);

		_m_sizef GetContentSize() override;

	private:
		CtrlMgr::UIAttribute<Attribute> m_attrib;

		UIControlStatus m_state = UIControlStatus_Normal;
	};
}
