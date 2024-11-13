// DmResEditor.cpp : 定义应用程序的入口点。
//
// 8-12-2020 Create

#include "framework.h"
#include "DmResEditor.h"
#include <ShellScalingApi.h>

HINSTANCE m_hInstance;
HWND g_hWnd;
std::wstring m_title = L"梦月资源编辑器v1.1";

bool g_istip = false;

using namespace Ctrl;

#if MUI_CFG_ENABLE_V1DMRES

DMResources dmRes;

XML::MuiXML* m_xmlui = nullptr;

extern void blockcallback(_m_size size, _m_size allsize, _m_param param);

static HRESULT WINAPI GetDpiForMonitor(
	_In_ HMONITOR hmonitor,
	_In_ MONITOR_DPI_TYPE dpiType,
	_Out_ UINT* dpiX,
	_Out_ UINT* dpiY)
{
	HINSTANCE hInstWinSta = LoadLibraryW(L"SHCore.dll");
	if (hInstWinSta == nullptr) return E_NOINTERFACE;

	typedef HRESULT(WINAPI* PFN_GDFM)(
		HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

	PFN_GDFM pGetDpiForMonitor =
		(PFN_GDFM)GetProcAddress(hInstWinSta, "GetDpiForMonitor");
	if (pGetDpiForMonitor == nullptr) return E_NOINTERFACE;

	return pGetDpiForMonitor(hmonitor, dpiType, dpiX, dpiY);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
	CtrlMgr::RegisterMuiControl();

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	//ShowDebugRect = true;
	m_hInstance = hInstance;
	std::wstring error;

	Render::MRender* render = new Render::MRender_GDIP();

	UINT_PTR gdiplusToken = 0;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	auto window = new MainWindow(render);
	if (window->Create(0, m_title, UIRect(CW_USEDEFAULT, CW_USEDEFAULT, 624, 652),
		std::bind(&MainWindow::AfterCreated, window), (_m_param)(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), 0))
	{
		window->SetMainWindow(true);

		//DPI缩放
		UINT dpiX = 0;
		UINT dpiY = 0;
		HRESULT hr = GetDpiForMonitor(
			MonitorFromWindow((HWND)window->GetWindowHandle(), MONITOR_DEFAULTTONEAREST),
			MDT_EFFECTIVE_DPI, (UINT*)&dpiX, (UINT*)&dpiY);
		if (FAILED(hr)) {
			HDC hdc = GetDC(NULL);
			dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
			ReleaseDC(NULL, hdc);
		}
		float scale = (float)dpiX / 96.f;
		window->ScaleWindow(int(624 * scale), int(652 * scale));

		window->SetCacheMode(true);
		window->CenterWindow();
		//window.ShowDebugRect(true);
		window->ShowWindow(true);

		Settings::UIMessageLoop();
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

_m_result MainWindow::EventSource(MEventCodeEnum code, _m_param param)
{
	//响应DPI更改
	auto message = ConvertEventCode(code);
	if (message == WM_DPICHANGED)
	{
		thread_local bool flag = false;
		if (flag)
			return 0;
		flag = true;
		auto pm = (std::pair<_m_param, _m_param>*)param;
		UINT dpiX = Helper::M_LOWORD((_m_long)pm->first);
		float scale = (float)dpiX / 96.f;
		ScaleWindow(int(624.f * scale), int(652.f * scale));
		flag = false;
		return 0;
	}
	else if(message == WM_DESTROY)
	{
		delete this;
		return 0;
	}
	return UIWindowBasic::EventSource(code, param);
}

bool MainWindow::AfterCreated()
{
	g_hWnd = (HWND)GetWindowHandle();
	HICON hIcon = LoadIconW(m_hInstance, MAKEINTRESOURCEW(IDI_DMRESEDITOR));
	SendMessageW(g_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	CreateControls();
	return true;
}

bool MainWindow::EventProc(UINotifyEvent event, UIControl* control, _m_param param)
{
	if (MUIEVENT(Event_Mouse_LClick, L"browse"))
	{
		std::vector<std::wstring> file;
		if (FS::UI::MBrowseForFile(false, false,
			{
				{ L"梦月资源文件 (*.dmres;*.dmspt)", L"*.dmres;*.dmspt" },
				{ L"所有文件 All Files (*.*)", L"*.*" }
			}, (_m_param)g_hWnd, file))
		{
			static_cast<UIEditBox*>(GetRootControl()->Child(L"path"))->SetCurText(file[0]);
		}
	}
	//新建资源
	else if (MUIEVENT(Event_Mouse_LClick, L"newdmres"))
	{
		std::wstring path = static_cast<UIEditBox*>(GetRootControl()->Child(L"path"))->GetCurText();
		if (path != L"")
		{
			static_cast<UIEditBox*>(GetRootControl()->Child(L"resclsname"))->SetCurText(L"DMResourceClass");
			control->SetEnabled(false);

			std::wstring resname = L"Resource1";
			std::wstring reskey = L"12345678";

			dmRes.CreateResource(L"DMResourceClass");
			dmRes.AddResource({ UIResource(), false, 0, L"" }, resname, reskey);

			UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
			ListItem* item = new ListItem();
			item->SetText(resname);
			listbox->AddItem(item, -1);
			listbox->SetEnabled(true, false);
			listbox->SetCurSelItem(0);

			static_cast<UIEditBox*>(GetRootControl()->Child(L"resname"))->SetCurText(resname);
			static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"))->SetCurText(reskey);

			GetRootControl()->Child(L"importres")->SetEnabled(true, false);
			GetRootControl()->Child(L"importstr")->SetEnabled(true, false);
			GetRootControl()->Child(L"addres")->SetEnabled(true);
			GetRootControl()->Child(L"delres")->SetEnabled(true);
			GetRootControl()->Child(L"savefile")->SetEnabled(true);
			GetRootControl()->Child(L"exitedit")->SetEnabled(true);
			GetRootControl()->Child(L"loaddmres")->SetEnabled(false);
			auto block = (UICheckBox*)GetRootControl()->Child(L"isblock");
			block->SetSel(false);
			block->SetEnabled(true);
			auto blocksize = (UIEditBox*)GetRootControl()->Child(L"blocksize");
			blocksize->SetCurText(L"1024");
			blocksize->SetEnabled(true);
			GetRootControl()->Child(L"upresname")->SetEnabled(false);

			GetRootControl()->Child(L"group")->SetEnabled(true);
		}
		else
			MessageBoxW(g_hWnd, L"文件路径不能为空！", L"失败", MB_ICONERROR);
	}
	//类名更改
	else if (MUIEVENT(Event_Edit_TextChanged, L"resclsname"))
	{
		GetRootControl()->Child(L"upclsname")->SetEnabled(true);
	}
	//更新类名
	else if (MUIEVENT(Event_Mouse_LClick, L"upclsname"))
	{
		UIEditBox* edit = (UIEditBox*)GetRootControl()->Child(L"resclsname");
		if (edit->GetCurText() == L"")
		{
			MessageBoxW(g_hWnd, L"更新名称失败！名称不能为空", L"更改类名", MB_ICONERROR);
			return false;
		}
		control->SetEnabled(false);
		dmRes.RenameClassName(edit->GetCurText().c_str());
		MessageBoxW(g_hWnd, (L"类名已更改为：" + edit->GetCurText()).c_str(), L"更改类名", MB_ICONINFORMATION);
	}
	//资源名更改
	else if (MUIEVENT(Event_Edit_TextChanged, L"resname"))
	{
		GetRootControl()->Child(L"upresname")->SetEnabled(true);
	}
	//更新资源名
	else if (MUIEVENT(Event_Mouse_LClick, L"upresname"))
	{
		UIEditBox* edit = (UIEditBox*)GetRootControl()->Child(L"resname");
		UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
		if (edit->GetCurText() == L"")
		{
			MessageBoxW(g_hWnd, L"更新名称失败！名称不能为空", L"更改资源名", MB_ICONERROR);
			return true;
		}

		if (dmRes.RenameResource(listbox->GetItem(listbox->GetCurSelItem())->GetText().data(), edit->GetCurText().c_str())) {
			listbox->GetItem(listbox->GetCurSelItem())->SetText(edit->GetCurText());
			control->SetEnabled(false);
			MessageBoxW(g_hWnd, (L"资源名已更改为：" + edit->GetCurText()).c_str(), L"更改资源名", MB_ICONINFORMATION);
		}
		else
			MessageBoxW(g_hWnd, L"类名更新失败..", L"更改资源名", MB_ICONERROR);
	}
	//分块选项
	else if (MUIEVENT(Event_Mouse_LClick, L"isblock"))
	{
		GetRootControl()->Child(L"blocksize")->SetEnabled(static_cast<UICheckBox*>(control)->GetSel());
	}
	else if (MUIEVENT(Event_Focus_False, L"blocksize"))
	{
		UIEditBox* edit = (UIEditBox*)control;
		if (edit->GetCurText() == L"")
			edit->SetCurText(L"1024");
	}
	//资源文本更改
	else if (MUIEVENT(Event_Edit_TextChanged, L"strpreview"))
	{
		UIEditBox* edit = (UIEditBox*)control;
		if (edit->GetCurTextLength(0) == 0)
		{
			GetRootControl()->Child(L"exportstr")->SetEnabled(false);
			GetRootControl()->Child(L"cleanstr")->SetEnabled(false);
		}
		else
		{
			GetRootControl()->Child(L"exportstr")->SetEnabled(true);
			GetRootControl()->Child(L"importstr")->SetEnabled(true);
			GetRootControl()->Child(L"cleanstr")->SetEnabled(true);
			GetRootControl()->Child(L"savestr")->SetEnabled(true);
		}
		ShowStrLength(edit->GetCurTextLength(0));
	}
	//写入数据
	else if (MUIEVENT(Event_Mouse_LClick, L"importres"))
	{
		WriteRes(control);
	}
	//清空数据
	else if (MUIEVENT(Event_Mouse_LClick, L"cleanres"))
	{
		if (MessageBoxW(g_hWnd, L"确定要清空资源数据吗？", L"清空数据", MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
			UIEditBox* reskey = static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"));
			std::wstring resname = listbox->GetItem(listbox->GetCurSelItem())->GetText().data();

			DMResKey key = dmRes.ReadResource(resname, reskey->GetCurText());
			if (key.res.data)
				key.res.Release();
			if (dmRes.ChangeResource(resname, key, reskey->GetCurText()))
			{
				GetRootControl()->Child(L"exportres")->SetEnabled(false);
				GetRootControl()->Child(L"importres")->SetEnabled(true);
				GetRootControl()->Child(L"isblock")->SetEnabled(true);
				GetRootControl()->Child(L"blocksize")->SetEnabled(true);
				control->SetEnabled(false);

				ShowResSize(0);

				MessageBoxW(g_hWnd, L"清空资源数据成功!", L"清空数据", MB_ICONINFORMATION);
			}
		}
	}
	//写出数据
	else if (MUIEVENT(Event_Mouse_LClick, L"exportres"))
	{
		SaveRes(control);
	}
	//写出文本
	else if (MUIEVENT(Event_Mouse_LClick, L"exportstr"))
	{
		std::wstring str = static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"))->GetCurText();
		str = Helper::M_ReplaceString(str, L"\r", L"\r\n");
		std::vector<std::wstring> selfile;
		if(FS::UI::MBrowseForFile(false, false, {{L"文本文档 (*.txt)", L"*.txt"}}, (_m_param)g_hWnd, selfile))
		{
			FILE* file = 0;
			_wfopen_s(&file, selfile[0].data(), L"wb");
			if (!file) return true;

			//Unicode文件头
			fwrite("\xFF\xFE", 1, 2, file);
			fwrite(str.c_str(), 1, str.length() * sizeof(wchar_t), file);
			fclose(file);
		}
	}
	//写入文本
	else if (MUIEVENT(Event_Mouse_LClick, L"importstr"))
	{
		std::vector<std::wstring> selfile;
		if (FS::UI::MBrowseForFile(true, false, { {L"UTF16 LE 文本文档 (*.txt)", L"*.txt"} }, (_m_param)g_hWnd, selfile))
		{
			FILE* file = 0;
			_wfopen_s(&file, selfile[0].data(), L"rb");
			if (!file) return true;

			bool end = false;

			fseek(file, 0L, SEEK_END);
			_m_long len = ftell(file);
			_m_long read = 0;
			//跳过Unicode文件头
			fseek(file, 2, SEEK_SET);
			std::wstring str;
			while (!end)
			{
				wchar_t wch;
				_m_long len_read = fread(&wch, 1, sizeof(wchar_t), file);
				if (len_read)
				{
					read += len_read;
					str += wch;
				}
				if (read + 2 >= len)
					break;
			}
			static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"))->SetCurText(str);
			ShowStrLength(str.length());
			fclose(file);
			if (str.length() != 0) {
				GetRootControl()->Child(L"exportstr")->SetEnabled(true);
				GetRootControl()->Child(L"cleanstr")->SetEnabled(true);
			}
			GetRootControl()->Child(L"savestr")->SetEnabled(true);
		}
	}
	//保存文本数据
	else if (MUIEVENT(Event_Mouse_LClick, L"savestr"))
	{
		UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
		UIEditBox* reskey = static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"));
		UIEditBox* resstrEdit = static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"));
		std::wstring resname = listbox->GetItem(listbox->GetCurSelItem())->GetText().data();

		DMResKey key = dmRes.ReadResource(resname.c_str(), reskey->GetCurText().c_str(), StringRes);
		key.resText = resstrEdit->GetCurText();

		if (dmRes.ChangeResource(resname.c_str(), key, reskey->GetCurText().c_str(), StringRes))
		{
			control->SetEnabled(false);
			MessageBoxW(g_hWnd, L"更新字符串资源成功!", L"保存文本", MB_ICONINFORMATION);
		}
		else
			MessageBoxW(g_hWnd, L"更新字符串资源失败...", L"保存文本", MB_ICONERROR);
	}
	//清空文本数据
	else if (MUIEVENT(Event_Mouse_LClick, L"cleanstr"))
	{
		control->SetEnabled(false);
		GetRootControl()->Child(L"exportstr")->SetEnabled(false);
		static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"))->SetCurText(L"");
		ShowStrLength(0);
		GetRootControl()->Child(L"savestr")->SetEnabled(true);
	}
	//添加资源
	else if (MUIEVENT(Event_Mouse_LClick, L"addres"))
	{
		UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");

		std::wstring newResname = L"Resource" + std::to_wstring(listbox->GetItemListCount() + 1);

		ListItem* item = new ListItem();
		item->SetText(newResname);
		listbox->AddItem(item, -1);
		listbox->SetCurSelItem(listbox->GetItemListCount() - 1);
		curSelItem = listbox->GetItemListCount() - 1;

		static_cast<UIEditBox*>(GetRootControl()->Child(L"resname"))->SetCurText(newResname);

		GetRootControl()->Child(L"upresname")->SetEnabled(false);

		GetRootControl()->Child(L"importres")->SetEnabled(true);
		GetRootControl()->Child(L"importstr")->SetEnabled(true);
		GetRootControl()->Child(L"exportres")->SetEnabled(false);
		GetRootControl()->Child(L"exportstr")->SetEnabled(false);

		auto block = (UICheckBox*)GetRootControl()->Child(L"isblock");
		block->SetSel(false, false);
		block->SetEnabled(true);

		auto blocksize = (UIEditBox*)GetRootControl()->Child(L"blocksize");
		blocksize->SetCurText(L"1024");
		blocksize->SetEnabled(true);

		GetRootControl()->Child(L"decoderes")->SetEnabled(false);
		GetRootControl()->Child(L"cleanres")->SetEnabled(false);
		GetRootControl()->Child(L"cleanstr")->SetEnabled(false);
		GetRootControl()->Child(L"savestr")->SetEnabled(false);
		GetRootControl()->Child(L"strpreview")->SetEnabled(true);

		GetRootControl()->Child(L"delres")->SetEnabled(true);
		GetRootControl()->Child(L"savefile")->SetEnabled(true);

		static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"))->SetCurText(L"");
		ShowResSize(0);
		ShowStrLength(0);
		GetRootControl()->Child(L"group")->SetEnabled(true);

		UIEditBox* reskey = static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"));
		dmRes.AddResource({ UIResource(), false, 0, L"" }, newResname, reskey->GetCurText());
	}
	//资源列表项目选中更改
	else if (MUIEVENT(Event_ListBox_ItemChanged, L"reslist"))
	{
		curSelItem = (int)param;
		OnSelChange(control);
	}
	//解码资源
	else if (MUIEVENT(Event_Mouse_LClick, L"decoderes"))
	{
		UIEditBox* preview = static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"));
		UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
		std::wstring resName = listbox->GetItem(listbox->GetCurSelItem())->GetText().data();
		UIEditBox* reskey = static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"));

		DMResKey key = dmRes.ReadResource(resName, reskey->GetCurText(), StringRes);
		
		preview->SetCurText(key.resText);
		preview->SetEnabled(true);
		GetRootControl()->Child(L"importstr")->SetEnabled(true);
		if (key.resText.length() != 0) {
			GetRootControl()->Child(L"cleanstr")->SetEnabled(true);
			GetRootControl()->Child(L"exportstr")->SetEnabled(true);
		}
		if (dmRes.ReadResourceSize(resName, DataRes) != 0)
		{
			GetRootControl()->Child(L"importres")->SetEnabled(false);
			GetRootControl()->Child(L"cleanres")->SetEnabled(true);
			GetRootControl()->Child(L"exportres")->SetEnabled(true);
		}
		else {
			GetRootControl()->Child(L"importres")->SetEnabled(true);
			GetRootControl()->Child(L"isblock")->SetEnabled(true);
			GetRootControl()->Child(L"blocksize")->SetEnabled(true);
		}
		control->SetEnabled(false);

		if (!g_istip) {
			g_istip = true;
			MessageBoxW(g_hWnd, L"资源读取完毕\n提示：\n  如果秘钥与写入资源时的秘钥不符 依然能读取数据 但数据并非正确数据! 请自行效验结果.", L"读取数据", MB_ICONINFORMATION);
		}
	}
	//删除资源
	else if (MUIEVENT(Event_Mouse_LClick, L"delres"))
	{
		UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
		std::wstring resName = listbox->GetItem(listbox->GetCurSelItem())->GetText().data();
		if (MessageBoxW(g_hWnd, (L"是否要删除资源\"" + resName + L"\"?").c_str(), L"删除资源", MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			if (dmRes.DeleteResource(resName)) {
				listbox->DeleteItem(listbox->GetCurSelItem());
				listbox->SetCurSelItem(-1);
				curSelItem = -1;
				UIEditBox* preview = static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"));
				preview->SetCurText(L"");
				preview->SetEnabled(false);
				UIEditBox* resName = static_cast<UIEditBox*>(GetRootControl()->Child(L"resname"));
				resName->SetCurText(L"");

				control->SetEnabled(false);
				ShowStrLength(0);
				ShowResSize(0);
				static_cast<UICheckBox*>(GetRootControl()->Child(L"isblock"))->SetSel(0);
				static_cast<UIEditBox*>(GetRootControl()->Child(L"blocksize"))->SetCurText(L"1024");
				GetRootControl()->Child(L"group")->SetEnabled(false);

				MessageBoxW(g_hWnd, L"删除资源成功!", L"删除资源", MB_ICONINFORMATION);
			}
			else
				MessageBoxW(g_hWnd, L"删除资源失败..", L"删除资源", MB_ICONERROR);
		}
	}
	else if (MUIEVENT(Event_Mouse_LClick, L"savefile"))
	{
		std::wstring path = static_cast<UIEditBox*>(GetRootControl()->Child(L"path"))->GetCurText();
		if (dmRes.SaveResource(path))
			MessageBoxW(g_hWnd, L"资源以成功保存到指定路径~", L"保存", MB_ICONINFORMATION);
		else
			MessageBoxW(g_hWnd, L"保存文件失败! 文件可能被占用或不存在", L"保存", MB_ICONERROR);
	}
	else if (MUIEVENT(Event_Mouse_LClick, L"exitedit"))
	{
		if (MessageBoxW(g_hWnd, L"确定退出编辑模式，返回启动时的状态吗？\n提示：如果修改了资源而未保存文件 将会丢失！", L"退出编辑", MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			control->SetEnabled(false);
			GetRootControl()->Child(L"loaddmres")->SetEnabled(true);
			GetRootControl()->Child(L"newdmres")->SetEnabled(true);
			GetRootControl()->Child(L"delres")->SetEnabled(false);
			GetRootControl()->Child(L"addres")->SetEnabled(false);
			GetRootControl()->Child(L"group")->SetEnabled(false);
			GetRootControl()->Child(L"savefile")->SetEnabled(false);
			GetRootControl()->Child(L"upclsname")->SetEnabled(false);
			GetRootControl()->Child(L"exportres")->SetEnabled(false);
			GetRootControl()->Child(L"exportstr")->SetEnabled(false);
			GetRootControl()->Child(L"cleanres")->SetEnabled(false);
			GetRootControl()->Child(L"cleanstr")->SetEnabled(false);
			GetRootControl()->Child(L"savestr")->SetEnabled(false);
			GetRootControl()->Child(L"decoderes")->SetEnabled(false);
			static_cast<UIEditBox*>(GetRootControl()->Child(L"resname"))->SetCurText(L"");
			UIEditBox* clsName = static_cast<UIEditBox*>(GetRootControl()->Child(L"resclsname"));
			clsName->SetCurText(L"");
			UIEditBox* resName = static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"));
			resName->SetCurText(L"");
			resName->SetEnabled(true);
			static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"))->SetCurText(L"");
			ShowStrLength(0);
			ShowResSize(0);
			UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
			listbox->DeleteAllItem();
			listbox->SetEnabled(false);
			dmRes.CloseResource();
		}
	}
	else if (MUIEVENT(Event_Mouse_LClick, L"loaddmres"))
	{
		std::wstring path = static_cast<UIEditBox*>(GetRootControl()->Child(L"path"))->GetCurText();
		if (dmRes.LoadResource(path, true))
		{
			control->SetEnabled(false);
			UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");

			std::vector<std::wstring> resList;
			dmRes.EnumResourceName(resList);
			for (auto name : resList)
			{
				ListItem* item = new ListItem();
				item->SetText(name);
				listbox->AddItem(item, -1, false);
			}
			listbox->SetCurSelItem(-1);
			listbox->SetEnabled(true);

			static_cast<UIEditBox*>(GetRootControl()->Child(L"resclsname"))->SetCurText(dmRes.GetResClassName());

			GetRootControl()->Child(L"savefile")->SetEnabled(true);
			GetRootControl()->Child(L"loaddmres")->SetEnabled(false);
			GetRootControl()->Child(L"exitedit")->SetEnabled(true);
			GetRootControl()->Child(L"addres")->SetEnabled(true);
			GetRootControl()->Child(L"newdmres")->SetEnabled(false);

			curSelItem = 0;
			if (resList.size())
			{
				listbox->SetCurSelItem(0);
				OnSelChange(listbox);
			}
		}
		else
			MessageBoxW(g_hWnd, L"打开文件失败! ", L"保存", MB_ICONERROR);
	}
	return false;
}

void MainWindow::CreateControls()
{
	//这些都是1.x 版本的写法 2x还能用! 懒得改了

	UIControl* root_control = GetRootControl();

	m_xmlui = XMLUI();
	auto mgr = m_xmlui->Mgr();

	auto editstyle = DMResources::ReadPEResource(IDB_PNG1, L"PNG");
	auto btnstyle = DMResources::ReadPEResource(IDB_PNG2, L"PNG");
	auto liststyle = DMResources::ReadPEResource(IDB_PNG3, L"PNG");
	auto listitemstyle = DMResources::ReadPEResource(IDB_PNG4, L"PNG");
	auto scrollstyle = DMResources::ReadPEResource(IDB_PNG5, L"PNG");
	auto checkstyle = DMResources::ReadPEResource(IDB_PNG6, L"PNG");
	auto progstyle = DMResources::ReadPEResource(IDB_PNG7, L"PNG");

	mgr->AddImageStyle(L"editbox", editstyle, 4, nullptr, true, { 2,2,2,2 });
	mgr->AddImageStyle(L"btnskin", btnstyle, 4, nullptr, true, { 2,2,2,2 });
	mgr->AddImageStyle(L"listbox", liststyle, 4, nullptr, true, { 2,2,2,2 });
	mgr->AddImageStyle(L"listitem", listitemstyle, 8, nullptr, true, { 5,5,5,5 });
	mgr->AddImageStyle(L"scroll", scrollstyle, 16, nullptr, true, { 2,2,2,2 });
	mgr->AddImageStyle(L"checkbox", checkstyle, 8, nullptr, true, { 5,5,5,5 });
	mgr->AddImageStyle(L"progskin", progstyle, 4, nullptr, true, { 1,1,1,1 });

	editstyle.Release();
	btnstyle.Release();
	liststyle.Release();
	listitemstyle.Release();
	scrollstyle.Release();
	checkstyle.Release();
	progstyle.Release();

	UILabel::Attribute itemStyle;
	itemStyle.textAlign = TextAlign(TextAlign_Left | TextAlign_VCenter);
	m_xmlui->AddFontStyle(L"listitem", itemStyle);

	std::wstring xml = LR"(
	<UIControl size="100%,100%" bgColor="255,255,255,255" autoSize="false" />
	<UILabel text="资源文件路径：" autoSize="true" pos="10,22" />
	<UIEditBox style="editbox" frame="10,45,493,23" name="path" autoSize="false" inset="2,2,2,2" />
	<UIButton style="btnskin" animate="true" aniAlphaType="true" text="浏览" textAlign="5" frame="515,45,82,23" name="browse" autoSize="false" />
	<UIButton style="btnskin" animate="true" aniAlphaType="true" text="新建资源类" textAlign="5" frame="287,80,95,23" name="newdmres" autoSize="false" />
	<UIButton style="btnskin" animate="true" aniAlphaType="true" text="加载资源类" textAlign="5" frame="395,80,95,23" name="loaddmres" autoSize="false" />
	<UIButton style="btnskin" animate="true" aniAlphaType="true" text="退出编辑模式" textAlign="5" frame="503,80,95,23" enable="false" name="exitedit" autoSize="false" />
	<UIListBox style="listbox" itemStyle="listitem" styleV="scroll" iFontStyle="listitem" enable="false" frame="10,79,264,497" name="reslist" drawOffset="5,0" autoSize="false" />
	<UIProgBar style="progskin" frame="0,0,624,4" max="100" name="progress" visible="false" autoSize="false" />
	<UIControl frameColor="142,142,142,255" frameWidth="1" frame="287,116,311,490" enable="false" name="group" align="Absolute" autoSize="false">
		<UILabel text="资源类名：" autoSize="true" pos="6,13" />
		<UIEditBox style="editbox" frame="68,7,135,23" name="resclsname" limitText="30" autoSize="false" inset="2,2,2,2" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="更新类名" textAlign="5" frame="216,7,87,23" name="upclsname" enable="false" autoSize="false" />
		<UIControl bgColor="142,142,142,255" frame="7,37,296,1" autoSize="false" />
		<UILabel text="资源名称：" autoSize="true" pos="6,51" />
		<UIEditBox style="editbox" frame="68,45,135,23" name="resname" autoSize="false" inset="2,2,2,2" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="更新名称" textAlign="5" frame="216,45,87,23" name="upresname" enable="false" autoSize="false" />
		<UILabel text="资源秘钥：" autoSize="true" pos="6,81" />
		<UIEditBox style="editbox" frame="68,75,235,23" name="reskey" autoSize="false" inset="2,2,2,2" />
		<UILabel text="资源数据：" autoSize="true" pos="6,111" />
		<UILabel text="字节大小：0B" fontColor="100,122,235,255" frame="69,111,235,15" name="ressize" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="解码资源" textAlign="5" frame="216,103,87,23" name="decoderes" enable="false" autoSize="false" />
		<UILabel text="分块存储：" autoSize="true" pos="29,134" />
		<UICheckBox style="checkbox" fontStyle="listitem" text="分块" frame="91,132,65,18" name="isblock" autoSize="false" />
		<UILabel text="块尺寸：" autoSize="true" pos="165,134" />
		<UIEditBox style="editbox" number="true" text="1024" frame="216,131,87,18" name="blocksize" enable="false" autoSize="false" inset="2,2,2,2" />
		<UILabel text="操作：" autoSize="true" pos="6,161" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="写出数据" textAlign="5" frame="68,156,68,23" enable="false" name="exportres" autoSize="false" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="写入数据" textAlign="5" frame="151,156,68,23" enable="false" name="importres" autoSize="false" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="清空数据" textAlign="5" frame="235,156,68,23" enable="false" name="cleanres" autoSize="false" />
		<UILabel text="文本数据：" autoSize="true" pos="6,190" />
		<UILabel text="文本长度：0" fontColor="100,122,235,255" frame="69,190,235,15" name="strsize" />
		<UIEditBox style="editbox" frame="6,211,297,243" multiline="true" scroll="true" styleV="scroll" styleH="scroll" name="strpreview" autoSize="false" inset="2,2,2,2" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="写出文本" textAlign="5" frame="6,461,66,23" enable="false" name="exportstr" autoSize="false" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="写入文本" textAlign="5" frame="83,461,66,23" enable="false" name="importstr" autoSize="false" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="保存文本" textAlign="5" frame="160,461,66,23" enable="false" name="savestr" autoSize="false" />
		<UIButton style="btnskin" animate="true" aniAlphaType="true" text="清空文本" textAlign="5" frame="237,461,66,23" enable="false" name="cleanstr" autoSize="false" />
	</UIControl>
	<UIButton style="btnskin" animate="true" aniAlphaType="true" text="添加资源" textAlign="5" frame="10,583,76,23" enable="false" name="addres" autoSize="false" />
	<UIButton style="btnskin" animate="true" aniAlphaType="true" text="删除资源" textAlign="5" frame="94,583,76,23" enable="false" name="delres" autoSize="false" />
	<UIButton style="btnskin" animate="true" aniAlphaType="true" text="保存到文件" textAlign="5" frame="178,583,96,23" enable="false" name="savefile" autoSize="false" />
	)";

	bool ret = m_xmlui->CreateUIFromXML(root_control, xml);
	if (!ret)
		MessageBoxW(nullptr, L"创建UI失败!", L"error", MB_ICONERROR);
}

void MainWindow::WriteRes(UIControl* control)
{
	std::vector<std::wstring> selfile;
	if (FS::UI::MBrowseForFile(true, false, { {L"AllFiles (*.*)", L"*.*"} }, (_m_param)g_hWnd, selfile))
	{
		UIResource files = dmRes.ReadFiles(selfile[0]);
		if (files.data)
		{
			UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
			UIEditBox* reskey = static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"));
			UICheckBox* isblock = static_cast<UICheckBox*>(GetRootControl()->Child(L"isblock"));
			std::wstring resname = listbox->GetItem(listbox->GetCurSelItem())->GetText().data();

			DMResKey newKey;
			newKey.res = files;
			newKey.block = isblock->GetSel();
			if(newKey.block)
				newKey.blockSize = std::stoi(static_cast<UIEditBox*>(GetRootControl()->Child(L"blocksize"))->GetCurText());
			
			if (newKey.block)
				EnableWindow(false);

			DMEncBlockCallback callback;
			if (newKey.block)
			{
				auto param = new std::pair<MainWindow*, std::pair<UIProgressBar*, UIResource>>;
				param->first = this;
				param->second.first = (UIProgressBar*)GetRootControl()->Child(L"progress");
				param->second.first->SetVisible(true);
				param->second.second = files;
				callback.callback = blockcallback;
				callback.param = (_m_param)param;
			}

			if (dmRes.ChangeResource(resname.c_str(), newKey, reskey->GetCurText().c_str(), DataRes, callback))
			{
				if (!newKey.block) {
					files.Release();
					WriteSuccess(newKey.res.size);
				}
			}
			else {
				files.Release();
				EnableWindow(true);
				MessageBoxW(g_hWnd, L"无法将资源写入资源内存！.", L"写入资源", MB_ICONERROR);
			}
		}
		else
			MessageBoxW(g_hWnd, L"文件读取失败！无法写入资源.", L"写入资源", MB_ICONERROR);
	}
}

void MainWindow::WriteSuccess(_m_size ressize)
{
	GetRootControl()->Child(L"importres")->SetEnabled(false);
	GetRootControl()->Child(L"isblock")->SetEnabled(false);
	GetRootControl()->Child(L"blocksize")->SetEnabled(false);
	GetRootControl()->Child(L"importres")->SetEnabled(false);
	GetRootControl()->Child(L"cleanres")->SetEnabled(true);
	GetRootControl()->Child(L"exportres")->SetEnabled(true);

	ShowResSize(ressize);
	SetWindowTextW(g_hWnd, ::m_title.c_str());

	MessageBoxW(g_hWnd, L"写入资源成功！", L"写入资源", MB_ICONINFORMATION);
}

void MainWindow::SaveRes(UIControl* control)
{
	std::vector<std::wstring> selfile;
	if (FS::UI::MBrowseForFile(false, false, { {L"AllFiles (*.*)", L"*.*"} }, (_m_param)g_hWnd, selfile))
	{
		UIListBox* listbox = (UIListBox*)GetRootControl()->Child(L"reslist");
		UIEditBox* reskey = static_cast<UIEditBox*>(GetRootControl()->Child(L"reskey"));
		std::wstring resname = listbox->GetItem(listbox->GetCurSelItem())->GetText().data();
		
		_m_size blocksize = 0;
		if (dmRes.ReadResBlockInfo(resname, &blocksize))
		{
			_m_size cur = 0;
			_m_size size = 0;
			_m_size allsize = dmRes.ReadResourceSize(resname, DataRes);
			std::wstring key = reskey->GetCurText();
			auto progress = (UIProgressBar*)GetRootControl()->Child(L"progress");
			progress->SetCurValue(0);
			progress->SetVisible(true, true);

			FILE* file;
			_wfopen_s(&file, selfile[0].data(), L"wb");
			EnableWindow(false);
			for (;;)
			{
				if (!file) {
					MessageBoxW(g_hWnd, L"写出资源失败! 无法写出到磁盘", L"写出数据", MB_ICONERROR);
					break;
				}

				_m_size ret = 0;
				UIResource res = dmRes.ReadResourceBlock(resname, key, cur, blocksize, &ret);

				if(ret)
					fwrite(res.data, 1, ret, file);

				res.Release();

				cur += blocksize;
				size += ret;
				_m_size value = _m_size((double)size / (double)allsize * 100.f);
				if (value != progress->GetCurValue())
					progress->SetCurValue(value);
				if (size == allsize) {
					fclose(file);
					MessageBoxW(g_hWnd, L"写出资源成功!", L"写出资源", MB_ICONINFORMATION);
					break;
				}
				Settings::UIMessageLoop();
			}
			EnableWindow(true);
			progress->SetVisible(false, true);
		}
		else {
			DMResKey key = dmRes.ReadResource(resname, reskey->GetCurText(), DataRes);

			if (dmRes.WriteFiles(selfile[0], key.res))
				MessageBoxW(g_hWnd, L"写出资源成功!", L"写出资源", MB_ICONINFORMATION);
			else
				MessageBoxW(g_hWnd, L"写出资源失败! 无法写出到磁盘", L"写出数据", MB_ICONERROR);
			key.res.Release();
		}
	}
}

void MainWindow::OnSelChange(UIControl* control)
{
	UIListBox* listbox = (UIListBox*)control;
	UIEditBox* resName = static_cast<UIEditBox*>(GetRootControl()->Child(L"resname"));
	std::wstring resname = listbox->GetItem(curSelItem)->GetText().data();
	resName->SetCurText(resname);
	resName->SetEnabled(true);

	auto block = (UICheckBox*)GetRootControl()->Child(L"isblock");
	_m_size blocksize = 1024;
	bool isblock = dmRes.ReadResBlockInfo(resname, &blocksize);
	if (!isblock)
		blocksize = 1024;
	block->SetSel(isblock);
	block->SetEnabled(false);
	auto blocksizeedit = (UIEditBox*)GetRootControl()->Child(L"blocksize");
	blocksizeedit->SetCurText(std::to_wstring(blocksize));
	blocksizeedit->SetEnabled(false);

	GetRootControl()->Child(L"group")->SetEnabled(true);
	GetRootControl()->Child(L"decoderes")->SetEnabled(true);
	GetRootControl()->Child(L"upresname")->SetEnabled(false);
	GetRootControl()->Child(L"exportres")->SetEnabled(false);
	GetRootControl()->Child(L"importres")->SetEnabled(false);
	GetRootControl()->Child(L"cleanres")->SetEnabled(false);
	GetRootControl()->Child(L"importstr")->SetEnabled(false);
	GetRootControl()->Child(L"exportstr")->SetEnabled(false);
	GetRootControl()->Child(L"cleanstr")->SetEnabled(false);
	GetRootControl()->Child(L"savestr")->SetEnabled(false);
	GetRootControl()->Child(L"delres")->SetEnabled(true);
	UIEditBox* resEdit = static_cast<UIEditBox*>(GetRootControl()->Child(L"strpreview"));
	resEdit->SetCurText(L"");
	resEdit->SetEnabled(false);
	ShowResSize(dmRes.ReadResourceSize(resname, DataRes));
	ShowStrLength(dmRes.ReadResourceSize(resname, StringRes));
}

void MainWindow::ShowResSize(_m_size size)
{
	std::wstring size_str, uint;

	double i = pow((double)2, 10);

	if (size < pow((double)2, 10))//dwSize < 1024
	{
		size_str = std::to_wstring(size);
		uint = L"B";
	}
	else if (pow((double)2, 10) <= size && size < pow((double)2, 20))// 1024 <= dwSize < 1024*1024
	{
		float fSize = (float)(size * 100 / 1024) / 100;
		size_str = std::to_wstring(fSize);
		uint = L"KB";
	}
	else if (pow((double)2, 20) <= size && size < pow((double)2, 30))// 1024*1024 <= dwSize < 1024*1024*1024
	{
		float fSize = (float)(size / 1024 * 100 / 1024) / 100;
		size_str = std::to_wstring(fSize);
		uint = L"MB";
	}
	else if (pow((double)2, 30) <= size && size < pow((double)2, 40)) // 1024*1024*1024 <= dwSize < 1024*1024*1024*1024
	{
		float fSize = (float)(size / 1024 * 100 / 1024 / 1024) / 100;
		size_str = std::to_wstring(fSize);
		uint = L"GB";
	}
	GetRootControl()->Child<UILabel>(L"ressize")->SetAttribute(
		L"text", L"字节大小：" + size_str.substr(0, size_str.find(L'.') + 3) + uint);
}

void MainWindow::ShowStrLength(_m_size lenght)
{
	GetRootControl()->Child<UILabel>(L"strsize")->SetAttribute(L"text", L"文本长度：" + std::to_wstring(lenght));
}

void blockcallback(_m_size size, _m_size allsize, _m_param param)
{
	auto param_ = (std::pair<MainWindow*, std::pair<UIProgressBar*, UIResource>>*)param;
	_m_size value = _m_size((double)size / (double)allsize * 100.f);
	if (value != param_->second.first->GetCurValue())
		param_->second.first->SetCurValue(value);
	if (size == allsize) {
		param_->second.second.Release();
		param_->first->WriteSuccess(size);
		param_->second.first->SetVisible(false, true);
		param_->first->EnableWindow(true);
		delete param_;
	}
}
#else
#error "需要启用DmResFileV1 (MUI_CFG_ENABLE_V1DMRES宏)在Mui_Config.h"
#endif