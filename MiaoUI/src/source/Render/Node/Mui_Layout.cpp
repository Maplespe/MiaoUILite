/**
 * FileName: Mui_Layout.cpp
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
#include <Render/Node/Mui_Layout.h>
#include <Render/Node/Mui_UINodeBase.h>
#include <Control/Mui_Control.h>

namespace Mui
{
	using namespace Render;

	void UILayouter::SetType(UIAlignment align)
	{
		m_type = align;

		m_linearParam_vertical = !(align == UIAlignment_LinearH || align == UIAlignment_LinearHL
			|| align == UIAlignment_LinearHLB || align == UIAlignment_LinearHB);

		m_linearParam_filp = align == UIAlignment_LinearHL || align == UIAlignment_LinearHLB
			|| align == UIAlignment_LinearVB || align == UIAlignment_LinearVBR;

		m_linearParam_swap = align == UIAlignment_LinearHLB || align == UIAlignment_LinearVBR
			|| align == UIAlignment_LinearHB || align == UIAlignment_LinearVR;
	}

	void UILayouter::Layout(UINodeBase* node, size_t begin, bool calcper)
	{
		/*if(node->m_isAnimation)
		{
			AnimationLayout(node);
			return;
		}*/
		switch (m_type)
		{
		case UIAlignment_Block:
			LayoutBlock(node, begin, calcper);
			break;
		case UIAlignment_LinearV:
		case UIAlignment_LinearVB:
		case UIAlignment_LinearVR:
		case UIAlignment_LinearVBR:
		case UIAlignment_LinearH:
		case UIAlignment_LinearHB:
		case UIAlignment_LinearHL:
		case UIAlignment_LinearHLB:
			LayoutLinear(node, begin, calcper);
			break;
		case UIAlignment_Absolute:
			LayoutAbsolute(node, begin, calcper);
			break;
		case UIAlignment_Center:
			LayoutCenter(node, calcper);
			break;
		case UIAlignment_Grid:
			LayoutGrid(node);
			break;
		}
	}

	void UILayouter::Intersect(_m_rectf* dst, const _m_rectf* rect1, const _m_rectf* rect2)
	{
		float rc1 = rect1->left;
		if (rect1->left <= rect2->left)
			rc1 = rect2->left;
		dst->left = rc1;

		float rc2 = rect1->right;
		if (rc2 >= rect2->right)
			rc2 = rect2->right;
		dst->right = rc2;

		if (Helper::M_DecimalEquals(rc1, rc2) || rc1 < rc2) 
		{
			float rc3 = rect1->top;
			if (rc3 <= rect2->top)
				rc3 = rect2->top;
			dst->top = rc3;

			float rc4 = rect1->bottom;
			if (rc4 >= rect2->bottom)
				rc4 = rect2->bottom;
			dst->bottom = rc4;

			if (Helper::M_DecimalEquals(rc3, rc4) || rc3 < rc4) 
			{
				return;
			}
		}

		dst->left = 0.0f;
		dst->top = 0.0f;
		dst->right = 0.0f;
		dst->bottom = 0.0f;
	}

	UINodeBase* UILayouter::GetPreviousNode(UINodeBase* node, size_t begin)
	{
		auto& list = node->GetNodeList();
		const size_t nodeCount = list.size();
		if (begin != 0 && begin < nodeCount)
		{
			//查找有效Node
			for (_m_long64 i = (_m_long64)(begin - 1); i >= 0; --i)
			{
				if (i < 0)
					break;
				auto cast = (UINodeBase*)list[i];
				if (!cast || cast->m_data.HideThis)
					continue;
				return cast;
			}
		}
		return nullptr;
	}

	std::pair<_m_rectf, _m_rectf> UILayouter::CalcPadding(UINodeBase* node)
	{
		_m_rectf box = node->m_data.Frame;
		auto padding = node->m_data.Padding.ToRectT<float>();
		auto [xs, ys, ws, hs] = node->GetRectScale();
		box.left += _scale_to(padding.left, ws);
		box.right -= _scale_to(padding.right, ws);
		box.top += _scale_to(padding.top, hs);
		box.bottom += _scale_to(padding.bottom, hs);
		return std::make_pair(box, node->m_data.ClipFrame);
	}

	_m_rectf& UILayouter::CalcBaseFrame(UINodeBase* node, const _m_rectf& box, _m_pointf& pt, _m_pointf start, bool calcper, int filp)
	{
		pt = node->GetCalcedPoint();
		_m_rectf& frame = node->m_data.Frame;

		if(filp == 1 || filp == 3)
			frame.right = start.x - pt.x;
		else
			frame.left = start.x + pt.x;

		if(filp == 2 || filp == 3)
			frame.bottom = start.y - pt.y;
		else
			frame.top = start.y + pt.y;

		//自动尺寸先给定父Size计算出子Size
		if (node->m_data.AutoSize 
			|| node->m_data.SizeUnit.x_w == UINodeBase::Percentage || node->m_data.SizeUnit.y_h == UINodeBase::Percentage
			|| node->m_data.SizeUnit.x_w == UINodeBase::FillMinus || node->m_data.SizeUnit.y_h == UINodeBase::FillMinus)
		{
			if(auto parent = node->GetValidParent())
			{
				if (node->m_data.SizeUnit.x_w == UINodeBase::Percentage || node->m_data.SizeUnit.x_w == UINodeBase::FillMinus)
				{
					float sizeW = static_cast<float>(node->m_data.Size.width) / 100.f;
					if (node->m_data.SizeUnit.x_w == UINodeBase::FillMinus)
						sizeW = 1.f;
					if(filp == 1 || filp == 3)
						frame.left = frame.right - (frame.right - parent->m_data.Frame.left) * sizeW;
					else
						frame.right = frame.left + (parent->m_data.Frame.right - frame.left) * sizeW;
				}
				else
				{
					if (filp == 1 || filp == 3)
						frame.left = box.left;
					else
						frame.right = box.right;
				}
				if (node->m_data.SizeUnit.y_h == UINodeBase::Percentage || node->m_data.SizeUnit.y_h == UINodeBase::FillMinus)
				{
					float sizeH = static_cast<float>(node->m_data.Size.height) / 100.f;
					if (node->m_data.SizeUnit.y_h == UINodeBase::FillMinus)
						sizeH = 1.f;
					if(filp == 2 || filp == 3)
						frame.top = frame.bottom - (frame.bottom - parent->m_data.Frame.top) * sizeH;
					else
						frame.bottom = frame.top + (parent->m_data.Frame.bottom - frame.top) * sizeH;
				}
				else
				{
					if (filp == 2 || filp == 3)
						frame.top = box.top;
					else
						frame.bottom = box.bottom;
				}
			}
			else
			{
				if(filp == 1 || filp == 3)
					frame.left = box.left;
				else
					frame.right = box.right;

				if(filp == 2 || filp == 3)
					frame.top = box.top;
				else
					frame.bottom = box.bottom;
			}
		}
		if(node->m_data.AutoSize)
			node->m_data.Align.Layout(node, 0, calcper);
		return frame;
	}

	_m_sizef UILayouter::CalcContentSize(UINodeBase* node, bool calcper)
	{
		_m_sizef ret;
		_m_scale scale = { 1.f, 1.f };
		_m_scale nodescale;
		bool minus = node->m_data.SizeUnit.x_w == UINodeBase::FillMinus || node->m_data.SizeUnit.y_h == UINodeBase::FillMinus;
		if (minus)
		{
			scale = node->GetRectScale().scale();
			nodescale = scale;
		}
		else
			nodescale = node->GetRectScale().scale();

		//按百分比尺寸
		if(!node->m_data.AutoSize && 
			(node->m_data.SizeUnit.x_w == UINodeBase::Percentage || node->m_data.SizeUnit.y_h == UINodeBase::Percentage || minus))
		{
			auto parent = node->GetValidParent();
			if (calcper && parent)
			{
				if (node->m_data.SizeUnit.x_w == UINodeBase::Percentage || node->m_data.SizeUnit.x_w == UINodeBase::FillMinus)
				{
					float sizeW = static_cast<float>(node->m_data.Size.width) / 100.f;
					if (node->m_data.SizeUnit.x_w == UINodeBase::FillMinus)
						sizeW = 1.f;

					ret.width = parent->m_data.Frame.GetWidth();
					ret.width = ret.width * sizeW;

					if(minus)
						ret.width -= _scale_to((float)node->m_data.Size.width, scale.cx);
				}
				else
					ret.width = node->GetContentSize().width;

				if (node->m_data.SizeUnit.y_h == UINodeBase::Percentage || node->m_data.SizeUnit.y_h == UINodeBase::FillMinus)
				{
					float sizeH = static_cast<float>(node->m_data.Size.height) / 100.f;
					if (node->m_data.SizeUnit.y_h == UINodeBase::FillMinus)
						sizeH = 1.f;

					ret.height = parent->m_data.Frame.GetHeight();
					ret.height = ret.height * sizeH;

					if (minus)
						ret.height -= _scale_to((float)node->m_data.Size.height, scale.cy);
				}
				else
					ret.height = node->GetContentSize().height;
			}
			else
			{
				if (node->m_data.SizeUnit.x_w == UINodeBase::Percentage || node->m_data.SizeUnit.x_w == UINodeBase::FillMinus)
				{
					ret.width = node->m_data.Frame.GetWidth();
					if(node->m_data.SizeUnit.x_w == UINodeBase::FillMinus)
						ret.width -= _scale_to((float)node->m_data.Size.width, scale.cx);
				}
				else
					ret.width = node->GetContentSize().width;

				if (node->m_data.SizeUnit.y_h == UINodeBase::Percentage || node->m_data.SizeUnit.y_h == UINodeBase::FillMinus)
				{
					ret.height = node->m_data.Frame.GetHeight();
					if(node->m_data.SizeUnit.y_h == UINodeBase::FillMinus)
						ret.height -= _scale_to((float)node->m_data.Size.height, scale.cy);
				}
				else
					ret.height = node->GetContentSize().height;
			}
		}
		else
			ret = node->GetContentSize();
		return SizeClamp(node, ret, nodescale);
	}

	_m_sizef UILayouter::SizeClamp(UINodeBase* node, _m_sizef size, _m_scale scale)
	{
		_m_sizef minsize = { (float)node->m_data.MinSize.width, (float)node->m_data.MinSize.height };

		minsize.width = _scale_to(minsize.width, scale.cx);
		minsize.height = _scale_to(minsize.height, scale.cy);

		if (size.width < minsize.width)
			size.width = minsize.width;
		if (size.height < minsize.height)
			size.height = minsize.height;

		_m_sizef maxsize = { (float)node->m_data.MaxSize.width, (float)node->m_data.MaxSize.height };
		if (!Helper::M_DecimalEquals(maxsize.width, -1.f))
		{
			maxsize.width = _scale_to(maxsize.width, scale.cx);
			if (size.width > maxsize.width)
				size.width = maxsize.width;
		}
		if (!Helper::M_DecimalEquals(maxsize.height, -1.f))
		{
			maxsize.height = _scale_to(maxsize.height, scale.cx);
			if (size.height > maxsize.height)
				size.height = maxsize.height;
		}

		return size;
	}

	void UILayouter::LayoutBlock(UINodeBase* node, size_t begin, bool calcper)
	{
		auto [box, clip] = CalcPadding(node);
		//计算子NodeFrame
		const float lineMaxR = box.right + 1.f;
		float lineMaxH = 0, top = box.top, lastX = box.left;

		//获取上一个Node基准位置
		if(const auto pre = GetPreviousNode(node, begin); pre)
		{
			const _m_rectf& frame = pre->m_data.Frame;
			lastX = frame.right;
			top = frame.top;
		}

		auto& list = node->GetNodeList();

		//获取Node所在行的最大高度
		if(begin != 0)
		{
			for (size_t i = begin - 1; i > 0; --i)
			{
				auto cast = (UINodeBase*)list[i];
				if (!cast || cast->m_data.HideThis)
					continue;
				//确定和目标Node在同一行
				if (!Helper::M_DecimalEquals(top, cast->m_data.Frame.top))
					break;
				lineMaxH = Helper::M_MAX(lineMaxH, cast->m_data.Frame.GetHeight());
			}
		}

		//计算子Node Frame
		const size_t nodeCount = list.size();
		for(size_t i = begin; i < nodeCount; ++i)
		{
			auto cast = (UINodeBase*)list[i];
			if (!cast) continue;

			_m_pointf pt;
			_m_rectf& frame = CalcBaseFrame(cast, box, pt, { lastX, top }, calcper);
			const _m_sizef _size = CalcContentSize(cast, calcper);

			frame.right = frame.left + _size.width;
			frame.bottom = frame.top + _size.height;

			//本行已经无法容纳 从下一行开始
			if(frame.right > lineMaxR && !Helper::M_DecimalEquals(lineMaxH, 0.f)) //lineMaxH代表此Node是本行唯一一个 因此无需换行
			{
				frame.left = box.left + pt.x;
				frame.right = frame.left + _size.width;
				frame.top += lineMaxH;
				frame.bottom = frame.top + _size.height;

				top += lineMaxH;
				lastX = frame.right;
				lineMaxH = pt.y + _size.height;
			}
			else
			{
				lastX = frame.right;
				lineMaxH = Helper::M_MAX(lineMaxH, pt.y + _size.height);
			}

			Intersect(&cast->m_data.ClipFrame, &clip, &cast->m_data.Frame);

			cast->m_data.Align.Layout(cast, 0, false);

			if (!calcper)
				cast->OnLayoutCalced();
		}
	}

	void UILayouter::LayoutLinear(UINodeBase* node, size_t begin, bool calcper)
	{
		const bool horizontal = !m_linearParam_vertical;	//是否为水平
		const bool filp = m_linearParam_filp;				//是否上下反转或左右反转
		const bool swap = m_linearParam_swap;				//是否左右反转或上下反转

		auto [box, clip] = CalcPadding(node);
		float last = horizontal ? (filp ? box.right : box.left) : (filp ? box.bottom : box.top);

		auto& list = node->GetNodeList();

		//获取上一个Node基准位置
		if (begin != 0)
		{
			if (const auto pre = GetPreviousNode(node, begin); pre)
			{
				const _m_rectf& frame = pre->m_data.Frame;
				last = horizontal ? (filp ? frame.left : frame.right) : (filp ? frame.top : frame.bottom);
			}
		}
		int type = 0;
		if (filp)
			type = horizontal ? 1 : 2;

		const size_t nodeCount = list.size();
		for (size_t i = begin; i < nodeCount; ++i)
		{
			auto cast = (UINodeBase*)list[i];
			if (!cast) continue;

			_m_pointf pt;
			_m_rectf& frame = CalcBaseFrame(cast, box, pt,
				{
					horizontal ? last : box.left,
					horizontal ? box.top : last
				},
				calcper, type
			);
			_m_sizef _size = CalcContentSize(cast, calcper);

			if (horizontal)
			{
				//Right
				if (filp)
				{
					frame.right = last - pt.x;
					frame.left = frame.right - _size.width;
				}
				else
				{
					frame.left = last + pt.x;
					frame.right = frame.left + _size.width;
				}

				//Bottom
				if (swap)
				{
					frame.bottom = box.bottom - pt.y;
					frame.top = frame.bottom - _size.height;
				}
				else
				{
					frame.top = box.top + pt.y;
					frame.bottom = frame.top + _size.height;
				}
			}
			else
			{
				//Right
				if (swap)
				{
					frame.right = box.right - pt.x;
					frame.left = frame.right - _size.width;
				}
				else
				{
					frame.left = box.left + pt.x;
					frame.right = frame.left + _size.width;
				}

				//Bottom
				if (filp)
				{
					frame.bottom = last - pt.y;
					frame.top = frame.bottom - _size.height;
				}
				else
				{
					frame.top = last + pt.y;
					frame.bottom = frame.top + _size.height;
				}
			}

			last = horizontal ? (filp ? frame.left : frame.right) : (filp ? frame.top : frame.bottom);

			Intersect(&cast->m_data.ClipFrame, &clip, &cast->m_data.Frame);

			cast->m_data.Align.Layout(cast, 0, false);

			if (!calcper)
				cast->OnLayoutCalced();
		}
	}

	void UILayouter::LayoutAbsolute(UINodeBase* node, size_t begin, bool calcper)
	{
		auto calcframe = CalcPadding(node);
		auto& list = node->GetNodeList();
		const size_t nodeCount = list.size();

		auto calc = [&list, &calcframe, &calcper, this](size_t index)
		{
			auto cast = (UINodeBase*)list[index];
			if (!cast) return;

			_m_pointf pt;
			_m_rectf& frame = CalcBaseFrame(cast, calcframe.first, pt, { calcframe.first.left, calcframe.first.top }, calcper);
			const _m_sizef _size = CalcContentSize(cast, calcper);
			frame.right = frame.left + _size.width;
			frame.bottom = frame.top + _size.height;

			Intersect(&cast->m_data.ClipFrame, &calcframe.second, &cast->m_data.Frame);

			cast->m_data.Align.Layout(cast, 0, false);

			if(!calcper)
				cast->OnLayoutCalced();
		};

		if (begin != 0 && begin < nodeCount)
			calc(begin);
		else
		{
			for (size_t i = 0; i < nodeCount; i++)
				calc(i);
		}
	}

	void UILayouter::LayoutCenter(UINodeBase* node, bool calcper)
	{
		auto [box, clip] = CalcPadding(node);
		const _m_sizef boxSize = { box.GetWidth(), box.GetHeight() };

		for (auto& _node : node->GetNodeList())
		{
			auto cast = (UINodeBase*)_node;
			if (!cast) continue;

			_m_pointf pt;
			_m_rectf& frame = CalcBaseFrame(cast, box, pt, { box.left, box.top }, calcper);
			_m_sizef _size = CalcContentSize(cast, calcper);

			frame.left = box.left + (boxSize.width - _size.width) / 2.f + pt.x;
			frame.top = box.top + (boxSize.height - _size.height) / 2.f + pt.y;
			frame.right = frame.left + _size.width;
			frame.bottom = frame.top + _size.height;

			Intersect(&cast->m_data.ClipFrame, &clip, &cast->m_data.Frame);

			cast->m_data.Align.Layout(cast, 0, false);

			if (!calcper)
				cast->OnLayoutCalced();
		}
	}

	void UILayouter::LayoutGrid(UINodeBase* node)
	{
	}

	void UILayouter::AnimationLayout(UINodeBase* node)
	{
		/*for (auto& _node : node->GetNodeList())
		{
			auto cast = (UINodeBase*)_node;

			auto scale = cast->GetRectScale();
			cast->m_data.ClipFrame = 
			{
				_scale_to(cast->aniClip.left, scale.xs),
				_scale_to(cast->aniClip.top, scale.ys),
				_scale_to(cast->aniClip.right, scale.ws),
				_scale_to(cast->aniClip.bottom, scale.hs)
			};
			cast->m_data.Frame =
			{
				_scale_to(cast->m_aniFrame.left, scale.xs),
				_scale_to(cast->m_aniFrame.top, scale.ys),
				_scale_to(cast->m_aniFrame.right, scale.ws),
				_scale_to(cast->m_aniFrame.bottom, scale.hs)
			};
			cast->m_data.Align.Layout(cast, 0, false);
		}*/
	}
}
