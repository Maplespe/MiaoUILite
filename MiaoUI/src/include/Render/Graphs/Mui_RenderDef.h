/**
 * FileName: Mui_RenderDef.h
 * Note: UI渲染接口定义
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
 * date: 2021-4-18 Create
*/

#pragma once
#include <Mui_Base.h>

namespace Mui::Render
{
	class MRenderCmd;

	namespace Def
	{
		enum class MImgFormat
		{
			PNG,
			JPG,
			BMP
		};

		class MRenderObj : public RAII::MBasicObj
		{
		protected:
			void ReleaseProc() final;
			virtual void ReleaseThis() = 0;

			bool IsBaseThread();
			bool IsDrawing();

			void* GetBaseRender();

			template<typename T>
			T* GetBaseRenderCast()
			{
				return static_cast<T*>(GetBaseRender());
			}

			void* m_base = nullptr;

			friend class Mui::Render::MRenderCmd;
		};

		//位图
		class MBitmap : public MRenderObj
		{
		public:

			//获取位图尺寸
			virtual UISize GetSize() = 0;
			//获取位图宽度
			virtual int GetWidth() = 0;
			//获取位图高度
			virtual int GetHeight() = 0;

		};

		using MBitmapPtr = RAII::Mui_Ptr<MBitmap>;

		//画布
		class MCanvas : public MRenderObj
		{
		public:

			//获取画布尺寸
			virtual UISize GetSize() = 0;
			//获取画布宽度
			virtual int GetWidth() = 0;
			//获取画布高度
			virtual int GetHeight() = 0;
			//获取画布标志
			virtual _m_param GetFlag() = 0;
			//获取画布子矩形
			virtual _m_rect GetSubRect() = 0;

		};

		using MCanvasPtr = RAII::Mui_Ptr<MCanvas>;

		//画笔
		class MPen : public MRenderObj
		{
		public:

			//设置画笔颜色
			virtual void SetColor(_m_color color) = 0;
			//设置画笔宽度
			virtual void SetWidth(_m_uint width) = 0;
			//设置画笔不透明度
			virtual void SetOpacity(_m_byte alpha) = 0;
			//设置宽度和颜色
			virtual void SetWidthAndColor(_m_uint width, _m_color color) = 0;

			//获取颜色
			virtual _m_color GetColor() = 0;
			//获取宽度
			virtual _m_uint GetWidth() = 0;
			//获取不透明度
			virtual _m_byte GetOpacity() = 0;

		};

		using MPenPtr = RAII::Mui_Ptr<MPen>;

		//画刷
		class MBrush : public MRenderObj
		{
		public:

			//设置画刷颜色
			virtual void SetColor(_m_color color) = 0;
			//设置画刷不透明度
			virtual void SetOpacity(_m_byte alpha) = 0;

			//获取颜色
			virtual _m_color GetColor() = 0;
			//获取不透明度
			virtual _m_byte GetOpacity() = 0;

		};

		using MBrushPtr = RAII::Mui_Ptr<MBrush>;

		//线性渐变画刷
		class MGradientBrush : public MRenderObj
		{
		public:

			//获取画刷颜色顶点数量
			virtual _m_uint GetColorPosCount() = 0;
			//获取画刷顶点颜色
			virtual _m_color GetPosColor(_m_uint index) = 0;

			//设置不透明度
			virtual void SetOpacity(_m_byte alpha) = 0;
			//获取不透明度
			virtual _m_byte GetOpacity() = 0;

			//获取开始点
			virtual UIPoint GetStartPoint() = 0;
			//设置开始点
			virtual void SetStartPoint(UIPoint start) = 0;

			//获取结束点
			virtual UIPoint GetEndPoint() = 0;
			//设置结束点
			virtual void SetEndPoint(UIPoint end) = 0;
		};

		using MGradientBrushPtr = RAII::Mui_Ptr<MGradientBrush>;

		//字体
		class MFont : public MRenderObj
		{
		public:

			//设置字体名称
			virtual void SetFontName(std::wstring_view name) = 0;
			//设置字体大小
			virtual void SetFontSize(_m_uint size, std::pair<_m_uint, _m_uint> range) = 0;
			//设置字体样式
			virtual void SetFontStyle(UIFontStyle style, std::pair<_m_uint, _m_uint> range) = 0;
			//设置字体颜色
			virtual void SetFontColor(MBrush* brush, std::pair<_m_uint, _m_uint> range) = 0;
			//设置文本
			virtual void SetText(std::wstring_view text) = 0;

			//测量字体矩形
			virtual UIRect GetMetrics() = 0;

			//获取字体名称
			virtual const UIString& GetFontName() = 0;
			//获取字体大小
			virtual _m_uint GetFontSize() = 0;
			//获取字体样式
			virtual UIFontStyle GetFontStyle() = 0;
			//获取字体颜色
			virtual _m_color GetFontColor() = 0;
			//获取文本
			virtual const UIString& GetText() = 0;

		};

		using MFontPtr = RAII::Mui_Ptr<MFont>;

		//效果
		class MEffects : public MRenderObj
		{
		public:

			enum Types
			{
				GaussianBlur,
				BlackAndWhite,
				Sepia
			};

			//获取效果名称
			virtual std::wstring GetEffectName() = 0;

			//获取效果类型
			virtual Types GetEffectType() = 0;

			//设置效果参数
			virtual void SetEffectValue(float param) = 0;

			//获取效果参数
			virtual float GetEffectValue() = 0;

		};

		using MEffectPtr = RAII::Mui_Ptr<MEffects>;
			
		//图形
		class MGeometry : public MRenderObj
		{
		public:

			enum MGeometryTypes
			{
				RoundRect,
				Ellipse
			};

			virtual MGeometryTypes GetGeometryType() = 0;
		};

		using MGeometryPtr = RAII::Mui_Ptr<MGeometry>;

		//批位图
		class MBatchBitmap : public MRenderObj
		{
		public:

			//添加子区域
			virtual void AddSub(_m_rect dst, _m_rect src, _m_byte alpha = 255) = 0;

			//删除子区域
			virtual bool DelSub(_m_uint index) = 0;

			//获取子区域数量
			virtual _m_uint GetCount() = 0;

			//清空子区域
			virtual void Clear() = 0;
		};

		using MBatchBitmapPtr = RAII::Mui_Ptr<MBatchBitmap>;

		class MRender : public MRenderObj
		{
		public:
			//渲染接口名称
			virtual _m_lpcwstr GetRenderName() = 0;

			//初始化渲染器
			virtual bool InitRender(_m_uint width, _m_uint height) = 0;

			//调整画布大小
			virtual bool Resize(_m_uint width, _m_uint height) = 0;

			/*创建空白画布
			* @param width - 位图宽度
			* @param heigth - 位图高度
			* @param param - 保留flag
			*
			* @return MCanvas 画布对象指针
			*/
			virtual MCanvas* CreateCanvas(_m_uint width, _m_uint height, _m_param param = 0U) = 0;

			/*创建内存位图从内存资源
			* @param resource - 图片资源数据
			* @param param - 保留
			*
			* @return MBitmap 内存位图
			*/
			virtual MBitmap* CreateBitmap(UIResource resource, _m_param param = 0U) = 0;

			/*创建内存位图从文件
			* @param path - 图片路径
			* @param param - 保留
			*
			* @return MBitmap 内存位图
			*/
			virtual MBitmap* CreateBitmap(std::wstring_view path, _m_param param = 0U) = 0;

			/*从像素数据创建位图
			* @param width - 图片宽
			* @param height - 图片高
			* @param bit - 像素数据
			* @param len - 像素字节
			* @param stride - 数据步长
			*
			* @return MBitmap 内存位图
			*/
			virtual MBitmap* CreateBitmap(_m_uint width, _m_uint height, void* bit, _m_uint len, _m_uint stride) = 0;

			/*从SVG文档创建位图 需要启用(MUI_CFG_ENABLE_SVGLOADSUP标志)
			* @param path - 文档路径 使用UTF-16编码
			* @param width - 栅格化宽度 0=图像原始宽度
			* @param height - 栅格化高度 0=图像原始高度
			* @param repColor - 是否用新的颜色替换
			* @param color - 如果替换 指定此颜色
			*
			* @return MBitmap 内存位图
			*/
			virtual MBitmap* CreateSVGBitmap(std::wstring_view path, _m_uint width, _m_uint height, bool repColor = false, _m_color color = 0) = 0;

			/*从SVG文档创建位图 需要启用(MUI_CFG_ENABLE_SVGLOADSUP标志)
			* @param xml - XML文档内容
			* @param width - 栅格化宽度 0=图像原始宽度
			* @param height - 栅格化高度 0=图像原始高度
			* @param repColor - 是否用新的颜色替换
			* @param color - 如果替换 指定此颜色
			*
			* @return MBitmap 内存位图
			*/
			virtual MBitmap* CreateSVGBitmapFromXML(std::wstring_view xml, _m_uint width, _m_uint height, bool repColor = false, _m_color color = 0) = 0;

			/*创建画笔
			* @param width - 画笔宽度
			* @param color - 画笔颜色
			*
			* @return MPen 内存画笔
			*/
			virtual MPen* CreatePen(_m_uint width, _m_color color) = 0;

			/*创建画刷
			* @param color - 画刷颜色
			*
			* @return MBrush 内存画刷
			*/
			virtual MBrush* CreateBrush(_m_color color) = 0;

			/*创建线性渐变画刷
			* @param vertex - 顶点数组<first-顶点颜色 second-顶点位置 0.0-0.1>
			* @param count - 顶点数量
			* @param start - 矩形开始位置
			* @param end - 矩形结束位置
			*/
			virtual MGradientBrush* CreateGradientBrush(const std::pair<_m_color, float>* vertex, _m_ushort count, UIPoint start, UIPoint end) = 0;

			/*创建字体
			* @param text - 文本内容
			* @param fontName - 字体名称
			* @param fontSize - 字体大小
			* @param fontCollection - 自定义字体集指针 需确定渲染引擎而定
			*
			* @return MFont 内存字体
			*/
			virtual MFont* CreateFonts(std::wstring_view text, std::wstring_view fontName, _m_uint fontSize, _m_ptrv fontCollection = 0) = 0;

			/*创建渲染效果
			* @param effect - 欲创建效果类型
			* @param value - 效果附加值w
			*/
			virtual MEffects* CreateEffects(MEffects::Types effect, float value) = 0;

			//创建圆角矩形图形
			virtual MGeometry* CreateRoundGeometry(_m_rect dest, float round) = 0;

			//创建椭圆图形
			virtual MGeometry* CreateEllipseGeometry(_m_rect dest) = 0;

			//创建子图集画布
			virtual MCanvas* CreateSubAtlasCanvas(_m_uint width, _m_uint height) = 0;

			//创建批位图
			virtual MBatchBitmap* CreateBatchBitmap() = 0;

			//复制位图内容
			virtual bool CopyBitmapContent(MBitmap* dst, MBitmap* src, UIPoint dstPt, _m_rect srcRect) = 0;

			virtual bool CopyBitmapContent(MCanvas* dst, MCanvas* src, UIPoint dstPt, _m_rect srcRect) = 0;

			//开始绘制
			virtual void BeginDraw() = 0;

			//设置当前渲染画布
			virtual void SetCanvas(MCanvas* canvas) = 0;

			//还原默认渲染画布
			virtual void ResetCanvas() = 0;

			/*绘制图像
			* @param Img - 输入图像
			* @param alpha - 绘制透明度
			* @param dest - 绘制区域
			* @param src - 源图区域
			* @param highQuality - 是否高质量绘制模式
			*/
			virtual void DrawBitmap(MBitmap* img, _m_byte alpha = 255, _m_rect dest = { 0 }, _m_rect src = { 0 }, bool highQuality = true) = 0;

			virtual void DrawBitmap(MCanvas* canvas, _m_byte alpha = 255, _m_rect dest = { 0 }, _m_rect src = { 0 }, bool highQuality = true) = 0;

			/*绘制批图像
			* @param bmp - 批图像列表
			* @param input - 输入图像
			* @param highQuality - 是否高质量绘制模式
			*/
			virtual void DrawBatchBitmap(MBatchBitmap* bmp, MBitmap* input, bool highQuality = true) = 0;

			virtual void DrawBatchBitmap(MBatchBitmap* bmp, MCanvas* input, bool highQuality = true) = 0;

			/*九宫格模式绘制图像
			* @param Img - 输入图像
			* @param alpha - 绘制透明度
			* @param dest - 绘制区域
			* @param src - 源图区域
			* @param margin - 九宫区
			* @param highQuality - 是否高质量绘制模式
			*/
			virtual void DrawNinePalacesImg(MBitmap* img, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin, bool highQuality = true) = 0;

			virtual void DrawNinePalacesImg(MCanvas* canvas, _m_byte alpha, _m_rect dest, _m_rect src, _m_rect margin, bool highQuality = true) = 0;

			/*绘制矩形边框
			* @param dest - 目标矩形位置
			* @param pen - 绘制用画笔
			*/
			virtual void DrawRectangle(_m_rect dest, MPen* pen) = 0;

			/*绘制圆角矩形边框
			* @param dest - 目标矩形位置
			* @param round - 圆角度
			* @param pen - 绘制用画笔
			*/
			virtual void DrawRoundedRect(_m_rect dest, float round, MPen* pen) = 0;

			/*填充矩形区域
			* @param dest - 目标矩形位置
			* @param brush - 绘制用画刷
			*/
			virtual void FillRectangle(_m_rect dest, MBrush* brush) = 0;

			virtual void FillRectangle(_m_rect dest, MGradientBrush* brush) = 0;

			/*填充圆角矩形区域
			* @param dest - 目标矩形位置
			* @param round - 圆角度
			* @param brush - 绘制用画刷
			*/
			virtual void FillRoundedRect(_m_rect dest, float round, MBrush* brush) = 0;

			/*绘制文本布局
			* @param font - 字体
			* @param dest - 目标矩形位置
			* @param brush - 绘制用画刷
			* @param alignment - 文本对齐方式
			*/
			virtual void DrawTextLayout(MFont* font, _m_rect dest, MBrush* brush, TextAlign alignment) = 0;

			/*绘制图像效果
			* @param img - 输入图像
			* @param effect - 输入效果
			* @param dest - 绘制区域
			* @param src - 源图区域
			*/
			virtual void DrawBitmapEffects(MBitmap* img, MEffects* effect, _m_byte alpha = 255, _m_rect dest = { 0 }, _m_rect src = { 0 }) = 0;

			virtual void DrawBitmapEffects(MCanvas* canvas, MEffects* effect, _m_byte alpha = 255, _m_rect dest = { 0 }, _m_rect src = { 0 }) = 0;

			/*绘制线
			* @param x - 线起始坐标
			* @param y - 线结束坐标
			* @param pen - 绘制用画笔
			*/
			virtual void DrawLine(UIPoint x, UIPoint y, MPen* pen) = 0;

			/*绘制椭圆
			* @param dest - 椭圆矩形区域
			* @param pen - 绘制用画笔
			*/
			virtual void DrawEllipse(_m_rect dest, MPen* pen) = 0;

			/*填充椭圆
			* @param dest - 椭圆矩形区域
			* @param pen - 绘制用画刷
			*/
			virtual void FillEllipse(_m_rect dest, MBrush* brush) = 0;

			/*设置裁剪矩形
			* @param rect - 裁剪区域
			*/
			virtual void PushClipRect(_m_rect rect) = 0;

			//恢复裁剪区域
			virtual void PopClipRect() = 0;

			/*设置裁剪图形
			* @param rect - 裁剪图形
			*/
			virtual void PushClipGeometry(MGeometry* geometry) = 0;

			//恢复裁剪图形
			virtual void PopClipGeometry() = 0;

			/*清空内容
			* @param color - 用指定颜色填充 默认透明
			*/
			virtual void Clear(_m_color color = 0) = 0;

			//结束绘制
			virtual _m_result EndDraw() = 0;

			//取当前画布
			virtual MCanvas* GetCanvas() = 0;

			//取渲染画布
			virtual MCanvas* GetRenderCanvas() = 0;

			//取原始对象
			virtual void* Get() = 0;

			//立即刷新命令队列
			virtual void Flush() = 0;

			//取共享画布
			virtual MCanvas* GetSharedCanvas() = 0;

			/*保存图像到文件
			* @param bitmap - 位图对象
			* @param path - 保存路径
			* @param format - 保存的格式
			* 
			* @return 是否成功
			*/
			virtual bool SaveMBitmap(MBitmap* bitmap, std::wstring_view path, MImgFormat format) = 0;

			virtual bool SaveMCanvas(MCanvas* canvas, std::wstring_view path, MImgFormat format) = 0;

			/*保存图像到内存
			* @param bitmap 位图对象
			* @param format - 保存的格式
			* 
			* @return 成功返回包含数据 失败返回空的UIResource()
			*/
			virtual UIResource SaveMBitmap(MBitmap* bitmap, MImgFormat format) = 0;

			virtual UIResource SaveMCanvas(MCanvas* canvas, MImgFormat format) = 0;

			/*检查子图集源
			* @param canvas - 待比较画布
			* @param canvas1 - 待比较画布
			* 
			* @return 如果两个子图集同属一个Atlas返回true
			*/
			virtual bool CheckSubAtlasSource(MCanvas* canvas, MCanvas* canvas1) = 0;
		};
	}
}
