/**
 * FileName: Mui_Helper.cpp
 * Note: 界面库助手函数
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
 * date: 2023-5-11 Create
*/
#include <Mui_Helper.h>

#include <Render/Graphs/Mui_Render.h>

#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#endif // _WIN32

namespace Mui::Helper
{
	int M_CalcAspectRatio(int fromWidth, int fromHeight, int toWidthOrHeight, bool isWidth)
	{
		if (isWidth)
			return (int)round((float)fromHeight * ((float)toWidthOrHeight / (float)fromWidth));
		return (int)round((float)fromWidth * ((float)toWidthOrHeight / (float)fromHeight));
	}

	std::string M_WStringToString(std::wstring_view width)
	{
#ifdef _WIN32
		int len = WideCharToMultiByte(CP_ACP, 0, width.data(), (int)width.size(), NULL, 0, NULL, NULL);
		char* buffer = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, width.data(), (int)width.size(), buffer, len, NULL, NULL);
		buffer[len] = '\0';
		std::string ret = buffer;
		delete[] buffer;
		return ret;
#elif __ANDROID__
		auto len = width.length();
		char* buffer = new char[len + 1];
		wcstombs(buffer, width.data(), len);
		buffer[len] = '\0';
		std::string ret = buffer;
		ret = ret.substr(0, len);
		delete[] buffer;
		return ret;
#else
#error __TODO__
		return std::string();
#endif // _WIN32
	}

	std::wstring M_StringToWString(std::string_view str)
	{
#ifdef _WIN32
		int len = MultiByteToWideChar(CP_ACP, 0, str.data(), (int)str.size(), NULL, 0);
		wchar_t* buffer = new wchar_t[len + 1];
		MultiByteToWideChar(CP_ACP, 0, str.data(), (int)str.size(), buffer, len);
		buffer[len] = '\0';
		std::wstring ret = buffer;
		delete[] buffer;
		return ret;
#elif __ANDROID__
		auto len = str.length();
		wchar_t* buffer = new wchar_t[len + 1];
		mbstowcs(buffer, str.data(), len);
		buffer[len] = L'\0';
		std::wstring ret = buffer;
		delete[] buffer;
		return ret;
#else
#error __TODO__
		return std::wstring();
#endif // _WIN32
	}

	std::wstring M_ReplaceString(std::wstring str, std::wstring_view old_value, std::wstring_view new_value)
	{
		if (old_value.empty() || str.empty())
			return str;
		size_t cur = 0;
		while (true) 
		{
			if (std::wstring::size_type pos; (pos = str.find(old_value, cur)) != std::wstring::npos) 
			{
				str.replace(pos, old_value.length(), new_value);
				cur = pos + 1;
			}
			else
				break;
		}
		return str;
	}

	_m_uint M_GetTextCount(std::wstring_view src, std::wstring_view text)
	{
		if (size_t pos = src.find(text); pos != std::wstring::npos)
		{
			int couts = 0;
			couts++;
			while (src.find(text, pos + 1) != std::wstring::npos)
			{
				pos = src.find(text, pos + 1);
				couts++;
			}
			return couts;
		}
		return 0;
	}

	std::wstring M_GetTextLine(std::wstring_view src, _m_uint line)
	{
		if (line == 0) return L"";

		size_t start = 0;
		size_t end = src.find(L'\n');

		std::wstring ret;

		size_t i = 1;
		while (end != std::wstring::npos) 
		{
			if (line == i)
				return std::wstring(src.substr(start, end - start));

			start = end + 1;
			end = src.find(L'\n', start);
			i++;
		}
		if (i == line)
			return std::wstring(src.substr(start, end - start));

		return L"";
	}

#define M_STOX(x) try { ret = x; } catch(const std::exception& ex) { MErrorThrow(ex.what()); }

	_m_int M_StoInt(std::wstring_view str)
	{
		if (str.empty()) return 0;
		_m_int ret = 0;
		M_STOX(std::stoi(str.data()))
		return ret;
	}

	_m_long M_StoLong(std::wstring_view str)
	{
		if (str.empty()) return 0l;
		_m_long ret = 0;
		M_STOX(std::stol(str.data()))
		return ret;
	}

	_m_long64 M_StoLong64(std::wstring_view str)
	{
		if (str.empty()) return 0ll;
		_m_long64 ret = 0;
		M_STOX(std::stoll(str.data()))
		return ret;
	}

	_m_ulong M_StoULong(std::wstring_view str)
	{
		if (str.empty()) return 0u;
		unsigned long ret = 0;
		M_STOX(std::stoul(str.data()))
		return (_m_ulong)ret;
	}

	_m_ulong64 M_StoULong64(std::wstring_view str)
	{
		if (str.empty()) return 0ull;
		_m_ulong64 ret = 0;
		M_STOX(std::stoull(str.data()))
		return ret;
	}

	float M_StoFloat(std::wstring_view str)
	{
		if (str.empty()) return 0.f;
		float ret = 0.f;
		M_STOX(std::stof(str.data()))
		return ret;
	}

	double M_StoDouble(std::wstring_view str)
	{
		if (str.empty()) return 0.0;
		double ret = 0.0;
		M_STOX(std::stod(str.data()))
		return ret;
	}

	void M_GetAttribValue(std::wstring_view value, std::vector<std::wstring>& destValue, _m_uint count)
	{
		//清除空格
		std::wstring dst = M_ReplaceString(value.data(), L" ", L"");
		//将分号改为换行符
		dst = M_ReplaceString(dst, L",", L"\n");
		dst += L"\n";
		//获取每行的内容
		_m_uint line = M_GetTextCount(dst, L"\n");
		for (_m_uint i = 0; i < count; i++)
		{
			if (i + 1 <= line)
				destValue.push_back(M_GetTextLine(dst, i + 1));
			else
				destValue.emplace_back(L"");
		}
	}

	void M_GetAttribValueInt(std::wstring_view value, std::vector<int>& destValue, _m_uint count)
	{
		std::vector<std::wstring> list;
		M_GetAttribValue(value, list, count);
		for (_m_uint i = 0; i < count; i++)
		{
			if (list.size() > i && !list[i].empty())
				destValue.push_back(M_StoInt(list[i]));
			else
				destValue.push_back(0);
		}
	}

	_m_color M_GetAttribValueColor(std::wstring_view value)
	{
		std::wstring dst = M_ReplaceString(value.data(), L" ", L"");
		dst = M_ReplaceString(dst, L",", L"\n");
		dst += L"\n";
		//获取每行的内容
		_m_uint line = M_GetTextCount(dst, L"\n");
		int rgba[4]{ 0 };
		for (_m_uint i = 0; i < 4; i++)
		{
			if (i + 1 <= line)
			{
				std::wstring str = M_GetTextLine(dst, i + 1);
				if (i == 0 && str.find(L"@hex:") == 0)
				{
					str = str.substr(5);
					return { str, false, str.length() == 8 };
				}
				rgba[i] = M_StoInt(str);
			}
			else
				rgba[i] = 0xFF;
		}
		return Color::M_RGBA((_m_byte)rgba[0], (_m_byte)rgba[1], (_m_byte)rgba[2], (_m_byte)rgba[3]);
	}

	namespace Rect
	{
		bool IsCross(const _m_rect& rect1, const _m_rect& rect2)
		{
			int x1 = rect1.left;
			int y1 = rect1.top;
			int x2 = rect1.left + rect1.GetWidth();
			int y2 = rect1.top + rect1.GetHeight();

			int x3 = rect2.left;
			int y3 = rect2.top;
			int x4 = rect2.left + rect2.GetWidth();
			int y4 = rect2.top + rect2.GetHeight();

			return (((x1 >= x3 && x1 < x4) || (x3 >= x1 && x3 <= x2)) &&
				((y1 >= y3 && y1 < y4) || (y3 >= y1 && y3 <= y2)));
		}

		bool IsPtInside(const _m_rect& rect, const UIPoint& point)
		{
			return (rect.left - point.x) * (rect.right - point.x) <= 0 && (rect.top - point.y) * (rect.bottom - point.y) <= 0;
		}

		void Offset(_m_rect* rect, int dx, int dy)
		{
			rect->left += dx;
			rect->right += dx;
			rect->top += dy;
			rect->bottom += dy;
		}

		void Inflate(_m_rect* rect, int dx, int dy)
		{
			rect->left -= dy;
			rect->right += dy;
			rect->top -= dy;
			rect->top += dy;
		}

		void Union(_m_rect* dst, const _m_rect* rect1, const _m_rect* rect2)
		{
			_m_rect pRet;

			long rect1_ = rect1->left;

			bool rc1 = rect1->left >= rect1->right || rect1->top >= rect1->bottom;
			bool rc2 = rect2->left >= rect2->right || rect2->top >= rect2->bottom;
			if (rc1)
			{
				if (rc2)
				{
					dst->left = 0;
					dst->top = 0;
					dst->right = 0;
					dst->bottom = 0;
					return;
				}
				pRet = *rect2;
			}
			else
			{
				if (!rc2)
				{
					long num[3];
					if (rect1_ >= rect2->left)
						rect1_ = rect2->left;
					dst->left = rect1_;
					num[0] = rect1->top;
					if (num[0] >= rect2->top)
						num[0] = rect2->top;
					dst->top = num[0];
					num[1] = rect1->right;
					if (num[1] <= rect2->right)
						num[1] = rect2->right;
					dst->right = num[1];
					num[2] = rect1->bottom;
					if (num[2] <= rect2->bottom)
						num[2] = rect2->bottom;
					dst->bottom = num[2];
					return;
				}
				pRet = *rect1;
			}
			*dst = pRet;
		}

		void Intersect(_m_rect* dst, const _m_rect* rect1, const _m_rect* rect2)
		{
			long rc1 = rect1->left;
			if (rect1->left <= rect2->left)
				rc1 = rect2->left;
			dst->left = rc1;

			long rc2 = rect1->right;
			if (rc2 >= rect2->right)
				rc2 = rect2->right;
			dst->right = rc2;

			if (rc1 < rc2)
			{
				long rc3 = rect1->top;
				if (rc3 <= rect2->top)
					rc3 = rect2->top;
				dst->top = rc3;

				long rc4 = rect1->bottom;
				if (rc4 >= rect2->bottom)
					rc4 = rect2->bottom;
				dst->bottom = rc4;
				if (rc3 < rc4)
					return;
			}
			dst->left = 0;
			dst->top = 0;
			dst->right = 0;
			dst->bottom = 0;
		}
	}

	void M_Sleep(_m_uint ms)
	{
		if (ms > 1)
			ms--;
#ifdef _WIN32
		timeBeginPeriod(1);
#endif // _WIN32
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#ifdef _WIN32
		timeEndPeriod(1);
#endif // _WIN32

	}
}