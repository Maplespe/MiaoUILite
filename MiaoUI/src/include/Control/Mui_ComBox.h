/**
 * FileName: Mui_Combox.h
 * Note: UI组合框控件声明
 *
 * Copyright (C) 2021-2023 Maplespe (mapleshr@icloud.com)
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
 * date: 2021-4-2 Create
*/

#pragma once
#include <Control/Mui_Label.h>
#include <Control/Mui_ListBox.h>

namespace Mui::Ctrl
{
	/* UIComBox 控件
	* 属性列表:
	* style (UIStyle*)
	* dropIcon (UIResource*)			- 下拉列表图标
	* dropIconXPos (int)				- 下拉列表图标X偏移位置
	* dropIconAutoPos (bool)			- 自动计算下拉列表图标位置
	* popTop (bool)						- 向上弹出菜单
	* menuHeight (int)					- 弹出菜单高度(px)
	* listShadowBlur (float)			- 弹出列表阴影模糊度 默认6.f
	* listShadowColor (_m_color) 4x		- 弹出列表阴影颜色 默认RGBA(0,0,0,50)
	* listShadowOffset (UIPoint) 2x		- 弹出列表阴影偏移位置 x,y 默认 0,5
	* listShadowExtend (int)			- 弹出列表阴影扩展 默认0
	* 列表属性:
	* listStyle (UIStyle*)				- 列表样式
	* itemStyle (UIStyle*)				- 列表项样式
	* itemHeight (int)					- 列表项高度(px)
	* lineSpace (int)					- 列表行间距(px)
	* iFontStyle (UIListBox::ItemFont)	- 列表项字体默认样式
	* iFont	(std::wstring_view)			- 列表项字体默认样式 - 字体名称
	* iFontSize (_m_ushort)				- 列表项字体默认样式 - 字体尺寸 默认12px
	* iFontBold (bool)					- 列表项字体默认样式 - 是否粗体
	* iFontItalics (bool)				- 列表项字体默认样式 - 是否斜体
	* iFontUnderline (bool)				- 列表项字体默认样式 - 是否下划线
	* iFontStrikeout (bool)				- 列表项字体默认样式 - 是否删除线
	* iFontColor 4x						- 列表项字体默认样式 - 字体颜色 默认RGBA(0,0,0,255)
	* iTextAlign						- 列表项字体默认样式 - 文本对齐方式 仅数字 为TextAlignment枚举值 默认LeftTop
	* iFontCustom (_m_param)			- 列表项字体默认样式 - 自定义字体集对象参数
	* drawOffset (UIPoint) 2x			- 列表内容绘制偏移参数(px)
	* 列表滚动条属性:
	* styleV (UIStyle*)					- 垂直滚动条样式
	* animate (bool)					- 是否启用动画效果
	* button (bool)						- 是否显示滚动条上下调节按钮
	* dragValueV (int)					- 垂直滚动条当前值
	* barWidth (int)					- 滚动条宽度
	* barMinHeight (int)				- 滚动条按钮最小高(宽)度
	* btnHeight (int)					- 滚动条上下调节按钮高度
	* inset (_m_rect_t<int>) 4x			- 内边距
	* ...
	* 还继承UILabel的属性...
	*/
	class UIComBox : public UILabel
	{
		//重写列表框事件 用于失去焦点隐藏列表
		class UIListBoxCom : public UIListBox
		{
		public:
			UIListBoxCom(Attribute attrib, UIScroll::Attribute scrollAttrib = {})
			: UIListBox(std::move(attrib), std::move(scrollAttrib))
			{
				HideNode(true, false);
			}

		protected:
			bool OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam) override;
			bool SendEvent(UINotifyEvent event, _m_param param) override;

			friend class UIComBox;
			UIComBox* combox = nullptr;
		};
	public:
		MCTRL_DEFINE_EX
		(
			L"UIComBox",
			MCTRL_MAKE_ATTRIB
			(
				{ CtrlMgr::AttribType::UIStyle, L"style" },
				{ CtrlMgr::AttribType::UIStyle, L"listStyle" },
				{ CtrlMgr::AttribType::UIBitmap, L"dropIcon" }
			),
			MCTRL_BASE_ATTRIB(UILabel),
			MCTRL_BASE_ATTRIB(UIListBox)
		);

		MCTRL_DEFSLOT
		(
			CtrlMgr::EventSlot<int> itemClicked;//项目被左键单击
			CtrlMgr::EventSlot<int> itemDBClicked;//项目被左键双击
			CtrlMgr::EventSlot<int, bool&> itemChanging;//选中项目即将被更改 设置变量为false则阻止更改
			CtrlMgr::EventSlot<int> itemChanged;//选中项目已被更改
		);

		struct Attribute
		{
			UIStylePtr style = nullptr;
			UIBitmapPtr dropIcon = nullptr;
			int dropIconXPos = 0;
			bool dropIconAutoPos = true;
			bool popTop = false;
			int menuHeight = 300;

			float listShadowBlur = 6.f;
			_m_color listShadowColor = 0x32000000;
			UIPoint listShadowOffset = { 0, 6 };
			int listShadowExtend = 0;
		};

		UIComBox(UIControl* parent, Attribute attrib, UIListBox::Attribute listAttrib = {},
			UIScroll::Attribute scrollAttrib = {}, UILabel::Attribute fontStyle = {});
		~UIComBox() override;

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		template<typename T>
		bool SetAttributeSrc(std::wstring_view attribName, T&& value, bool draw = true)
		{
			if (!m_attrib.SetAttribute<T>(attribName, std::forward<T>(value), this))
			{
				if (!UILabel::SetAttributeSrc<T>(attribName, std::forward<T>(value), draw))
					return m_popList->SetAttributeSrc<T>(attribName, std::forward<T>(value), draw);
				return true;
			}
			m_cacheUpdate = true;
			if (draw) UpdateDisplay();
			return true;
		}
		[[nodiscard]] const Attribute& GetAttribute() const;

		/*添加一个列表项目
		* @param item - 自定义列表项目
		* @param index - 列表位置 -1=尾部
		* @param draw - 是否立即绘制
		*/
		int AddItem(ListItem* item, int index, bool draw = true);

		/*获取一个列表项目指针
		* @param index - 列表索引 -1=尾部
		*
		* @return 成功返回列表指针 失败返回nullptr
		*/
		ListItem* GetItem(int index) const;

		/*设置当前选中的列表项目
		* @param index - 索引项目 -1=不选中
		* @param draw - 是否立即绘制
		*
		* @return 是否成功 若失败则索引值无效
		*/
		bool SetCurSelItem(int index, bool draw = true);

		//获取当前选中的列表项目索引 -1=未选中
		int GetCurSelItem() const;

		//获取列表项目总数
		int GetItemListCount() const;

		/*删除一个列表项目 这不会删除ListItem的内存
		* @param index - 列表索引 -1=尾部
		* @param draw - 是否立即绘制
		*
		* @return 当前列表项目总数
		*/
		int DeleteItem(int index, bool draw = true);

		/*删除一个列表项目
		* @param index - 列表索引 -1=尾部
		* @param delItem - 是否删除列表内存 使用delete
		* @param draw - 是否立即绘制
		*
		* @return 当前列表项目总数
		*/
		int DeleteItem(int index, bool delItem, bool draw = true);

		/*删除所有列表项
		* @param delItem - 是否删除列表内存
		* @param draw - 是否立即绘制
		*/
		void DeleteAllItem(bool delItem = true, bool draw = true);

	protected:
		explicit UIComBox(Attribute attrib, UIListBox::Attribute listAttrib = {},
			UIScroll::Attribute scrollAttrib = {}, UILabel::Attribute fontStyle = {});

		void OnPaintProc(MPCPaintParam param) override;
		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;
		bool OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam) override;
		void OnScale(_m_scale scale) override;
		void OnLayoutCalced() override;

	private:
		void ShowMenu(bool show);
		void CalcMenuFrame();
		void SetText(std::wstring_view text);
		bool IsListAttrib(std::wstring_view name);

		CtrlMgr::UIAttribute<Attribute, UIComBox*> m_attrib;

		static void BindAttribute();

		bool m_isHover = false;

		UIControl* m_root = nullptr;
		UIListBoxCom* m_popList = nullptr;

		UIControlStatus m_state = UIControlStatus_Normal;

		friend class UIListBoxCom;
	};
}
