/**
 * FileName: Mui_Render.cpp
 * Note: 渲染功能助手实现
 *
 * Copyright (C) 2020-2022 Maplespe (mapleshr@icloud.com)
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

#include <Render/Graphs/Mui_Render.h>

namespace Mui::Render
{
	void NinePalaceDraw(const std::function<void(_m_rect, _m_rect)>& callback, _m_rect dest, _m_rect src, _m_rect margin)
	{
		//copy from soui
		int xDest[4] = { dest.left, dest.left + margin.left, dest.right - margin.right, dest.right };
		int xSrc[4] = { src.left, src.left + margin.left, src.right - margin.right, src.right };
		int yDest[4] = { dest.top, dest.top + margin.top, dest.bottom - margin.bottom, dest.bottom };
		int ySrc[4] = { src.top, src.top + margin.top, src.bottom - margin.bottom, src.bottom };

		//首先保证九宫分割正常
		if (!(xSrc[0] <= xSrc[1] && xSrc[1] <= xSrc[2] && xSrc[2] <= xSrc[3])) return;
		if (!(ySrc[0] <= ySrc[1] && ySrc[1] <= ySrc[2] && ySrc[2] <= ySrc[3])) return;

		//调整目标位置
		int nDestWid = dest.right - dest.left;
		int nDestHei = dest.bottom - dest.top;

		if ((margin.left + margin.right) > nDestWid)
		{//边缘宽度大于目标宽度的处理
			if (margin.left >= nDestWid)
			{//只绘制左边部分
				xSrc[1] = xSrc[2] = xSrc[3] = xSrc[0] + nDestWid;
				xDest[1] = xDest[2] = xDest[3] = xDest[0] + nDestWid;
			}
			else if (margin.right >= nDestWid)
			{//只绘制右边部分
				xSrc[0] = xSrc[1] = xSrc[2] = xSrc[3] - nDestWid;
				xDest[0] = xDest[1] = xDest[2] = xDest[3] - nDestWid;
			}
			else
			{//先绘制左边部分，剩余的用右边填充
				int nRemain = xDest[3] - xDest[1];
				xSrc[2] = xSrc[3] - nRemain;
				xDest[2] = xDest[3] - nRemain;
			}
		}

		if (margin.top + margin.bottom > nDestHei)
		{
			if (margin.top >= nDestHei)
			{//只绘制上边部分
				ySrc[1] = ySrc[2] = ySrc[3] = ySrc[0] + nDestHei;
				yDest[1] = yDest[2] = yDest[3] = yDest[0] + nDestHei;
			}
			else if (margin.bottom >= nDestHei)
			{//只绘制下边部分
				ySrc[0] = ySrc[1] = ySrc[2] = ySrc[3] - nDestHei;
				yDest[0] = yDest[1] = yDest[2] = yDest[3] - nDestHei;
			}
			else
			{//先绘制左边部分，剩余的用右边填充
				int nRemain = yDest[3] - yDest[1];
				ySrc[2] = ySrc[3] - nRemain;
				yDest[2] = yDest[3] - nRemain;
			}
		}

		for (int y = 0; y < 3; y++)
		{
			if (ySrc[y] == ySrc[y + 1]) continue;
			for (int x = 0; x < 3; x++)
			{
				if (xSrc[x] == xSrc[x + 1]) continue;
				_m_rect rcSrc;
				rcSrc.left = xSrc[x];
				rcSrc.top = ySrc[y];
				rcSrc.right = xSrc[x + 1];
				rcSrc.bottom = ySrc[y + 1];

				_m_rect rcDest;
				rcDest.left = xDest[x];
				rcDest.top = yDest[y];
				rcDest.right = xDest[x + 1];
				rcDest.bottom = yDest[y + 1];

				callback(rcDest, rcSrc);
			}
		}
	}
}
