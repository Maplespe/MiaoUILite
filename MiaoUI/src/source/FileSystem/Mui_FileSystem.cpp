/**
 * FileName: Mui_FileSystem.cpp
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
#include <FileSystem/Mui_FileSystem.h>
#include <Mui_Helper.h>
#include <filesystem>
#ifdef _WIN32
#include <ShObjIdl_core.h>
#endif

#pragma warning(disable:4003)

#define CATCH_FSERROR(x) catch (const std::filesystem::filesystem_error& ex) \
{ \
	if (ex.code() == std::errc::permission_denied) \
		MErrorThrow(MErrorCode::AccessDenied); \
	else if (ex.code() == std::errc::io_error) \
		MErrorThrow(MErrorCode::IOError); \
	else \
		MErrorThrow(ex.what()); \
	return x; \
}

namespace Mui::FS
{
	bool UI::MBrowseForFile(bool isOpen, bool multiple, const std::vector<DlgFilter>& filter, _m_param parentWndHandle,
		std::vector<std::wstring>& selectedFiles, std::wstring_view defExtName)
	{
#ifdef _WIN32
		IFileDialog* fileDialog = nullptr;
		DWORD flags = FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST;
		if (multiple)
			flags |= FOS_ALLOWMULTISELECT;

		HRESULT hr = CoCreateInstance(isOpen ? CLSID_FileOpenDialog : CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER,
			isOpen ? IID_IFileOpenDialog : IID_IFileSaveDialog, (LPVOID*)&fileDialog);
		if (FAILED(hr))
			return false;

		//设置文件过滤器
		if(filter.empty())
		{
			COMDLG_FILTERSPEC cdlgfilter{ L"All Files(*.*)", L"*.*" };
			fileDialog->SetFileTypes(1, &cdlgfilter);
		}
		else
		{
			auto cdlgfilter = std::make_unique<COMDLG_FILTERSPEC[]>(filter.size());
			for (size_t i = 0; i < filter.size(); ++i)
			{
				cdlgfilter[i].pszName = filter[i].description.c_str();
				cdlgfilter[i].pszSpec = filter[i].extension.c_str();
			}
			fileDialog->SetFileTypes((UINT)filter.size(), cdlgfilter.get());
		}
		//设置Flag
		FILEOPENDIALOGOPTIONS options;
		fileDialog->GetOptions(&options);
		fileDialog->SetOptions(options | flags);
		fileDialog->SetDefaultExtension(defExtName.data());
		hr = fileDialog->Show((HWND)parentWndHandle);
		if (FAILED(hr))
		{
			fileDialog->Release();
			return false;
		}
		if (multiple && isOpen)
		{
			IShellItemArray* pItems = nullptr;
			static_cast<IFileOpenDialog*>(fileDialog)->GetResults(&pItems);

			if (!pItems)
			{
				fileDialog->Release();
				return false;
			}

			DWORD itemCount;
			pItems->GetCount(&itemCount);
			selectedFiles.reserve(itemCount);
			for (DWORD i = 0; i < itemCount; ++i)
			{
				IShellItem* pItem = nullptr;
				pItems->GetItemAt(i, &pItem);
				if (!pItem)
					continue;

				PWSTR pszFilePath;
				pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				selectedFiles.emplace_back(pszFilePath);
				CoTaskMemFree(pszFilePath);
				pItem->Release();
			}
			pItems->Release();
		}
		else
		{
			IShellItem* pItems = nullptr;
			fileDialog->GetResult(&pItems);
			if (!pItems)
			{
				fileDialog->Release();
				return false;
			}
			PWSTR pszFilePath;
			pItems->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
			selectedFiles.emplace_back(pszFilePath);
			CoTaskMemFree(pszFilePath);
			pItems->Release();
		}
		fileDialog->Release();
		return true;
#else
#error __TODO__
#endif
	}

	UIResource MReadFile(std::wstring_view filepath, bool string)
	{
		FILE* file = MOpenFile(filepath, "rb");
		if (!file) return {};

		_fseeki64(file, 0L, SEEK_END);
		auto len = _ftelli64(file);

		if(len == -1)
		{
			fclose(file);
			MErrorThrow(MErrorCode::IOError);
		}

		rewind(file);
		//读文本长度+1 结尾添加结束符
		_m_byte* data = new(std::nothrow) _m_byte[string ? size_t(len + 1) : len];
		if (!data)
			MErrorThrow(MErrorCode::OutOfMemory, len);

		if (string) data[len] = '\0';
		len = (_m_ulong)fread(data, 1, len, file);

		fclose(file);
		return { data, (_m_size)len };
	}

	_m_size MWriteFile(std::wstring_view filepath, UIResource res)
	{
		FILE* file = MOpenFile(filepath, "wb");
		if (!file) return 0;

		auto len = fwrite(res.data, 1, res.size, file);
		fclose(file);
		return len;
	}

	FILE* MOpenFile(std::wstring_view filepath, const char* mode)
	{
		FILE* file = nullptr;
		_m_error err = 0;
#ifdef _WIN32
		err = _wfopen_s(&file, filepath.data(), Helper::M_StringToWString(mode).c_str());
#elif __ANDROID__
		file = fopen64((char*)M_WStringToString(path).c_str(), mode);
#else
#error __TODO__
#endif
		if (!file && err != 0)
			MErrorThrow(MErrorCode::IOError, err);
		return file;
	}

	void MSeekFile(FILE* file, _m_long64 offset, int origin)
	{
		int err = 0;
#ifdef _WIN32
		err = _fseeki64(file, offset, origin);
#elif __ANDROID__
		err = fseeko64(file, (_m_size)offset, origin);
#endif
		if (err != 0)
		{
			if (err == -1)
				MErrorThrow(MErrorCode::InvalidParameter);
			else
				MErrorThrow(MErrorCode::IOError, err);
		}
	}

	std::wstring MGetFileName(std::wstring_view path)
	{
		std::wstring::size_type pos = path.find_last_of('\\');
		if (pos == std::wstring::npos)
			pos = path.find_last_of('/');
		pos += 1;
		return path.substr(pos, path.length() - pos).data();
	}

	void MEnumDirectory(std::wstring_view path, std::vector<std::wstring>& dirlist, bool recursive) try
	{
		std::filesystem::path _path(path);
		if(recursive)
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(_path))
			{
				if (!entry.is_directory()) continue;
				dirlist.push_back(entry.path().wstring());
			}
		}
		else
		{
			for (const auto& entry : std::filesystem::directory_iterator(_path))
			{
				if (!entry.is_directory()) continue;
				dirlist.push_back(entry.path().wstring());
			}
		}
	}
	CATCH_FSERROR()

	void MEnumFiles(std::wstring_view path, const std::vector<std::wstring>& filter,
		std::vector<std::wstring>& dirlist, bool recursive) try
	{
		std::filesystem::path _path(path);
		auto checkext = [&](std::wstring_view ext)
		{
			if (filter.empty()) return true;
			return std::any_of(filter.begin(), filter.end(),
				[&](const std::wstring& str) { return str == ext; });
		};

		if (recursive)
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(_path))
			{
				if (!entry.is_regular_file() || !checkext(entry.path().extension().wstring())) continue;
				dirlist.push_back(entry.path().wstring());
			}
		}
		else
		{
			for (const auto& entry : std::filesystem::directory_iterator(_path))
			{
				if (!entry.is_regular_file() || !checkext(entry.path().extension().wstring())) continue;
				dirlist.push_back(entry.path().wstring());
			}
		}
	}
	CATCH_FSERROR()

	bool MRemoveFile(std::wstring_view path) try
	{
		std::filesystem::path file(path);
		return std::filesystem::remove(file);
	}
	CATCH_FSERROR(false)

	bool MRemoveDirectory(std::wstring_view path, bool removeAll) try
	{
		std::filesystem::path file(path);
		if(removeAll)
			return std::filesystem::remove_all(file);
		try
		{
			if (std::filesystem::directory_iterator(file) != std::filesystem::directory_iterator())
				return false;
		}
		catch (const std::filesystem::filesystem_error&)
		{
			return false;
		}
		return std::filesystem::remove(file);
	}
	CATCH_FSERROR(false)

	bool MFileExists(std::wstring_view path, bool isdir) try
	{
		std::filesystem::path file(path);
		bool exists = std::filesystem::exists(file);
		if (!exists)
			return false;
		bool isDir = std::filesystem::is_directory(file);
		if (isdir && !isDir)
			return false;
		if (!isdir && isDir)
			return false;
		return true;
	}
	CATCH_FSERROR(false)

	bool MCreateDirectory(std::wstring_view path, bool mult) try
	{
		if (MFileExists(path, true)) return true;

		std::filesystem::path file(path);
		if (mult)
			return std::filesystem::create_directories(file);
		return std::filesystem::create_directory(file);
	}
	catch (const std::filesystem::filesystem_error& ex) 
	{
		if (ex.code() == std::errc::permission_denied) MErrorThrow(MErrorCode::AccessDenied);
		else if (ex.code() == std::errc::io_error) MErrorThrow(MErrorCode::IOError);
		return false;
	}

	std::wstring MGetCurrentDir()
	{
#ifdef _WIN32
		wchar_t sPath[MAX_PATH];
		GetModuleFileNameW(nullptr, sPath, MAX_PATH);
		std::wstring path = sPath;
		path = path.substr(0, path.rfind(L'\\'));

		return path;
#else
#error __TODO__
#endif
	}
}
