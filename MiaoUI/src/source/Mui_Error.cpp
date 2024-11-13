/**
 * FileName: Mui_Error.cpp
 * Note: 错误信息和类型定义
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
 * date: 2024-9-18 Create
*/
#include <Mui_Error.h>
#include <Mui_Helper.h>
#include <iomanip>
#include <sstream>

namespace Mui
{
	MError::MError(MErrorCode err, _m_error ext) noexcept
	{
		m_errCode = err;
		m_extCode = ext;
	}

	MError::MError(const char* what) noexcept
		: std::exception(what)
	{
		m_errCode = MErrorCode::STDError;
	}

	MError::MError(_m_error hresult, std::wstring_view err)
	{
		m_errCode = MErrorCode::WinCOMError;
		m_extCode = hresult;
		m_extInfo = err;
	}

	MError::MError(std::wstring_view info)
	{
		m_errCode = MErrorCode::MUIError;
		m_extInfo = info;
	}

	const char* MError::what() const noexcept
	{
		if (auto err = exception::what(); m_errCode == MErrorCode::STDError && err)
			return err;
		return toMessage(m_errCode);
	}

	std::wstring MError::whatW() const
	{
		if (auto err = exception::what(); m_errCode == MErrorCode::STDError && err)
			return Helper::M_StringToWString(err);
		if (m_errCode == MErrorCode::WinCOMError || m_errCode == MErrorCode::MUIError)
			return m_extInfo;
		return toMessageW(m_errCode);
	}

	MErrorCode MError::errCode() const noexcept
	{
		return m_errCode;
	}

	_m_error MError::extCode() const noexcept
	{
		return m_extCode;
	}

	const char* MError::toMessage(MErrorCode errCode)
	{
		switch (errCode)
		{
		case MErrorCode::OK:
			return "OK.";
		case MErrorCode::False:
			return "Failed.";
		case MErrorCode::InvalidParameter:
			return "Invalid parameters, one or more parameters are invalid.";
		case MErrorCode::NotSupported:
			return "The current operation or function is not supported.";
		case MErrorCode::InvalidData:
			return "Invalid data, or incorrect formatting.";
		case MErrorCode::DeviceLost:
			return "The device has been lost or its status has changed, discard the associated device resource and recreate it.";
		case MErrorCode::InternalDeviceError:
			return "Internal device error, please check the relevant device information or driver.";
		case MErrorCode::OutOfMemory:
			return "Memory allocation failed, or there is not enough memory.";
		case MErrorCode::IndexOutOfRange:
			return "The index is outside the specified array or prescribed range.";
		case MErrorCode::TheThreadIsBusy:
			return "The thread is currently busy, please wait or visit later.";
		case MErrorCode::BufferTooSmall:
			return "The buffer is too small, please consider increasing it.";
		case MErrorCode::AccessDenied:
			return "Access denied.";
		case MErrorCode::IOError:
			return "I/O error, or file system error.";
		case MErrorCode::OutdatedInterface:
			return "Outdated interfaces are not recommended, but if you want to use them, add the appropriate compilation flags.";
		case MErrorCode::RenderInternalError:
			return "There is an abnormality inside the renderer. Please check the implementation of the rendering interface.";
		case MErrorCode::STDError:
			return "C++ standard library exception, use what() to retrieve information.";
		case MErrorCode::WinCOMError:
			return "Windows COM HRESULT error code, please check extCode.";
		case MErrorCode::MUIError:
			return "MUI other errors, use whatW() to get detailed information.";
		}
		return "Unknown error";
	}

	std::wstring MError::toMessageW(MErrorCode errCode)
	{
		std::wstring ret;
		switch (errCode)
		{
		case MErrorCode::OK:
			ret = L"成功。";
			break;
		case MErrorCode::False:
			ret = L"已失败。";
			break;
		case MErrorCode::InvalidParameter:
			ret = L"无效的参数，一个或多个参数无效。";
			break;
		case MErrorCode::NotSupported:
			ret = L"不支持当前的操作或者功能。";
			break;
		case MErrorCode::InvalidData:
			ret = L"无效的数据，或格式不正确。";
			break;
		case MErrorCode::DeviceLost:
			ret = L"设备已丢失或状态发生改变，请丢弃关联的设备资源然后重新创建。";
			break;
		case MErrorCode::InternalDeviceError:
			ret = L"设备内部错误，请检查相关的设备信息或者驱动程序。";
			break;
		case MErrorCode::OutOfMemory:
			ret = L"内存分配失败，或内存不足。";
			break;
		case MErrorCode::IndexOutOfRange:
			ret = L"索引超出指定的数组或规定范围。";
			break;
		case MErrorCode::TheThreadIsBusy:
			ret = L"线程当前正忙，请等待或稍后再访问。";
			break;
		case MErrorCode::BufferTooSmall:
			ret = L"缓冲区过小，请考虑增大。";
			break;
		case MErrorCode::AccessDenied:
			ret = L"权限不足。";
			break;
		case MErrorCode::IOError:
			ret = L"I/O 错误，或文件系统错误。";
			break;
		case MErrorCode::OutdatedInterface:
			ret = L"已过时的接口，不建议使用，若要使用 请添加相应的编译Flag.";
			break;
		case MErrorCode::RenderInternalError:
			ret = L"渲染器内部异常，请检查渲染器接口实现。";
			break;
		case MErrorCode::STDError:
			ret = L"C++标准库异常，使用what()获取信息。";
			break;
		case MErrorCode::WinCOMError:
			ret = L"Windows COM HRESULT错误代码，请检查extCode。";
			break;
		case MErrorCode::MUIError:
			ret = L"MUI其他错误，使用whatW()获取详细信息。";
			break;
		case MErrorCode::Unknown:
		default:
			ret = L"未知错误。";
			break;
		}

		ret += L"\n(" + Helper::M_StringToWString(toMessage(errCode)) + L")";
		return ret;
	}

	std::wstring MError::fullText() const
	{
		std::wstringstream stream;
		stream << std::hex << std::uppercase << std::setfill(L'0') << std::setw(2);

		if (m_errCode == MErrorCode::WinCOMError)
			stream << (_m_long)m_extCode;
		else
			stream << (_m_error)m_errCode;

		std::wstring ret = L" (0x" + stream.str() + L"): ";
		ret += whatW();

		if (m_errCode == MErrorCode::WinCOMError)
			ret = L"HRESUL COM ErrCode" + ret;
		else
			ret = L"MiaoUI ErrCode" + ret;

		return ret;
	}
}
