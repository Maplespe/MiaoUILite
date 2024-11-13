/**
 * FileName: DreamMoonRes.cpp
 * Note: 梦月资源类实现
 *
 * Copyright (C) 2020-2022 Maplespe (mapleshr@icloud.com)
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
 * date: 2020-12-8 Create
*/
#include <FileSystem\DreamMoonRes.h>
#include <Mui_Helper.h>

#if MUI_CFG_ENABLE_V1DMRES

#define wsize 2
#define clsnameMax 30

#if MUI_CFG_ENABLE_CRYPTOPP
#define DMRES_USE_CRYPTOPP
#endif

#ifdef DMRES_USE_CRYPTOPP
#include <cryptlib.h>
#include <filters.h>
#include <modes.h>
#include <aes.h>
#include <hex.h>
#include <sha.h>
#else
#include <MiaoUI\src\source\ThirdParty\aes256.h>
#include <MiaoUI\src\source\ThirdParty\picosha2.h>
#endif // DMRES_USE_CRYPTOPP

#ifdef __ANDROID__
#include <unistd.h>
#include <android/log.h>
#include "errno.h"
#endif

namespace Mui {

	DMResources::~DMResources()
	{
		CloseResource();
	}

	bool DMResources::LoadResource(std::wstring resfiles, bool mem)
	{
		if (m_files || m_resource.data)
			return false;
		if (mem) {
			m_resource = ReadFiles(resfiles);
			if (!m_resource.data)
				return false;
			bool ret = LoadResList(!mem);
			if (!ret)
				CloseResource();
			return ret;
		}
		else {
#ifdef _WIN32
			m_files = _wfsopen(resfiles.c_str(), L"rb", _SH_DENYNO);
			//_wfopen_s(&m_files, resfiles.c_str(), L"rb");
#endif // _WIN32
#ifdef __ANDROID__
			m_files = fopen((char*)M_WStringToString(resfiles).c_str(), "rb");
#endif // __ANDROID__
			if (m_files) {
				bool ret = LoadResList(!mem);
				if (!ret)
					CloseResource();
				return ret;
			}
		}
		return false;
	}

#ifdef _WIN32

	UIResource DMResources::ReadPEResource(DWORD resID, LPCWSTR type)
	{
		UIResource ret;
		HINSTANCE hDll = GetModuleHandle(NULL);
		HRSRC hRes = FindResourceW(hDll, MAKEINTRESOURCE(resID), type);
		HGLOBAL hgRes = ::LoadResource(hDll, hRes);
		DWORD nResSize = SizeofResource(hDll, hRes);
		BYTE* pRes = (BYTE*)LockResource(hgRes);
		ret.data = new _m_byte[nResSize];
		ret.size = nResSize;
		memcpy(ret.data, pRes, nResSize);
		UnlockResource(hgRes);
		FreeResource(hgRes);
		return ret;
	}

	bool DMResources::LoadResource(_m_ulong resid, _m_lpcwstr restype)
	{
		if (m_resource.data || m_resource.size)
			return false;
		m_resource = ReadPEResource(resid, restype);
		if (m_resource.data && m_resource.size)
			return LoadResList();
		return false;
	}

#endif // _WIN32

	bool DMResources::LoadResource(UIResource memres)
	{
		if (m_resource.data || m_resource.size || !memres.data || memres.size == 0)
			return false;

		m_resource.size = memres.size;
		m_resource.data = (_m_byte*)malloc(memres.size);

		memcpy(m_resource.data, memres.data, memres.size);
		return LoadResList();
	}

	_m_size DMResources::GetResourceCount()
	{
		return (_m_size)dmResList.size();
	}

	DMResKey DMResources::ReadResource(std::wstring resname, std::wstring reskey, ResType type)
	{
		DMResKey key;
		mx.lock();
		for (auto item : dmResList)
		{
			if (item.resname == resname)
			{
				if (m_files)
				{
					if (type == AllRes || type == DataRes) {
						if (item.resPos.second != 0)
						{
							item.res.data = new _m_byte[item.resPos.second];
							fseek(m_files, item.resPos.first, SEEK_SET);
							fread(item.res.data, 1, item.resPos.second, m_files);
							int err = ferror(m_files);
							key.res = Deciphering(item.res, reskey);
							delete[] item.res.data;
						}
					}
					if (type == AllRes || type == StringRes) {
						if (item.strPos.second != 0)
						{
							char16_t* str = new char16_t[item.strPos.second];
							fseek(m_files, item.strPos.first, SEEK_SET);
							fread(str, 1, item.strPos.second * wsize, m_files);
							key.resText = DecipheringText({ (_m_byte*)str, item.strPos.second }, reskey);
							delete[] str;
						}
					}
				}
				else
				{
					if (type == AllRes || type == DataRes) {
						if (item.res.data)
							key.res = Deciphering(item.res, reskey);
					}
					if (type == AllRes || type == StringRes) {
						if (item.text.data)
							key.resText = DecipheringText(item.text, reskey);
					}
				}
				break;
			}
		}
		mx.unlock();
		return key;
	}

	UIResource DMResources::ReadResourceBlock(std::wstring resname, std::wstring reskey, _m_uint offset, _m_size len, _m_size* retlen)
	{
		UIResource res;
		mx.lock();
		for (auto item : dmResList)
		{
			if (item.resname == resname)
			{
				if (m_files)
				{
					if (item.resPos.second != 0)
					{
						_m_size readsize = len;
						//偏移量超过数据大小 返回空
						if (offset > item.resPos.second) {
							mx.unlock();
							return res;
						}
						//偏移量+要读取的数据大于剩余数据
						else if (offset + len > item.resPos.second)
							readsize = item.resPos.second - offset;//取可读的数据

						item.res.data = new _m_byte[readsize];
						fseek(m_files, item.resPos.first + offset, SEEK_SET);
						_m_size read = (_m_size)fread(item.res.data, 1, readsize, m_files);
						int err = ferror(m_files);
						item.res.size = read;
						res = Deciphering(item.res, reskey);
						delete[] item.res.data;
						if (retlen)
							*retlen = read;
					}
				}
				else
				{
					if (item.res.data) {
						_m_size readsize = len;
						if (offset > item.res.size) {
							mx.unlock();
							return res;
						}
						else if (offset + len > item.res.size)
							readsize = item.res.size - offset;
						item.res.data += offset;
						item.res.size = readsize;
						res = Deciphering(item.res, reskey);
						if (retlen)
							*retlen = readsize;
					}
				}
				break;
			}
		}
		mx.unlock();
		return res;
	}

	_m_size DMResources::ReadResourceSize(std::wstring resname, ResType type)
	{
		std::lock_guard<std::mutex> lock(mx);
		for (auto item : dmResList)
		{
			if (item.resname == resname)
			{
				if (m_files)
				{
					if (type == DataRes)
						return item.resPos.second;
					else if (type == StringRes && item.strPos.second)
						return (_m_size)((double)item.strPos.second / (double)sizeof(char16_t));
				}
				else
				{
					if (type == DataRes)
						return item.res.size;
					else if (type == StringRes && item.text.size)
						return (_m_size)((double)item.text.size / (double)sizeof(char16_t));
				}
			}
		}
		return 0;
	}

	bool DMResources::ReadResBlockInfo(std::wstring resname, _m_size* blocksize)
	{
		for (auto item : dmResList)
		{
			if (item.resname == resname)
			{
				if (blocksize)
					*blocksize = item.blockSize;
				return item.block;
			}
		}
		return false;
	}

	bool DMResources::EnumResourceName(std::vector<std::wstring>& nameList)
	{
		nameList.clear();
		if (!m_resource.data)
			return false;
		mx.lock();
		for (auto item : dmResList)
		{
			nameList.push_back(item.resname);
		}
		mx.unlock();
		return true;
	}

	_m_lpcwstr DMResources::GetResClassName()
	{
		return m_resheader.classname.c_str();
	}


	bool DMResources::CreateResource(std::wstring classname)
	{
		if (classname.length() > clsnameMax)
			return false;
		if (!m_resource.data && !m_resource.size)
		{
			m_resheader.classname = classname;
			m_resource.data = (_m_byte*)1;
			return true;
		}
		return false;
	}

	bool DMResources::AddResource(DMResKey res, std::wstring resname, std::wstring reskey, DMEncBlockCallback callback)
	{
		if (!m_resource.data)
			return false;
		DMResItem item;
		item.resname = resname;

		if (res.res.data && res.res.size)
		{
			if (res.block) 
			{
				if (callback.callback) 
				{
					if (!res.resText.empty())
						item.text = EncipheringText(res.resText, reskey);
					mx.lock();
					dmResList.push_back(item);
					mx.unlock();
					EncBlockResource(res.res, resname, reskey, (_m_uint)res.blockSize, callback);
					return true;
				} else
					item.res = EncBlockResource(res.res, resname, reskey, (_m_uint)res.blockSize, callback);
			}
			else
				item.res = Enciphering(res.res, reskey);
		}
		if (!res.resText.empty())
			item.text = EncipheringText(res.resText, reskey);

		dmResList.push_back(item);
		return true;
	}

	bool DMResources::RenameResource(std::wstring resname, std::wstring newname)
	{
		if (!m_resource.data)
			return false;
		std::lock_guard<std::mutex> lock(mx);
		for (size_t i = 0; i < dmResList.size(); i++)
		{
			if (dmResList[i].resname == resname)
			{
				dmResList[i].resname = newname;
				return true;
			}
		}
		return false;
	}

	void DMResources::RenameClassName(std::wstring newname)
	{
		m_resheader.classname = newname;
	}

	bool DMResources::ChangeResource(std::wstring resname, DMResKey newres, std::wstring reskey, ResType type,
		DMEncBlockCallback callback)
	{
		if (!m_resource.data)
			return false;
		for (size_t i = 0; i < dmResList.size(); i++)
		{
			if (dmResList[i].resname == resname)
			{
				//释放原始资源
				if (type == AllRes || type == DataRes) {
					if (dmResList[i].res.data && dmResList[i].res.size)
						dmResList[i].res.Release();

					if (newres.res.data && newres.res.size) {
						dmResList[i].block = newres.block;
						if (newres.block) {
							dmResList[i].blockSize = newres.blockSize;
							dmResList[i].res = EncBlockResource(newres.res, resname, reskey, newres.blockSize, callback);
						}
						else {
							dmResList[i].blockSize = 0;
							dmResList[i].res = Enciphering(newres.res, reskey);
						}
					}
				}
				if (type == AllRes || type == StringRes) {
					if (dmResList[i].text.data && dmResList[i].text.size)
						dmResList[i].text.Release();

					if (newres.resText != L"")
						dmResList[i].text = EncipheringText(newres.resText, reskey);
				}
				return true;
			}
		}
		return false;
	}

	bool DMResources::DeleteResource(std::wstring resname)
	{
		if (!m_resource.data)
			return false;
		std::lock_guard<std::mutex> lock(mx);
		for (size_t i = 0; i < dmResList.size(); i++)
		{
			if (dmResList[i].resname == resname)
			{
				//释放资源
				if (dmResList[i].res.data && dmResList[i].res.size)
					dmResList[i].res.Release();
				if (dmResList[i].text.data && dmResList[i].text.size)
					dmResList[i].text.Release();

				dmResList.erase(dmResList.begin() + i);
				return true;
			}
		}
		return false;
	}

	bool DMResources::SaveResource(std::wstring filename)
	{
		m_resheader.rescount = (_m_uint)dmResList.size();
		m_resheader.ressign = L"DreamMoonResFile";
		m_resheader.resver = DMResVer;

		FILE* file;
#ifdef _WIN32
		_wfopen_s(&file, filename.c_str(), L"wb");
#else
#ifdef __ANDROID__
		char* path = (char *)M_WStringToString(filename).c_str();

		file = fopen(path, "wb+");
        //__android_log_print(ANDROID_LOG_ERROR, "ERRON:", "%d", errno);
#endif
#endif
		if (!file)
			return false;

		/*写文件头*/

		//写入类名长度和类名
		UIResource classname = EncipheringText(m_resheader.classname, L"DMResFile");
		_m_uint strLen = (_m_uint)classname.size;
		fwrite(&strLen, 1, sizeof(strLen), file);
		fwrite(classname.data, 1, strLen, file);
		classname.Release();
		//写入签名长度和签名
		strLen = (_m_uint)m_resheader.ressign.length();
		fwrite(&strLen, 1, sizeof(strLen), file);
#ifdef _WIN32
        fwrite(m_resheader.ressign.data(), 1, m_resheader.ressign.length() * wsize, file);
#endif
#ifdef __ANDROID__
        char16_t * str = wchar32Towchar16((wchar_t *)m_resheader.ressign.data(), m_resheader.ressign.length());
        fwrite(str, 1, m_resheader.ressign.length() * wsize, file);
        delete[] str;
#endif
		//写入版本号长度和版本号
		strLen = (_m_uint)m_resheader.resver.length();
		fwrite(&strLen, 1, sizeof(strLen), file);
#ifdef _WIN32
        fwrite(m_resheader.resver.data(), 1, m_resheader.resver.length() * wsize, file);
#endif
#ifdef __ANDROID__
        str = wchar32Towchar16((wchar_t *)m_resheader.resver.data(), m_resheader.resver.length());
        fwrite(str, 1, m_resheader.resver.length() * wsize, file);
        delete[] str;
#endif
		//写入资源数量
		fwrite(&m_resheader.rescount, 1, sizeof(m_resheader.rescount), file);

		/*写资源列表数据*/
		for (auto item : dmResList)
		{
			//写资源长度和资源
			_m_uint size = (_m_uint)item.res.size;
			fwrite(&size, 1, sizeof(size), file);
			if (size != 0)
				fwrite(item.res.data, 1, item.res.size, file);
			//写文本数据长度和数据
			size = (_m_uint)item.text.size;
			fwrite(&size, 1, sizeof(size), file);
			if (size != 0)
				fwrite(item.text.data, 1, item.text.size, file);
			//写资源名文本长度
			size = (_m_uint)item.resname.length();
			fwrite(&size, 1, sizeof(size), file);
			if (size != 0) {
#ifdef _WIN32
                fwrite(item.resname.data(), 1, size * wsize, file);
#endif
#ifdef __ANDROID__
                str = wchar32Towchar16((wchar_t *)item.resname.data(), strLen);
                fwrite(str, 1, strLen * wsize, file);
                delete[] str;
#endif
            }
			//写块资源长度和数据
			size = item.blockSize;
			fwrite(&size, 1, sizeof(size), file);
		}

		if (fclose(file) == 0)
			return true;

		return false;
	}

	bool DMResources::LoadResList(bool files)
	{
		//当前读取内存位置
		_m_ulong curMemPos = 0;
		//读取类名二进制字符串长度
		_m_uint strLen = 0;
		if (files) {
			fseek(m_files, 0, SEEK_SET);
			fread(&strLen, 1, sizeof(_m_uint), m_files);
		} else
			memcpy(&strLen, m_resource.data, sizeof(_m_uint));
		if (strLen > clsnameMax)
			return false;
		//读取类名
		char16_t* str = (char16_t*)new char16_t[strLen + 1];
		curMemPos = sizeof(_m_uint);//一个UINT数据记录二进制字节长度
		if (files) {
			fseek(m_files, curMemPos, SEEK_SET);
			fread(str, 1, strLen * wsize, m_files);
		} else
			memcpy(str, m_resource.data + curMemPos, strLen * wsize);
		str[strLen] = '\0';//添加字符串结尾
		m_resheader.classname = DecipheringText({ (_m_byte*)str, strLen }, L"DMResFile");
		delete[] str;
		//读取签名长度
		curMemPos += strLen;//原位置加上原数据长度 后面是存签名长度的UINT类型
		strLen = 0;
		if (files) {
			fseek(m_files, curMemPos, SEEK_SET);
			fread(&strLen, 1, sizeof(_m_uint), m_files);
		}
		else
			memcpy(&strLen, m_resource.data + curMemPos, sizeof(_m_uint));
		//读取签名
		str = new char16_t[strLen + 1];
		curMemPos += sizeof(_m_uint);//过UINT数据后是签名字符串数据
		if (files) {
			fseek(m_files, curMemPos, SEEK_SET);
			fread(str, 1, strLen * wsize, m_files);
		}
		else
			memcpy(str, m_resource.data + curMemPos, strLen * wsize);
		str[strLen] = '\0';//添加字符串结尾
		m_resheader.ressign = wchar16Towchar32(str, strLen);
		delete[] str;
		//读取版本号长度
		curMemPos += strLen * wsize;//上一个位置加上字符串数据长度 后面是存版本号长度的UINT类型
		strLen = 0;
		if (files) {
			fseek(m_files, curMemPos, SEEK_SET);
			fread(&strLen, 1, sizeof(_m_uint), m_files);
		}
		else
			memcpy(&strLen, m_resource.data + curMemPos, sizeof(_m_uint));
		//读取版本号
		str = new char16_t[strLen * wsize];
		curMemPos += sizeof(_m_uint);//过UINT数据后是签名字符串数据
		if (files) {
			fseek(m_files, curMemPos, SEEK_SET);
			fread(str, 1, strLen * wsize, m_files);
		}
		else
			memcpy(str, m_resource.data + curMemPos, strLen * wsize);
		str[strLen] = '\0';//添加字符串结尾
		m_resheader.resver = wchar16Towchar32(str, strLen);
		delete[] str;
		//读取资源数量
		curMemPos += strLen * wsize;
		strLen = 0;
		if (files) {
			fseek(m_files, curMemPos, SEEK_SET);
			fread(&strLen, 1, sizeof(_m_uint), m_files);
		}
		else
			memcpy(&strLen, m_resource.data + curMemPos, sizeof(_m_uint));
		m_resheader.rescount = strLen;

		if (m_resheader.classname == L""
			|| !m_resheader.rescount || m_resheader.ressign == L""
			|| m_resheader.resver == L"")
			return false;
		else {
			/*读取资源列表数据*/
			_m_uint dataLen = 0;
			curMemPos += sizeof(_m_uint);//过资源数量后 地址是资源列表的开始
			for (size_t i = 0; i < m_resheader.rescount; i++)
			{
				/*数据资源*/
				UIResource data;
				DMResItem item;
				//读取资源数据长度
				dataLen = 0;
				if (files)
				{
					fseek(m_files, curMemPos, SEEK_SET);
					fread(&dataLen, 1, sizeof(_m_uint), m_files);
				} else
					memcpy(&dataLen, m_resource.data + curMemPos, sizeof(_m_uint));
				curMemPos += sizeof(_m_uint);//过数据长度 地址是数据开始位置
				data.size = dataLen;
				if (dataLen != 0 && !files)
				{
					_m_byte* res = new _m_byte[dataLen];
					//读取数据
					memcpy(res, m_resource.data + curMemPos, data.size);
					data.data = res;
				}
				item.res = data;
				item.resPos = std::make_pair(curMemPos, (_m_uint)data.size);

				/*文本资源*/
				curMemPos += (_m_ulong)data.size;//文本数据长度地址
				dataLen = 0;
				if (files)
				{
					fseek(m_files, curMemPos, SEEK_SET);
					fread(&dataLen, 1, sizeof(_m_uint), m_files);
				}
				else
					memcpy(&dataLen, m_resource.data + curMemPos, sizeof(_m_uint));
				curMemPos += sizeof(_m_uint);
				UIResource dataText;
				dataText.size = dataLen;
				if (dataLen != 0 && !files)
				{
					_m_byte* res = new _m_byte[dataLen];
					//读取数据
					memcpy(res, m_resource.data + curMemPos, dataText.size);
					dataText.data = res;
				}
				item.text = dataText;
				item.strPos = std::make_pair(curMemPos, (_m_uint)dataText.size);

				/*资源名*/
				curMemPos += (_m_ulong)dataText.size;

				dataLen = 0;
				if (files) {
					fseek(m_files, curMemPos, SEEK_SET);
					fread(&dataLen, 1, sizeof(_m_uint), m_files);
				} else
					memcpy(&dataLen, m_resource.data + curMemPos, sizeof(_m_uint));
				curMemPos += sizeof(_m_uint);
				if (dataLen != 0)
				{
					char16_t* str = new char16_t[dataLen + 1];
					if (files) {
						fseek(m_files, curMemPos, SEEK_SET);
						fread(str, 1, dataLen * wsize, m_files);
					} else
						memcpy(str, m_resource.data + curMemPos, dataLen * wsize);
					str[dataLen] = '\0';//添加字符串结尾
					item.resname = wchar16Towchar32(str, dataLen);
					delete[] str;
				}
				curMemPos += dataLen * wsize;//过文本数据长度后是块尺寸

				/*块信息*/
				dataLen = 0;
				if (files) {
					fseek(m_files, curMemPos, SEEK_SET);
					fread(&dataLen, 1, sizeof(_m_uint), m_files);
				}
				else
					memcpy(&dataLen, m_resource.data + curMemPos, sizeof(_m_uint));

				if (dataLen) {
					item.block = true;
					item.blockSize = dataLen;
				}

				dmResList.push_back(item);
				curMemPos += sizeof(_m_uint);
			}
			if(!files)
				free(m_resource.data);
			m_resource.data = (_m_byte*)1;
			return true;
		}

		return false;
	}

	bool DMResources::CloseResource()
	{
		if (m_files) {
			fclose(m_files);
			m_files = nullptr;
		}
		for (size_t i = 0; i < dmResList.size(); i++)
		{
			if (dmResList[i].res.data)
				dmResList[i].res.Release();
			if (dmResList[i].text.data)
				dmResList[i].text.Release();
		}
		dmResList.clear();
		m_resheader.classname = L"";
		m_resheader.rescount = 0;
		m_resheader.ressign = L"";
		m_resheader.resver = L"";
		m_resource = { 0, 0 };
		return true;
	}

	UIResource DMResources::ReadFiles(std::wstring filepath, bool string)
	{
		FILE* file;
#ifdef _WIN32
		_wfopen_s(&file, filepath.c_str(), L"rb");
#else
#ifdef __ANDROID__
		file = fopen((char*)M_WStringToString(filepath).c_str(), "rb");
#endif
#endif
		if (!file)
			return UIResource();
		fseek(file, 0L, SEEK_END);
		_m_ulong len = ftell(file);
		rewind(file);
		//读文本长度+1
		_m_byte* data = (_m_byte*)malloc(string ? len + 1 : len);
		//如果读取文本 结尾添加\0
		if (string)
			data[len] = '\0';
		len = (_m_ulong)fread(data, 1, len, file);
		fclose(file);
		UIResource buffer = { data, len };

		return buffer;
	}

	bool DMResources::WriteFiles(std::wstring filepath, UIResource res)
	{
		_m_ulong fSize = (_m_ulong)res.size;
		FILE* file;
#ifdef _WIN32
		_wfopen_s(&file, filepath.c_str(), L"wb");
#else
#ifdef __ANDROID__
		file = fopen((char*)M_WStringToString(filepath).c_str(), "wb");
#endif
#endif
		if (!file)
			return false;
		_m_ulong len = (_m_ulong)fwrite(res.data, 1, fSize, file);
		fclose(file);
		return len == fSize;
	}

	_m_byte* DMResources::HexStringToBytes(std::string str)
	{
		auto charToByte = [](char c) -> _m_byte { //bug abcdef会被转化成0 因为输入的sha256是大写 将错就错了 不改了
			return (_m_byte)static_cast<std::string>("0123456789abcdef").find(c);
		};

		int length = (int)str.length() / 2;
		_m_byte* b = new _m_byte[length];
		for (int i = 0; i < length; i++) {
			int pos = i * 2;
			b[i] = (_m_byte)(charToByte(str[pos]) << 4 | charToByte(str[pos + 1]));
		}
		return b;
	}

	std::string DMResources::Sha256StringHex(std::wstring str)
	{
		auto achar = Helper::M_WStringToString(str);
#ifdef DMRES_USE_CRYPTOPP
		CryptoPP::SHA256 hash;
		byte digest[CryptoPP::SHA256::DIGESTSIZE];

		hash.CalculateDigest(digest, (byte*)achar.c_str(), (int)str.length());

		CryptoPP::HexEncoder encoder;
		std::string output;
		encoder.Attach(new CryptoPP::StringSink(output));
		encoder.Put(digest, sizeof(digest));
		encoder.MessageEnd();
		return output;
#else
		std::string hash_hex_str;
		picosha2::hash256_hex_string(achar, hash_hex_str);
		std::transform(hash_hex_str.begin(), hash_hex_str.end(), hash_hex_str.begin(), ::toupper);
		return hash_hex_str;
#endif
	}

	std::wstring DMResources::wchar16Towchar32(char16_t* wchar16, _m_uint len) {
#ifdef _WIN32
		return std::wstring((wchar_t*)wchar16);
#endif
#ifdef __ANDROID__
		wchar_t* wcs = new wchar_t [len + 1];
		int u32;
		unsigned short* end = (u_short*)(wchar16 + len);
		wchar_t* ret = wcs;
		int u32low = 0;
		while ((u_short*)(wchar16) < end)
		{
			u32 = *wchar16++;
			if (u32 >= 0xD800 && u32 < 0xDC00)
			{
				u32low = *wchar16++;
				u32 &= 0x3FF;
				u32low &= 0x3FF;
				u32 <<= 10;
				u32 += u32low;
				u32 += 0x10000;
			}
			*wcs++ = u32;
		}
		ret[len] = L'\0';
		std::wstring ret1 = ret;
		delete[] ret;
		return ret1;
#endif
	}

    char16_t * DMResources::wchar32Towchar16(wchar_t * wchar32, _m_uint len)
    {
		int wc;
		char16_t * utf16 = new char16_t [len];
		wchar_t *end = wchar32 + len;
		unsigned short * ret = (unsigned short *)utf16;
		while(wchar32 < end)
		{
			wc = *(wchar32++);
			if (wc > 0xFFFF)
			{
				wc -= 0x00010000L;
				*utf16++ = 0xD800 | (wc >> 10);
				*utf16++ = 0xDC00 | (wc & 0x03FF);
			}
			else
			{
				*utf16++ = (char16_t)wc;
			}
		}
		//ret[len] = '\0';
		return reinterpret_cast<char16_t *>(ret);
    }

	UIResource DMResources::Enciphering(UIResource res, std::wstring key)
	{
		_m_byte * key_ = HexStringToBytes(Sha256StringHex(key));
		_m_byte* iv_ = HexStringToBytes(Sha256StringHex(key).substr(0, 32));

		_m_byte* pOut = new _m_byte[res.size];

#ifdef DMRES_USE_CRYPTOPP
		CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryption(key_, CryptoPP::AES::MAX_KEYLENGTH, iv_);
		encryption.ProcessData(pOut, res.data, res.size);
		res.data = pOut;
#else
		AES256_CFB(res, pOut, key_, iv_, true);
		res.data = pOut;
#endif
		delete[] key_;
		delete[] iv_;
		return res;
	}

	UIResource DMResources::EncipheringText(std::wstring text, std::wstring key)
	{
		UIResource res;
#ifdef _WIN32
        res.data = (_m_byte*)text.data();
#endif
#ifdef __ANDROID__
        char16_t * str = wchar32Towchar16((wchar_t *)text.data(), text.length());
        res.data = (_m_byte*)str;
#endif
		res.size = (_m_size)text.size() * wsize;
		res = Enciphering(res, key);
#ifdef __ANDROID__
        delete[] str;
#endif
		return res;
	}

	UIResource DMResources::Deciphering(UIResource res, std::wstring key)
	{
		_m_byte* key_ = HexStringToBytes(Sha256StringHex(key));
		_m_byte* iv_ = HexStringToBytes(Sha256StringHex(key).substr(0, 32));

		_m_byte* pOut = new _m_byte[res.size];

#ifdef DMRES_USE_CRYPTOPP
		CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decryption(key_, CryptoPP::AES::MAX_KEYLENGTH, iv_);
		decryption.ProcessData(pOut, res.data, res.size);
		res.data = pOut;
#else
		AES256_CFB(res, pOut, key_, iv_, false);
		res.data = pOut;
#endif
		delete[] key_;
		delete[] iv_;
		return res;
	}

	std::wstring DMResources::DecipheringText(UIResource res, std::wstring key)
	{
		res.size += wsize;
		UIResource out = Deciphering(res, key);
		for (size_t i = 0; i < wsize; i++)
		{
			out.data[out.size - 1 - i] = '\0';
		}

		std::wstring ret = wchar16Towchar32((char16_t*)out.data, _m_uint(res.size / 2));
		ret = ret;
		out.Release();
		return ret;
	}

	void DMResources::EncBlockProc(UIResource res, std::wstring resname, std::wstring key, _m_uint blocksize, UIResource* ret, DMEncBlockCallback callback)
	{
		_m_byte* buffer = new _m_byte[res.size];
		_m_size ensize = 0;//已编码资源
		_m_ulong offset = 0;//偏移指针
		while (ensize != res.size)
		{
			//要写入内存的大小
			_m_size writeSize = blocksize;
			//如果剩余数据小于块大小 将块大小设置为剩余数据大小
			if (offset + blocksize > res.size)
				writeSize = res.size - offset;

			UIResource block;
			block.data = res.data + offset;
			block.size = writeSize;

			UIResource encData = Enciphering(block, key);
			memcpy(buffer + offset, encData.data, writeSize);
			encData.Release();

			offset += blocksize;
			ensize += writeSize;
			if (callback.callback)
				callback.callback(ensize, res.size, callback.param);
		}
		if (callback.callback)
		{
			for (size_t i = 0; i < dmResList.size(); i++)
			{
				DMResItem* item = &dmResList[i];
				if (item->resname == resname)
				{
					if (item->res.data)
					item->res.Release();
					item->res.data = buffer;
					item->res.size = res.size;
					return;
				}
			}
			delete[] buffer;
		}
		else if (ret)
			*ret = UIResource(buffer, res.size);
		else
			delete[] buffer;
	}

	UIResource DMResources::EncBlockResource(UIResource res, std::wstring resname, std::wstring key, _m_uint blocksize, DMEncBlockCallback callback)
	{
		if (blocksize > res.size || blocksize == 0)
			return Enciphering(res, key);
		else
		{
			std::thread encthread = std::thread(&DMResources::EncBlockProc, this, res, resname, key, blocksize, &res, callback);
			if(callback.callback)
				encthread.detach();
			else {
				encthread.join();
				return res;
			}
		}
		return UIResource();
	}
}
#endif