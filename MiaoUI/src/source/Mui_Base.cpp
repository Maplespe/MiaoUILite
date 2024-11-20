/**
 * FileName: Mui_Base.cpp
 * Note: UI基本类型定义
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
 * date: 2020-10-17 Create
*/

#include <Mui_Base.h>
#include <Mui_Helper.h>
#include <Render/Graphs/Mui_Render.h>
#include <sstream>
#include <iomanip>

using namespace std::chrono;

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace Mui
{

#pragma region _m_color

	_m_color::_m_color(std::wstring_view hex, bool _argb, bool alpha) : argb(0)
	{
		std::wstringstream ss;
		ss << std::hex << std::setw(8) << hex;
		ss >> argb;
		if (!alpha)
		{
			a = 255;
			if(!_argb) return;
		}
		if (!_argb)
		{
			std::swap(a, r);
			std::swap(b, g);
		}
		else
			std::swap(r, b);
	}

	std::wstring _m_color::HEX(bool _argb, bool alpha)
	{
		std::wstringstream ss;

		if(_argb && alpha)
			ss << std::hex << std::setw(2) << std::setfill(L'0') << a;

		ss << std::hex << std::setw(2) << std::setfill(L'0') << r
			<< std::hex << std::setw(2) << std::setfill(L'0') << g
			<< std::hex << std::setw(2) << std::setfill(L'0') << b;

		if (!_argb && alpha)
			ss << std::hex << std::setw(2) << std::setfill(L'0') << a;

		return ss.str();
	}

	_m_byte _m_color::AlphaBlend(_m_byte dst, _m_byte src)
	{
		return (_m_byte)((float)dst * (float)src / 255.f);
	}

#pragma endregion


#pragma region UIRect

	UIRect::UIRect()
	{
		left = top = right = bottom = 0;
	}

	UIRect::UIRect(const _m_rect& src)
	{
		left = (int)src.left;
		top = (int)src.top;
		right = (int)src.right;
		bottom = (int)src.bottom;
	}

	UIRect::UIRect(int x, int y, int width, int height)
	{
		left = x;
		top = y;
		right = x + width;
		bottom = y + height;
	}

	_m_rect UIRect::ToRect() const
	{
		return { left, top, right, bottom };
	}

	void UIRect::SetWidth(int width)
	{
		right = left + width;
	}

	void UIRect::SetHeight(int height)
	{
		bottom = top + height;
	}

	bool UIRect::IsEmpty() const
	{
		return left == 0 && top == 0 && right == 0 && bottom == 0;
	}

	void UIRect::Empty()
	{
		left = top = right = bottom = 0;
	}

	void UIRect::Join(const UIRect& rc)
	{
		if (rc.left < left) left = rc.left;
		if (rc.top < top) top = rc.top;
		if (rc.right > right) right = rc.right;
		if (rc.bottom > bottom) bottom = rc.bottom;
	}

	void UIRect::ResetOffset()
	{
		Helper::Rect::Offset(this, -offsetx, -offsety);
		offsetx = offsety = 0;
	}

	void UIRect::Normalize()
	{
		if (left > right) 
		{
			int old = left;
			left = right;
			right = old;
		}
		if (top > bottom) 
		{ 
			int old = top; 
			top = bottom;
			bottom = old;
		}
	}

	void UIRect::Offset(int cx, int cy)
	{
		offsetx += cx;
		offsety += cy;
		Helper::Rect::Offset(this, cx, cy);
	}

	void UIRect::Inflate(int cx, int cy)
	{
		Helper::Rect::Inflate(this, cx, cy);
	}

	void UIRect::Deflate(int cx, int cy)
	{
		Helper::Rect::Inflate(this, -cx, -cy);
	}

	void UIRect::Union(const UIRect& rc)
	{
		Helper::Rect::Union(this, this, &rc);
	}

	bool UIRect::operator==(const UIRect& rc) const
	{
		return rc.left == left && rc.top == top
			&& rc.right == right && rc.bottom == bottom;
	}

	bool UIRect::operator!=(const UIRect& rc) const
	{
		return rc.left != left || rc.top != top
			|| rc.right != right || rc.bottom != bottom;
	}

#pragma endregion

#pragma region UIFontStyle

	bool UIFontStyle::operator==(const UIFontStyle& font) const
	{
		return bold == font.bold && italics == font.italics &&
			underline == font.underline && strikeout == font.strikeout;
	}

	bool UIFontStyle::operator!=(const UIFontStyle& font) const
	{
		return bold != font.bold || italics != font.italics ||
			underline != font.underline || strikeout != font.strikeout;
	}

#pragma endregion

#pragma region UIString

	UIString::UIString()
	{
		CopyForm(L"");
	}

	UIString::UIString(_m_lpcwstr pstr)
	{
		CopyForm(pstr);
	}

	UIString::UIString(std::wstring_view strview) : UIString(strview.data())
	{
	}

	UIString::UIString(const std::wstring& string) : UIString(string.data())
	{
	}

	UIString::UIString(UIString&& other) noexcept
	{
		std::swap(m_length, other.m_length);
		m_string = std::move(other.m_string);
	}

	UIString::UIString(const UIString& other)
	{
		CopyForm(other.m_string.get());
	}

	UIString& UIString::operator=(const UIString& other)
	{
		if (&other == this)
			return *this;

		CopyForm(other.m_string.get());

		return *this;
	}

	UIString& UIString::operator=(UIString&& other) noexcept
	{
		if (&other == this)
			return *this;
		std::swap(m_length, other.m_length);
		m_string = std::move(other.m_string);
		return *this;
	}

	UIString::~UIString() = default;

	bool UIString::operator==(_m_lpcwstr other) const
	{
		return wcscmp(other, m_string.get()) == 0;
	}

	bool UIString::operator!=(_m_lpcwstr other) const
	{
		return !this->operator==(other);
	}

	bool UIString::operator==(const UIString& other) const
	{
		return wcscmp(other.m_string.get(), m_string.get()) == 0;
	}

	bool UIString::operator!=(const UIString& other) const
	{
		return !this->operator==(other);
	}

	bool UIString::operator==(std::wstring_view other) const
	{
		return view() == other;
	}

	bool UIString::operator!=(std::wstring_view other) const
	{
		return !this->operator==(other);
	}

	void UIString::CopyForm(_m_lpcwstr pstr)
	{
		m_string = nullptr;
		m_length = wcslen(pstr);
		auto string = new wchar_t[m_length + 1];
		memcpy(string, pstr, m_length * sizeof(wchar_t));
		string[m_length] = 0;
		m_string.reset(string);
	}

#pragma endregion

#pragma region MTimer

	MTimers::MTimers() : MThread([this] { ThreadProc(); })
	{
		MThread::Start(true);
	}

	MTimers::~MTimers()
	{
		MThread::Stop();
		for(auto& id : m_idList)
		{
			delete (_m_param*)id.first;
		}
		m_idList.clear();
		m_timerList.clear();
	}

	MTimers::ID MTimers::AddTimer(_m_uint elapse, CallBack callback)
	{
		std::lock_guard<std::mutex> lock(m_lock);
		ID id = (ID)new _m_param;
		timerID tid = std::make_pair(GetMilliSeconds() + (_m_param)elapse, id);
		m_idList[id] = tid;
		timer _timer;
		_timer.callback = std::move(callback);
		_timer.tid = id;
		_timer.elapse = elapse;
		m_timerList[tid] = _timer;
		Resume();
		return id;
	}

	bool MTimers::DelTimer(ID id)
	{
		std::lock_guard<std::mutex> lock(m_lock);
		auto p = m_idList.find(id);
		if (p == m_idList.end())
			return false;

		delete (_m_param*)id;
		m_idList.erase(p);

		for (auto iter = m_timerList.begin(); iter != m_timerList.end(); ++iter)
		{
			if (iter->second.tid != id)
				continue;
			m_timerList.erase(iter);
			break;
		}
		return true;
	}

	_m_param MTimers::GetMilliSeconds()
	{
		auto duration = steady_clock::now();
		auto micro = duration_cast<std::chrono::milliseconds>(duration.time_since_epoch()).count();
		return _m_param(micro);
	}

	_m_param MTimers::GetSleepTime()
	{
		if (!m_timerList.empty())
			return m_timerList.begin()->first.first - GetMilliSeconds();
		return 0;
	}

	void MTimers::ThreadProc()
	{
		//auto sleepTime = GetSleepTime();
		//if (sleepTime > 0)
		if (!IsRuning()) return;
		Helper::M_Sleep(1);

		auto micro = GetMilliSeconds();
		while (!m_timerList.empty() && m_timerList.begin()->first.first < micro) 
		{

			auto iter = m_timerList.begin();
			auto tid = iter->first;
			auto timer = iter->second;

			timer.time += timer.elapse;//累加时间
			if (timer.callback) {
				timer.callback(timer.tid, timer.time);
			}

			std::lock_guard<std::mutex> lock(m_lock);
			m_timerList.erase(iter);

			//如果用户没有调用DelTimer
			auto p = m_idList.find(timer.tid);
			//更新计时器时间
			if (p != m_idList.end()) {
				tid.first = GetMilliSeconds() + timer.elapse;
				m_timerList.insert(std::make_pair(tid, timer));
			}

			micro = GetMilliSeconds();
		}
		if(m_timerList.empty())
			Pause();
	}

#pragma endregion

#pragma region MFPSCounter

	_m_uint MFPSCounter::CalcFPS()
	{
		curTime = steady_clock::now();

		auto duration = duration_cast<microseconds>(curTime - lastTime);
		float duration_s = float(duration.count()) * microseconds::period::num / microseconds::period::den;

		if (duration_s > 1)//1秒之后开始统计FPS
		{
			fps = (float)frameCount / duration_s;
			frameCount = 0;
			lastTime = curTime;
		}

		++frameCount;
		return (_m_uint)fps;
	}

	void MFPSCounter::SetMaxFPS(float _fps)
	{
		if (_fps < 0.f)
			fpstime = -1.f;
		else {
			fpstime = _fps;
			m_fpsLimit = std::chrono::duration_cast<steady_clock::duration>(std::chrono::duration<double>{ 1.f / fps });
		}
	}

	float MFPSCounter::GetMaxFPS()
	{
		if (fpstime == -1.f)
			return -1.f;
		return fpstime;
	}

	void MFPSCounter::LimitFPS()
	{
		if (fpstime != -1.0f)
		{
			auto time_in_seconds = time_point_cast<seconds>(steady_clock::now());
			++frame_count_per_second;
			if (time_in_seconds > prev_time_in_seconds)
			{
				frame_count_per_second = 0;
				prev_time_in_seconds = time_in_seconds;
			}
			std::this_thread::sleep_until(m_EndFrame);
			m_BeginFrame = m_EndFrame;
			m_EndFrame = m_BeginFrame + m_fpsLimit;
		}
	}

#pragma endregion
}