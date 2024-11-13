/**
 * FileName: Mui_ControlMgr.h
 * Note: UI控件管理器声明
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
 * date: 2022-3-8 Create
*/

#pragma once
#include <Mui_Base.h>
#include <Mui_Helper.h>
#include <Render/Node/Mui_UINodeBase.h>
#include <any>
#include <utility>
#include <array>

/*控件类名和注册方法调用
 * 用法 在控件类开头使用 定义控件在XML中的名称 和特殊属性:
 * UICtrlDemo : UILabel
 * {
 * public:
 *		//有属性列表的情况:
 *		MCTRL_DEFINE
 *		(
 *			L"UICtrlDemo", //这里是控件类名
 *			//这里是特殊属性列表(AttribType类型)
 *			{ CtrlMgr::AttribType::UIStyle, L"style" },
 *			{ CtrlMgr::AttribType::UIBitmap, L"icon" },
 *			....
 *		);
 *
 *		//无属性列表 直接使用MCTRL_DEFNAME
 *		MCTRL_DEFNAME(L"UICtrlDemo");
 *
 *		//有属性列表和继承控件的情况:
 *		MCTRL_DEFINE_EX
 *		(
 *			L"UICtrlDemo", //这里是控件类名
 *			//需要使用MCTRL_MAKE_ATTRIB生成属性列表
 *			MCTRL_MAKE_ATTRIB(
 *			{ CtrlMgr::AttribType::UIStyle, L"style" },
 *			{ CtrlMgr::AttribType::UIBitmap, L"icon" }
 *			),
 *			//然后是继承的属性
 *			MCTRL_BASE_ATTRIB(UILabel)
 *			....
 *		);
 *
 *		//可选 定义事件槽
 *		MCTRL_DEFSLOT
		(
			CtrlMgr::EventSlot<UIPoint> clicked;//鼠标左键单击
			....
		);
 * }
 */

#define MCTRL_ATTRIB _mctrl_attrib_attrib

//获取基类属性列表
#define MCTRL_BASE_ATTRIB(base) base::MCTRL_ATTRIB

//定义控件Element名称和注册方法
#define MCTRL_DEFNAME(x) static constexpr auto ClassName = x; \
virtual std::wstring GetClsName() const override { return ClassName; } \
static void Register()

//控件特殊属性 CtrlMgr::AttribType
#define MCTRL_MAKE_ATTRIB(...) Helper::ToArray<CtrlMgr::AttribType>({__VA_ARGS__})

/*定义控件
* @param name - 控件在XML中的element名称
* @param ... 特殊属性列表
*/
#define MCTRL_DEFINE(x,...) MCTRL_DEFNAME(x); \
inline static constexpr std::array MCTRL_ATTRIB = Helper::MergeArrays(MCTRL_MAKE_ATTRIB(__VA_ARGS__))

/*定义控件
* @param name - 控件在XML中的element名称
* @param attrib - 特殊属性列表 使用MCTRL_MAKE_ATTRIB生成
* @param ... 继承的控件属性列表
*/
#define MCTRL_DEFINE_EX(x,attrib,...) MCTRL_DEFNAME(x); \
inline static constexpr std::array MCTRL_ATTRIB = Helper::MergeArrays(attrib, __VA_ARGS__)

#define mslot mctrl_eventslot
//定义事件槽
#define MCTRL_DEFSLOT(...) public: struct { __VA_ARGS__ } mslot

/* 注册控件方法
 * 此宏自动添加了注册的属性列表
 * 否则也可以手动调用RegisterControl注册
 */
#define MCTRL_REGISTER(method) CtrlMgr::RegisterControl(ClassName, method, Helper::ArrayToVector(MCTRL_ATTRIB))

namespace Mui::Ctrl
{
	template<typename T>
	constexpr bool mis_uinodecls = std::is_base_of_v<Render::UINodeBase, T>
		|| std::is_base_of_v<T, Render::UINodeBase>;

	template<typename Target, typename T>
	Target* mcast_control(T* control)
	{
		//检查参数类型是合法的
		static_assert(mis_uinodecls<T>, //目标<control参数>的类型不是UINodeBase的基类或派生类
			"[MiaoUI cast]: The target<control param>type is not a base or derived class of UINodeBase.");

		static_assert(mis_uinodecls<Target>, //目标<Target参数>的类型不是UINodeBase的基类或派生类
			"[MiaoUI cast]: The target<Target param>type is not a base or derived class of UINodeBase.");

		constexpr bool isBase = std::is_base_of_v<Target, T>;
		constexpr bool isDerived = std::is_base_of_v<T, Target>;

		static_assert(isBase || isDerived, //参数和要转换的目标之间没有继承或派生关系。
			"[MiaoUI cast]: There is no inheritance or derivation relationship between the parameters and the target to be converted.");

#ifdef _DEBUG
		Target* ptr = dynamic_cast<Target*>(control);
		if (!ptr) //无法转换到目标类型
			throw std::exception("[MiaoUI cast]: Unable to convert to target type.");
#endif
		return static_cast<Target*>(control);
	}
}

namespace Mui::CtrlMgr
{
	namespace Attrib
	{
		template<typename T>
		std::wstring Value_Make2x(T a, T b)
		{
			return std::to_wstring(a) + L"," + std::to_wstring(b);
		}

		template<typename T>
		std::wstring Value_Make4x(T a, T b, T c, T d)
		{
			return std::to_wstring(a) + L"," + std::to_wstring(b) + L","
				+ std::to_wstring(c) + L"," + std::to_wstring(d);
		}

		inline std::wstring Value_Make2x(UIPoint pt)
		{
			return Value_Make2x(pt.x, pt.y);
		}

		inline std::wstring Value_Make2x(UISize size)
		{
			return Value_Make2x(size.width, size.height);
		}

		inline std::wstring Value_Make4x(UIRect rect)
		{
			return Value_Make4x(rect.left, rect.top, rect.right, rect.bottom);
		}

		template<typename T>
		std::wstring Value_Make4x(_m_rect_t<T> rect)
		{
			return Value_Make4x<T>(rect.left, rect.top, rect.right, rect.bottom);
		}
	}

	typedef std::function<Ctrl::UIControl*(Ctrl::UIControl*)> RegMethod;

	struct AttribType
	{
		enum type
		{
			defaults,		//默认文本类型
			labelStyle,		//UILabel::Attribute*属性类型
			listfontStyle,	//UIListBox::ItemFont*属性类型
			UIStyle,		//UIStyle*属性类型
			UIResource,		//UIResource*属性类型
			UIBitmap		//UIBitmap属性类型
		} attribType = defaults;
		std::wstring_view attribName;
	};

	/*inline std::vector<AttribType> MergeAttrib(const std::initializer_list<std::vector<AttribType>>& lists)
	{
		std::vector<AttribType> ret;
		for (const auto& list : lists) 
		{
			ret.insert(ret.end(), list.begin(), list.end());
		}
		return ret;
	}*/


	class AttribConverter
	{
	public:
		/* 默认转换器
		 * 支持类型:
		 *
		 * bool
		 * float
		 * double
		 *
		 * std::wstring
		 * std::wstring_view
		 * wchar_t
		 *
		 * _m_uchar
		 * _m_byte
		 * _m_short
		 * _m_ushort
		 * _m_int
		 * _m_long
		 * _m_uint
		 * _m_ulong
		 * _m_color
		 * _m_param
		 * _m_long64
		 * _m_ulong64
		 * _m_size
		 * _m_ptr
		 * _m_ptrv
		 *
		 * _m_rect
		 * _m_rect_t<int>
		 *
		 * UIRect
		 * UIPoint
		 * UISize
		 *
		 * void*
		 *
		 * UIStylePtr
		 * UIStyle*
		 * UIBitmapPtr
		 * UIBitmap*
		 */
		AttribConverter();
		virtual ~AttribConverter() = default;

		template<typename T>
		T Convert(std::wstring_view value)
		try
		{
			auto iter = m_methodList.find(typeid(T).name());
			if (iter == m_methodList.end())
				return {};
			return std::any_cast<T>(iter->second.get(value));
		}
		catch(...)
		{
#ifdef _DEBUG
			_M_OutErrorDbg_(L"不支持的类型转换", false);
#endif
			return {};
		}

		template<typename T>
		std::wstring Convert(const T& value)
		try
		{
			auto iter = m_methodList.find(typeid(T).name());
			if (iter == m_methodList.end())
				return {};
			return iter->second.set(value);
		}
		catch(...)
		{
#ifdef _DEBUG
			_M_OutErrorDbg_(L"不支持的类型转换", false);
#endif
			return {};
		}

		using ConversionMethod_Get = std::function<std::any(std::wstring_view value)>;
		using ConversionMethod_Set = std::function<std::wstring(std::any value)>;

	protected:
		//添加转换方法
		template<typename T>
		void AddMethod(ConversionMethod_Get get, ConversionMethod_Set set)
		{
			m_methodList.insert(std::make_pair((std::string)typeid(T).name(), data{ std::move(get), std::move(set)}));
		}

	private:
		struct data
		{
			ConversionMethod_Get get = nullptr;
			ConversionMethod_Set set = nullptr;
		};
		static std::unordered_map<std::string, data> m_methodList;
	};

	//member, name, seter, geter
#define MakeUIAttrib(__struct, ...) decltype(__struct)::MakeAttrib(__VA_ARGS__)
	//member(final), offset, name, seter, geter
#define MakeUIAttribEx(__struct, member, offset, ...) decltype(__struct)::MakeAttribAtOffset(member, decltype(__struct)::MOffsetOf(offset), __VA_ARGS__)

	//在赋值属性之后调用Set方法回调
#define MakeUIAttrib_AfterSetOnly(__struct, member, name, paramname, code) decltype(__struct)::MakeAttrib(member, name, \
	[](decltype(__struct)::SetData param) \
	{ \
		param.Assign(member); \
		return [](decltype(__struct)::SetData& paramname) -> bool \
		code(paramname); \
	})

	//在赋值属性之后调用Get方法回调
#define MakeUIAttrib_AfterGetOnly(__struct, member, name) decltype(__struct)::MakeAttrib(member, name, nullptr, \
	[](decltype(__struct)::GetData param) \
	{ \
		code \
	} \

	//只转换指针类型 不使用AttribConverter类 能减少一些编译代码体积 这不会检查类型是否匹配
	template<typename T>
	T* ConvertAttribPtr(std::wstring_view attrib)
	{
		return reinterpret_cast<T*>(Helper::M_StoULong64(attrib));
	}

	template<typename T>
	std::wstring ConvertAttribPtr(T* attrib)
	{
		return std::to_wstring((_m_ptrv)attrib);
	}

	template<typename T, typename Param = bool>
	class UIAttribute
	{
	public:
		struct SetData
		{
			T* data = nullptr;
			AttribConverter* converter = nullptr;
			std::wstring_view attribName;
			std::wstring_view attribValue;
			Param param;

			//将属性转换到目标类型
			template<typename Type = std::wstring_view>
			inline Type GetValue()
			{
				//退化成void*来转换
				if constexpr (std::is_pointer_v<Type>)
					return (Type)ConvertAttribPtr<Type>(attribValue);
				//字符串无需转换 用不着再跑一遍Convert类
				else if constexpr (std::is_same_v<Type, std::wstring>)
					return attribValue.data();
				else if constexpr (std::is_same_v<Type, std::wstring_view> || std::is_same_v<Type, UIString>)
					return attribValue;
				else if constexpr (std::is_enum_v<Type>)
					return (Type)converter->Convert<int>(attribValue);
				else
					return converter->Convert<Type>(attribValue);
			}

			//对属性赋值 将attribValue的值转换并赋值到绑定的结构体成员
			template<typename Struct, typename Member>
			void Assign(Member Struct::* member)
			{
				auto pData = (Member*)(_m_ptrv(data) + MOffsetOf<Struct>(member));
				*pData = GetValue<Member>();
			}

			template<typename Struct, typename Member>
			void AssignAtOffset(Member Struct::* member, _m_ptrv offset)
			{
				auto pData = (Member*)(_m_ptrv(data) + MOffsetOf<Struct>(member) + offset);
				*pData = GetValue<Member>();
			}
		};

		struct GetData
		{
			T* data = nullptr;
			AttribConverter* converter = nullptr;
			std::wstring_view attribName;
			Param param;
		};

		using SetAttribCallback = std::function<bool(SetData set)>;
		using GetAttribCallback = std::function<void(GetData get)>;

		struct attribCallback
		{
			SetAttribCallback set = nullptr;
			std::function<std::wstring(GetData get)> get = nullptr;
		};

		UIAttribute(T attrib, const std::shared_ptr<AttribConverter>& converter = std::make_shared<AttribConverter>())
		: m_attribData(std::move(attrib)), m_converter(converter) {}

		/*设置属性
		* @param attribName - 属性名
		* @param attrib - 属性值 (*如果指定类型则必须与源类型匹配)
		* @param param - 附加参数
		*/
		template<typename Type = std::wstring_view>
		bool SetAttribute(std::wstring_view attribName, const Type& attrib, Param param = {})
		{
			auto iter = m_attribCallbackList.find(attribName.data());
			if (iter == m_attribCallbackList.end() || !iter->second.set)
				return false;

			bool ret;
			if constexpr (std::is_same_v<Type, std::wstring_view>)
				ret = iter->second.set({ &m_attribData, m_converter.get(), attribName, attrib, std::move(param) });
			else
			{
				std::wstring value = m_converter->Convert(attrib);
				ret = iter->second.set({ &m_attribData, m_converter.get(), attribName, value, std::move(param) });
			}

			return ret;
		}

		//获取属性 (*如果指定类型则必须与源类型匹配)
		//@return 如果找到属性 返回true
		template <typename Type = std::wstring>
		bool GetAttribute(std::wstring_view attribName, Type& dstValue, Param param = {})
		{
			auto iter = m_attribCallbackList.find(attribName.data());
			if (iter == m_attribCallbackList.end() || !iter->second.get)
				return false;

			auto ret = iter->second.get({ &m_attribData, m_converter.get(), attribName, std::move(param) });

			if constexpr (std::is_same_v<Type, std::wstring>)
				dstValue = ret;

			dstValue = m_converter->Convert<Type>(ret);
			return true;
		}

		//注册设置方法
		static void RegisterAttrib(const std::vector<std::pair<std::wstring, attribCallback>>& attribList)
		{
			for (auto& attrib : attribList)
				m_attribCallbackList.insert(std::make_pair(attrib.first, attrib.second));
		}

		template<typename... Args>
		static void RegisterAttrib(Args&&... args)
		{
			(void)std::initializer_list<int>{ (m_attribCallbackList.insert(std::forward<Args>(args)), 0) ... };
		}

		template<typename Args>
		static void RegisterAttrib(std::initializer_list<Args>& args)
		{
			for (const auto& arg : args)
			{
				m_attribCallbackList.insert(arg);
			}
		}

		/*生成属性绑定
		 * @param member - 要绑定成员的变量
		 * @param name - 属性文本名称
		 * @param set - 设置属性回调 属性被设置时调用 可为空 默认nullptr 属性会自动赋值，如果指定回调，需要在回调手动给变量赋值
		 * @param get - 获取属性回调 获取属性时调用 可为空 默认nullptr
		 */
		template<typename Struct, typename Member>
		static auto MakeAttrib(Member Struct::* member, std::wstring_view name, SetAttribCallback set = nullptr, GetAttribCallback get = nullptr)
		{
			auto _set = [_callback = set, member](SetData data) -> bool
			{
				if (_callback)
					return _callback(std::move(data));

				data.template Assign<Struct, Member>(member);

				return true;
			};
			auto _get = [_callback = get, member](GetData data) -> std::wstring
			{
				auto pData = (Member*)(_m_ptrv(data.data) + MOffsetOf<Struct>(member));

				if (_callback)
					_callback(data);

				if constexpr (std::is_pointer_v<Member>)
					return data.converter->Convert((void*)*pData);
				else if constexpr (std::is_enum_v<Member>)
					return data.converter->Convert((int)*pData);
				else
					return data.converter->Convert(*pData);
			};
			return std::make_pair(std::wstring{ name }, attribCallback{ std::move(_set), std::move(_get) });
		}

		/*生成属性绑定从附加偏移量
		 * @param member - 要绑定的最终成员类型
		 * @param offset - 结构体成员偏移量(不包括最终成员)
		 * @param name - 属性文本名称
		 * @param set - 设置属性回调 属性被设置时调用 可为空 默认nullptr 属性会自动赋值，如果指定回调，需要在回调手动给变量赋值
		 * @param get - 获取属性回调 获取属性时调用 可为空 默认nullptr
		 */
		template<typename Struct, typename Member>
		static auto MakeAttribAtOffset(Member Struct::* member, _m_ptrv offset, std::wstring_view name, SetAttribCallback set = nullptr, GetAttribCallback get = nullptr)
		{
			auto _set = [_callback = set, member, offset](SetData data) -> bool
			{
				if (_callback)
					return _callback(std::move(data));

				data.template AssignAtOffset<Struct, Member>(member, offset);

				return true;
			};
			auto _get = [_callback = get, member, offset](GetData data) -> std::wstring
			{
				auto pData = (Member*)(_m_ptrv(data.data) + MOffsetOf<Struct>(member) + offset);

				if (_callback)
					_callback(data);

				if constexpr (std::is_pointer_v<Member>)
					return data.converter->Convert((void*)*pData);
				else if constexpr (std::is_enum_v<Member>)
					return data.converter->Convert((int)*pData);
				else
					return data.converter->Convert(*pData);
			};
			return std::make_pair(std::wstring{ name }, attribCallback{ std::move(_set), std::move(_get) });
		}

		template<typename Struct, typename Member>
		static constexpr _m_ptrv MOffsetOf(Member Struct::* member)
		{
			return _m_ptrv(&((Struct*)nullptr->*member));
		}

		//直接访问数据
		[[nodiscard]] const T& Get() const { return m_attribData; }
		T& Set() { return m_attribData; }
		T* GetPtr() { return &m_attribData; }
		void Set(const T& data) { m_attribData = data; }
		void Set(T&& data) { m_attribData = std::move(data); }

		using DataType = T;
	private:
		DataType m_attribData;
		std::shared_ptr<AttribConverter> m_converter;
		static std::unordered_map<std::wstring, attribCallback> m_attribCallbackList;
	};
	template<typename T, typename Param>
	std::unordered_map<std::wstring, typename UIAttribute<T, Param>::attribCallback> UIAttribute<T, Param>::m_attribCallbackList;

	template<class... T>
	class EventSlot
	{
	public:
		using ProcType = std::function<void(T...)>;

		void Bind(ProcType slot)
		{
			m_slotList.emplace_back(std::move(slot));
		}

		template<typename... Args>
		void Emit(Args... args)
		{
			for (auto& slot : m_slotList)
				slot(std::forward<Args>(args)...);
		}

	private:
		std::vector<ProcType> m_slotList;
	};

	/* 注册控件 注册后可以使用MuiXML类创建
	 * @param name - 控件类名 在XML中的名称
	 * @param method - 创建方法
	 * @param ptrAttrib - 特殊属性列表
	 * 特殊属性列表:
	 * 指定为特殊属性的属性将在SetAttribute时将value转换成对应类型的指针传递给字符串
	 * @return 如果重复注册或者已有重名将失败
	 */
	bool RegisterControl(std::wstring_view name, RegMethod&& method, const std::vector<AttribType>& ptrAttrib = {});

	void RegisterMuiControl();

	Ctrl::UIControl* CreateControl(std::wstring_view name, Ctrl::UIControl* parent);

	AttribType::type GetAttributeType(std::wstring_view name, std::wstring_view attribName);
}
