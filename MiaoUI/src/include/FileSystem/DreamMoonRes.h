/**
 * FileName: DreamMoonRes.h
 * Note: 梦月资源类声明
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
#pragma once
#include <Mui_Base.h>

#if MUI_CFG_ENABLE_V1DMRES

#define DMResVer L"v1.0.1";

namespace Mui {

	//梦月资源类 资源键类型
	struct DMResKey
	{
		//数据资源
		UIResource res;
		//块模式存储
		bool block = false;
		//分块尺寸
		unsigned long blockSize = 0;
		//文本资源
		std::wstring resText;
	};

	//资源类型
	enum ResType
	{
		AllRes,//全部资源
		DataRes,//数据资源
		StringRes//字符串资源
	};

	struct DMEncBlockCallback
	{
		std::function<void(_m_size encsize, _m_size allsize, _m_param param)> callback = nullptr;
		_m_param param = 0;
	};

	//梦月资源类
	//该类仅作为兼容和迁移旧项目使用 由于性能和安全问题 不建议再使用 请使用新的资源管理类DMResFile V2 需要在编译时定义MUI_CFG_ENABLE_V2DMRES
	class DMResources
	{
	public:
		~DMResources();

		/*资源读取*/

		/*从文件初始化资源 资源仅初始化一次 多次返回false
		* @param resfiles dmres文件路径
		* @param mem 是否使用内存加载模式 内存加载模式支持写入和修改
		*/
		bool LoadResource(std::wstring resfiles, bool mem = false);

#ifdef _WIN32
		//从WindowsPE读取资源
		static UIResource ReadPEResource(DWORD resID, LPCWSTR type);

		//从PE初始化资源 资源仅初始化一次 多次返回false
		bool LoadResource(_m_ulong resid, _m_lpcwstr restype);
#endif // _WIN32

		//从内存加载资源文件
		bool LoadResource(UIResource memres);

		//读取资源数量
		_m_size GetResourceCount();

		/*读取资源 资源需要使用Release释放
		* @param resname - 资源名称
		* @param reskey - 资源秘钥
		* @param type - 资源类型
		* 
		* @return 资源数据 如果Type=AllRes那么返回的结构体都有数据 否则仅有数据或文本
		*/
		DMResKey ReadResource(std::wstring resname, std::wstring reskey, ResType type = AllRes);

		/*读取分块数据资源
		* @param resname - 资源名称
		* @param reskey - 资源秘钥
		* @param offset - 指针偏移位置
		* @param len - 读取长度
		* @param out retlen - 返回已读取长度
		* 
		* @return 资源数据 需要使用Release释放
		*/
		UIResource ReadResourceBlock(std::wstring resname, std::wstring reskey, _m_uint offset, _m_size len, _m_size* retlen);

		/*读取数据资源尺寸
		* @param resname - 资源名称
		* @param type - 资源类型 仅DataRes或StringRes
		* 
		* @return DataRes为资源字节大小 StringRes为文本长度 AllRes返回0
		*/
		_m_size ReadResourceSize(std::wstring resname, ResType type);

		/*读取资源块信息
		* @param resname - 资源名称
		* @param out - 块尺寸
		* 
		* @return 资源是否为块模式存储
		*/
		bool ReadResBlockInfo(std::wstring resname, _m_size* blocksize);

		//枚举资源名称
		bool EnumResourceName(std::vector<std::wstring>& nameList);
		//获取类名
		_m_lpcwstr GetResClassName();

		/*资源写入*/

		//新建一个内存资源类 资源仅初始化一次 多次返回false 类名长度不能大于30
		bool CreateResource(std::wstring classname);
		//添加一个资源
		bool AddResource(DMResKey res, std::wstring resname, std::wstring reskey, DMEncBlockCallback callback = DMEncBlockCallback());
		//重命名一个资源
		bool RenameResource(std::wstring resname, std::wstring newname);
		//重命名资源类名
		void RenameClassName(std::wstring newname);
		//修改一个资源
		bool ChangeResource(std::wstring resname, DMResKey newres, std::wstring reskey,
			ResType type = AllRes, DMEncBlockCallback callback = DMEncBlockCallback());
		//删除一个资源
		bool DeleteResource(std::wstring resname);
		//保存资源文件 将所有更改写入文件
		bool SaveResource(std::wstring filename);

		//关闭资源文件 成功后可以重新初始化资源
		bool CloseResource();

		//读取文件
		UIResource ReadFiles(std::wstring filepath, bool string = false);
		//写入文件
		bool WriteFiles(std::wstring filepath, UIResource res);

	private:
		_m_byte* HexStringToBytes(std::string str);
		std::string Sha256StringHex(std::wstring str);
        std::wstring wchar16Towchar32(char16_t * wchar16, _m_uint len);
        char16_t * wchar32Towchar16(wchar_t  * wchar32, _m_uint len);
		//加密资源
		UIResource Enciphering(UIResource res, std::wstring key);
		UIResource EncipheringText(std::wstring text, std::wstring key);
		//解密资源
		UIResource Deciphering(UIResource res, std::wstring key);
		std::wstring DecipheringText(UIResource res, std::wstring key);
		//编码块资源
		void EncBlockProc(UIResource res, std::wstring resname, std::wstring key, _m_uint blocksize, UIResource* ret, DMEncBlockCallback callback);
		UIResource EncBlockResource(UIResource res, std::wstring resname, std::wstring key, _m_uint blocksize, DMEncBlockCallback callback);

		//加载资源列表
		bool LoadResList(bool file = false);

		//资源头
		struct DMResHeader
		{
			//资源类名
			std::wstring classname;
			//资源签名
			std::wstring ressign;
			//资源版本号
			std::wstring resver;
			//资源数量
			_m_uint rescount = 0;
		};

		struct DMResItem
		{
			std::wstring resname;//资源名称
			UIResource res;//被加密过的资源
			bool block = false;//块资源模式
			_m_uint blockSize = 0;//块尺寸
			UIResource text;//被加密过的文本资源
			std::pair<_m_ulong, _m_uint> resPos;//资源在文件位置的指针
			std::pair<_m_ulong, _m_uint> strPos;//文本在文件位置的指针
		};

		std::vector<DMResItem> dmResList;
		DMResHeader m_resheader;
		UIResource m_resource;
		FILE* m_files = nullptr;

		std::mutex mx;
	};

}
#endif