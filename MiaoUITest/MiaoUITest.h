#pragma once
#include <sdkddkver.h>
#include <Windows.h>
#include <Mui.h>
#include <User/Mui_Engine.h>

//窗口事件回调
namespace UI
{
	extern bool EventProc(Mui::MWindowCtx*, Mui::UINotifyEvent, Mui::Ctrl::UIControl*, Mui::_m_param);
	extern bool InitWindow(Mui::MWindowCtx*, Mui::Ctrl::UIControl*, Mui::XML::MuiXML*);
}