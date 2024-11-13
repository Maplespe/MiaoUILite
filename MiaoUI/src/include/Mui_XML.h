/**
 * FileName: Mui_XML.h
 * Note: 界面创建助手声明
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

#pragma once
#include <Mui_Base.h>
#include <Window/Mui_BasicWnd.h>
#include <Control/Mui_Label.h>

#ifndef MUIEVENT
/*判断UI事件
* @param event - 事件类型
* @param name - 控件名
* @return 是否为此控件触发的事件
*/
#define MUIEVENT(_event, _name) (event == (_event) && control->GetName() == (_name))
#endif
#ifndef MUIEVENTS
/*判断UI事件
* @param event - 事件类型
* @param control - 控件指针
* @return 是否为此控件触发的事件
*/
#define MUIEVENTS(_event, _control) (event == (_event) && control == (_control))
#endif

#ifndef MXMLCODE
//内联XML代码
#define MXMLCODE(code) L#code
#endif

namespace Mui::XML
{
	/*MUI XML UI类
	* 用于从XML创建界面
	* 使用CreateUIFromXML来创建界面
	*/
	class MuiXML
	{
	public:

		UIResourceMgr* Mgr() { return m_window->GetResourceMgr(); }

		/*创建界面自XML文档
		* @param parent - 父控件或Root控件
		* @param xmlDoc - xml文档
		*
		* @return 是否创建成功
		* @exception MErrorCode::MUIError
		*/
		bool CreateUIFromXML(Ctrl::UIControl* parents, std::wstring xmlDoc);

		/*添加字体样式
		* @param name - 样式类名称
		* @param style - 样式
		*
		* @return 如果有重复名称 将会失败
		*/
		bool AddFontStyle(std::wstring_view name, const Ctrl::UILabel::Attribute& style);

#if MUI_MXML_ENABLE_DEFSTYLE
		//加载默认UI样式
		void LoadDefaultStyle();
#endif

		/*获取已添加的字体样式
		* 如果未找到 返回默认字体样式
		*/
		Ctrl::UILabel::Attribute GetFontStyle(std::wstring_view name);

		/*删除已添加的字体样式
		* @param name - 样式名称
		*
		* @return 是否删除成功 如果不存在将返回false
		*/
		bool DeleteFontStyle(std::wstring_view name);

		/*新建属性组 (PropGroup)
		* @param id - 属性组id
		*
		* @return 如果存在重复属性组返回false
		*/
		bool CreatePropGroup(std::wstring_view id);

		/*删除属性组 (PropGroup)
		* @param id - 属性组id
		*
		* @return 是否删除成功 如果不存在属性组将返回false
		*/
		bool DeletePropGroup(std::wstring_view id);

		/*删除属性组属性 (PropGroup)
		* @param id - 属性组id
		* @param attribName - 属性名
		*
		* @return 是否删除成功 如果不存在属性组或属性将返回false
		*/
		bool DeletePropGroupAttrib(std::wstring_view id, std::wstring_view attribName);

		/*设置属性组属性值 (PropGroup)
		* @param id - 属性组id
		* @param attribName - 属性名
		* @param attribValue - 属性值
		*
		* @return 是否设置成功 如果不存在属性组或属性将返回false
		*/
		bool SetPropGroupAttrib(std::wstring_view id, std::wstring_view attribName, std::wstring_view attribValue);

		/*添加属性组属性值 (PropGroup)
		* @param id - 属性组id
		* @param attribName - 属性名
		* @param attribValue - 属性值
		*
		* @return 添加设置成功 如果不存在属性组或属性名重复将返回false
		*/
		bool AddPropGroupAttrib(std::wstring_view id, std::wstring_view attribName, std::wstring_view attribValue);

		/*应用属性组属性 (PropGroup)
		* 给指定控件应用指定属性组的所有属性
		* @param dst - 目标控件
		* @param id - 属性组id
		* @param draw - 是否更新绘制
		*
		* @return 是否应用成功 如果属性组不存在 或控件无效(nullptr)将返回false
		*/
		bool ApplyPropGroup(Ctrl::UIControl* dst, std::wstring_view id, bool draw = true);

		/*添加默认属性组 (DefProp)
		* 默认属性组将绑定XML控件树的所有控件 指定控件默认拥有这些属性
		* @param xml - xml代码
		* @param replace - 如果已经存在默认的属性 是否替换 否则仅追加不存在的属性
		*
		* @return 是否添加成功 仅检查xml
		*/
		bool AddDefPropGroup(std::wstring_view xml, bool replace);

		/*删除默认属性组
		* @param name - 绑定的控件名
		*
		* @return 如果指定名称的属性组不存在将返回false
		*/
		bool DeleteDefPropGroup(std::wstring_view name);

		/*设置默认属性组属性值
		* @param name - 绑定的控件名
		* @param attribList - 要设定的属性列表 如果属性不存在将添加
		* @param draw - 是否更新绘制
		*
		* @return 如果指定名称的属性组不存在将返回false
		*/
		bool SetDefPropGroupAttrib(std::wstring_view name,
			const std::vector<std::pair<std::wstring, std::wstring>>& attribList, bool draw = true);

		/*添加字符串列表
		* @param name - 名称
		* @param value - 值
		*
		* @return 如果重复将返回false
		*/
		bool AddStringList(std::wstring_view name, std::wstring_view value);

		/*设置字符串值
		* 所有引用该字符串的控件内容都将被更新
		* @param name - 名称
		* @param value - 值
		* @param draw - 更新关联控件绘制
		*
		* @return name无效将返回false
		*/
		bool SetStringValue(std::wstring_view name, std::wstring_view value, bool draw = true);

		/*删除字符串值
		* 从字符串列表删除字符串 但已引用该字符串的控件不会受到影响
		* @param name - 名称
		*
		* @return 是否成功
		*/
		bool DeleteStringList(std::wstring_view name);

		/*读取字符串值
		* @param name - 名称
		*
		* @return 如果找不到将返回名称 否则为目标字符串值
		*/
		std::wstring GetStringValue(std::wstring_view name);

		/*设置控件属性 与控件的SetAttribute不同的是 这里的特殊属性值会被自动转换 和XML中一样
		* 例如 style 原本是指针 在此处则为样式名称 fontStyle、Resource等也一样 value为资源名称 而不再是指针
		* @param control - 要设置的控件
		* @param name - 属性名称
		* @param value - 属性值
		*/
		virtual void SetAttribute(Ctrl::UIControl* control, std::wstring_view name, std::wstring value);

	protected:
		MuiXML(Window::UIWindowBasic* window);
		virtual ~MuiXML();

		//@exception MErrCode::MUIError
		virtual Ctrl::UIControl* CreateControl(std::wstring_view controlName, Ctrl::UIControl* parent);
		void SetAttributeInternal(Ctrl::UIControl* control, std::wstring_view name, std::wstring value, bool strlist);

		Ctrl::UILabel::Attribute* FindFontStyle(std::wstring_view name);

		Ctrl::UILabel::Attribute defaultStyle;
		std::unordered_map<std::wstring, Ctrl::UILabel::Attribute> m_fontStyleList;

		struct PropGroup
		{
			std::unordered_map<std::wstring, std::wstring> prop;
			std::wstring id;
		};
		std::vector<PropGroup> m_propList;
		std::vector<PropGroup> m_defpropList;

		Window::UIWindowBasic* m_window = nullptr;

		struct bindInfo
		{
			//引用的字符串列表
			struct strdata
			{
				//first 字符串名 second 属性名
				std::pair<std::wstring, std::wstring> str;
				int menuIndex = -1;//-1则非菜单引用 否则为菜单索引
			};
			std::vector<strdata> strList;
		};
		std::unordered_map<Ctrl::UIControl*, bindInfo> m_bindCtrlList;

		std::unordered_map<std::wstring, std::wstring> m_stringList;

#if MUI_MXML_ENABLE_DEFSTYLE
		bool m_defStyleInited = false;
#endif

		friend class Window::UIWindowBasic;
	};
}