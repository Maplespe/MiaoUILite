/**
 * FileName: Mui_XML.cpp
 * Note: 界面创建助手实现
 *
 * Copyright (C) 2020-2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2020-12-12 Create
*/
#include <Mui_DefUIStyle.h>
#include <MiaoUI/src/source/ThirdParty/pugixml/pugixml.hpp>
#include <Mui.h>

namespace Mui::XML
{
    using namespace Ctrl;

    bool MuiXML::SetDefPropGroupAttrib(std::wstring_view name, 
        const std::vector<std::pair<std::wstring, std::wstring>>& attribList, bool draw)
    {
        //查找属性组
        PropGroup* propPtr = nullptr;
        for(auto& prop : m_defpropList)
        {
            if (prop.id != name)
                continue;
            propPtr = &prop;
            break;
        }
        if(!propPtr)
            return false;

        //设置所有指定控件属性
        for (auto& ctrl : m_bindCtrlList)
        {
            if (ctrl.first->GetClsName() != name)
                continue;

            //遍历要添加的属性
            for (const auto& [attribName, attribValue] : attribList)
            {
                propPtr->prop[attribName] = attribValue;

                //设置属性
                SetAttribute(ctrl.first, attribName, attribValue);
            }
        }
        if (draw)
            m_window->UpdateDisplay(nullptr);
        return true;
    }

    bool MuiXML::AddStringList(std::wstring_view name, std::wstring_view value)
    {
        return m_stringList.insert(std::make_pair(name, value)).second;
    }

    bool MuiXML::SetStringValue(std::wstring_view name, std::wstring_view value, bool draw)
    {
        auto iter = m_stringList.find(name.data());
        if (iter == m_stringList.end())
            return false;
        iter->second = value.data();

        //更新关联的控件
        for(auto& ctrl : m_bindCtrlList)
        {
            for(auto& str : ctrl.second.strList)
            {
                if (str.str.first != name)
                    continue;

            	if(str.menuIndex == -1)
                    SetAttributeInternal(ctrl.first, str.str.second, value.data(), true);
            }
        }
        if(draw)
			m_window->UpdateDisplay(nullptr);
        return true;
    }

    bool MuiXML::DeleteStringList(std::wstring_view name)
    {
        if (auto iter = m_stringList.find(name.data()); iter != m_stringList.end())
        {
            m_stringList.erase(iter);
            //清除关联控件绑定
            for(auto& ctrl : m_bindCtrlList)
            {
                for (auto _it = ctrl.second.strList.begin(); _it != ctrl.second.strList.end();)
                {
                    if (_it->str.first != name)
                    {
                        ++_it;
                        continue;
                    }
                    _it = ctrl.second.strList.erase(_it);
                }
            }
            return true;
        }
        return false;
    }

    std::wstring MuiXML::GetStringValue(std::wstring_view name)
    {
        if (auto iter = m_stringList.find(name.data()); iter != m_stringList.end())
        {
            return iter->second;
        }
        return name.data();
    }

    MuiXML::MuiXML(Window::UIWindowBasic* window)
    {
        m_window = window;
        m_window->m_renderRoot->SetUnBindCallback([this](MRenderNode* node)
        {
	        if(auto iter = m_bindCtrlList.find((UIControl*)node);iter != m_bindCtrlList.end())
	        {
                m_bindCtrlList.erase(iter);
	        }
        });
    }

    MuiXML::~MuiXML()
    {
        m_window->m_renderRoot->SetUnBindCallback(nullptr);
    }

    bool MuiXML::CreateUIFromXML(UIControl* parents, std::wstring xmlDoc)
    {
        M_ASSERT(parents)
    	pugi::xml_document doc;

        //把非标准的\\n替换成XML标准中的换行符代码
        xmlDoc = Helper::M_ReplaceString(xmlDoc, L"\\n", L"&#x000A;");
        xmlDoc = L"<root>" + xmlDoc + L"</root>";

        if (!doc.load_string(xmlDoc.c_str()))
            return false;
        //遍历XML
        std::function<void(pugi::xml_node, UIControl*)> Enum = [&](pugi::xml_node node, UIControl* parent)
        {
            //子节点
            for (auto& nodes : node.children())
            {
                std::wstring TagName = nodes.name();
                UIControl* control = CreateControl(TagName, parent);
                if (!control) control = parents;

                //属性组
                bool propGroup = TagName == L"PropGroup";
                PropGroup prop;

                if(!propGroup)
                {
	                //应用默认属性组
                    for(auto& attrib : m_defpropList)
                    {
                        if (attrib.id != TagName)
                            continue;
                        for (auto& value : attrib.prop)
                            SetAttribute(control, value.first, value.second);
                    }
                }

                //遍历属性
                for (auto& attrib : nodes.attributes())
                {
                    if (propGroup)
                    {
                        std::wstring name = attrib.name();
                        if (name == L"id")
                            prop.id = attrib.value();
                        else
                            prop.prop.insert({ name, attrib.value() });
                    }
                    else 
                        SetAttribute(control, attrib.name(), attrib.value());
                }
                if (propGroup)
                    m_propList.push_back(prop);
                //遍历子节点
            	Enum(nodes, control);
            }
        };
        if (auto root = doc.child(L"root")) 
        {
            Enum(root, parents);
            return true;
        }
        return false;
    }

    bool MuiXML::AddFontStyle(std::wstring_view name, const UILabel::Attribute& style)
    {
        if (FindFontStyle(name) != &defaultStyle)
            return false;
        m_fontStyleList.insert(std::make_pair(name, style));
        return true;
    }

#if MUI_MXML_ENABLE_DEFSTYLE

    void MuiXML::LoadDefaultStyle()
    {
        if (m_defStyleInited) return;
        m_defStyleInited = true;

        DefStyle::_g_mui_default_uistyle_loadxml(Mgr());
        AddDefPropGroup(DefStyle::_g_mui_default_uistyle, true);
    }

#endif

    UILabel::Attribute MuiXML::GetFontStyle(std::wstring_view name)
    {
        return *FindFontStyle(name);
    }

    bool MuiXML::DeleteFontStyle(std::wstring_view name)
    {
        auto iter = m_fontStyleList.find(name.data());
        if (iter != m_fontStyleList.end())
        {
            m_fontStyleList.erase(iter);
            return true;
        }
        return false;
    }

    bool MuiXML::CreatePropGroup(std::wstring_view id)
    {
        for (auto& i : m_propList)
        {
            if (i.id != id) continue;
            return false;
        }
        PropGroup group;
        group.id = id;
        m_propList.push_back(group);
        return true;
    }

    bool MuiXML::DeletePropGroup(std::wstring_view id)
    {
        for (size_t i = 0; i < m_propList.size(); ++i)
        {
            if (m_propList[i].id != id) continue;
            m_propList.erase(m_propList.begin() + (long long)i);
            return true;
        }
        return false;
    }

    bool MuiXML::DeletePropGroupAttrib(std::wstring_view id, std::wstring_view attribName)
    {
        std::unordered_map<std::wstring, std::wstring>* propList = nullptr;
        for (auto& i : m_propList)
        {
            if (i.id != id) continue;
            propList = &i.prop;
        }
        if (!propList) return false;

        auto iter = propList->find(attribName.data());
        if(iter == propList->end())
        	return false;

        propList->erase(iter);
        return true;
    }

    bool MuiXML::SetPropGroupAttrib(std::wstring_view id, std::wstring_view attribName, std::wstring_view attribValue)
    {
        std::unordered_map<std::wstring, std::wstring>* propList = nullptr;
        for (auto& i : m_propList)
        {
            if (i.id != id) continue;
            propList = &i.prop;
        }
        if (!propList) return false;

        auto iter = propList->find(attribName.data());
        if (iter == propList->end())
            return false;

        iter->second = attribValue;

        return true;
    }

    bool MuiXML::AddPropGroupAttrib(std::wstring_view id, std::wstring_view attribName, std::wstring_view attribValue)
    {
        std::unordered_map<std::wstring, std::wstring>* propList = nullptr;
        for (auto& i : m_propList)
        {
            if (i.id != id) continue;
            propList = &i.prop;
        }
        if (!propList) return false;

        return propList->insert({ attribName.data(), attribValue.data()}).second;
    }

    bool MuiXML::ApplyPropGroup(UIControl* dst, std::wstring_view id, bool draw)
    {
        if (!dst) 
            return false;

        for (auto& prop : m_propList)
        {
            if (prop.id != id)
                continue;
            for (auto& p : prop.prop)
            {
                if (p.first != L"prop")
                    SetAttribute(dst, p.first, p.second);
            }
            if (draw)
                dst->UpdateDisplay();
            return true;
        }
        return false;
    }

    bool MuiXML::AddDefPropGroup(std::wstring_view xml, bool replace)
    {
        if (xml.empty()) return false;

        std::wstring _xml = L"<root>";
        _xml += xml;
    	_xml += +L"</root>";

        pugi::xml_document doc;

        if (!doc.load_string(_xml.c_str()))
            return false;

        auto FindProp = [&](std::wstring_view id) -> PropGroup*
        {
	        for(auto& prop : m_defpropList)
	        {
                if (prop.id == id) return &prop;
	        }
            return nullptr;
        };

        for(auto& list : doc.child(L"root").children())
        {
            if (std::wstring_view(list.name()) != L"DefPropGroup")
                continue;
            PropGroup prop;
            for (auto& attrib : list.attributes())
            {
                std::wstring name = attrib.name();
                if (name == L"control")
                    prop.id = attrib.value();
                else
                    prop.prop[name] = attrib.value();
            }
            if (prop.id.empty()) 
                continue;

            const auto defprop = FindProp(prop.id);
            if (!defprop)
            {
                m_defpropList.push_back(prop);
                continue;
            }
            //检查和替换属性
            for(const auto& [name, value] : prop.prop)
            {
                if (replace)
                    defprop->prop[name] = value;
                else
                    defprop->prop.insert({ name, value });
            }
        }

        return true;
    }

    bool MuiXML::DeleteDefPropGroup(std::wstring_view name)
    {
        for (size_t i = 0; i < m_defpropList.size(); i++)
        {
            if (m_defpropList[i].id == name)
            {
                m_defpropList.erase(m_defpropList.begin() + (long long)i);
                return true;
            }
        }
        return false;
    }

    UIControl* MuiXML::CreateControl(std::wstring_view controlName, UIControl* parent)
    {
        if (controlName == L"PropGroup")
            return nullptr;

        UIControl* ret = CtrlMgr::CreateControl(controlName, parent);
        if (controlName != L"root" && !ret)
        {
#if MUI_MXML_THROW_UNKNOWCTRL
            if (controlName != L"UIControl")
            {
                std::wstring err = L"Unknown control: ";
            	err += controlName;
                MErrorThrow(err);
            }
#endif
            ret = new UIControl();
            parent->AddChildren(ret);
        }
        if (ret)
        {
            m_bindCtrlList.insert(std::make_pair(ret, bindInfo()));
        }
        return ret;
    }

    void MuiXML::SetAttribute(UIControl* control, std::wstring_view name, std::wstring value)
    {
        SetAttributeInternal(control, name, std::move(value), false);
    }

    void MuiXML::SetAttributeInternal(UIControl* control, std::wstring_view name, std::wstring value,
	    bool strlist)
    {
        //映射字符串
        bool strlistAttrib = false;
        if (value.length() > 1)
        {
            //使用字符串表
            if (value[0] == '#')
            {
                strlistAttrib = true;

                auto strName = value.substr(1, value.length() - 1);
                auto iter = m_stringList.find(strName);
                if (iter == m_stringList.end())
                    goto next;

                value = iter->second;

                //设置绑定列表属性
                auto ctrl = m_bindCtrlList.find(control);
                if (ctrl == m_bindCtrlList.end())
                    goto next;

                //查看是否已经绑定了当前属性名
                bool finded = false;
                for (auto& prop : ctrl->second.strList)
                {
                    if (prop.str.second == name)
                    {
                        prop.str.first = strName;
                        finded = true;
                        break;
                    }
                }
                //没找到 添加
                if (!finded)
                {
                    bindInfo::strdata data;
                    data.str = std::make_pair(strName, name.data());
                    data.menuIndex = -1;
                    ctrl->second.strList.push_back(data);
                }
            }
            //转义成#
            else if (value[0] == '\\' && value[1] == '#')
            {
                value = Helper::M_ReplaceString(value, L"\\#", L"#");
            }
        }
        if (!strlistAttrib && ! strlist)
        {
            auto ctrl = m_bindCtrlList.find(control);
            if (ctrl == m_bindCtrlList.end())
                goto next;

            //查看是否已经绑定了当前属性名 如果有 则覆盖默认属性
            for (size_t i = 0; i < ctrl->second.strList.size(); ++i)
            {
                auto& item = ctrl->second.strList[i];
                if (item.str.second != name)
                    continue;

                ctrl->second.strList.erase(ctrl->second.strList.begin() + i);
                break;
            }
        }
    next:

        //使用属性组
        if (name == L"prop")
        {
            for (auto& prop : m_propList)
            {
                if (prop.id != value)
                    continue;
                for (auto& p : prop.prop)
                {
                    if (p.first != L"prop")
                        SetAttribute(control, p.first, p.second);
                }
                break;
            }
        }
        else if (name == L"align")
        {
            //映射到枚举值
            if (value == L"Block")
                value = std::to_wstring(UIAlignment_Block);
            else if (value == L"LinearV")
                value = std::to_wstring(UIAlignment_LinearV);
            else if (value == L"LinearVB")
                value = std::to_wstring(UIAlignment_LinearVB);
            else if (value == L"LinearVR")
                value = std::to_wstring(UIAlignment_LinearVR);
            else if (value == L"LinearVBR")
                value = std::to_wstring(UIAlignment_LinearVBR);
            else if (value == L"LinearH")
                value = std::to_wstring(UIAlignment_LinearH);
            else if (value == L"LinearHL")
                value = std::to_wstring(UIAlignment_LinearHL);
            else if (value == L"LinearHB")
                value = std::to_wstring(UIAlignment_LinearHB);
            else if (value == L"LinearHLB")
                value = std::to_wstring(UIAlignment_LinearHLB);
            else if (value == L"Absolute")
                value = std::to_wstring(UIAlignment_Absolute);
            else if (value == L"Center")
                value = std::to_wstring(UIAlignment_Center);
            else if (value == L"Grid")
                value = std::to_wstring(UIAlignment_Grid);
        }
        else if (name == L"frame")
        {
            std::vector<std::wstring> dst;
            Helper::M_GetAttribValue(value, dst, 4);

            UINodeBase::PosSizeUnit posUint;
            UINodeBase::PosSizeUnit sizeUint = posUint;

            for (size_t i = 0; i < dst.size(); i++)
            {
                const auto& v = dst[i];
                auto type = UINodeBase::Percentage;
                if (v.find('%') == std::wstring::npos)
                {
                    if (v.find('f') == std::wstring::npos)
                        continue;
                    type = UINodeBase::FillMinus;
                }

                if (i < 2)
                {
                    if (i == 0)
                        posUint.x_w = type;
                    else
                        posUint.y_h = type;
                }
                else
                {
                    if (i == 2)
                        sizeUint.x_w = type;
                    else
                        sizeUint.y_h = type;
                }
            }

            control->SetPosUnit(posUint, false);
            control->SetSizeUnit(sizeUint, false);
        }
        else if (name == L"size")
        {
            std::vector<std::wstring> dst;
            Helper::M_GetAttribValue(value, dst, 2);
            UINodeBase::PosSizeUnit sizeUint;

            for (size_t i = 0; i < 2; ++i)
            {
                auto type = UINodeBase::Percentage;
                if (dst[i].find('%') == std::wstring::npos)
                {
                    if (dst[i].find('f') == std::wstring::npos)
                        continue;
                    type = UINodeBase::FillMinus;
                }

                if (i == 0)
                    sizeUint.x_w = type;
                else
                    sizeUint.y_h = type;
            }
            control->SetSizeUnit(sizeUint, false);
        }
        else if (name == L"pos")
        {
            std::vector<std::wstring> dst;
            Helper::M_GetAttribValue(value, dst, 2);

            UINodeBase::PosSizeUnit posUint;

            for (size_t i = 0; i < 2; ++i)
            {
                auto type = UINodeBase::Percentage;
                if (dst[i].find('%') == std::wstring::npos)
                {
                    if (dst[i].find('f') == std::wstring::npos)
                        continue;
                    type = UINodeBase::FillMinus;
                }
                if (i == 0)
                    posUint.x_w = type;
                else
                    posUint.y_h = type;
            }
            control->SetPosUnit(posUint, false);
        }
        //内置字体样式属性
        else if (auto type = CtrlMgr::GetAttributeType(control->GetClsName(), name); type != CtrlMgr::AttribType::defaults)
        {
            switch (type)
            {
            case CtrlMgr::AttribType::labelStyle:
                value = std::to_wstring(_m_ptrv(FindFontStyle(value)));
                break;
            case CtrlMgr::AttribType::listfontStyle:
            {
                auto p = FindFontStyle(value);
                UIListBox::ItemFont ifont;
                ifont.font = p->font;
                ifont.fontColor = p->fontColor;
                ifont.fontCustom = p->fontCustom;
                ifont.fontSize = p->fontSize;
                ifont.fontStyle = p->fontStyle;
                ifont.textAlign = p->textAlign;
                control->SetAttribute(name, std::to_wstring(_m_ptrv(&ifont)), false);
                return;
            }
            case CtrlMgr::AttribType::UIResource:
            {
                UIResource res = m_window->GetResourceMgr()->ReadResource(value);
                control->SetAttribute(name, std::to_wstring((_m_ptrv)&res), false);
                res.Release();
                return;
            }
            case CtrlMgr::AttribType::UIStyle:
            {
                UIStyle* style = m_window->GetResourceMgr()->FindStyle(value);
                if (style || value.empty())
                    value = std::to_wstring(_m_ptrv(style));
                else
                {
                    MErrorThrow(L"Unknown Style: " + value);
                    return;
                }
            }
            break;
            case CtrlMgr::AttribType::UIBitmap:
            {
            	auto bitmap = m_window->GetResourceMgr()->CreateSharedUIBitmap(value);

                if (bitmap || value.empty())
                {
                    value = std::to_wstring(_m_ptrv(bitmap.get()));
                    control->SetAttribute(name, value, false);
                    return;
                }

                MErrorThrow(L"Unknown Bitmap: " + value);
                return;
            }
            default:
                break;
            }
        }
        control->SetAttribute(name, value, false);
    }

    UILabel::Attribute* MuiXML::FindFontStyle(std::wstring_view name)
    {
        auto iter = m_fontStyleList.find(name.data());
        if (iter != m_fontStyleList.end())
            return &iter->second;
        return &defaultStyle;
    }
}