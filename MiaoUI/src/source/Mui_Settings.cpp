/**
 * FileName: Mui_Settings.h
 * Note: MiaoUI 附加设置实现
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
 * date: 2022-1-23 Create
*/
#include <Mui_Settings.h>
#include <Mui_Base.h>

namespace Mui::Settings
{
	_m_lpcwstr MuiLogoText = LR"(
___  ___ _                _   _  _____ 
| .\/. |(_)  __ _   ___  | | | ||_   _|
| |\/| || | / _` | / _ \ | | | |  | |  
| |  | || || (_| || (_) || |_| | _| |_ 
\_|  |_/|_| \__,_| \___/  \___/  \___/
)";

		//UI引擎版本号
#ifdef _WIN32
	_m_lpcwstr MuiEngineVer = L"v2.0.8 (Lite)";
#endif // _WIN32

	void UIMessageLoop()
	{
#ifdef _WIN32

		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

#endif // _WIN32
	}

	std::unordered_map<ExSection, ExFlag> g_exflagList;
	std::mutex g_exflagLock;
	void SetExtensionFlag(ExSection section, _m_param flag, bool override)
	{
		std::lock_guard lock{ g_exflagLock };
		g_exflagList[section] = { flag, override };
	}

	void ResExtensionFlag(ExSection section)
	{
		std::lock_guard lock{ g_exflagLock };
		if (auto iter = g_exflagList.find(section); iter != g_exflagList.end())
			g_exflagList.erase(iter);
	}

	bool GetExtensionFlag(ExSection section, std::pair<_m_param, bool>& param)
	{
		std::lock_guard lock{ g_exflagLock };
		if (const auto iter = g_exflagList.find(section); iter != g_exflagList.end())
		{
			param = iter->second;
			return true;
		}
		return false;
	}
};