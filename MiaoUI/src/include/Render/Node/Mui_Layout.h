/**
 * FileName: Mui_Layout.h
 * Note: UI布局器
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
 * date: 2023-4-2 Create
*/
#pragma once
#include <Mui_Base.h>

namespace Mui
{
	namespace Render { class UINodeBase; }

	class UILayouter
	{
	public:
		UILayouter() = default;
		virtual ~UILayouter() = default;
		void Layout(Render::UINodeBase*, size_t = 0, bool = true);

		void SetType(UIAlignment align);

		[[nodiscard]] UIAlignment GetType() const { return m_type; }

	protected:
		UIAlignment m_type = UIAlignment_Block;
		bool m_linearParam_vertical = true;
		bool m_linearParam_filp = false;
		bool m_linearParam_swap = false;

		void Intersect(_m_rectf* dst, const _m_rectf* rect1, const _m_rectf* rect2);

		//查找目标Node指定Index相邻且有效的上一个Node 找不到返回nullptr
		Render::UINodeBase* GetPreviousNode(Render::UINodeBase*, size_t);

		//计算Frame内容尺寸
		_m_sizef CalcContentSize(Render::UINodeBase*, bool);

		_m_sizef SizeClamp(Render::UINodeBase*, _m_sizef, _m_scale scale);

		//获取计算内边距后的Frame和ClipFrame
		std::pair<_m_rectf, _m_rectf> CalcPadding(Render::UINodeBase*);

		//基本Frame计算 int 0=默认 1=翻转x 2=翻转y 3=翻转xy
		_m_rectf& CalcBaseFrame(Render::UINodeBase*, const _m_rectf&, _m_pointf&, _m_pointf, bool, int = 0);

		void LayoutBlock(Render::UINodeBase*, size_t, bool);

		//param: vertical(bool)
		void LayoutLinear(Render::UINodeBase*, size_t, bool);

		void LayoutAbsolute(Render::UINodeBase*, size_t, bool);

		void LayoutCenter(Render::UINodeBase*, bool);

		void LayoutGrid(Render::UINodeBase*);

		void AnimationLayout(Render::UINodeBase*);

		friend class Render::UINodeBase;
	};
}