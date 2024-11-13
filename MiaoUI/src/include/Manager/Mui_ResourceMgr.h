/**
 * FileName: Mui_ResourceMgr.h
 * Note: UI资源管理器
 *
 * Copyright (C) 2023-2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2023-12-7 Create
*/
#pragma once
#include <set>
#include <Render/Graphs/Mui_RenderDef.h>
#include <FileSystem/DreamMoonRes.h>
#define MUIRESFILE std::pair<Mui::DMResources*, std::wstring>

namespace Mui
{
	enum class UIStyleType
	{
		Null,
		Image,
		Geometry
	};

	class UIResourceMgr;

	namespace Window { class UIWindowBasic; }

	class UIBitmap : public RAII::MBasicObj
	{
	public:
		UIBitmap(UIBitmap&&) = delete;
		UIBitmap(const UIBitmap&) = delete;
		UIBitmap& operator=(const UIBitmap&) = delete;
		UIBitmap& operator=(UIBitmap&&) = delete;

		Render::Def::MBitmapPtr GetBitmap() const { return m_bitmap; }

	protected:
		UIBitmap() = default;
		Render::Def::MBitmapPtr m_bitmap = nullptr;

		friend class UIResourceMgr;
	};

	using UIBitmapPtr = RAII::Mui_Ptr<UIBitmap>;

	class UIStyle : public RAII::MBasicObj
	{
	public:
		UIStyle(UIStyle&&) = delete;
		UIStyle(const UIStyle&) = delete;
		UIStyle& operator=(const UIStyle&) = delete;
		UIStyle& operator=(UIStyle&&) = delete;

		[[nodiscard]] UIStyleType GetType() const { return m_type; }
		[[nodiscard]] _m_ushort GetPartCount() const { return m_partCount; }
		[[nodiscard]] _m_rect_t<int> GetNineGridPath() const { return m_gridPath; }
		[[nodiscard]] bool IsNineGrid() const { return m_nineGrid; }

		//scale属性仅针对Geometry样式的线宽和圆角度缩放 不影响frame
		//@exception MErrorCode::InvalidParameter
		virtual void PaintStyle(Render::MRenderCmd* render, MPCRect dest,
			_m_byte alpha, int state, _m_ushort count, _m_scale scale = { 1.f, 1.f }) = 0;

	protected:
		UIStyle() = default;
		UIStyleType m_type = UIStyleType::Null;

		_m_ushort m_partCount = 0;

		bool m_nineGrid = false;
		_m_rect_t<int> m_gridPath = { 0 };

		friend class UIResourceMgr;
	};

	using UIStylePtr = RAII::Mui_Ptr<UIStyle>;

	class UIResourceMgr final
	{
	public:
		explicit UIResourceMgr(Render::MRenderCmd* render) : m_render(render) {}
		UIResourceMgr(UIResourceMgr&&) = delete;
		UIResourceMgr(const UIResourceMgr&) = delete;
		UIResourceMgr& operator=(const UIResourceMgr&) = delete;
		UIResourceMgr& operator=(UIResourceMgr&&) = delete;
		~UIResourceMgr();

		bool AddResourcePath(std::wstring_view path, std::wstring_view key);
		bool AddResourceMem(UIResource memfile, std::wstring_view key);

		//删除资源文件引用 如果有正在使用的共享的资源 这将导致共享资源在需要重载时无法加载
		bool RemoveResource(std::wstring_view path);

		/* 从已加载的Style列表中查找
		 * @param name - Style名称
		 * @return UIStyle* 该指针由资源管理器管理 不可释放
		 */
		UIStyle* FindStyle(std::wstring_view name);

		/* 从已添加的所有资源文件中加载Style到列表
		 * 已有或重名的不会重复加载
		 * @exception MErrorCode::InvalidParameter
		 */
		void LoadStyleList();

		/* 从资源文件中加载Style到列表
		 * @param resname - 在资源文件中的名称
		 * @param out dst - 输出UIStyle指针 可为空 不接收 (默认nullptr)
		 * @return 如果找不到资源项目或者列表已存在同名资源将失败(false)
		 */
		bool LoadStyle(std::wstring_view resname, UIStyle** dst = nullptr);

		/* 从内存创建ImageStyle 并添加到列表
		 * @param name - Style名称
		 * @param memimg - 内存图像资源
		 * @param count - Style Part数
		 * @param out dst - 输出UIStyle指针 可为空 不接收 (默认nullptr)
		 * @param nineGrid - 是否为九宫格素材 (默认false)
		 * @param gridpath - 宫格Rect (默认0)
		 * @return 重名或资源无效将导致失败(false)
		 */
		bool AddImageStyle(std::wstring_view name, UIResource memimg, _m_ushort count,
			UIStyle** dst = nullptr, bool nineGrid = false, _m_rect_t<int> gridpath = { 0 });

		/* 从xml创建图形命令Style 并添加到列表
		 * @param name - Style名称
		 * @param xml - 图形命令xml代码
		 * @param out dst - 输出UIStyle指针 可为空 不接收 (默认nullptr)
		 * @return 重名或xml代码格式无效将导致失败(false)
		 */
		bool AddGeometryStyle(std::wstring_view name, std::wstring_view xml, UIStyle** dst = nullptr);

		/* 从列表移除Style
		 * 此函数将Style从资源管理器的列表移除 不再管理 需要手动调用MSafeRelease释放资源
		 * @return 返回目标Style指针
		 */
		[[nodiscard]] UIStyle* RemoveStyle(std::wstring_view resname);

		/* 从资源文件中读取指定资源UIResource
		 * @param name - 资源名称
		 * @return 如果资源读取成功 需要使用Release释放资源
		 */
		UIResource ReadResource(std::wstring_view name);

		/* 从资源文件创建独立UI位图
		 * 删除资源文件引用时也不影响位图重建
		 * @param - name - 资源名称
		 * @return 如果资源读取成功 返回位图指针 否则返回nullptr
		 */
		UIBitmapPtr CreateUniqueUIBitamp(std::wstring_view name);

		/* 从内存资源创建独立UI位图
		 * 删除资源文件引用时也不影响位图重建
		 * @param name - 资源名称
		 * @return 如果资源读取成功 返回位图指针 否则返回nullptr
		 */
		UIBitmapPtr CreateUniqueUIBitamp(UIResource res);

		/* 从资源文件创建共享UI位图
		 * @param - name 资源名称
		 * @return 如果资源读取成功 返回位图指针 否则返回nullptr
		 * 多次创建同一资源名将只返回同一对象 共同使用
		 */
		UIBitmapPtr CreateSharedUIBitmap(std::wstring_view name);

	private:
		Render::MRenderCmd* m_render = nullptr;
		struct resfile
		{
			MUIRESFILE file;
			std::wstring path;
		};
		std::vector<resfile> m_resList;

		struct StyleData
		{
			UIStylePtr style = nullptr;
			bool memres = false;
			std::wstring srcname;
		};
		std::unordered_map<std::wstring, StyleData> m_styleList;
		std::unordered_map<std::wstring, UIBitmap*> m_sharedBmpList;

		bool LoadStyleFromDMRes(MUIRESFILE& file, std::wstring_view name, std::wstring& dst,
			bool res = false, UIResource* dstres = nullptr);

		bool LoadStyleInternal(MUIRESFILE& file, std::wstring_view name, UIStyle** dststyle);

		bool AddImageStyleInternal(UIResource memimg, _m_ushort count,
			UIStyle** dst = nullptr, bool nineGrid = false, _m_rect_t<int> gridpath = { 0 });

		bool AddGeometryStyleInternal(std::wstring_view xml, UIStyle** dst = nullptr);

		void FreeResFile(MUIRESFILE& file);

		friend class UIStyle;
		friend class UIMgrResource;
		friend class UIBitmapShared;
		friend class UIBitmapSVG;
		friend class Window::UIWindowBasic;
	};

	class UIStyleImage final : public UIStyle
	{
	protected:
		void PaintStyle(Render::MRenderCmd* render, MPCRect dest,
			_m_byte alpha, int state, _m_ushort count, _m_scale scale) override;

		UIBitmapPtr m_bitmap = nullptr;

		friend class UIResourceMgr;
	};

	class UIStyleGeometry final : public UIStyle
	{
	protected:
		void PaintStyle(Render::MRenderCmd* render, MPCRect dest,
			_m_byte alpha, int state, _m_ushort count, _m_scale scale) override;

		void InitResource(Render::MRenderCmd* render);

		struct CMD
		{
			int type = 0;
			_m_rect_t<int> m_dst = { 0 };
			_m_byte m_rctype[4] = { 0 };
			_m_color color = 0;
			float param = 0.f;
			int width = 0;
		};

		Render::Def::MPenPtr m_pen = nullptr;
		Render::Def::MBrushPtr m_brush = nullptr;
		std::vector<std::vector<CMD>> m_cmdlist;

		friend class UIResourceMgr;
	};
}