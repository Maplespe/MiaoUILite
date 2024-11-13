/**
 * FileName: Mui_Scroll.h
 * Note: UI滚动条控件声明
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
 * date: 2020-11-15 Create
*/

#pragma once
#include <Control/Mui_Control.h>

namespace Mui::Ctrl
{
	/* UIScroll 控件
	* 属性列表:
	* styleV (UIStyle*)			- 垂直滚动条样式
	* styleH (UIStyle*)			- 水平滚动条样式
	* vertical (bool)			- 显示垂直滚动条
	* horizontal (bool)			- 显示水平滚动条
	* active (bool)				- 始终显示滚动条(无效)
	* animate (bool)			- 是否启用动画效果 Lite版本无效
	* button (bool)				- 是否显示滚动条上下调节按钮
	* range (UISize)			- 滚动条最大可滚动区域
	* rangeV (int)				- 垂直滚动条最大可滚动区域
	* rangeH (int)				- 水平滚动条最大可滚动区域
	* dragValue (UISize)		- 滚动条当前值
	* dragValueV (int)			- 垂直滚动条当前值
	* dragValueH (int)			- 水平滚动条当前值
	* barWidth (int)			- 滚动条宽度
	* barMinHeight (int)		- 滚动条按钮最小高(宽)度
	* btnHeight (int)			- 滚动条上下调节按钮高度
	* inset (_m_rect_t<int>)	- 内边距
	*/
	class UIScroll : public UIControl
	{
	public:
		MCTRL_DEFINE
		(
			L"UIScroll",
			{ CtrlMgr::AttribType::UIStyle, L"styleV" },
			{ CtrlMgr::AttribType::UIStyle, L"styleH" }
		);

		/* 回调函数
		* @param UIScroll* 滚动条控件指针
		* @param int 滚动条当前值
		* @param bool 是否为水平滚动条 否则为垂直
		*/
		using ScrollCallBack = std::function<void(UIScroll*, int, bool)>;

		struct Attribute
		{
			UIStylePtr styleV = nullptr;
			UIStylePtr styleH = nullptr;

			ScrollCallBack callback = nullptr;

			bool vertical = false;
			bool horizontal = false;
			bool active = true;
			bool animate = true;	//Lite版本无效
			bool button = true;
			UISize range;
			UISize dragValue;
			int barWidth = 8;
			int barMinHeight = 20;
			int btnHeight = 8;
			_m_rect_t<int> inset = { 0 };
		};

		UIScroll(UIControl* parent, Attribute attrib);

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

		/*获取滚动条可拖动范围
		* @param horizontal - 是否为水平滚动条
		*
		* @return 返回指定滚动条值
		*/
		[[nodiscard]] int GetRange(bool horizontal) const;

		[[nodiscard]] UISize GetRange() const;

		/*设置可滚动范围
		* @param horizontal - 是否为水平滚动条
		* @param range - 范围 该值不能小于DragValue
		* @param draw - 是否立即绘制
		*/
		void SetRange(bool horizontal, int range, bool draw = true);

		void SetRange(UISize range, bool draw = true);

		/*获取拖拽按钮当前值
		* @param horizontal - 是否为水平滚动条
		*
		* @return 返回指定滚动条值
		*/
		[[nodiscard]] int GetDragValue(bool horizontal) const;

		[[nodiscard]] UISize GetDragValue() const;

		/*设置拖拽按钮当前值
		* @param horizontal - 是否为水平滚动条
		* @param value - 滚动值 0-Range
		* @param draw - 是否立即绘制
		*/
		void SetDragValue(bool horizontal, int value, bool draw = true);
		void SetDragValueNoAni(bool horizontal, int value, bool draw);

		/*计算偏移滚动值,值用于控件的底边和顶边转换
		* @param horizontal - 是否计算水平滚动条的值
		* @param dragValue - 要计算的滚动值
		* @param offset - 要计算的值
		*
		* @return 去除offset后的dragValue
		*/
		int CalcOffsetDragValue(bool horizontal, int dragValue, int offset);

	protected:
		enum ScrollButtonType
		{
			ButtonNull,
			TopButton,
			BottomButton,
			ThumbButton,
			Track,
			Intersect
		};

		explicit UIScroll(Attribute attrib);

		void OnPaintProc(MPCPaintParam param) override;

		bool OnMouseMove(_m_uint flag, const UIPoint& point) override;
		bool OnMouseExited(_m_uint flag, const UIPoint& point) override;
		bool OnLButtonDown(_m_uint flag, const UIPoint& point) override;
		bool OnLButtonUp(_m_uint flag, const UIPoint& point) override;
		bool OnMouseWheel(_m_uint flag, short delta, const UIPoint& point) override;

		//计算鼠标在哪个部件内
		ScrollButtonType CalcPointIn(const UIPoint& pt);

		//计算滚动条控件内的各部件矩形
		void CalcControlRect();
		//根据DragValue计算拖拽按钮位置
		void CalcThumbBtnPos(bool horizontal);
		//根据ThumbButton位置计算DragValue
		void CalcDragValue(bool horizontal);
		//绘制部件
		void PaintControl(MRenderCmd* render, UIStyle* style, UIControlStatus status,
		                  ScrollButtonType type, UIRect& drawRect, _m_byte& alpha, _m_scale scale);

		//指定整个控件范围Wheel消息都可以触发滚动条 否则仅限滚动条区域
		bool m_ALLWheel = false;

		void SetCallback(const ScrollCallBack& callback)
		{
			m_attrib.Set().callback = callback;
		}

	private:

		//获得一个滚动page后的pos
		int GetScrollPage(bool up, bool horz, float count);

		CtrlMgr::UIAttribute<Attribute, UIScroll*> m_attrib;

		static void BindAttribute();

		UISize m_size;

		struct ScrollData
		{
			UIRect scrollBtn[2];//滚动调节按钮
			UIRect thumbButton;//拖拽按钮
			UIRect track;//轨道
		};

		UIRect m_intersect;//垂直滚动条与水平滚动条的交汇点
		ScrollData m_scroll[2];

		UIPoint m_clickPos;

		UIControlStatus m_status = UIControlStatus_Normal;
		ScrollButtonType m_btnType = ButtonNull;
		bool m_btnTypeH = false;
		bool m_isClick = false;
	};
}
