/**
 * FileName: Mui_Framework.h
 * Note: 基础库导入
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
 * date: 2024-1-24 Create
*/
#pragma once
//Windows SDK
#ifdef _WIN32
#include <sdkddkver.h>
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // _WIN32

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <unordered_map>
#include <deque>
#include <memory>
#include <exception>
#include <future>