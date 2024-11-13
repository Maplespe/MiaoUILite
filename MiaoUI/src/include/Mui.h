/**
 * FileName: Mui.h
 * Note: MiaoUI Lite library 声明
 * 
 * ___  ___ _                _   _  _____ 
 * | .\/. |(_)  __ _   ___  | | | ||_   _|
 * | |\/| || | / _` | / _ \ | | | |  | |  
 * | |  | || || (_| || (_) || |_| | _| |_ 
 * \_|  |_/|_| \__,_| \___/  \___/  \___/ 
 *
 * Copyright (C) 2020-2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2020-10-17 Create
*/

#pragma once
//界面助手
#include <Mui_DefUIStyle.h>
//界面库扩展设置
#include <Mui_Settings.h>
//控件
#include <Control/Mui_Button.h>
#include <Control/Mui_CheckBox.h>
#include <Control/Mui_ComBox.h>
#include <Control/Mui_EditBox.h>
#include <Control/Mui_ImgBox.h>
#include <Control/Mui_Label.h>
#include <Control/Mui_ListBox.h>
#include <Control/Mui_Progress.h>
#include <Control/Mui_Scroll.h>
#include <Control/Mui_Slider.h>
#include <Control/Mui_ColorPicker.h>
#include <Control/Mui_NavBar.h>

//控件管理器 用于注册和通过类名创建控件
#include <Manager/Mui_ControlMgr.h>

#include <Window/Mui_Windows.h>
#include <Render/Graphs/Mui_GdipRender.h>

//用户助手类
#include <User/Mui_Engine.h>
//文件系统
#include <FileSystem/Mui_FileSystem.h>

//Windows库引入
#ifdef _DEBUG
#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "MiaoUILite64d.lib")
#else
#pragma comment(lib, "MiaoUILite32d.lib")
#endif //_WIN64
#endif //_WIN32

#else
#ifdef _WIN32
#ifdef _WIN64
#pragma comment(lib, "MiaoUILite64.lib")
#else
#pragma comment(lib, "MiaoUILite32.lib")
#endif //_WIN64
#endif //_WIN32
#endif // DEBUG