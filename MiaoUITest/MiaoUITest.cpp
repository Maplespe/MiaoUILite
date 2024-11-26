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
	base->SetRenderMode(true);

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
		<UIImgBox size="100%,100%" bgColor="255,255,255,255" img="bkgnd" imgStyle="3" autoSize="false">
			<UILabel pos="100,10" text="Hello World! 你好" fontSize="16" fontColor="3,166,44,255" />
			<UIButton pos="10,10" text="点击设置Dark" name="darkStyle" />
			<UICheckBox pos="10,10" text="CheckBox" fontSize="14" />
			<UIComBox frame="10,10,200,30" text="ComBox" fontSize="14" name="testcom" />
			<UIEditBox frame="10,10,200,30" text="测试编辑框单行" />
			<UIProgBar frame="10,10,200,10" max="100" value="30" />
			<UIControl pos="10,0">
				<UISlider frame="0,10,200,20" value="0" name="testscale" />
				<UILabel pos="10,10" url="https://www.baidu.com" text="超链接标签: https://www.baidu.com" hyperlink="true" fontColor="@hex:FAC527FF" 
				fontUnderline="true" fontSize="14" shadowUse="true" shadowColor="120,120,120,255" />
				<UIButton frame="10,10,200,30" text="#teststr" name="teststr" />
			</UIControl>
			<UINavBar pos="10,10" name="navbar" shadowUse="true" shadowColor="255,255,255,255" barColor="92,183,255,255" />
			<UITestCtrl frame="10,10,860,370" autoSize="false" bgColor="100,100,100,200" frameWidth="1" frameColor="160,160,160,255" />
			<UIListBox frame="10,10,200,280" itemHeight="30" name="testlist" />
			<UIControl align="LinearV">
				<UIColorPicker frame="10,10,150,150" autoSize="false" name="color" />
				<UISlider frame="10,5,200,20" value="0" maxValue="359" name="hsv" />
			</UIControl>
			<UIControl align="LinearV">
				<UIControl pos="10,10" align="LinearH">
					<UILabel pos="10,10" text="00:00" name="player_cur" fontColor="255,255,255,255" />
					<UISlider frame="5,10,300,20" name="player_pos" />
					<UILabel pos="10,10" text="00:00" name="player_time" fontColor="255,255,255,255" />
					<UIButton frame="10,10,50,20" text="继续" name="player_pause" data="1" autoSize="false" />
					<UIButton frame="10,10,100,20" text="打开视频" name="player_open" autoSize="false" />
				</UIControl>
			</UIControl>
		</UIImgBox>
		<UILabel pos="10,10" fontSize="14" fontColor="0,0,255,255" text="FPS:" name="fps" />

		);

		xmlUI->AddStringList(L"teststr", L"测试字符串");

		if (!xmlUI->CreateUIFromXML(root, xml))
		{
			MessageBoxW(nullptr, L"XML代码有误!创建UI失败!", L"error", MB_ICONERROR);
			return false;
		}

		UIComBox* com = root->Child<UIComBox>(L"testcom");
		UIListBox* list = root->Child<UIListBox>(L"testlist");
		for (size_t i = 0; i < 100; i++)
		{
			ListItem* item = new ListItem();
			item->SetText(L"测试列表项" + std::to_wstring(i + 1));
			com->AddItem(item, -1, false);
			item = new ListItem();
			item->SetText(L"测试列表项" + std::to_wstring(i + 1));
			list->AddItem(item, -1, false);
		}

		UINavBar* navbar = root->Child<UINavBar>(L"navbar");
		for (int i = 0; i < 5; i++)
		{
			std::wstring title = L"导航项目" + std::to_wstring(i + 1);
			navbar->AddItem(title, -1, false);
		}

		//设置一个计时器 每1秒更新一次fps
		UILabel* label = root->Child<UILabel>(L"fps");
		root->GetParentWin()->SetTimer(1000, [=](_m_ptrv window, _m_param, _m_ulong)
		{
			auto _wnd = reinterpret_cast<Window::UIWindowBasic*>(window);
			auto fps = _wnd->GetLastFPS();
			std::wstring info = L"FPS: " + std::to_wstring(fps);
			info += L"\n即时渲染: ";
			if (_wnd->GetRenderMode())
				info += L"true";
			else
				info += L"false";
			label->SetAttribute(L"text", info);
		});

		return true;
	}
}