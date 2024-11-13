/**
 * FileName: Mui_FileSystem.h
 * Note: 基本文件系统操作
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
 * date: 2024-10-9 Create
*/
#pragma once
#include <Mui_Base.h>

namespace Mui::FS
{
	namespace UI
	{
		struct DlgFilter
		{
			//描述信息,扩展名
			std::wstring description, extension;
		};
		/* 打开浏览文件对话框
		 * @param [in] isOpen - 是否为打开文件 否则为保存文件对话框
		 * @param [in] multiple - 是否支持多选文件 保存对话框不支持多选
		 * @param [in] filter - 文件过滤器列表 例如 { L"AllFiles(*.*), L"*.*" },{ L"ImageFiles(*.png;*.jpg)", L"*.png;*.jpg" }
		 * @param [in] parentWndHandle - 父窗口句柄
		 * @param [out] selectedFiles - 返回已选择的文件列表
		 * @param [in] defExtName - 默认文件扩展名
		 *
		 * @return 如果打开对话框失败或者用户关闭对话框 返回false
		 */
		extern bool MBrowseForFile(bool isOpen, bool multiple, const std::vector<DlgFilter>& filter,
			_m_param parentWndHandle, std::vector<std::wstring>& selectedFiles, std::wstring_view defExtName = {});
	}

	//获取当前程序运行所在目录 
	std::wstring MGetCurrentDir();

	/*读取文件
	* @param filepath - 文件路径
	* @param string - 是否为字符串类型 如果为true 则结尾长度+1 并添加 '\0'
	* @return 如果成功返回内存资源 否则为空
	* @exception MErrorCode::IOError; MErrorCode::OutOfMemory
	*/
	UIResource MReadFile(std::wstring_view filepath, bool string = false);

	/*写入文件
	* @param filepath - 文件路径
	* @param res - 内存数据
	* @return 返回写入成功的字节数
	* @exception MErrorCode::IOError;
	*/
	_m_size MWriteFile(std::wstring_view filepath, UIResource res);

	/*打开文件(64位长度)
	* @param filepath - 文件路径
	* @param mode - 打开模式 wb、rb、w、r、a、+
	* @return C API文件句柄
	* @exception MErrorCode::IOError;
	*/
	FILE* MOpenFile(std::wstring_view filepath, const char* mode);

	/*移动文件指针(64位长度)
	* @param file - 使用OpenFile打开的文件指针
	* @param offset - 目标位置
	* @param origin - 移动起点 SEEK_SET、SEEK_CUR、SEEK_END
	* @exception MErrorCode::IOError; MErrorCode::InvalidParameter
	*/
	void MSeekFile(FILE* file, _m_long64 offset, int origin = SEEK_SET);

	/*获取路径最后的文件名
	* @param path - 路径 可以为正斜杠或反斜杠
	* @return 例如 C:\\Test\\Path\\filename.txt 返回 filename.txt
	*/
	std::wstring MGetFileName(std::wstring_view path);

	/*枚举目录
	* @param path - 起始路径
	* @param [out] dirlist - 返回的目录列表
	* @param recursive - 递归获取所有子级目录 否则仅获取起始路径下的目录
	* @exception MErrorCode::STDError; MErrorCode::AccessDenied; MErrorCode::IOError
	*/
	void MEnumDirectory(std::wstring_view path, std::vector<std::wstring>& dirlist, bool recursive = false);

	/*枚举文件
	* @param path - 起始目录路径
	* @param filter - 文件过滤器列表 例如 { L".txt", L".png" } 为空{}则输出所有的文件 
	* @param [out] dirlist - 返回的文件路径列表
	* @param recursive - 递归获取所有子级目录下的文件 否则仅获取起始路径下的文件
	* @exception MErrorCode::STDError; MErrorCode::AccessDenied; MErrorCode::IOError
	*/
	void MEnumFiles(std::wstring_view path, const std::vector<std::wstring>& filter,
		std::vector<std::wstring>& dirlist, bool recursive = false);

	/*删除文件
	* @param path - 文件路径
	* @return 如果删除成功 返回true，文件不存在 返回false
	* @exception MErrorCode::STDError; MErrorCode::AccessDenied; MErrorCode::IOError
	*/
	bool MRemoveFile(std::wstring_view path);

	/*删除目录
	* @param path - 目录路径
	* @param removeAll - 是否删除非空目录及其子目录的所有的内容
	* @return 如果删除成功 返回true, 如果目录不存在返回false 如果removeAll=false且目录中有文件 返回false
	* @exception MErrorCode::STDError; MErrorCode::AccessDenied; MErrorCode::IOError
	*/
	bool MRemoveDirectory(std::wstring_view path, bool removeAll = false);

	/*检查文件或目录是否存在
	* @param path - 路径
	* @param isdir - 是否检查目录
	* @return 如果文件或目录不存在 返回false
	* @exception MErrorCode::STDError; MErrorCode::AccessDenied; MErrorCode::IOError
	*/
	bool MFileExists(std::wstring_view path, bool isdir = false);

	/*创建目录
	* @param path - 创建路径
	* @param mult - 是否创建多级目录
	* @return 如果成功或者目录已存在返回true 如果mult=false 且父目录不存在 返回false
	* @exception MErrorCode::AccessDenied; MErrorCode::IOError
	*/
	bool MCreateDirectory(std::wstring_view path, bool mult);
}