/**
 * FileName: Mui_TypeDef.h
 * Note: 基本类型定义
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
 * date: 2023-4-1 Create
*/
#pragma once
#include <cstdint>

namespace Mui
{
	//数据类型
	using _m_uchar = std::uint8_t;	 //0 ~ 255 (0xFF)
	using _m_short = std::int16_t;	 //-32768 ~ 32767
	using _m_ushort = std::uint16_t; //0 ~ 65535 (0xFFFF)
	using _m_int = std::int32_t;	 //-2147483648 ~ 2147483647
	using _m_uint = std::uint32_t;	 //0 ~ 4294967295 (0xFFFFFFFF)

	using _m_param = std::int64_t;	 //-9223372036854775808 ～ 9223372036854775807
	using _m_ptr = void*;
	using _m_ptrv = std::uintptr_t;

	using _m_long = long;
	using _m_long64 = _m_param;
	using _m_ulong = _m_uint;
	using _m_ulong64 = std::uint64_t;//0 ~ 18446744073709552000 (0xFFFFFFFFFFFFFFFF)

	using _m_result = _m_long;
	using _m_byte = std::uint8_t;
	using _m_lpbyte = _m_byte*;
	using _m_word = _m_ushort;
	using _m_size = _m_ulong64;
	using _m_lpcwstr = const wchar_t*;
	using _m_msg = _m_int;

	//默认字体
	constexpr auto M_DEF_SYSTEM_FONTNAME =
#ifdef _WIN32
		L"Microsoft YaHei UI";
#elif __ANDROID__
		L"Noto Sans CJK TC";
#else
#error __TODO__
#endif

	//窗口消息事件代码
	enum MEventCodeEnum
	{
		M_WND_MOVE = -0x0ffffff,
		M_WND_SIZE,
		M_WND_SETFOCUS,
		M_WND_KILLFOCUS,
		M_WND_PAINT,
		M_WND_CLOSE,
		M_WND_KEYDOWN,
		M_WND_KEYUP,
		M_WND_COMMAND,
		M_WND_SYSCOMMAND,
		M_WND_TIMER,

		M_MOUSE_MOVE,
		M_MOUSE_WHEEL,
		M_MOUSE_HOVER,
		M_MOUSE_LEAVE,
		M_MOUSE_LBDOWN,
		M_MOUSE_LBUP,
		M_MOUSE_LBDBCLICK,
		M_MOUSE_RBDOWN,
		M_MOUSE_RBUP,
		M_MOUSE_RBDBCLICK,
		M_MOUSE_MBDOWN,
		M_MOUSE_MBUP,
		M_MOUSE_MBDBCLICK,

		M_SETCURSOR
	};

	//文本对齐方式
	enum TextAlign
	{
		TextAlign_Left = 0x0000,	//居左
		TextAlign_Center = 0x0001,	//居中
		TextAlign_Right = 0x0002,	//居右

		TextAlign_Top = 0x0000,		//居顶
		TextAlign_VCenter = 0x0004, //垂直居中
		TextAlign_Bottom = 0x0008,	//居底
	};

	//消息事件
	enum UINotifyEvent
	{
		Event_Mouse_Hover,			//鼠标进入
		Event_Mouse_Exited,			//鼠标退出
		Event_Mouse_Move,			//鼠标移动

		Event_Mouse_LDown,			//鼠标左键按下
		Event_Mouse_LUp,			//鼠标左键弹起
		Event_Mouse_LClick,			//鼠标左键单击
		Event_Mouse_LDoubleClicked,	//鼠标左键双击

		Event_Mouse_MDown,			//鼠标中键按下
		Event_Mouse_MUp,			//鼠标中键弹起
		Event_Mouse_MDoubleClicked, //鼠标中键双击

		Event_Mouse_RDown,			//鼠标右键按下
		Event_Mouse_RUp,			//鼠标右键弹起
		Event_Mouse_RDoubleClicked, //鼠标右键双击

		Event_Mouse_Wheel,			//鼠标滚轮消息

		Event_Control_SetCursor,	//设置光标
		Event_Control_OnTimer,		//计时器消息

		Event_Focus_True,			//获得焦点
		Event_Focus_False,			//失去焦点

		Event_Key_Down,				//按下某键
		Event_Key_Up,				//按键弹起

		Event_Edit_TextChanged,		//文本变化

		Event_ListBox_ItemLClick,	//表项左键单击
		Event_ListBox_ItemLDBClick,	//表项左键双击
		Event_ListBox_ItemChanging,	//列表框选项准备更改
		Event_ListBox_ItemChanged,	//列表框选项已更改

		Event_Slider_Change,		//滑块值更改
		Event_ColorPicker_Change,	//颜色选择器颜色更改

		Event_NavBar_ItemChange,	//导航项目已更改

		Event_Menu_ItemLClick		//菜单项目被左键单击
	};

	//控件状态
	enum UIControlStatus
	{
		UIControlStatus_Normal,		//普通状态
		UIControlStatus_Hover,		//热点状态
		UIControlStatus_Pressed,	//按下状态
		UIControlStatus_Disable,	//禁止状态
	};

	//控件定位方式
	enum UIAlignment
	{
		UIAlignment_Block,			//按块方式横向堆叠 如果横向空间不足从下一行开始依次类推
		UIAlignment_LinearV,		//按线性方式纵向堆叠
		UIAlignment_LinearVR,		//按线性方式纵向堆叠 靠右
		UIAlignment_LinearVB,		//按线性方式纵向堆叠 从下向上 靠左
		UIAlignment_LinearVBR,		//按线性方式纵向堆叠 从下向上 靠右
		UIAlignment_LinearH,		//按线性方式横向堆叠
		UIAlignment_LinearHB,		//按线性方式横向堆叠 靠底
		UIAlignment_LinearHL,		//按线性方式横向堆叠 从右向左 靠顶
		UIAlignment_LinearHLB,		//按线性方式横向堆叠 从右向左 靠底
		UIAlignment_Absolute,		//绝对布局 坐标不受布局限制
		UIAlignment_Center,			//居中开始
		UIAlignment_Grid			//网格布局 暂未实现
	};
}