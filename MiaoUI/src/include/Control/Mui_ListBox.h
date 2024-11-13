/**
 * FileName: Mui_ListBox.h
 * Note: UI列表框控件声明
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
 * date: 2020-11-28 Create
*/
#pragma once
#include <Control/Mui_Scroll.h>

namespace Mui::Ctrl
{
	class UIListBox;

	class ListItem
	{
	public:
		ListItem() = default;
		ListItem(std::wstring_view title, _m_color color = 0, UIFontStyle style = {});

		virtual ~ListItem();

		virtual void SetText(std::wstring_view text, _m_color color = 0, UIFontStyle style = {}, bool updateStyle = false);

		virtual void SetFontSize(_m_uint size);

		virtual std::wstring_view GetText();

	protected:
		enum ItemStatus
		{
			UIListItemNormal,		//普通
			UIListItemHover,		//热点
			UIListItemPressed,		//按下
			UIListItemDisbale,		//禁止
			UIListItemNormalSel,	//普通选中
			UIListItemHoverSel,		//热点选中
			UIListItemPressedSel,	//按下选中
			UIListItemDisbaleSel	//禁止选中
		};

		struct PaintParam
		{
			MRenderCmd* render = nullptr;
			MPCRect dstRect = nullptr;
			UIStyle* style = nullptr;
			_m_byte dstAlpha = 0;
			_m_rcscale dstScale;
			UIPoint offset;
			_m_color defcolor = 0;
		};

		virtual void OnPaintProc(const PaintParam* param);

		virtual void PaintStyle(MRenderCmd* render, UIStyle* style, _m_rect frame,
			ItemStatus state, _m_byte alpha, _m_scale scale = { 1.f, 1.f });

		virtual bool OnMouseMessage(_m_uint message, _m_param wParam, _m_param lParam);

		bool IsSel();

		UIListBox* m_parent = nullptr;
		ItemStatus m_state = UIListItemNormal;

		bool m_isClick = false;

		UIString m_text;
		UIFontStyle m_style;
		_m_uint m_size = 12;
		_m_color m_color = 0;

		friend class UIListBox;
	};

	/* UIListBox 控件
	* 属性列表:
	* style (UIStyle*)
	* itemStyle (UIStyle*)				- 列表项样式
	* itemHeight (int)					- 列表项高度(px)
	* lineSpace (int)					- 列表行间距(px)
	* iFontStyle (UIListBox::ItemFont*)	- 列表项字体默认样式
	* iFont	(std::wstring_view)			- 列表项字体默认样式 - 字体名称
	* iFontSize (_m_ushort)				- 列表项字体默认样式 - 字体尺寸 默认12px
	* iFontBold (bool)					- 列表项字体默认样式 - 是否粗体
	* iFontItalics (bool)				- 列表项字体默认样式 - 是否斜体
	* iFontUnderline (bool)				- 列表项字体默认样式 - 是否下划线
	* iFontStrikeout (bool)				- 列表项字体默认样式 - 是否删除线
	* iFontColor 4x						- 列表项字体默认样式 - 字体颜色 默认RGBA(0,0,0,255)
	* iTextAlign						- 列表项字体默认样式 - 文本对齐方式 仅数字 为TextAlign枚举值 默认LeftTop
	* iFontCustom (_m_param)			- 列表项字体默认样式 - 自定义字体集对象参数
	* drawOffset (UIPoint) 2x			- 列表内容绘制偏移参数(px)
	* allowRightSel (bool)				- 允许右键点击也能选中列表项目
	* ...
	* 还继承UIScroll的属性...
	*/
	class UIListBox : public UIScroll
	{
	public:
		MCTRL_DEFINE_EX
		(
			L"UIListBox",
			MCTRL_MAKE_ATTRIB
			(
				{ CtrlMgr::AttribType::UIStyle, L"style" },
				{ CtrlMgr::AttribType::UIStyle, L"itemStyle" },
				{ CtrlMgr::AttribType::listfontStyle, L"iFontStyle" }
			),
			MCTRL_BASE_ATTRIB(UIScroll)
		);

		MCTRL_DEFSLOT
		(
			CtrlMgr::EventSlot<UIPoint, int> itemClicked;//项目被左键单击
			CtrlMgr::EventSlot<UIPoint, int> itemDBClicked;//项目被左键双击
			CtrlMgr::EventSlot<int, bool&> itemChanging;//选中项目即将被更改 设置变量为false则阻止更改
			CtrlMgr::EventSlot<int> itemChanged;//选中项目已被更改
		);

		struct ItemFont
		{
			UIString font = M_DEF_SYSTEM_FONTNAME;

			_m_color fontColor = 0xff000000;

			_m_ushort fontSize = 12;
			_m_ptrv fontCustom = 0;

			UIFontStyle fontStyle;
			TextAlign textAlign = TextAlign_Top;
		};

		struct Attribute
		{
			UIStylePtr style = nullptr;
			UIStylePtr itemStyle = nullptr;

			int itemHeight = 30;
			int lineSpace = 0;
			bool allowRightSel = false;

			UIPoint drawOffset;

			ItemFont fontStyle;
		};

		UIListBox(UIControl* parent, Attribute attrib, UIScroll::Attribute scrollAttrib = {});
		~UIListBox() override;

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		template<typename T>
		bool SetAttributeSrc(std::wstring_view attribName, T&& value, bool draw = true)
		{
			if (!m_attrib.SetAttribute<T>(attribName, std::forward<T>(value), this))
				return UIScroll::SetAttributeSrc<T>(attribName, std::forward<T>(value), draw);
			m_cacheUpdate = true;
			if (draw) UpdateDisplay();
			return true;
		}
		[[nodiscard]] const Attribute& GetAttribute() const;

		/*添加一个列表项目
		* @param item - 自定义列表项目
		* @param index - 列表位置 -1=尾部
		* @param draw - 是否立即绘制
		* 
		* @return 返回item总数
		* @exception MErrorCode::IndexOutOfRange
		*/
		int AddItem(ListItem* item, int index, bool draw = true);

		/*获取一个列表项目指针
		* @param index - 列表索引 -1=尾部
		*
		* @return 成功返回列表指针 失败返回nullptr
		*/
		[[nodiscard]] ListItem* GetItem(int index) const;

		/*设置当前选中的列表项目
		* @param index - 索引项目 -1=不选中
		* @param draw - 是否立即绘制
		*
		* @return 是否成功 若失败则索引值无效
		*/
		bool SetCurSelItem(int index, bool draw = true);

		//获取当前选中的列表项目索引 -1=未选中
		[[nodiscard]] int GetCurSelItem() const;

		//获取列表项目总数
		[[nodiscard]] int GetItemListCount() const;

		/*删除一个列表项目 这不会删除ListItem的内存
		* @param index - 列表索引 -1=尾部
		* @param draw - 是否立即绘制
		*
		* @return 当前列表项目总数
		* @exception MErrorCode::IndexOutOfRange
		*/
		int DeleteItem(int index, bool draw = true);

		/*删除一个列表项目
		* @param index - 列表索引 -1=尾部
		* @param delItem - 是否删除列表内存 使用delete
		* @param draw - 是否立即绘制
		*
		* @return 当前列表项目总数
		* @exception MErrorCode::IndexOutOfRange
		*/
		int DeleteItem(int index, bool delItem, bool draw = true);

		/*删除所有列表项
		* @param delItem - 是否删除列表内存 使用delete
		* @param draw - 是否立即绘制
		*/
		void DeleteAllItem(bool delItem = true, bool draw = true);

	protected:
		explicit UIListBox(Attribute attrib, UIScroll::Attribute scrollAttrib = {});

		void OnLoadResource(MRenderCmd* render, bool recreate) override;

		void OnPaintProc(MPCPaintParam param) override;

		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;

		void OnScrollView(UIScroll*, int dragValue, bool horizontal);

		bool DispatchItemMsg(UIPoint& point, _m_uint msg, _m_param wParam, _m_param lParam);

		void OnLayoutCalced() override { CalcListView(); }

		//设置视图Top
		void SetViewTop(int top);
		//索引检查 防止越界
		bool IndexCheck(int index) const;
		//计算当前可视区域
		void CalcListView();
		//绘制视图列表
		void PaintListView(MRenderCmd* render, MPCRect destRect, bool cache);
		//设置当前选中项目
		void SetSelItem(ListItem* item, UIPoint pt);
		//更新索引
		void UpdateIndex();

		//默认字体和画刷
		MFontPtr m_fontDef = nullptr;
		MBrushPtr m_brushDef = nullptr;

	private:
		CtrlMgr::UIAttribute<Attribute, UIListBox*> m_attrib;

		static void BindAttribute();

		void updateStyle(UIFontStyle& style);

	protected:
		UIControlStatus m_state = UIControlStatus_Normal;

		//项目列表
		std::vector<ListItem*> m_itemList;

		struct ViewData
		{
			UIPoint offsetPos;
			int beginIndex = 0;
			int indexCount = 0;
				
			int top = 0;
			int bottom = 0;
			int viewHeight = 0;
			int itemHeight = 0;
			int lineSpace = 0;
			UIRect boxRect;
			_m_rcscale scale;
			std::vector<UIRect> viewItemRect;
		} m_view;

		int m_curSelIndex = -1;//当前选中项目索引
		ListItem* m_hoverIndex = nullptr;//当前焦点项目
		ListItem* m_curSelItem = nullptr;//当前选中项目
		UIRect m_curItemRect;

		//模拟触控滑动相关
		int m_downViewY = 0;
		friend class ListItem;
	};

}
