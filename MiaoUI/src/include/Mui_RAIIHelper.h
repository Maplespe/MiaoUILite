/**
 * FileName: Mui_RAIIHelper.h
 * Note: RAII助手
 *
 * Copyright (C) 2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2024-1-24 Create
*/
#pragma once
#include <Mui_Framework.h>
#include <Mui_Debug.h>

namespace Mui::RAII
{
	template <typename TLambda>
	class lambda_call
	{
	public:
		lambda_call(const lambda_call&) = delete;
		lambda_call& operator=(const lambda_call&) = delete;
		lambda_call& operator=(lambda_call&& other) = delete;

		explicit lambda_call(TLambda&& lambda) noexcept : m_lambda(std::move(lambda))
		{
			static_assert(std::is_same_v<decltype(lambda()), void>, "scope_exit的lambda函数一定不能有返回值");
			static_assert(!std::is_lvalue_reference_v<TLambda> && !std::is_rvalue_reference_v<TLambda>, "scope_exit只能直接与lambda函数一起使用");
		}

		lambda_call(lambda_call&& other) noexcept : m_lambda(std::move(other.m_lambda)), m_call(other.m_call)
		{
			other.m_call = false;
		}

		~lambda_call() noexcept
		{
			reset();
		}
		//确保scope_exit lambda不会被调用
		void release() noexcept
		{
			m_call = false;
		}
		//立刻执行scope_exit lambda如果还没有运行的话；确保它不再次运行
		void reset() noexcept
		{
			if (m_call)
			{
				m_call = false;
				m_lambda();
			}
		}
		//返回true如果scope_exit lambda仍要被执行
		[[nodiscard]] explicit operator bool() const noexcept
		{
			return m_call;
		}
	protected:
		TLambda m_lambda;
		bool m_call = true;
	};

	/*
		返回一个对象，该对象在销毁时执行给定的lambda函数，请使用auto捕获返回的对象
		使用reset()提前执行lambda或使用release()避免执行
		在lambda中抛出的异常将引发快速失败
		你可以认为这个对象的作用跟finally差不多
	*/
	template <typename TLambda>
	[[nodiscard]] inline auto scope_exit(TLambda&& lambda) noexcept
	{
		return lambda_call<TLambda>(std::forward<TLambda>(lambda));
	}

	/* Mui资源基本对象 该类型以类似COM的方式进行引用计数释放
	 * 一般资源仅用于当前上下文，不可拷贝 不可通用
	 * 可以使用Mui_Ptr来自动管理
	 */
	class MBasicObj
	{
	public:
		virtual ~MBasicObj() noexcept = default;
		MBasicObj(MBasicObj&&) = delete;
		MBasicObj(const MBasicObj&) = delete;
		MBasicObj& operator=(const MBasicObj&) = delete;
		MBasicObj& operator=(MBasicObj&&) = delete;

		void AddRef() noexcept { ++m_ref; }
		void Release()
		{
			if (m_ref > 0)
				--m_ref;
			if (m_ref != 0)
				return;
			try
			{
				ReleaseProc();
			}
			catch (...)
			{
#ifdef _DEBUG
				_M_OutErrorDbg_(L"对象释放发生异常", false);
#endif
			}
			delete this;
		}

	protected:
		MBasicObj() noexcept : m_ref(0) {}
		virtual void ReleaseProc() {}

	private:
		std::atomic_int m_ref;
	};

	template <typename T>
	class Mui_Ptr
	{
	public:
		Mui_Ptr() noexcept : m_ptr(nullptr) {}

		Mui_Ptr(T* other) noexcept : m_ptr(other)
		{
			if (m_ptr)
				m_ptr->AddRef();
		}

		Mui_Ptr(const Mui_Ptr& other) noexcept : m_ptr(other.m_ptr)
		{
			if (m_ptr)
				m_ptr->AddRef();
		}

		Mui_Ptr(Mui_Ptr&& other) noexcept : m_ptr(nullptr)
		{
			swap(other);
		}

		~Mui_Ptr() noexcept
		{
			if (m_ptr)
				m_ptr->Release();
		}

		Mui_Ptr& operator=(T* other) noexcept
		{
			if (m_ptr != other)
				Mui_Ptr(other).swap(*this);
			
			return *this;
		}

		Mui_Ptr& operator=(const Mui_Ptr& other) noexcept
		{
			if (this == &other) 
				return *this;
			Mui_Ptr(other).swap(*this);
			return *this;
		}

		Mui_Ptr& operator=(Mui_Ptr&& other) noexcept
		{
			Mui_Ptr(std::move(other)).swap(*this);
			return *this;
		}

		bool operator!=(const Mui_Ptr& other) noexcept
		{
			return m_ptr != other.m_ptr;
		}

		bool operator!=(const T* other) noexcept
		{
			return m_ptr != other;
		}

		bool operator==(const Mui_Ptr& other) noexcept
		{
			return m_ptr == other.m_ptr;
		}

		bool operator==(const T* other) noexcept
		{
			return m_ptr == other;
		}

		[[nodiscard]] T* get() const noexcept
		{
			return m_ptr;
		}

		T** operator&() noexcept
		{
			return &m_ptr;
		}

		T* operator->() const noexcept
		{
			return m_ptr;
		}

		explicit operator bool() const noexcept
		{
			return m_ptr != nullptr;
		}

		void swap(Mui_Ptr& other) noexcept
		{
			std::swap(m_ptr, other.m_ptr);
		}

		template<typename CAST>
		Mui_Ptr<CAST> cast() noexcept
		{
			return reinterpret_cast<CAST*>(m_ptr);
		}

		void rest() noexcept
		{
			operator=(nullptr);
		}

		void detach() noexcept
		{
			m_ptr = nullptr;
		}

		using Type = T;
	private:
		T* m_ptr = nullptr;
	};
}