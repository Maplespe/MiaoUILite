/**
 * FileName: Mui_RenderDef.cpp
 * Note: UI渲染接口基本方法实现
 *
 * Copyright (C) 2022 Maplespe (mapleshr@icloud.com)
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
 * date: 2022-8-13 Create
*/

#include <Render/Graphs/Mui_RenderDef.h>
#include <Render/Mui_RenderMgr.h>

namespace Mui::Render::Def
{
	void MRenderObj::ReleaseProc()
	{
		//如果是渲染线程 可以直接调用释放
		if (IsBaseThread())
		{
			ReleaseThis();
			return;
		}
		//否则需要将释放任务交给渲染线程调用
		static_cast<MRenderCmd*>(m_base)->Task([this]
		{
			ReleaseThis();
		});
	}

	bool MRenderObj::IsBaseThread()
	{
		return static_cast<MRenderCmd*>(m_base)->IsTaskThread();
	}

	bool MRenderObj::IsDrawing()
	{
		return static_cast<MRenderCmd*>(m_base)->m_begindraw;
	}

	void* MRenderObj::GetBaseRender()
	{
		return static_cast<MRenderCmd*>(m_base)->m_base;
	}
}
