/**
 * FileName: Mui_Debug.h
 * Note: 调试输出Helper
 *
 * Copyright (C) 2023-2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2023-1-19 Create
*/
#include <Mui_Base.h>
#include <Mui_Helper.h>
#include <iomanip>

#ifdef _MSC_VER
#include <intrin.h>
#define __ADDRESS__ _AddressOfReturnAddress()
#elif __clang__ 
#define __ADDRESS__ __builtin_return_address(0)
#else
#error "__ADDRESS__ 没有可用定义"
#endif

namespace Mui
{
	std::exception_ptr _g_last_merror_exception = nullptr;
	std::function<void(const MError&)> _g_exception_callback = nullptr;

	std::string addressFromat(void* pointer = __ADDRESS__)
	{
		std::stringstream ss;
		ss << "0x" << std::setfill('0') << std::setw(sizeof(void*) * 2) << std::hex << reinterpret_cast<uintptr_t>(pointer);
		return ss.str();
	}

#if (MUI_CFG_ENABLE_DBGSOURCE && MUI_CFG_ENABLE_DBGFUNNAME)
	std::wstring _m_dbg_format_(std::string_view __FILE, std::string_view __FUN)
	{
		auto pos = __FILE.rfind('\\');
		if (pos == std::string::npos) pos = __FILE.rfind('/');
		if (pos == std::string::npos) pos = 0;
		else pos++;
		std::string str = "[";
		str += __FILE.substr(pos, __FILE.length() - pos);
		str += "] -> ";
		str += __FUN;
		return Helper::M_StringToWString(str);
	}
#elif (MUI_CFG_ENABLE_DBGSOURCE || MUI_CFG_ENABLE_DBGFUNNAME)
	std::wstring _m_dbg_format_(std::string_view __INFO)
	{
#if MUI_CFG_ENABLE_DBGSOURCE
		auto pos = __INFO.rfind('\\');
		if (pos == std::string::npos) pos = __INFO.rfind('/');
		if (pos == std::string::npos) pos = 0;
		else pos++;
		std::string str = "[";
		str += __INFO.substr(pos, __INFO.length() - pos);
		str += "] -> ";
		str += addressFromat();
#else
		std::string str = "[" + addressFromat() + "] -> ";
		str += __INFO;
#endif
		return Helper::M_StringToWString(str);
	}
#else
	std::wstring _m_dbg_format_()
	{
		std::string str = "[" + addressFromat() + "]";
		return Helper::M_StringToWString(str);
	}
#endif

	void ErrorDialog(std::wstring_view info)
	{
#ifdef _WIN32
		MessageBoxW(0, info.data(), L"MiaoUI Library", MB_ICONERROR | MB_OK);
#elif __ANDROID__
		__android_log_print(ANDROID_LOG_ERROR, "MUIERROR", "%s", M_WStringToString(info).c_str());
#else
#error __TODO__
#endif
	}

	void _g_last_exception_callback(const MError& ex)
	{
		std::wstring errinfo = L"很抱歉，MiaoUI抛出了一个错误 :(\n该异常未经处理，程序已被迫终止。\n";
		errinfo += L"I'm sorry, but MiaoUI is throwing an error :(\nThe exception was unhandled and the program is about to be forced to terminate.\n";
		errinfo += L"错误信息(Error message): " + ex.fullText();
		ErrorDialog(errinfo);
	}

	void M_OutError(std::wstring_view cls, std::wstring_view error, bool exit)
	{
#ifdef _DEBUG
		std::wstring str = L"\n";
		str += cls;
		str += L" -> ";
		str += error;
		M_DbgOutInfo(str.c_str());
#ifdef _WIN32
		_CrtDbgBreak();
#endif
#else
		std::wstring errinfo;
		if (!exit)
			errinfo = L"该错误为非致命错误 程序将可以继续运行.";
		else
			errinfo = L"该错误为致命错误 程序无法继续运行.";

		errinfo = L"很抱歉，MiaoUI库内部抛出了一个错误 :(\n" + errinfo + L"\n错误信息:\n";
		errinfo += cls;
		errinfo += L" -> ";
		errinfo += error;
		ErrorDialog(errinfo.c_str());
#endif
		if (exit)
			::exit(1);
	}

	void M_OutError(const MError& err, std::wstring_view cls)
	{
		if (cls.empty())
			M_OutError(L"[Mui_Error.cpp]", err.fullText());
		else
			M_OutError(cls, err.fullText());
	}

	void M_SetLastExceptionNotify(const std::function<void(const MError&)>& callback)
	{
		_g_exception_callback = callback;
		std::set_terminate([]
		{
			if (!_g_last_merror_exception) return;
			try
			{
				std::rethrow_exception(_g_last_merror_exception);
			}
			catch (const MError& ex)
			{
				_g_exception_callback(ex);
			}
		});
	}

	void M_ThreadPostException(std::exception_ptr ex)
	{
		if (_g_exception_callback)
		{
			try { std::rethrow_exception(_g_last_merror_exception); }
			catch (const MError& ex)
			{
				_g_exception_callback(ex);
				std::abort();
			}
		}
		else
			std::rethrow_exception(ex);
	}
}
