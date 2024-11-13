/**
 * FileName: Mui_Control.h
 * Note: UI控件基本类型声明
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
 * date: 2020-10-17 Create
*/
#pragma once
#include <Manager/Mui_ControlMgr.h>
#include <Render/Node/Mui_UINodeBase.h>
#include <Render/Graphs/Mui_Render.h>
#include <Mui_Helper.h>

namespace Mui::Ctrl
{
	using namespace Render;

	class UIControl : public UINodeBase
	{
	public:
		UIControl();
		~UIControl() override;

		//控件类名和注册方法
		static constexpr auto ClassName = L"UIControl";
		static void Register();
		virtual std::wstring GetClsName() const { return ClassName; }

		//设置用户参数
		virtual void SetUserData(_m_param data);

		//获取用户参数
		[[nodiscard]] virtual _m_param GetUserData() const;

		/*设置控件属性
		* @param attribName - 属性名
		* @param attrib - 属性值
		* @param draw - 立即绘制(默认true)
		*/
		virtual void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true);

		//获取控件属性
		virtual std::wstring GetAttribute(std::wstring_view attribName);

		/*设置控件可用性
		* @param enabled - 可用
		* @param draw - 立即绘制(默认true)
		*/
		virtual void SetEnabled(bool enabled, bool draw = true);

		//控件是否可用
		[[nodiscard]] virtual bool IsEnabled() const;

		//获取父窗口
		[[nodiscard]] virtual Window::UIWindowBasic* GetParentWin() const;

		template<typename T>
		T* FindChildren(const UIString& name) const
		{
			return mcast_control<T>(Child(name));
		}

		//FindChildren
		[[nodiscard]] UIControl* Child(const UIString& name) const
		{
			return mcast_control<UIControl>(UINodeBase::FindChildren(name));
		}

		template<typename T>
		T* Child(const UIString& name) const
		{
			return FindChildren<T>(name);
		}

		void AddChildren(UINodeBase* UINode) override;

		//设置控件焦点状态
		virtual void SetFocus(bool focus);

		//设置是否可接受焦点
		virtual void SetEnableFocus(bool enable);

		//设置控件消息穿透 消息向下转发到父窗口
		virtual void SetMsgFilter(bool filter);

		/*设置控件消息忽略
		* 该控件将忽略鼠标以及窗口消息 并向下传递
		* @param ignore - 是否忽略消息
		* @param child - 是否忽略子控件消息
		*/
		virtual void SetMsgIgnore(bool ignore, bool child = true);

		/*缩放控件到指定尺寸(使用父窗口缩放比) 会设置Scale
		* @param width - 目标宽度
		* @param height - 目标高度
		* @param child - 等比例缩放子控件
		*/
		virtual void ScaleControl(_m_uint width, _m_uint height, bool child);

		/*缩放控件到指定比例(使用父窗口缩放比) 会设置Scale
		* @param scale - 目标缩放比
		* @param child - 等比例缩放子控件
		*/
		virtual void ScaleControl(_m_scale scale, bool child);

	protected:

		//设置消息捕获
		void SetCapture();
		void ReleaseCapture();
		bool isCapture();

		//设置计时器
		MTimers::ID SetTimer(_m_uint uTimeout);
		void KillTimer(MTimers::ID idTimer);

		virtual UIControl* FindMouseControl(const UIPoint& point);

		//获取窗口顶级控件层
		UIControl* GetWindowTopCtrl() const;
		UIControl* GetWindowTopCtrl(Window::UIWindowBasic* wnd) const;

		//设置光标状态
		virtual void SetCursor(_m_lpcwstr cursor_name);

		//鼠标和窗口消息
		virtual bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam);
		virtual bool OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam);
		//鼠标进入与退出消息
		virtual bool OnMouseEntered(_m_uint flag, const UIPoint& point);
		virtual bool OnMouseExited(_m_uint flag, const UIPoint& point);
		//鼠标左键消息
		virtual bool OnLButtonDown(_m_uint flag, const UIPoint& point);
		virtual bool OnLButtonUp(_m_uint flag, const UIPoint& point);
		virtual bool OnLButtonDoubleClicked(_m_uint flag, const UIPoint& point);
		//鼠标右键消息
		virtual bool OnRButtonDown(_m_uint flag, const UIPoint& point);
		virtual bool OnRButtonUp(_m_uint flag, const UIPoint& point);
		virtual bool OnRButtonDoubleClicked(_m_uint flag, const UIPoint& point);
		//鼠标中键消息
		virtual bool OnMButtonDown(_m_uint flag, const UIPoint& point);
		virtual bool OnMButtonUp(_m_uint flag, const UIPoint& point);
		virtual bool OnMButtonDoubleClicked(_m_uint flag, const UIPoint& point);
		//鼠标滚轮
		virtual bool OnMouseWheel(_m_uint flag, short delta, const UIPoint& point);
		//鼠标移动
		virtual bool OnMouseMove(_m_uint flag, const UIPoint& point);
		//设置光标
		virtual bool OnSetCursor(_m_param hCur, _m_param lParam);

		//计时器消息
		virtual void OnTimer(MTimers::ID idTimer);
			
		//发送事件
		virtual bool SendEvent(UINotifyEvent event, _m_param param = 0);

		//获取调试画笔
		[[nodiscard]] MPenPtr GetDbgFramePen() const;

		//控件数据
		struct ControlData
		{
			_m_param UserData = 0;		//用户参数
			bool Enabled = true;		//可用
			bool ParentEnabled = true;	//父控件可用
			bool MsgFilter = false;		//消息穿透
			bool MsgIgnore = false;		//消息忽略 消息将不被处理 控件不会被命中
			bool MsgIgnoreChild = false;//忽略子控件消息
			bool IsFocus = true;		//是否接受焦点
		} m_data;

	private:
		//派发消息
		bool DispatchMouseMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam);
		bool DispatchWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam);

		friend class Window::UIWindowBasic;
	};
}
