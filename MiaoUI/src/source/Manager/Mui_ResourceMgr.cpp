/**
 * FileName: Mui_ResourceMgr.cpp
 * Note: UI资源管理器
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
 * date: 2023-12-7 Create
*/
#include <Manager/Mui_ResourceMgr.h>
#include <Render/Graphs/Mui_Render.h>
#include <Render/Mui_RenderMgr.h>
#include <Mui_Helper.h>
#include <MiaoUI/src/source/ThirdParty/pugixml/pugixml.hpp>

namespace Mui
{
	using namespace Helper;

	UIResourceMgr::~UIResourceMgr()
	{
		m_sharedBmpList.clear();
		for(auto& file : m_resList)
		{
			FreeResFile(file.file);
		}
	}

	bool UIResourceMgr::AddResourcePath(std::wstring_view path, std::wstring_view key)
	{
		DMResources* dmres = new DMResources();
		if(dmres->LoadResource(path.data(), false))
		{
			m_resList.emplace_back(resfile{ std::make_pair(dmres, key.data()), path.data() });
			return true;
		}
		delete dmres;
		return false;
	}

	bool UIResourceMgr::AddResourceMem(UIResource memfile, std::wstring_view key)
	{
		DMResources* dmres = new DMResources();
		if (dmres->LoadResource(memfile))
		{
			m_resList.emplace_back(resfile{ std::make_pair(dmres, key.data()), L"" });
			return true;
		}
		delete dmres;
		return false;
	}

	bool UIResourceMgr::RemoveResource(std::wstring_view path)
	{
		if (path.empty())
			return false;
		for(auto iter = m_resList.begin(); iter != m_resList.end(); ++iter)
		{
			if (iter->path != path)
				continue;
			FreeResFile(iter->file);
			m_resList.erase(iter);
			return true;
		}
		return false;
	}

	UIStyle* UIResourceMgr::FindStyle(std::wstring_view name)
	{
		if (auto iter = m_styleList.find(name.data()); iter != m_styleList.end())
			return iter->second.style.get();
		return nullptr;
	}

	void UIResourceMgr::LoadStyleList()
	{
		for(auto& file : m_resList)
		{
			std::vector<std::wstring> list;
			file.file.first->EnumResourceName(list);
			for (const auto& name : list)
			{
				//v1通过名称前缀来确定类型
				if (name.substr(0, 6) != L"style_")
					continue;
				LoadStyleInternal(file.file, name, nullptr);
			}
		}
	}

	bool UIResourceMgr::LoadStyle(std::wstring_view resname, UIStyle** dst)
	{
		for (auto& file : m_resList)
		{
			std::vector<std::wstring> list;
			file.file.first->EnumResourceName(list);
			for (const auto& name : list)
			{
				if(name == resname)
					return LoadStyleInternal(file.file, name, dst);
			}
		}
		return false;
	}

	bool UIResourceMgr::AddImageStyle(std::wstring_view name, UIResource memimg, _m_ushort count,
		UIStyle** dst, bool nineGrid, _m_rect_t<int> gridpath)
	{
		StyleData data;
		data.memres = true;
		if(AddImageStyleInternal(memimg, count, &data.style, nineGrid, gridpath))
		{
			auto ret = m_styleList.insert(std::make_pair(name.data(), data));
			if (!ret.second)
			{
				data.style.rest();
				return false;
			}
			if (dst)
				*dst = data.style.get();
			return true;
		}
		return false;
	}

	bool UIResourceMgr::AddGeometryStyle(std::wstring_view name, std::wstring_view xml, UIStyle** dst)
	{
		StyleData data;
		data.memres = false;
		if (AddGeometryStyleInternal(xml, &data.style))
		{
			auto ret = m_styleList.insert(std::make_pair(name.data(), data));
			if (!ret.second)
			{
				data.style.rest();
				return false;
			}
			if (dst)
				*dst = data.style.get();
			return true;
		}
		return false;
	}

	UIStyle* UIResourceMgr::RemoveStyle(std::wstring_view resname)
	{
		auto iter = m_styleList.find(resname.data());
		if (iter != m_styleList.end())
		{
			auto style = iter->second.style;
			style->AddRef();
			m_styleList.erase(iter);
			return style.get();
		}
		return nullptr;
	}

	UIResource UIResourceMgr::ReadResource(std::wstring_view name)
	{
		for (auto& file : m_resList)
		{
			UIResource res = file.file.first->ReadResource(name.data(), file.file.second, DataRes).res;
			if (res.data)
				return res;
		}
		return {};
	}

	UIBitmapPtr UIResourceMgr::CreateUniqueUIBitamp(std::wstring_view name)
	{
		auto res = ReadResource(name);
		auto ret = CreateUniqueUIBitamp(res);
		res.Release();
		return ret;
	}

	UIBitmapPtr UIResourceMgr::CreateUniqueUIBitamp(UIResource res)
	{
		if (!res) return nullptr;

		const auto bitmap = m_render->CreateBitmap(res);
		if (!bitmap) return nullptr;

		const auto bmp = new UIBitmap();
		bmp->m_bitmap = bitmap;

		return bmp;
	}

	UIBitmapPtr UIResourceMgr::CreateSharedUIBitmap(std::wstring_view name)
	{
		auto iter = m_sharedBmpList.find(name.data());
		if (iter != m_sharedBmpList.end())
			return iter->second;

		auto res = ReadResource(name);
		if (!res) return nullptr;

		const auto bitmap = m_render->CreateBitmap(res);
		res.Release();
		if (!bitmap) return nullptr;

		const auto ret = new UIBitmap();
		ret->m_bitmap = bitmap;

		m_sharedBmpList[name.data()] = ret;

		return ret;
	}

	bool UIResourceMgr::LoadStyleFromDMRes(MUIRESFILE& file, std::wstring_view name, std::wstring& dst, bool res, UIResource* dstres)
	{
		auto reskey = file.first->ReadResource(name.data(), file.second, res ? AllRes : StringRes);
		if ((reskey.res.data || reskey.res.size == 0) && reskey.resText.empty())
		{
			reskey.res.Release();
			return false;
		}
		if (res)
			*dstres = reskey.res;
		dst = reskey.resText;
		dst = M_ReplaceString(dst, L"\r\n", L"\n");
		dst = M_ReplaceString(dst, L"\r", L"\n");

		return true;
	}

	bool UIResourceMgr::LoadStyleInternal(MUIRESFILE& file, std::wstring_view name, UIStyle** dststyle)
	{
		std::wstring dst;
		UIResource res;
		if (!LoadStyleFromDMRes(file, name, dst, true, &res))
			return false;

		auto clean = RAII::scope_exit([&] { res.Release(); });

		auto lineCount = M_GetTextCount(dst, L"\n") + 1;

		if (lineCount < 2)
		{
			res.Release();
			return false;
		}

		/* StyleImage
		 * 1 - 类型
		 * 4 - count
		 * 3,3,3,3 - path
		 *
		 * StyleGeometry
		 * 2 - 类型
		 * xml ....
		 */

		auto type = M_StoInt(M_GetTextLine(dst, 1));
		if (type < 1 || type > 2)
			return false;

		StyleData data;
		data.memres = false;
		data.srcname = name;

		if (type == 1)
		{
			auto count = (_m_ushort)M_StoInt(M_GetTextLine(dst, 2));
			if (count < 1)
				return false;

			bool nineGrid = false;
			_m_rect_t gridpath = { 0 };
			if (lineCount > 2)
			{
				nineGrid = true;
				std::vector<int> dstValue;
				M_GetAttribValueInt(M_GetTextLine(dst, 3), dstValue, 4);
				gridpath = { dstValue[0], dstValue[1], dstValue[2], dstValue[3] };
			}
			if (!AddImageStyleInternal(res, count, &data.style, nineGrid, gridpath))
				return false;
		}
		else if (type == 2)
		{
			auto start = dst.find('\n');
			dst = dst.substr(start, dst.length() - start);
			if (!AddGeometryStyleInternal(dst, &data.style))
				return false;
		}
		//重名
		if (auto [iter, success] = m_styleList.insert(std::make_pair(name, data)); !success)
		{
			data.style.rest();
			return false;
		}
		if (dststyle)
			*dststyle = data.style.get();
		return true;
	}

	bool UIResourceMgr::AddImageStyleInternal(UIResource memimg, _m_ushort count,
		UIStyle** dst, bool nineGrid, _m_rect_t<int> gridpath)
	{
		auto bmp = CreateUniqueUIBitamp(memimg);
		if (!bmp)
			return false;

		UIStyleImage* ret = new UIStyleImage();
		ret->m_bitmap = bmp;
		ret->m_nineGrid = nineGrid;
		ret->m_gridPath = gridpath;
		ret->m_partCount = count;
		ret->AddRef();
		*dst = ret;

		return true;
	}

	bool UIResourceMgr::AddGeometryStyleInternal(std::wstring_view xml, UIStyle** dst)
	{
		pugi::xml_document doc;
		std::wstring _xml = xml.data();
		_xml += L"<root>" + _xml + L"</root>";
		if (!doc.load_string(_xml.c_str()))
			return false;

		auto root = doc.child(L"root");
		if (root.empty())
			return false;

		UIStyleGeometry* style = nullptr;
		int count = 0;

		//解析指令
		static auto ParseCMD = [](UIStyleGeometry::CMD& data, const pugi::xml_node& node)
		{
			std::wstring_view name = node.name();
			if (name == L"fill_rect") data.type = 0;
			else if (name == L"fill_round") data.type = 1;
			else if (name == L"draw_rect") data.type = 2;
			else if (name == L"draw_round") data.type = 3;
			else if (name == L"draw_line") data.type = 4;
			else if (name == L"fill_ellipse") data.type = 5;
			else if (name == L"draw_ellipse") data.type = 6;
			else return false;

			for (auto attrib : node.attributes())
			{
				name = attrib.name();
				if (name == L"rc")
				{
					std::vector<std::wstring> value;
					M_GetAttribValue(attrib.value(), value, 4);
					for (size_t i = 0; i < 4; ++i)
					{
						std::wstring& v = value[i];
						if (v.empty())
							continue;
						if (v[0] == L'l' || v[0] == L't' || v[0] == L'r' || v[0] == L'b')
						{
							switch (v[0])
							{
							case L'l':
								data.m_rctype[i] = 1;
								break;
							case L't':
								data.m_rctype[i] = 2;
								break;
							case L'r':
								data.m_rctype[i] = 3;
								break;
							case L'b':
								data.m_rctype[i] = 4;
								break;
							default:
								data.m_rctype[i] = 0;
								break;
							}
							v = v.substr(1, v.length() - 1);
						}
						const int num = M_StoInt(v);
						switch (i)
						{
						case 0:
							data.m_dst.left = num;
							break;
						case 1:
							data.m_dst.top = num;
							break;
						case 2:
							data.m_dst.right = num;
							break;
						case 3:
							data.m_dst.bottom = num;
							break;
						default:
							break;
						}
					}
				}
				else if (name == L"color")
					data.color = M_GetAttribValueColor(attrib.value());
				else if (name == L"value")
					data.param = M_StoFloat(attrib.value());
				else if (name == L"width")
					data.width = M_StoInt(attrib.value());
			}
			return true;
		};

		for (auto& node : root.children())
		{
			if (const std::wstring_view name = node.name(); name != L"part")
				continue;

			std::vector<UIStyleGeometry::CMD> cmdList;
			for (auto& cmd : node.children())
			{
				UIStyleGeometry::CMD data;
				if (!ParseCMD(data, cmd))
					continue;
				cmdList.push_back(data);
			}

			if (!style)
				style = new UIStyleGeometry();

			style->m_cmdlist.push_back(std::move(cmdList));

			count++;
		}
		if(style)
		{
			style->m_partCount = (_m_ushort)count;
			style->AddRef();
			style->InitResource(m_render);
			if (dst)
				*dst = style;
			return true;
		}
		return false;
	}

	void UIResourceMgr::FreeResFile(MUIRESFILE& file)
	{
		delete file.first;
		file.first = nullptr;
	}

	void UIStyleImage::PaintStyle(Render::MRenderCmd* render, MPCRect dest, _m_byte alpha,
	                              int state, _m_ushort count, _m_scale scale)
	{
		if (!m_bitmap) 
			return;

		auto bitmap = m_bitmap->GetBitmap();

		UISize styleSize = bitmap->GetSize();
		styleSize.width /= m_partCount;

		if (m_partCount == count)
		{
			_m_rect rect_ = _m_rect(styleSize.width * state, 0, styleSize.width * (state + 1), styleSize.height);
			if (state == 0)
				rect_.left = 0;
			if (!m_nineGrid)
				render->DrawBitmap(bitmap, alpha, *dest, rect_);
			else
			{
				render->DrawNinePalacesImg(bitmap, alpha, *dest, rect_, m_gridPath);
			}
		}
		else
			MErrorThrow(MErrorCode::InvalidParameter);
	}

	void UIStyleGeometry::PaintStyle(Render::MRenderCmd* render, MPCRect dest, _m_byte alpha,
		int state, _m_ushort count, _m_scale scale)
	{
		if(m_partCount != count)
		{
			MErrorThrow(MErrorCode::InvalidParameter);
			return;
		}
		if (m_cmdlist.size() != count) return;

		auto CalcValue = [&](_m_byte type, _m_byte rctype, int src)
		{
			int value;
			bool add = rctype < 2;
			switch (rctype)
			{
			case 1: //top
				value = dest->top;
				break;
			case 2: //right
				value = dest->right;
				break;
			case 3: //bottom
				value = dest->bottom;
				break;
			default://left
				value = dest->left;
				break;
			}

			switch (type)
			{
			case 1://left
				return dest->left - src;
			case 2://top
				return dest->top - src;
			case 3://right
				return dest->right - src;
			case 4://bottom
				return dest->bottom - src;
			case 0:
			default:
				if (add)
					return value + src;
				return value - src;
			}
		};

		for (auto& cmd : m_cmdlist[state])
		{
			_m_rect dstrc;
			dstrc.left = CalcValue(cmd.m_rctype[0], 0, _scale_to(cmd.m_dst.left, scale.cx));
			dstrc.top = CalcValue(cmd.m_rctype[1], 1, _scale_to(cmd.m_dst.top, scale.cy));
			dstrc.right = CalcValue(cmd.m_rctype[2], 2, _scale_to(cmd.m_dst.right, scale.cx));
			dstrc.bottom = CalcValue(cmd.m_rctype[3], 3, _scale_to(cmd.m_dst.bottom, scale.cy));

			if (cmd.type == 0 || cmd.type == 1 || cmd.type == 5)
			{
				m_brush->SetColor(cmd.color);
				m_brush->SetOpacity(alpha);
			}
			else if (cmd.type == 2 || cmd.type == 3 || cmd.type == 4 || cmd.type == 6)
			{
				m_pen->SetColor(cmd.color);
				m_pen->SetWidth(_scale_to(cmd.width, scale.cx));
				m_pen->SetOpacity(alpha);
			}
			switch (cmd.type)
			{
			case 0:
				render->FillRectangle(dstrc, m_brush);
				break;
			case 1:
				render->FillRoundedRect(dstrc, _scale_to(cmd.param, M_MIN(scale.cx, scale.cy)), m_brush);
				break;
			case 2:
				render->DrawRectangle(dstrc, m_pen);
				break;
			case 3:
				render->DrawRoundedRect(dstrc, _scale_to(cmd.param, M_MIN(scale.cx, scale.cy)), m_pen);
				break;
			case 4:
				render->DrawLine({ dstrc.left, dstrc.top }, { dstrc.right, dstrc.bottom }, m_pen);
				break;
			case 5:
				render->FillEllipse(dstrc, m_brush);
				break;
			case 6:
				render->DrawEllipse(dstrc, m_pen);
				break;
			default:
				break;
			}
		}
	}

	void UIStyleGeometry::InitResource(Render::MRenderCmd* render)
	{
		for (auto& part : m_cmdlist)
		{
			for (const auto& cmd : part)
			{
				//创建资源
				if (!m_brush && (cmd.type == 0 || cmd.type == 1 || cmd.type == 5))
					m_brush = render->CreateBrush(Color::M_None);
				else if (!m_pen && (cmd.type == 2 || cmd.type == 3 || cmd.type == 4 || cmd.type == 6))
					m_pen = render->CreatePen(1, Color::M_None);

				if (m_brush && m_pen) break;
			}
			if (m_brush && m_pen) break;
		}
	}
}
