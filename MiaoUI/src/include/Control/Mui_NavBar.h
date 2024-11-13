/**
 * FileName: Mui_NavBar.h
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
#pragma once
#include <Control/Mui_Label.h>

namespace Mui::Ctrl
{
	/* UINavBar(UINavigationBar) 控件 MiaoUI Core 2.0.6版本
	* 属性列表:
	* fontStyle (UILabel::Attribute*)   - 字体样式
	* font (std::wstring_view)			- 字体
	* fontColor (_m_color) 4x			- 字体颜色 默认RGBA(120,120,120,255)
	* fontHoverColor (_m_color) 4x		- 字体颜色 Hover状态 默认RGBA(0,0,0,255)
	* fontPressColor (_m_color) 4x		- 字体颜色 按下状态 默认RGBA(150,150,150,255)
	* fontSize (_m_ushort)				- 字体尺寸 默认12px
	* fontBold (bool)					- 是否粗体
	* fontItalics (bool)				- 是否斜体
	* fontUnderline (bool)				- 是否下划线
	* fontStrikeout (bool)				- 是否删除线
	* fontCustom (_m_ptrv)				- 自定义字体集对象参数
	* shadowUse (bool)					- 使用文本阴影效果
	* shadowBlur (float)				- 文本阴影模糊度 默认1.f
	* shadowColor (_m_color) 4x			- 文本阴影颜色 默认RGBA(0,0,0,255)
	* shadowOffset (UIPoint) 2x			- 文本阴影偏移位置 x,y 默认 1,1
	* shadowLow (bool)					- 是否使用低质量阴影 如果为true=没有模糊效果 blur属性无效
	* itemSpace (int)					- item间距
	* barSpace (int)					- 横条和文字间距
	* barHeight (int)					- 横条高度 默认4px
	* barAnitime (int)					- 横条过渡动画时长(ms) 默认300ms
	* barRound (float)					- 横条圆角度 默认1.5f
	* barColor (_m_color) 4x			- 横条颜色
	*/
	class UINavBar : public UIControl
	{
	public:
		MCTRL_DEFINE
		(
			L"UINavBar",
			{ CtrlMgr::AttribType::labelStyle, L"fontStyle" }
		);

		struct Attribute
		{
			UIString font = M_DEF_SYSTEM_FONTNAME;
			_m_color fontColor = 0xff787878;
			_m_color fontHoverColor = Color::M_Black;
			_m_color fontPressColor = 0xff969696;
			_m_ushort fontSize = 12;
			UIFontStyle fontStyle;
			_m_ptrv fontCustom = 0;

			bool shadowUse = false;
			bool shadowLow = false;
			float shadowBlur = 1.f;
			UIPoint shadowOffset = { 1,1 };
			_m_color shadowColor = Color::M_Black;

			int itemSpace = 10;
			int barSpace = 5;
			int barHeight = 4;
			int barAnitime = 300;
			float barRound = 1.5f;
			_m_color barColor = 0xffffb375;
		};

		UINavBar(UIControl* parent, Attribute attrib);

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		template<typename T>
		bool SetAttributeSrc(std::wstring_view attribName, T&& value, bool draw = true)
		{
			if (m_attrib.SetAttribute(attribName, std::forward<T>(value), this))
			{
				m_cacheUpdate = true;
				if (draw) UpdateDisplay();
				return true;
			}
			return false;
		}
		[[nodiscard]] const Attribute& GetAttribute() const { return m_attrib.Get(); }

		void SetFontStyle(UILabel::Attribute& attrib, bool draw = true);

		/*添加一个标题项目
		* @param title - 标题
		* @param index - 列表位置 -1=尾部
		* @param draw - 是否立即绘制
		*
		* @return 当前项目总数
		*/
		int AddItem(std::wstring_view title, int index = -1, bool draw = true);

		//设置项目标题 -1=尾部
		void SetItemTitle(int index, std::wstring_view title, bool draw = true);

		//获取项目标题 -1=尾部
		[[nodiscard]] std::wstring GetItemTitle(int index) const;

		/*设置当前选中的列表项目
		* @param index - 索引项目
		* @param draw - 是否立即绘制
		*
		* @return 是否成功 若失败则索引值无效
		*/
		bool SetCurSelItem(int index, bool draw = true);

		//获取当前选中的列表项目索引
		[[nodiscard]] int GetCurSelItem() const;

		//获取列表项目总数
		[[nodiscard]] int GetItemListCount() const;

		/*删除一个列表项目
		* @param index - 列表索引 -1=尾部
		* @param draw - 是否立即绘制
		*
		* @return 当前项目总数
		*/
		int DeleteItem(int index, bool draw = true);

		//删除所有列表项
		void DeleteAllItem(bool draw = true);

	protected:
		explicit UINavBar(Attribute attrib);

		void OnLoadResource(MRenderCmd* render, bool recreate) override;
		void OnPaintProc(MPCPaintParam param) override;
		bool OnMouseEntered(_m_uint flag, const UIPoint& point) override;
		bool OnMouseExited(_m_uint flag, const UIPoint& point) override;
		bool OnLButtonDown(_m_uint flag, const UIPoint& point) override;
		bool OnLButtonUp(_m_uint flag, const UIPoint& point) override;
		bool OnMouseMove(_m_uint flag, const UIPoint& point) override;

		void OnScale(_m_scale scale) override;
		_m_sizef GetContentSize() override;
		void OnLayoutCalced() override;

		bool IndexCheck(int index) const;

	private:
		std::pair<std::wstring, UIRect> InitItemRect(std::wstring_view title, int index);
		void CalcItemRect();
		bool IsHitItem(const UIPoint& pt);

		CtrlMgr::UIAttribute<Attribute, std::pair<UINavBar*, bool>> m_attrib;
		static void BindAttribute();

		void updateStyle(UIFontStyle& style);
		bool updateRender(bool draw, bool layout = false);

		MFontPtr m_font = nullptr;
		MBrushPtr m_brush = nullptr;
		MEffectPtr m_effect = nullptr;

		UISize m_size;

		std::vector<std::pair<std::wstring, UIRect>> m_itemList;

		int m_curSelIndex = 0;
		int m_curHoverIndex = -1;
		UIControlStatus m_state = UIControlStatus_Normal;

		bool m_down = false;
		int m_lastHover = -1;

	protected:
		int m_baroffset = 0;
		int m_barwidth = 0;
	};
}