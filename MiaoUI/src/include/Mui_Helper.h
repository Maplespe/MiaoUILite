/**
 * FileName: Mui_Helper.h
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
#pragma once
#include <Mui_Base.h>

namespace Mui::Helper
{
	//将2个16位数合成为1个32位数
	inline _m_long M_MAKELONG(_m_word a, _m_word b)
	{
		return _m_long(_m_ushort(a & 0xffff) | _m_ulong(_m_ushort(b & 0xffff)) << 16);
	}

	//高低位
	inline _m_word M_LOWORD(_m_long l)
	{
		return _m_word(_m_uint(l) & 0xffff);
	}

	inline _m_word M_HIWORD(_m_long l)
	{
		return _m_word(_m_uint(l) >> 16) & 0xffff;
	}

	inline UIPoint M_GetMouseEventPt(_m_param lParam)
	{
		UIPoint pt;
		pt.x = static_cast<int>((short)M_LOWORD((_m_long)lParam));
		pt.y = static_cast<int>((short)M_HIWORD((_m_long)lParam));
		return pt;
	}

	template <typename T>
	T M_MAX(T a, T b)
	{
		return a > b ? a : b;
	}

	template <typename T>
	T M_MIN(T a, T b)
	{
		return a < b ? a : b;
	}

	template <typename T>
	T M_Clamp(T min, T max, T value)
	{
		if (value > max)
			return max;
		if (value < min)
			return min;
		return value;
	}

	//检查小数是否相等
	template <typename T>
	bool M_DecimalEquals(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
	{
		return std::fabs(a - b) < epsilon;
	}

	template <typename T, std::size_t N1, std::size_t N2, std::size_t... I1, std::size_t... I2>
	constexpr std::array<T, N1 + N2> MergeArrayImpl(const std::array<T, N1>& a, const std::array<T, N2>& b,
		std::index_sequence<I1...>, std::index_sequence<I2...>)
	{
		return { a[I1]..., b[I2]... };
	}

	template <typename T, std::size_t N>
	constexpr std::array<T, N> MergeArrays(const std::array<T, N>& a)
	{
		return a;
	}

	//合并数组(std::array)
	template <typename T, std::size_t N1, std::size_t N2>
	constexpr std::array<T, N1 + N2> MergeArrays(const std::array<T, N1>& a, const std::array<T, N2>& b)
	{
		return MergeArrayImpl(a, b, std::make_index_sequence<N1>{}, std::make_index_sequence<N2>{});
	}

	//合并数组(std::array)
	template <typename T, std::size_t N1, std::size_t N2, typename... Rest>
	constexpr auto MergeArrays(const std::array<T, N1>& a, const std::array<T, N2>& b, const Rest&... rest)
	{
		return MergeArrays(MergeArrays(a, b), rest...);
	}

	template <class T, size_t Size, size_t... Idx>
	constexpr std::array<std::remove_cv_t<T>, Size> ToArrayImpl(T(&& Array)[Size], std::index_sequence<Idx...>)
	{
		return { {std::move(Array[Idx])...} };
	}

	//C++20 std::to_array替代
	template <class T, size_t Size>
	constexpr std::array<std::remove_cv_t<T>, Size> ToArray(T(&& Array)[Size])
	{
		return ToArrayImpl(std::move(Array), std::make_index_sequence<Size>{});
	}

	//Array转Vector
	template <typename T, std::size_t Size>
	constexpr std::vector<T> ArrayToVector(const std::array<T, Size>& a)
	{
		std::vector<T> vec(Size);
		for (size_t i = 0; i < Size; ++i)
		{
			vec[i] = a[i];
		}
		return vec;
	}

	/*计算长宽比
	* @param fromWidth - 原始宽度
	* @param fromHeight - 原始高度
	* @param toWidthOrHeight - 目标宽度或高度
	*
	* @return 为true则返回结果为高度，否则为宽度
	*/
	extern int M_CalcAspectRatio(int fromWidth, int fromHeight, int toWidthOrHeight, bool isWidth);

	/*-------------字符串操作相关函数-------------*/

	//wstring 转 string
	extern std::string M_WStringToString(std::wstring_view width);

	//string 转 wstring
	extern std::wstring M_StringToWString(std::string_view str);

	/*替换字符串
	* @param str - 源字符串
	* @param old_value - 欲替换文本
	* @param new_value - 用作替换的文本
	*
	* @return 替换后的文本内容
	*/
	extern std::wstring M_ReplaceString(std::wstring str, std::wstring_view old_value, std::wstring_view new_value);

	/*取文本出现次数
	* @param src - 原始字符串
	* @param text - 欲查找的文本
	*
	* @return 指定文本在原始字符串中的出现次数
	*/
	extern _m_uint M_GetTextCount(std::wstring_view src, std::wstring_view text);

	/*取文本指定行内容
	* @param src - 原始字符串
	* @param line - 欲取的文本行 从1开始
	*
	* @return 指定行文本内容
	*/
	extern std::wstring M_GetTextLine(std::wstring_view src, _m_uint line);

	//文本转数字
	//@exception MErrorCode::InvalidParameter
	extern _m_int M_StoInt(std::wstring_view str);
	extern _m_long M_StoLong(std::wstring_view str);
	extern _m_long64 M_StoLong64(std::wstring_view str);
	extern _m_ulong M_StoULong(std::wstring_view str);
	extern _m_ulong64 M_StoULong64(std::wstring_view str);
	extern float M_StoFloat(std::wstring_view str);
	extern double M_StoDouble(std::wstring_view str);

	/*取属性值 x,x,x,x
	* @param value - 值
	* @param out destValue - 输出值
	* @param count - 参数数量
	*/
	extern void M_GetAttribValue(std::wstring_view value, std::vector<std::wstring>& destValue, _m_uint count);

	/*取属性值Int x,x,x,x
	* @param value - 值
	* @param out destValue - 输出值
	* @param count - 参数数量
	* @exception MErrorCode::InvalidParameter
	*/
	extern void M_GetAttribValueInt(std::wstring_view value, std::vector<int>& destValue, _m_uint count);

	/*取属性值RGBA
	* @param value - 颜色值 例如 RGBA、RGB、HEX 255,255,255,255、\@hex:FFFFFFFF 不带\\ 这边是为了注释在IDE正常显示
	*
	* @return 颜色值
	* @exception MErrorCode::InvalidParameter
	*/
	extern _m_color M_GetAttribValueColor(std::wstring_view value);

	//矩形操作相关函数
	namespace Rect
	{
		//判断矩形是否相交
		extern bool IsCross(const _m_rect& rect1, const _m_rect& rect2);

		//判断点是否在矩形内
		extern bool IsPtInside(const _m_rect& rect, const UIPoint& point);

		/*偏移矩形
		* @param rect - 目标矩形
		* @param dx - X坐标移动
		* @param dy - Y坐标移动
		*/
		extern void Offset(_m_rect* rect, int dx, int dy);

		/*扩展矩形
		* @param rect - 目标矩形
		* @param dx - 扩展宽度
		* @param dy - 扩展高度
		*/
		extern void Inflate(_m_rect* rect, int dx, int dy);

		/*联合矩形
		* @param dst - 目标矩形
		* @param rect1 - 联合1
		* @param rect2 - 联合2
		*/
		extern void Union(_m_rect* dst, const _m_rect* rect1, const _m_rect* rect2);

		/*相交矩形
		* @param dst - 目标矩形
		* @param rect1 - 相交1
		* @param rect2 - 相交2
		*/
		extern void Intersect(_m_rect* dst, const _m_rect* rect1, const _m_rect* rect2);
	}

	//延时 毫秒级
	extern void M_Sleep(_m_uint ms);

}