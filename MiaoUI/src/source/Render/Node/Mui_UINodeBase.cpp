/**
 * FileName: Mui_UINodeBase.cpp
 * Note: UINodeBase实现
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
#include <Render/Node/Mui_UINodeBase.h>
#include <Render/Graphs/Mui_Render.h>
#include <Control/Mui_Control.h>

namespace Mui::Render
{
	UINodeBase::UINodeBase()
	{
		m_initialized = false;
	}

	UINodeBase::~UINodeBase()
	{
		auto& list = GetNodeList();
		for (size_t i = 0; i < list.size(); i++)
		{
			delete list[i];
		}
		list.clear();
	}

	void UINodeBase::SetName(const UIString& name)
	{
		Name(name);
	}

	UIString UINodeBase::GetName() const
	{
		return Name();
	}

	void UINodeBase::SetPos(UIPoint position, bool draw)
	{
		m_data.Position = position;
		if (draw) UpdateLayout();
	}

	void UINodeBase::SetPos(int x, int y, bool draw)
	{
		SetPos({ x, y }, draw);
	}

	UIPoint UINodeBase::GetPos() const
	{
		return m_data.Position;
	}

	void UINodeBase::SetPosUnit(PosSizeUnit unit, bool draw)
	{
		m_data.PosUnit = unit;
		if (draw) UpdateLayout();
	}

	UINodeBase::PosSizeUnit UINodeBase::GetPosUnit() const
	{
		return m_data.PosUnit;
	}

	void UINodeBase::SetSize(UISize size, bool draw)
	{
		m_data.Size = size;
		if (draw) UpdateLayout();
	}

	void UINodeBase::SetSize(int width, int height, bool draw)
	{
		SetSize({ width, height }, draw);
	}

	UISize UINodeBase::GetSize() const
	{
		return m_data.Size;
	}

	void UINodeBase::SetSizeUnit(PosSizeUnit unit, bool draw)
	{
		m_data.SizeUnit = unit;
		if (draw) UpdateLayout();
	}

	UINodeBase::PosSizeUnit UINodeBase::GetSizeUnit() const
	{
		return m_data.SizeUnit;
	}

	void UINodeBase::SetMinSize(UISize size, bool draw)
	{
		m_data.MinSize = size;
		if (draw) UpdateLayout();
	}

	UISize UINodeBase::GetMinSize() const
	{
		return m_data.MinSize;
	}

	void UINodeBase::SetMaxSize(UISize size, bool draw)
	{
		m_data.MaxSize = size;
		if (draw) UpdateLayout();
	}

	UISize UINodeBase::GetMaxSize() const
	{
		return m_data.MaxSize;
	}

	void UINodeBase::SetPadding(UIRect padding, bool draw)
	{
		m_data.Padding = padding;
		if (draw) UpdateLayout();
	}

	UIRect UINodeBase::GetPadding() const
	{
		return m_data.Padding;
	}

	void UINodeBase::SetAlignType(UIAlignment type, bool draw)
	{
		m_data.Align.SetType(type);

		if (draw)
			UpdateLayout();
	}

	UIAlignment UINodeBase::GetAlignType() const
	{
		return m_data.Align.GetType();
	}

	void UINodeBase::SetVisible(bool visible, bool draw)
	{
		Visible(visible);

		if (draw)
			UpdateDisplay();
	}

	bool UINodeBase::IsVisible() const
	{
		return Visible();
	}

	void UINodeBase::SetAlpha(_m_byte alpha, bool draw)
	{
		m_data.AlphaSrc = alpha;
		_m_byte parentAlpha = 255;
		if(auto parent = static_cast<UINodeBase*>(Parent()); parent)
		{
			parentAlpha = parent->m_data.AlphaDst;
		}
		m_data.AlphaDst = _m_color::AlphaBlend(parentAlpha, m_data.AlphaSrc);

		if (draw)
			UpdateDisplay();
	}

	_m_byte UINodeBase::GetAlpha() const
	{
		return m_data.AlphaSrc;
	}

	void UINodeBase::SetParent(UINodeBase* UINode)
	{
		Parent(UINode);
	}

	UINodeBase* UINodeBase::GetParent() const
	{
		return (UINodeBase*)Parent();
	}

	void UINodeBase::AddChildren(UINodeBase* UINode)
	{
		AddChildNode(UINode);
		UINode->m_data.ParentWnd = m_data.ParentWnd;
		UINode->m_data.DPIScale = m_data.DPIScale;
		UINode->m_data.AlphaDst = _m_color::AlphaBlend(m_data.AlphaDst, UINode->m_data.AlphaSrc);
		UINode->InitDeviceResource();
	}

	void UINodeBase::RemoveChildren(UINodeBase* UINode)
	{
		DelChildNode(UINode);
		UINode->m_data.ParentWnd = nullptr;
		UINode->m_data.DPIScale = { 1.f, 1.f };
	}

	void UINodeBase::RemoveChildren(const UIString& name)
	{
		DelChildNode(name);
	}

	UINodeBase* UINodeBase::FindChildren(const UIString& name) const
	{
		return (UINodeBase*)FindChildNode(name);
	}

	_m_uint UINodeBase::GetChildrenCount()
	{
		_m_uint count = 0;
		for (const auto node : GetNodeList())
		{
			if (!static_cast<UINodeBase*>(node)->m_data.HideThis)
				count++;
		}
		return count;
	}

	void UINodeBase::GetChildrenList(std::vector<UINodeBase*>& list)
	{
		for (auto node : GetNodeList())
		{
			auto _node = (UINodeBase*)node;
			if (!_node->m_data.HideThis)
				list.push_back(_node);
		}
	}

	void UINodeBase::UpdateDisplay(bool updateCache)
	{
		if (updateCache)
			m_cacheUpdate = true;
		if (m_data.ParentWnd)
		{
			auto t = m_data.Frame.ToRectT<int>();
			m_data.ParentWnd->UpdateDisplay(&t);
		}
	}

	void UINodeBase::UpdateLayout()
	{
		if (!m_render)
			return;

		m_render->RunTask([this]
		{
			const auto parent = GetValidParent();
			if (!parent)
			{
				if (dynamic_cast<UINodeBase*>(m_data.ParentWnd->GetRootControl()) == this)
					m_data.ParentWnd->UpdateLayout(nullptr);
				return;
			}
			if (parent->m_data.AutoSize)
			{
				parent->UpdateLayout();
				UpdateDisplay();
				return;
			}
			//线性布局将影响下一个Node
			switch (parent->m_data.Align.GetType())
			{
			case UIAlignment_Block:
			case UIAlignment_LinearV:
			case UIAlignment_LinearVB:
			case UIAlignment_LinearVR:
			case UIAlignment_LinearVBR:
			case UIAlignment_LinearH:
			case UIAlignment_LinearHB:
			case UIAlignment_LinearHL:
			case UIAlignment_LinearHLB:
			{
				//查找当前Node在父Node中的Index
				auto& list = parent->GetNodeList();
				const auto thisIter = std::find(list.begin(), list.end(), (MRenderNode*)this);
				const auto thisIndex = (size_t)std::distance(list.begin(), thisIter);
				//从当前node往后刷新
				parent->m_data.Align.Layout(parent, thisIndex, false);
			}
			break;
			//直接计算
			case UIAlignment_Absolute:
			{
				auto [box, clip] = m_data.Align.CalcPadding(parent);
				_m_pointf pt;
				_m_rectf& frame = m_data.Align.CalcBaseFrame(this, box, pt, { box.left, box.top }, false);
				const _m_sizef _size = m_data.Align.CalcContentSize(this, false);
				frame.right = frame.left + _size.width;
				frame.bottom = frame.top + _size.height;

				m_data.Align.Intersect(&m_data.ClipFrame, &box, &m_data.Frame);

				m_data.Align.Layout(this, 0, false);

				OnLayoutCalced();
			}
			break;
			case UIAlignment_Center:
			{
				auto [box, clip] = m_data.Align.CalcPadding(parent);
				const _m_sizef boxSize = { box.GetWidth(), box.GetHeight() };
				_m_pointf pt;
				_m_rectf& frame = m_data.Align.CalcBaseFrame(this, box, pt, { box.left, box.top }, false);
				const _m_sizef _size = m_data.Align.CalcContentSize(this, false);
				frame.left = box.left + (boxSize.width - _size.width) / 2.f + pt.x;
				frame.top = box.top + (boxSize.height - _size.height) / 2.f + pt.y;
				frame.right = frame.left + _size.width;
				frame.bottom = frame.top + _size.height;

				m_data.Align.Intersect(&m_data.ClipFrame, &box, &m_data.Frame);

				m_data.Align.Layout(this, 0, false);

				OnLayoutCalced();
			}
			break;
			default:
				m_data.Align.Layout(this);
				break;
			}

			m_cacheUpdate = true;
			UpdateDisplay();
		});
	}

	void UINodeBase::SetCacheType(UICacheType type)
	{
		m_data.CacheType = type;
		UpdateDisplay();
	}

	UICacheType UINodeBase::GetCacheType() const
	{
		return m_data.CacheType;
	}

	void UINodeBase::SetAnimationState(bool begin)
	{
		
	}

	void UINodeBase::SetBackground(UIBkgndStyle style)
	{
		if (style.bkgndColor == 0 && style.ShadowColor == 0)
			m_brush.BkgndBrush = nullptr;
		if (style.FrameWidth == 0)
			m_brush.FramePen = nullptr;

		if (memcmp(&m_bgStyle, &style, sizeof UIBkgndStyle) != 0)
		{
			if (style.bkgndColor != 0 || style.ShadowColor != 0) 
			{
				if (!m_brush.BkgndBrush)
					m_brush.BkgndBrush = m_render->CreateBrush(style.bkgndColor);
				else
					m_brush.BkgndBrush->SetColor(style.bkgndColor);
			}
			if (style.FrameWidth != 0)
			{
				if (!m_brush.FramePen)
					m_brush.FramePen = m_render->CreatePen(style.FrameWidth, style.FrameColor);
				else
					m_brush.FramePen->SetWidthAndColor(style.FrameWidth, style.FrameColor);
			}
			m_bgStyle = style;
			m_cacheUpdate = true;
			UpdateDisplay();
		}
	}

	UIBkgndStyle UINodeBase::GetBkgndStyle() const
	{
		return m_bgStyle;
	}

	void UINodeBase::SetScale(_m_rcscale scale, bool draw)
	{
		m_data.RectScale = scale;
		UpdateScale();
		if (draw)
			UpdateLayout();
	}

	_m_rcscale UINodeBase::GetScale() const
	{
		return m_data.RectScale;
	}

	void UINodeBase::EnableDPIScale(bool enable)
	{
		m_data.EnableDPIScale = enable;
		UpdateScale();
	}

	bool UINodeBase::IsDPIScaleEnabled() const
	{
		return m_data.EnableDPIScale;
	}

	void UINodeBase::Frame(_m_rect frame)
	{
		if (m_data.HideThis)
			m_data.Frame = frame.ToRectT<float>();
	}

	_m_rect UINodeBase::Frame() const
	{
		return m_data.Frame.ToRectT<int>();
	}

	void UINodeBase::OnLoadResource(MRenderCmd* render, bool recreate)
	{
		m_brush.BkgndBrush = nullptr;
		m_brush.FramePen = nullptr;

		if (m_bgStyle.bkgndColor != 0 || m_bgStyle.ShadowColor != 0)
		{
			if (!m_brush.BkgndBrush)
				m_brush.BkgndBrush = m_render->CreateBrush(m_bgStyle.bkgndColor);
			else
				m_brush.BkgndBrush->SetColor(m_bgStyle.bkgndColor);
		}
		if (m_bgStyle.FrameWidth != 0)
		{
			if (!m_brush.FramePen)
				m_brush.FramePen = m_render->CreatePen(m_bgStyle.FrameWidth, m_bgStyle.FrameColor);
			else
				m_brush.FramePen->SetWidthAndColor(m_bgStyle.FrameWidth, m_bgStyle.FrameColor);
		}
	}

	UINodeBase* UINodeBase::GetValidParent() const
	{
		auto parent_ = Parent();
		if (!parent_) return nullptr;

		std::function<UINodeBase* (MRenderNode*)> parent
			= [&parent](MRenderNode* node) -> UINodeBase*
		{
			if (const auto _cast = (UINodeBase*)node)
				return _cast;

			const auto _parent = static_cast<UINodeBase*>(node)->Parent();
			if (!_parent)
				return nullptr;

			return parent(_parent);
		};
		return parent(parent_);
	}

	void UINodeBase::UpdateScale()
	{
		if (m_data.ParentWnd)
			OnScale(m_data.ParentWnd->GetWindowScale());
		else
			OnScale({ 1.f, 1.f });
		m_cacheUpdate = true;
	}

	void UINodeBase::OnScale(_m_scale scale)
	{
		m_data.DPIScale = scale;
		for (const auto& node : GetNodeList())
		{
			if (auto cast = (UINodeBase*)node)
				cast->OnScale(scale);
		}
	}

	_m_sizef UINodeBase::GetContentSize()
	{
		auto getChildSize = [this](auto src) -> _m_rectf
		{
			if (src->GetNodeList().empty())
				return { 0, 0, 0, 0 };

			float minX = m_data.Frame.left, minY = m_data.Frame.top, maxR = (float)INT_MIN, maxB = (float)INT_MIN;
			for (const auto& node : src->GetNodeList())
			{
				const auto _node = (UINodeBase*)node;
				if (!_node || _node->m_data.HideThis) continue;

				const _m_rectf& frame = _node->m_data.Frame;

				if (frame.left < minX)
					minX = frame.left;
				if (frame.top < minY)
					minY = frame.top;
				if (frame.right > maxR)
					maxR = frame.right;
				if (frame.bottom > maxB)
					maxB = frame.bottom;
			}
			return { minX, minY, maxR, maxB };
		};

		_m_sizef retSize;

		//自动尺寸
		if(m_data.AutoSize || (m_data.SizeUnit.x_w == Percentage || m_data.SizeUnit.y_h == Percentage)
			|| (m_data.SizeUnit.x_w == FillMinus || m_data.SizeUnit.y_h == FillMinus))
		{
			const auto parent = GetValidParent();
			if (!parent && (m_data.SizeUnit.x_w == Percentage|| m_data.SizeUnit.y_h == Percentage
				|| m_data.SizeUnit.x_w == FillMinus || m_data.SizeUnit.y_h == FillMinus))
			{
				retSize = { 0, 0 };
			}
			else
			{
				const auto&& [xs, ys, ws, hs] = GetRectScale();
				const auto&& rect = getChildSize(this);
				retSize = { rect.right - rect.left, rect.bottom - rect.top };
				retSize.width += _scale_to(float(m_data.Padding.left + m_data.Padding.right), ws);
				retSize.height += _scale_to(float(m_data.Padding.top + m_data.Padding.bottom), hs);
			}
		}
		if (!m_data.AutoSize)
		{
			//绝对尺寸
			const auto&& [xs, ys, ws, hs] = GetRectScale();
			if (m_data.SizeUnit.x_w == Pixel)
				retSize.width = _scale_to((float)m_data.Size.width, ws);
			if (m_data.SizeUnit.y_h == Pixel)
				retSize.height = _scale_to((float)m_data.Size.height, hs);
		}
		return retSize;
	}

	_m_pointf UINodeBase::GetCalcedPoint()
	{
		_m_pointf pt;
		/*if(m_data.PosUint == Pixel)
		{*/
			auto [xs, ys, ws, hs] = GetRectScale();
			pt.x = _scale_to((float)m_data.Position.x, xs);
			pt.y = _scale_to((float)m_data.Position.y, ys);
		//}
		return pt;
	}

	void UINodeBase::OnRender(MRenderCmd* render, void* data)
	{
		bool isCache = false;

		_m_rect destRect = m_data.Frame.ToRectT<int>();

		//计算Alpha
		_m_byte parentAlpha = static_cast<UINodeBase*>(Parent())->m_data.AlphaDst;
		m_data.AlphaDst = _m_color::AlphaBlend(parentAlpha, m_data.AlphaSrc);

		render->PushClipRect(m_data.ClipFrame.ToRectT<int>());

		PaintBackground(render, &destRect, isCache);

		const auto clipRect = m_data.ClipFrame.ToRectT<int>();
		const MPaintParam paintpm = { render, &clipRect, &destRect, isCache, m_data.AlphaDst };
		OnPaintProc(&paintpm);

		render->PopClipRect();
	}

	void UINodeBase::OnRenderChildEnd(MRenderCmd* render, void* data)
	{
		if (!m_data.ParentWnd->m_dbgFrame && !m_brush.FramePen)
			return;

		_m_rect dst = m_data.Frame.ToRectT<int>();

		render->ResetCanvas();
		render->PushClipRect(m_data.ClipFrame.ToRectT<int>());

		if (m_brush.FramePen && m_bgStyle.FrameColor)
		{
			_m_byte&& parentAlpha = m_brush.FramePen->GetOpacity();
			auto scale = GetRectScale().scale();
			auto _scale = Helper::M_MIN(scale.cx, scale.cy);
			m_brush.FramePen->SetOpacity((_m_byte)((float)m_data.AlphaDst * (float)parentAlpha / 255.f));
			m_brush.FramePen->SetWidth(_scale_to(m_bgStyle.FrameWidth, _scale));
			if (m_bgStyle.RoundValue != 0.f)
			{
				const float clipValue = _scale * m_bgStyle.RoundValue;
				render->DrawRoundedRect(dst, clipValue, m_brush.FramePen);
			}
			else
				render->DrawRectangle(dst, m_brush.FramePen);

			m_brush.FramePen->SetOpacity(parentAlpha);
		}

		//调试矩形
		if (m_data.ParentWnd->m_dbgFrame)
		{
			if (m_data.ParentWnd->m_highlight && m_data.ParentWnd->m_focus.curFocus == this)
				m_data.ParentWnd->m_dbgFrame->SetColor(Color::M_BLUE);
			else
				m_data.ParentWnd->m_dbgFrame->SetColor(Color::M_RED);
			render->DrawRectangle(dst, m_data.ParentWnd->m_dbgFrame);
		}
		render->PopClipRect();
	}

	void UINodeBase::PaintBackground(MRenderCmd* render, MPCRect dst, bool cache)
	{
		if (m_bgStyle.bkgndColor && m_brush.BkgndBrush)
		{
			//保存Alpha
			_m_byte&& parentAlpha = m_brush.BkgndBrush->GetOpacity();
			//混合Alpha
			if (!cache)
				m_brush.BkgndBrush->SetOpacity(_m_byte((float)m_data.AlphaDst * (float)parentAlpha / 255.f));

			//绘制纯色背景
			if (m_bgStyle.RoundValue != 0.f)
			{
				auto scale = GetRectScale().scale();
				const float clipValue = Helper::M_MIN(scale.cx, scale.cy) * m_bgStyle.RoundValue;
				render->FillRoundedRect(*dst, clipValue, m_brush.BkgndBrush);
			}
			else
				render->FillRectangle(*dst, m_brush.BkgndBrush);

			//还原Alpha
			if (!cache)
				m_brush.BkgndBrush->SetOpacity(parentAlpha);
		}
	}

	void UINodeBase::HideNode(bool hide, bool draw)
	{
		m_data.HideThis = hide;
		if (draw)
		{
			if (const auto parent = GetValidParent(); parent)
				parent->UpdateLayout();
		}
	}

	bool UINodeBase::HideNode() const
	{
		return m_data.HideThis;
	}

	void UINodeBase::AutoSize(bool autosize, bool draw)
	{
		m_data.AutoSize = autosize;
		if (draw)
			UpdateLayout();
	}

	bool UINodeBase::AutoSize() const
	{
		return m_data.AutoSize;
	}

	void UINodeBase::InitDeviceResource()
	{
		if (m_initialized)
			return;
		OnLoadResource(m_render, false);
		m_initialized = true;
	}
}
