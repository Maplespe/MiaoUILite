/**
 * FileName: Mui_BasicWnd.h
 * Note: UI窗口容器声明
 *
 * Copyright (C) 2021-2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2021-4-28 Create
*/
#pragma once
#include <Render/Mui_RenderMgr.h>
#include <Render/Node/Mui_RenderNode.h>
#include <Manager/Mui_ResourceMgr.h>

namespace Mui
{
	namespace XML { class MuiXML; }
	namespace Render { class UINodeBase; }

	class MiaoUI;
	class MWindowCtx;

	namespace Window::IWindow
	{
		class UINativeWindow
		{
		public:
			UINativeWindow() = default;
			virtual ~UINativeWindow() = default;
			UINativeWindow(UINativeWindow&&) = delete;
			UINativeWindow(const UINativeWindow&) = delete;
			UINativeWindow& operator=(const UINativeWindow&) = delete;
			UINativeWindow& operator=(UINativeWindow&&) = delete;

			//设置窗口标题
			virtual void SetWindowTitle(std::wstring_view title) = 0;

			//获取窗口标题
			virtual std::wstring GetWindowTitle() = 0;

			/*显示或隐藏窗口
			* @param show - 是否显示窗口
			* @param focus - 如果显示窗口 是否将焦点移动到当前窗口
			*/
			virtual void ShowWindow(bool show, bool focus = true) = 0;

			//窗口是否可见
			virtual bool IsShowWindow() = 0;

			//设置窗口可用性
			virtual void EnableWindow(bool enable) = 0;

			//窗口是否可用
			virtual bool IsEnableWindow() = 0;

			//窗口是否最小化
			virtual bool IsMinimize() = 0;

			//窗口是否最大化
			virtual bool IsMaximize() = 0;

			//居中显示窗口
			virtual void CenterWindow() = 0;

			//关闭窗口
			virtual void CloseWindow() = 0;

			//设置窗口矩形
			virtual void SetWindowRect(UIRect rect) = 0;

			/*获取窗口矩形
			* @param client - 是否获取客户区矩形 否则获取窗口矩形
			*/
			virtual UIRect GetWindowRect(bool client = false) = 0;

			/*设置窗口不透明度 仅分层窗口有效
			* @param alpha - 不透明度0-255
			* @param draw - 是否立即重绘
			*/
			virtual void SetWindowAlpha(_m_byte alpha, bool draw = true) = 0;

			//获取窗口不透明度
			virtual _m_byte GetWindowAlpha() = 0;

			//取窗口参数(句柄)
			virtual _m_ptrv GetWindowHandle() = 0;

			template<typename T>
			T GetWindowHandle() { return reinterpret_cast<T>(GetWindowHandle()); }

			//设置窗口最小尺寸限制
			virtual void SetMinimSize(UISize min) = 0;

		protected:
			//获取窗口原始尺寸
			virtual UISize GetWindowSrcSize() = 0;
		};
	}

	namespace Window
	{
		class UIWindowBasic : public IWindow::UINativeWindow, MThreadT<std::recursive_mutex>
		{
		public:
			UIWindowBasic(Render::Def::MRender* render);
			~UIWindowBasic() override;
			UIWindowBasic(UIWindowBasic&&) = delete;
			UIWindowBasic(const UIWindowBasic&) = delete;
			UIWindowBasic& operator=(const UIWindowBasic&) = delete;
			UIWindowBasic& operator=(UIWindowBasic&&) = delete;

			//获取根控件
			virtual Ctrl::UIControl* GetRootControl() const;

			/*更新布局 并更新显示
			* @param rect - 重绘区域 nullptr = 全部区域
			*/
			virtual void UpdateLayout(MPCRect rect);

			/*更新显示
			* @param rect - 更新区域 nullptr = 全部区域
			*/
			virtual void UpdateDisplay(MPCRect rect);

			/*设置渲染模式
			* (默认被动渲染)
			* @param active - 是否为主动渲染模式
			*/
			virtual void SetRenderMode(bool active);

			//获取渲染模式
			virtual bool GetRenderMode() const;

			/*缩放窗口
			* 控件尺寸和位置将按照比例自动计算
			* @param newWidth - 新宽度
			* @param newHeight - 新高度
			*/
			virtual void ScaleWindow(_m_uint newWidth, _m_uint newHeight);

			/*缩放窗口 指定缩放比
			* 控件尺寸和位置将按照比例自动计算
			* @param scale - 新缩放比
			*/
			virtual void ScaleWindow(_m_scale scale);

			//取窗口缩放比例
			virtual _m_scale GetWindowScale() const;

			/*设定计时器
			* @param elapse - 计时器间隔
			* @param callback - 计时器回调函数
			* 
			* @return 返回定时器ID
			*/
			virtual MTimers::ID SetTimer(_m_uint elapse, const MTimerCallback& callback = nullptr);

			/*删除计时器
			* @param id - 计时器ID
			*/
			virtual void KillTimer(MTimers::ID id);

			/*限制最大渲染帧率
			* @param - fps 设置最大帧速率限制 -1=无限制
			*/
			virtual void SetMaxFPSLimit(int fps);

			//获取最后一次绘制的帧率
			virtual _m_uint GetLastFPS() const;

			//设置主窗口标志
			virtual void SetMainWindow(bool main);

			//是否为主窗口
			virtual bool IsMainWindow() const;

			/*设置资源缓存模式 Lite版本无效
			* 启用资源缓存将会利用更多的内存或显存缓存绘制结果来提高渲染效率
			* 并不是所有控件都一定有效果 这取决于控件的支持情况和渲染器的支持情况
			* MiaoUI的内置控件和渲染器均支持该选项
			* param cache - 是否启用资源缓存
			*/
			virtual void SetCacheMode(bool cache);

			//获取资源缓存模式
			virtual bool GetResMode();

			//获取渲染器
			Render::MRenderCmd* GetRender() const;

			//获取资源管理器
			UIResourceMgr* GetResourceMgr() const;

			//获取窗口当前焦点控件
			virtual UIFocus GetFocusControl() const;

			//查找顶级控件 例如菜单
			Ctrl::UIControl* FindTopLevelControl(std::wstring_view name) const;

			/*显示界面库控件的矩形边框
			* 仅供调试使用 会降低性能
			* 设置为true启用 false关闭 默认关闭
			*/
			void ShowDebugRect(bool show);

			/*以高亮标记当前焦点控件
			 * 仅供调试使用 需要启用调试矩形
			 */
			void ShowHighlight(bool show);

			//更新所有控件缓存
			virtual void UpdateCache();

			//取XMLUI类
			XML::MuiXML* XMLUI() { return m_xmlUI; }

			//批渲染数据
			struct renderData
			{
				
			};

		protected:

			//设置窗口当前焦点控件
			virtual void SetFocusControl(Ctrl::UIControl* control);

			//设置控件消息捕获
			virtual void SetCapture(Ctrl::UIControl* control);

			//释放控件消息捕获
			virtual void ReleaseCapture();

			//窗口原始消息处理
			virtual _m_result EventSource(MEventCodeEnum code, _m_param param);

			/*DispatchMouseMessage
			* or
			* DispatchWindowMessage
			*/
			bool DispatchControlMessage(Ctrl::UIControl* control, MEventCodeEnum code, _m_param param, bool mouse);

			//鼠标消息处理
			bool EventMouseProc(MEventCodeEnum code, _m_param param);

			/*消息处理过程
			* @param event - 控件消息类型
			* @param control - 发送消息的控件
			* @param param - 控件附加参数
			*
			* @return 返回值
			*/
			virtual bool EventProc(UINotifyEvent event, Ctrl::UIControl* contorl, _m_param param) = 0;

			//呈现绘制结果
			virtual void Present(Render::MRenderCmd* render, const _m_rect_t<int>* dirtyArea) = 0;

			//初始化渲染器
			virtual bool InitRender(Render::MRenderCmd* render) = 0;

			void SetInited(bool inited);

			void UpdateScale();

			//创建Node设备资源 调用窗口控件树UINodeBase的OnLoadResource
			void LoadNodeResource(bool recreate);

		private:
			Render::Def::MRender* m_render = nullptr;				//窗口渲染器
			Render::MRenderCmd* m_renderCmd = nullptr;				//渲染命令管理器
			UIResourceMgr* m_resourceMgr = nullptr;					//资源管理器
			Ctrl::UIControl* m_root = nullptr;			  			//根控件指针
			Ctrl::UIControl* m_toplay = nullptr;			  		//顶级控件指针
			Ctrl::UIControl* m_rootBox = nullptr;					//根容器
			_m_scale m_scale = { 1.f,1.f };							//窗口缩放比例
			_m_byte m_alpha = 255;								    //窗口不透明度
			MTimers* m_timer = nullptr;								//窗口定时器
			UIFocus m_focus = { nullptr };							//当前焦点控件
			Ctrl::UIControl* m_capture = nullptr;		  			//当前捕获控件
			Ctrl::UIControl* m_mouseCur = nullptr;					//当前鼠标消息控件
			bool m_mouseIn = false;									//当前鼠标Hover状态
			MFPSCounter m_fpsCounter;								//FPS计数器
			_m_uint m_fpsCache = 0;									//当前FPS缓存值
			bool m_isMainWnd = false;								//是否为主窗口
			bool m_cacheRes = false;								//资源缓存模式
			bool m_inited = false;									//是否已初始化完毕
			Render::MNodeRoot* m_renderRoot;
			Render::MPenPtr m_dbgFrame = nullptr;
			bool m_highlight = false;
			bool m_updateCache = true;

			XML::MuiXML* m_xmlUI = nullptr;

			//MQueue<_m_rect_t<int>> m_drawCmdList;					//绘制命令队列
			Render::MRingQueue<_m_rect_t<int>> m_drawCmdList;
			std::atomic_bool m_renderMode;					  		//是否为主动渲染模式
																	
			void RenderControlTree(const _m_rect_t<int>* dirtyArea);//渲染控件树
			void ThreadProc();										//独立窗口线程
			void FreeCurMouseCtrl();								//释放当前鼠标控件

			//线程任务
			struct taskParam
			{
				std::function<void()> task = nullptr;
				std::promise<void>& notification;
			};
			std::vector<taskParam> m_threadTaskList;
			void ExecuteThreadTask(const std::function<void()>& task);

			void ResumeThread();

			friend class Ctrl::UIControl;
			friend class Render::UINodeBase;
			friend class XML::MuiXML;
			friend class Mui::MiaoUI;
			friend class Mui::MWindowCtx;
		};
	}
}