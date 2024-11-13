/**
 * FileName: Mui_Error.h
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
#pragma once
#include <Mui_TypeDef.h>
#include <Mui_Framework.h>

namespace Mui
{
	using _m_error = _m_param;

	enum class MErrorCode : _m_error
	{
		OK,		//正常
		False,	//失败
		InvalidParameter,		//无效的参数
		NotSupported,			//不支持的操作
		InvalidData,			//无效的数据

		DeviceLost = 0x10001,	//设备已丢失 需要重建硬件资源
		InternalDeviceError,	//设备内部错误
		OutOfMemory,			//内存不足 内存分配失败 extCode为分配失败的字节数
		IndexOutOfRange,		//索引超出范围
		TheThreadIsBusy,		//线程当前正忙
		BufferTooSmall,			//缓冲区过小

		AccessDenied = 0x20001,	//拒绝访问 权限不足
		IOError,				//I/O错误 extCode可能为系统错误代码
		OutdatedInterface,		//已过时的接口

		RenderInternalError = 0x30001,//渲染器内部错误

		STDError = 0xA00001,	//C++标准库异常
		WinCOMError,			//Window COM HRESULT 错误代码 extCode为HRESULT错误代码
		MUIError,				//其他MiaoUI错误 使用whatW()获取详细信息

		Unknown = 0xFFFFFF		//未知异常
	};

	class MError final : std::exception
	{
	public:
		//MiaoUIError
		explicit MError(MErrorCode err, _m_error ext = 0) noexcept;
		//STDError
		explicit MError(const char* what) noexcept;
		//COMError
		MError(_m_error hresult, std::wstring_view err);
		//MUIError
		MError(std::wstring_view info);

		//获取错误文本说明(仅英文) 如果错误为COMError则只包括MError的说明文本 com错误的说明文本使用 whatW()获取
		[[nodiscard]] const char* what() const noexcept override;

		//获取宽字符版本的中文+英文文本说明 如果错误为COMError则为HRESULT的对应说明文本
		[[nodiscard]] std::wstring whatW() const;

		//获取错误代码
		[[nodiscard]] MErrorCode errCode() const noexcept;

		//获取扩展代码 如果异常另有说明 则此可以获取附带的信息
		[[nodiscard]] _m_error extCode() const noexcept;

		//获取MErrorCode错误代码具体说明文本(仅英文)
		static const char* toMessage(MErrorCode errCode);

		//获取MErrorCode错误代码具体说明文本(中文+英文)
		static std::wstring toMessageW(MErrorCode errCode);

		//获取完整的错误信息 错误代码+说明文本
		[[nodiscard]] std::wstring fullText() const;

	private:
		MErrorCode m_errCode = MErrorCode::Unknown;
		_m_error m_extCode = 0;
		std::wstring m_extInfo;
	};
}