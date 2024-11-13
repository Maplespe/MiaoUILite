#include "MiaoUITest.h"
#include "ControlTest.h"
#include <iomanip>

using namespace Mui;

//窗口尺寸
UISize g_wndSize = { 1226, 750 };

//应用程序入口点
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	MiaoUI::Render renderType = MiaoUI::Render::Auto;

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	M_SetLastExceptionNotify();

	//初始化界面库 全局仅有一个MiaoUI类
	MiaoUI engine;
	std::wstring err;
	if (!engine.InitEngine(err, renderType))
	{
		MessageBoxW(nullptr, (L"MiaoUI初始化失败! 错误信息: " + err).c_str(), L"error", MB_ICONERROR);
		return 0;
	}

	//注册自定义控件
	Ctrl::UITestCtrl::Register();

	//创建窗口上下文
	std::wstring title = L"MiaoUI Demo " + std::wstring(Settings::MuiEngineVer) + L" - Test";
	MWindowCtx* windowCtx = engine.CreateWindowCtx(UIRect(0, 0, g_wndSize.width, g_wndSize.height), MiaoUI::MWindowType::Normal, title, true, true);

	//加载默认样式和外部资源
	auto mgr = windowCtx->Base()->GetResourceMgr();

	mgr->AddResourcePath(FS::MGetCurrentDir() + L"\\resource.dmres", L"12345678");

	windowCtx->XML()->LoadDefaultStyle();

	//可以设置全局控件的默认属性
	std::wstring defList = MXMLCODE(
		<DefPropGroup control = "UIControl" autoSize="true" />
	);
	windowCtx->XML()->AddDefPropGroup(defList, true);

	//绑定窗口控件事件回调
	windowCtx->SetEventCallback(UI::EventProc);
	//初始化窗口
	if (!windowCtx->InitWindow(UI::InitWindow, false))
	{
		MessageBoxW(nullptr, L"初始化窗口失败!", L"error", MB_ICONERROR);
		return 0;
	}

	//可以访问窗口类设置更多
	auto base = windowCtx->Base();

	base->CenterWindow();
	//base->ShowDebugRect(true);
	//base->SetRenderMode(true);

	base->ShowWindow(true);

	//base->SetMaxFPSLimit(60);

	//窗口消息循环 直到窗口关闭
	windowCtx->EventLoop();
	return 0;
}

namespace UI
{
	bool EventProc(MWindowCtx* ctx, UINotifyEvent event, Ctrl::UIControl* control, _m_param param)
	{
		if (MUIEVENT(Event_Mouse_LDown, L"player_pos"))
		{
			control->SetUserData(1);
		}
		else if (MUIEVENT(Event_Mouse_LUp, L"player_pos"))
		{
			control->SetUserData(0);
			float pos = (float)static_cast<Ctrl::UISlider*>(control)->GetCurValue() / 100.f;
			auto cur = static_cast<Ctrl::UILabel*>(control->GetParent()->FindChildren(L"player_cur"));
			//m_player->SetTrackPlaybackPos(m_track, pos);
			//m_videoPlayer->Seek(pos);
		}
		else if (MUIEVENT(Event_Mouse_LClick, L"player_pause"))
		{
			if (control->GetUserData() == 0)
			{
				control->SetUserData(1);
				//m_player->PauseTrack(m_track);
				//m_videoPlayer->Pause();
				static_cast<Ctrl::UIButton*>(control)->SetAttribute(L"text", L"继续");
			}
			else
			{
				control->SetUserData(0);
				//m_player->PlayTrack(m_track);
				//m_videoPlayer->Play();
				static_cast<Ctrl::UIButton*>(control)->SetAttribute(L"text", L"暂停");
			}
		}
		else if (MUIEVENT(Event_Mouse_LClick, L"teststr"))
		{
			ctx->XML()->SetStringValue(L"teststr", L"已更新字符串");
		}
		else if (MUIEVENT(Event_Slider_Change, L"hsv"))
		{
			auto color = ctx->Base()->GetRootControl()->Child<Ctrl::UIColorPicker>(L"color");
			auto hsv = color->GetHSVColor();
			hsv.hue = (_m_ushort)param;
			color->SetHSVColor(hsv);
		}
		return false;
	}

	bool InitWindow(MWindowCtx* ctx, Ctrl::UIControl* root, XML::MuiXML* xmlUI)
	{
		using namespace Ctrl;
		std::wstring xml = MXMLCODE(
		<UIControl size="100%,100%" autoSize="false" align="Center" bgColor="@hex:ffffff">
			<UIControl align="LinearV">
				<UILabel fontSize="14" text="hello world!" fontColor="50,150,50,255" />
				<UIButton pos="0,5" text="click here" />
			</UIControl>
		</UIControl>
		);

		//xmlUI->AddStringList(L"teststr", L"测试字符串");

		if (!xmlUI->CreateUIFromXML(root, xml))
		{
			MessageBoxW(nullptr, L"XML代码有误!创建UI失败!", L"error", MB_ICONERROR);
			return false;
		}

		//UIComBox* com = root->Child<UIComBox>(L"testcom");
		//UIListBox* list = root->Child<UIListBox>(L"testlist");
		//for (size_t i = 0; i < 100; i++)
		//{
		//	ListItem* item = new ListItem();
		//	item->SetText(L"测试列表项" + std::to_wstring(i + 1));
		//	com->AddItem(item, -1, false);
		//	item = new ListItem();
		//	item->SetText(L"测试列表项" + std::to_wstring(i + 1));
		//	list->AddItem(item, -1, false);
		//}

		//UINavBar* navbar = root->Child<UINavBar>(L"navbar");
		//for (int i = 0; i < 5; i++)
		//{
		//	std::wstring title = L"导航项目" + std::to_wstring(i + 1);
		//	navbar->AddItem(title, -1, false);
		//}

		////设置一个计时器 每1秒更新一次fps
		//UILabel* label = root->Child<UILabel>(L"fps");
		//root->GetParentWin()->SetTimer(1000, [=](_m_ptrv window, _m_param, _m_ulong)
		//{
		//	auto _wnd = reinterpret_cast<Window::UIWindowBasic*>(window);
		//	auto fps = _wnd->GetLastFPS();
		//	std::wstring info = L"FPS: " + std::to_wstring(fps);
		//	info += L"\n即时渲染: ";
		//	if (_wnd->GetRenderMode())
		//		info += L"true";
		//	else
		//		info += L"false";
		//	label->SetAttribute(L"text", info);
		//});

		return true;
	}
}