/**
 * FileName: Mui_Config.h
 * Note: MiaoUI库编译配置选项
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
 * date: 2023-7-12 Create
*/
#pragma once

#ifndef __MUI__CONFIG__
#define __MUI__CONFIG__

/*文件系统*/

//是否启用文件系统 DmResFile v1版本
#define MUI_CFG_ENABLE_V1DMRES 1

//是否启用 DmResFile v1的硬件加速 需要使用Cryptopp库
#if (MUI_CFG_ENABLE_V1DMRES)
#define MUI_CFG_ENABLE_CRYPTOPP 0
#endif

#if (!MUI_CFG_ENABLE_V1DMRES && !MUI_CFG_ENABLE_V2DMRES)
#error "没有可用的文件系统支持!"
#endif

/*-------*/

/*UI*/
//是否启用控件默认样式 仅对MUI默认控件有效
#define MUI_MXML_ENABLE_DEFSTYLE 1

//当MXML创建控件时遇到未知控件是否抛出异常
#define MUI_MXML_THROW_UNKNOWCTRL 0
/*-------*/

/*Debug*/

//是否启用调试源信息
//指定使用M_OutError函数抛出错误时 错误信息是否包含源文件名
//此选项将导致编译的二进制文件中包含完整的源文件路径字符串信息 这取决于编译器
#define MUI_CFG_ENABLE_DBGSOURCE 0

//指定使用M_OutError函数抛出错误时 错误信息是否包含源函数名
#define MUI_CFG_ENABLE_DBGFUNNAME 1

#endif //__MUI__CONFIG__