/**
 * FileName: Mui_UINodeBase.h
 * Note: UINodeBase声明
 *
 * Copyright (C) 2022-2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2022-11-27 Create
*/
#pragma once
#include <Render/Node/Mui_RenderNode.h>
#include <Render/Node/Mui_Layout.h>
#include <Window/Mui_BasicWnd.h>

namespace Mui::Render
{
	enum class UICacheType
	{
		Auto,
		Enable,
		Disable
	};

	struct UIBkgndStyle
	{
		_m_color bkgndColor = 0;
		_m_color FrameColor = 0;
		_m_ushort FrameWidth = 0;
		float RoundValue = 0;
		_m_color ShadowColor = 0;		//Lite版本无效
		UIPoint ShadowOffset;			//Lite版本无效
		int ShadowExtend = 0;			//Lite版本无效	
		float ShadowRadius = 6.f;		//Lite版本无效
	};

	class UINodeBase : protected MRenderNode
	{
	public:
		UINodeBase();
		~UINodeBase() override;

		enum PosSizeUnitType
		{
			Default,	//默认
			Pixel,		//像素
			Percentage,	//百分比
			FillMinus	//填充减少
		};
		struct PosSizeUnit
		{
			PosSizeUnitType x_w = Pixel;
			PosSizeUnitType y_h = Pixel;
		};

		//设置名称
		virtual void SetName(const UIString& name);

		//获取名称
		[[nodiscard]] virtual UIString GetName() const;

		/*设置位置
		* @param point - 位置
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetPos(UIPoint position, bool draw = true);

		/*设置边框位置
		* @param x - left
		* @param y - top
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetPos(int x, int y, bool draw = true);

		//获取控件位置
		[[nodiscard]] virtual UIPoint GetPos() const;

		//设置pos单位
		virtual void SetPosUnit(PosSizeUnit unit, bool draw = true);

		//获取pos单位
		[[nodiscard]] virtual PosSizeUnit GetPosUnit() const;

		/*设置尺寸
		* @param size - 尺寸
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetSize(UISize size, bool draw = true);

		/*设置尺寸
		* @param width - 宽度
		* @param height - 高度
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetSize(int width, int height, bool draw = true);

		//获取尺寸
		[[nodiscard]] virtual UISize GetSize() const;

		//设置尺寸单位
		virtual void SetSizeUnit(PosSizeUnit unit, bool draw = true);

		//获取尺寸单位
		[[nodiscard]] virtual PosSizeUnit GetSizeUnit() const;

		/*设置最小尺寸限制
		* @param size - 尺寸
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetMinSize(UISize size, bool draw = true);

		//获取最小尺寸限制
		[[nodiscard]] virtual UISize GetMinSize() const;

		/*设置最大尺寸限制
		* @param size - 尺寸
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetMaxSize(UISize size, bool draw = true);

		//获取最大尺寸限制
		[[nodiscard]] virtual UISize GetMaxSize() const;

		/*设置内边距
		* @param padding - 内边距
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetPadding(UIRect padding, bool draw = true);

		//获取内边距
		[[nodiscard]] virtual UIRect GetPadding() const;

		/*设置定位类型
		* @param type - 定位方式
		* @param draw - 刷新绘制(默认true)
		*/
		virtual void SetAlignType(UIAlignment type, bool draw = true);

		//获取定位类型
		[[nodiscard]] virtual UIAlignment GetAlignType() const;

		/*设置可见性
		* @param visible - 可视
		* @param draw - 立即绘制(默认true)
		*/
		virtual void SetVisible(bool visible, bool draw = true);

		//是否可见
		[[nodiscard]] virtual bool IsVisible() const;

		/*设置不透明度
		* @param alpha - 不透明度(0-255)
		* @param draw - 立即绘制(默认true)
		*/
		virtual void SetAlpha(_m_byte alpha, bool draw = true);

		//获取不透明度
		[[nodiscard]] virtual _m_byte GetAlpha() const;

		//设置父控件 如果为null则将自身从父控件中移除
		virtual void SetParent(UINodeBase* UINode);

		//获取控件父级
		[[nodiscard]] virtual UINodeBase* GetParent() const;

		//添加子Node
		virtual void AddChildren(UINodeBase* UINode);

		//删除子Node
		virtual void RemoveChildren(UINodeBase* UINode);
		virtual void RemoveChildren(const UIString& name);

		//查找子Node
		[[nodiscard]] virtual UINodeBase* FindChildren(const UIString& name) const;

		//获取子Node数量
		[[nodiscard]] virtual _m_uint GetChildrenCount();

		//获取子Node列表
		virtual void GetChildrenList(std::vector<UINodeBase*>& list);

		//更新显示
		virtual void UpdateDisplay(bool updateCache = false);

		//更新布局显示
		virtual void UpdateLayout();

		//缓存支持
		virtual void SetCacheType(UICacheType type);

		//是否启用缓存
		[[nodiscard]] virtual UICacheType GetCacheType() const;

		/*设置动画状态 Lite版本无效
		* @param begin - 动画是准备开始还是已经结束
		*/
		virtual void SetAnimationState(bool begin);

		//设置背景样式
		virtual void SetBackground(UIBkgndStyle style);

		//获取背景样式
		[[nodiscard]] virtual UIBkgndStyle GetBkgndStyle() const;

		//设置Frame缩放比
		virtual void SetScale(_m_rcscale scale, bool draw = true);

		//获取Frame缩放比
		[[nodiscard]] virtual _m_rcscale GetScale() const;

		//是否启用DPI缩放
		virtual void EnableDPIScale(bool enable);

		[[nodiscard]] virtual bool IsDPIScaleEnabled() const;

		//设置Frame 仅HideNode = true时有效
		void Frame(_m_rect frame);

		//获取控件Frame矩形
		[[nodiscard]] _m_rect Frame() const;

		//隐藏当前Node 不参与计算 GetChildrenList也不包括 但不影响Find
		void HideNode(bool hide, bool draw = true);

		[[nodiscard]] bool HideNode() const;

		//设置是否自动计算尺寸
		void AutoSize(bool autosize, bool draw = true);

		//获取AutoSize
		[[nodiscard]] bool AutoSize() const;

		//手动初始化资源 如果资源已初始化则无效否则初始化资源
		void InitDeviceResource();

	protected:

		/*加载设备资源
		* 该函数至少会被调用一次 用以初始化设备资源
		* 如果为true则应当立即释放并重建MRender所创建的设备资源
		*/
		virtual void OnLoadResource(MRenderCmd* render, bool recreate);

		//获取有效的父Node 找不到返回nullptr
		[[nodiscard]] UINodeBase* GetValidParent() const;

		//已计算DPI
		_m_rcscale GetRectScale() const
		{
			_m_rcscale scale = m_data.RectScale;
			if (m_data.EnableDPIScale)
			{
				scale.xs = m_data.DPIScale.cx * scale.xs;
				scale.ys = m_data.DPIScale.cy * scale.ys;
				scale.ws = m_data.DPIScale.cx * scale.ws;
				scale.hs = m_data.DPIScale.cy * scale.hs;
			}
			return scale;
		}

		template<typename T>
		void SizeScale(T& dstX, T& dstY, bool dpi = true)
		{
			_m_scale scale = m_data.RectScale.scale();
			if(dpi && m_data.EnableDPIScale)
			{
				scale.cx *= m_data.DPIScale.cx;
				scale.cy *= m_data.DPIScale.cy;
			}
			dstX = _scale_to(dstX, scale.cx);
			dstY = _scale_to(dstY, scale.cy);
		}

		template<typename T>
		void SizeScale(T& dst, bool x, bool dpi = true)
		{
			_m_scale scale = m_data.RectScale.scale();
			if (dpi && m_data.EnableDPIScale)
			{
				if(x) scale.cx *= m_data.DPIScale.cx;
				else scale.cy *= m_data.DPIScale.cy;
			}
			dst = _scale_to(dst, x ? scale.cx : scale.cy);
		}

		void UpdateScale();
		virtual void OnScale(_m_scale scale);

		//获取控件尺寸 已计算DPI和Scale 在计算布局时被调用
		virtual _m_sizef GetContentSize();
		//获取已计算的Point值
		virtual _m_pointf GetCalcedPoint();
		//布局计算完成后调用
		virtual void OnLayoutCalced() {}

		//绘制
		void OnRender(MRenderCmd* render, void* data) override;
		void OnRenderChildEnd(MRenderCmd* render, void* data) override;

		struct MPaintParam
		{
			MRenderCmd* render = nullptr;
			MPCRect clipRect = nullptr;
			MPCRect destRect = nullptr;
			bool cacheCanvas = false;
			_m_byte blendedAlpha = 255;
		};
		using MPCPaintParam = const MPaintParam*;
		virtual void OnPaintProc(MPCPaintParam param) { }

		void PaintBackground(MRenderCmd* render, MPCRect dst, bool cache);
	public:
		struct data
		{
			UISize Size;				//Node尺寸
			PosSizeUnit SizeUnit;		//Node尺寸单位
			UIPoint Position;			//Node位置
			PosSizeUnit PosUnit;		//Node位置单位

			UISize MinSize;				//最小尺寸
			UISize MaxSize = { -1, -1 };//最大尺寸

			Window::UIWindowBasic* ParentWnd = nullptr;//父窗口

			_m_rectf Frame;				//Node矩形
			_m_rectf ClipFrame;			//clip矩形
			_m_rect Padding;			//内边距
			UILayouter Align;			//UI布局器

			_m_byte AlphaSrc = 255;		//原始不透明度值
			_m_byte AlphaDst = 255;		//当前不透明度值

			UICacheType CacheType = UICacheType::Auto;//缓存类型 Lite版本无效

			MCanvasPtr SubAtlas = nullptr;//子图集画布
			bool HideThis = false;		//隐藏当前Node
			bool AutoSize = false;		//自动计算Size
			bool EnableDPIScale = true;	//启用DPI缩放

			_m_scale DPIScale = { 1.f,1.f };
			_m_rcscale RectScale = { 1.f,1.f,1.f,1.f };
		};

	protected:

		data m_data;

		struct brush
		{
			MPenPtr FramePen = nullptr;	 //边框画笔
			MBrushPtr BkgndBrush = nullptr;//背景画刷
		} m_brush;

		UIBkgndStyle m_bgStyle;

		//Lite版本无效
		bool m_cacheSupport = false;
		bool m_cacheUpdate = true;

		std::atomic_bool m_initialized;

		friend class Mui::UILayouter;
		friend class Window::UIWindowBasic;
	};

}
