/**
 * FileName: Mui_RenderNode.h
 * Note: 渲染Node声明
 *
 * Copyright (C) 2022-2023 Maplespe (mapleshr@icloud.com)
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
 * date: 2022-11-23 Create
*/
#pragma once
#include <Render/Mui_RenderMgr.h>

namespace Mui::Render
{
	class MNodeRoot;

	class MRenderNode
	{
	public:
		virtual ~MRenderNode();
	protected:
		MRenderNode() = default;
		MRenderNode(MRenderNode* parent);
			
		//设置和获取Node可见性
		void Visible(bool visible);
		[[nodiscard]] bool Visible() const;

		//设置和获取Name
		void Name(const UIString& name);
		[[nodiscard]] UIString Name() const;

		//添加子Node
		void AddChildNode(MRenderNode* node);
		//移除子Node
		bool DelChildNode(MRenderNode* node);
		bool DelChildNode(const UIString& name);
		//查找子Node
		[[nodiscard]] MRenderNode* FindChildNode(const UIString& name) const;

		//设置父和Node nullptr则从父Node移除
		void Parent(MRenderNode* node);
		[[nodiscard]] MRenderNode* Parent() const;

		virtual void OnRender(MRenderCmd* render, void* data) = 0;
		virtual void OnRenderChildEnd(MRenderCmd* render, void* data) = 0;

		MRenderNode(MNodeRoot* root);
		void DelChildNode(std::vector<MRenderNode*>::iterator& iter);

		auto& GetNodeList() { return m_nodeList; }

		MRenderCmd* m_render = nullptr;

	private:
		bool m_visible = true;
		bool m_parentVisible = true;

		MRenderNode* m_parent = nullptr;
		MNodeRoot* m_root = nullptr;

		UIString m_name;

		std::vector<MRenderNode*> m_nodeList;

		std::recursive_mutex m_lock;

		friend class MNodeRoot;
	};

	class MNodeRoot
	{
	public:
		MNodeRoot(MRenderNode* root);
		virtual ~MNodeRoot();
		[[nodiscard]] MRenderNode* RootNode() const;

		void RenderTree(void* data);

		[[nodiscard]] size_t GetCount() const
		{
			return m_drawList.size();
		}

		//设定回调后UnbindNodeRenderFunc时将会调用 通知控件树发生已变动
		void SetUnBindCallback(std::function<void(MRenderNode*)>&& callback)
		{
			m_callback = std::move(callback);
		}

	private:
		std::mutex mx;

		//Node绘制列表
		struct drawNode
		{
			MRenderNode* node = nullptr;
		};
		std::vector<drawNode> m_drawList;
		MRenderNode* m_rootNode = nullptr;
		MRenderCmd* m_render = nullptr;

		std::function<void(MRenderNode*)> m_callback = nullptr;

		void BindNodeRenderFunc(MRenderNode* last, MRenderNode* node);
		void UnbindNodeRenderFunc(MRenderNode* node);

		friend class MRenderNode;
	};
}
