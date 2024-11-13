/**
 * FileName: Mui_EditBox.cpp
 * Note: UI编辑框控件实现
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
 * date: 2020-10-28 Create
*/

#include <Control/Mui_EditBox.h>

#include <Render/Graphs/Mui_GdipRender.h>
#include <windowsx.h>
#pragma comment(lib, "Msimg32.lib")

#define MTEXTHOST(code) TextHost([&](MTextHost* host) code)
#define MTEXTSERV(code) TxServices([&](ITextServices2* serv) code)

namespace Mui::Ctrl
{
	using namespace Helper;

	HFONT CreateFontObject(const UILabel::Attribute& attrib, _m_scale scale)
	{
		const float fontSize = round(M_MIN(scale.cx, scale.cy) * (float)attrib.fontSize);

		LOGFONT lf = { 0 };
		lf.lfHeight = -int(fontSize);
		lf.lfCharSet = DEFAULT_CHARSET;

		if (attrib.fontStyle.bold) lf.lfWeight += FW_BOLD;
		if (attrib.fontStyle.underline) lf.lfUnderline = TRUE;
		if (attrib.fontStyle.italics) lf.lfItalic = TRUE;
		if (attrib.fontStyle.strikeout) lf.lfStrikeOut = TRUE;

		wcscpy_s(lf.lfFaceName, attrib.font.cstr());

		return CreateFontIndirectW(&lf);
	}

#ifdef _WIN32
	UIEditBox::UIEditBox(UIControl* parent, Attribute attrib, UIScroll::Attribute scrollAttrib)
		: UIEditBox(std::move(attrib), std::move(scrollAttrib))
	{
		M_ASSERT(parent)
			parent->AddChildren(this);
	}

	UIEditBox::UIEditBox(Attribute attrib, UIScroll::Attribute scrollAttrib)
		: UIScroll(std::move(scrollAttrib)), m_attrib(attrib)
	{
		UIScroll::m_ALLWheel = true;
		m_font = CreateFontObject(attrib.fontStyle, GetRectScale().scale());
		OnInitTextSer();
		SetCallback([this](auto&& PH1, auto&& PH2, auto&& PH3)
			{
				OnScrollView(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2),
				std::forward<decltype(PH3)>(PH3));
			});
	}

	UIEditBox::~UIEditBox()
	{
		if (m_font)
			DeleteObject(m_font);
		if (m_ClearBrush)
			DeleteObject(m_ClearBrush);
		if (m_oldPaint.PaintBMP)
			DeleteObject(m_oldPaint.PaintBMP);
		if (m_oldPaint.PaintDC)
			DeleteDC(m_oldPaint.PaintDC);
		if (m_txtHost)
		{
			m_txtHost->GetServices()->OnTxInPlaceDeactivate();
			m_txtHost->GetServices()->Release();
			m_txtHost->Release();
			if (m_txtHost->m_hDll) FreeLibrary(m_txtHost->m_hDll);
		}
		if (CaretTimer)
			UIControl::GetParentWin()->KillTimer(CaretTimer);
	}

	void UIEditBox::SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw)
	{
		if (m_attrib.SetAttribute(attribName, attrib, this))
		{
			m_cacheUpdate = true;
			if (draw)
				UpdateDisplay();
		}
		else
			UIScroll::SetAttribute(attribName, attrib, draw);
	}

	std::wstring UIEditBox::GetAttribute(std::wstring_view attribName)
	{
		if (std::wstring ret; m_attrib.GetAttribute(attribName, ret, this))
			return ret;
		return UIScroll::GetAttribute(attribName);
	}

	void UIEditBox::Register()
	{
		BindAttribute();

		static auto method = [](UIControl* parent)
			{
				return new UIEditBox(parent, Attribute());
			};
		MCTRL_REGISTER(method);
	}

	const UIEditBox::Attribute& UIEditBox::GetAttribute() const
	{
		m_attrib.Set().fontStyle.text = GetCurText();
		return m_attrib.Get();
	}

	void UIEditBox::SetSel(int first, int second, bool bNoScroll)
	{
		CHARRANGE cr = { first, second };
		LRESULT lResult = 0;
		TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult);
		if (bNoScroll)
			TxSendMessage(EM_SCROLLCARET, 0, 0L, &lResult);
	}

	void UIEditBox::ReplaceSel(std::wstring_view text, bool bCanUndo)
	{
		TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)text.data(), nullptr);
	}

	bool UIEditBox::GetWordWrap() const
	{
		return m_attrib.Get().wordWrap;
	}

	void UIEditBox::SetWordWrap(bool wrap)
	{
		m_attrib.Set().wordWrap = wrap;
		MTEXTHOST({ host->SetWordWrap(wrap); });
	}

	bool UIEditBox::GetReadOnly() const
	{
		return m_attrib.Get().readOnly;
	}

	void UIEditBox::SetReadOnly(bool read)
	{
		m_attrib.Set().readOnly = read;
		MTEXTHOST({ host->SetReadOnly(read); });
	}

	int UIEditBox::GetLimitText() const
	{
		return m_attrib.Get().limitText;
	}

	void UIEditBox::SetLimitText(int lenght)
	{
		m_attrib.Set().limitText = lenght;
		MTEXTHOST({ host->LimitText(); });
	}

	void UIEditBox::AppendText(std::wstring_view text, bool canUndo)
	{
		SetSel(-1, -1);
		ReplaceSel(text, canUndo);
	}

	_m_word UIEditBox::GetEditAlign() const
	{
		_m_word ret = 0;
		MTEXTHOST({ ret = host->GetAlign(); });
		return ret;
	}

	void UIEditBox::SetEditAlign(_m_word align)
	{
		m_attrib.Set().editAlign = align;
		MTEXTHOST({ host->SetAlign(align); });
	}

	bool UIEditBox::IsRichMode() const
	{
		return m_attrib.Get().isRich;
	}

	void UIEditBox::SetRichMode(bool rich)
	{
		m_attrib.Set().isRich = rich;
		MTEXTHOST({ host->SetRichTextFlag(rich); });
	}

	void UIEditBox::SetMultiline(bool multiline)
	{
		m_attrib.Set().multiline = multiline;
		MTEXTHOST({ host->SetMultiline(multiline); });
	}

	void UIEditBox::SetNumber(bool number)
	{
		m_attrib.Set().number = number;
	}

	void UIEditBox::SetTextColor(_m_color color)
	{
		m_attrib.Set().fontStyle.fontColor = color;
		MTEXTHOST({ host->SetColor(color); });
	}

	HRESULT UIEditBox::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT* plresult) const
	{
		if (!m_txtHost)
			return S_FALSE;

		LRESULT lr = 0;
		auto task = [&]
			{
				lr = m_txtHost->GetServices()->TxSendMessage(msg, wparam, lparam, plresult);
			};
		if (m_render)
			m_render->RunTask(task);
		else
			task();
		return (HRESULT)lr;
	}

	void UIEditBox::TxServices(std::function<void(ITextServices2*)> task)
	{
		if (!m_txtHost) return;
		if (m_render)
			m_render->RunTask([&] { task(m_txtHost->GetServices()); });
		else
			task(m_txtHost->GetServices());
	}

	void UIEditBox::OnLoadResource(MRenderCmd* render, bool recreate)
	{
		UINodeBase::OnLoadResource(render, recreate);
		CaretBrush = nullptr;
	}

	void UIEditBox::OnPaintProc(MPCPaintParam param)
	{
		if (!m_txtHost || !m_txtHost->m_services) return;

		auto& attrib = m_attrib.Get();

		//设置指针画刷
		SetCaretColor(attrib.caretColor, param->render);

		//绘制Style
		if (attrib.style && m_txtHost->IsITextHost2())
		{
			attrib.style->PaintStyle(param->render, param->destRect, param->blendedAlpha,
				IsEnabled() ? m_controlState : UIControlStatus_Disable, 4, GetRectScale().scale());
		}

		UIRect newRect = GetInsetFrame(*param->destRect);
		//只支持D2D渲染器
		if (std::wstring_view(param->render->GetRenderName()) == L"GDIPlus")
		{
			GDIPaint(param->render, param->destRect, &newRect, param->cacheCanvas);
		}
		if (CaretVisiable && CaretBrush)
		{
			auto rc = GetCaretRect();
			if (UINodeBase::m_data.SubAtlas)
			{
				auto frame = Frame();
				rc.left -= frame.left;
				rc.right -= frame.left;
				rc.top -= frame.top;
				rc.bottom -= frame.top;
				rc.left += param->destRect->left;
				rc.top += param->destRect->top;
				rc.right += param->destRect->left;
				rc.bottom += param->destRect->top;
			}
			param->render->FillRectangle(rc, CaretBrush);
		}
		if (attrib.scroll)
			UIScroll::OnPaintProc(param);
	}

	void UIEditBox::GDIPaint(MRenderCmd* render, MPCRect destRect, MPCRect insetRect, bool cacheCanvas)
	{
		UISize curSize = { (int)UINodeBase::m_data.Frame.GetWidth(), (int)UINodeBase::m_data.Frame.GetHeight() };
		if (curSize.width == 0 || curSize.height == 0)
			return;
		if (curSize != m_oldPaint.bufferSize)
		{
			if (m_oldPaint.PaintBMP)
				DeleteObject(m_oldPaint.PaintBMP);
			if (m_oldPaint.PaintDC)
				DeleteDC(m_oldPaint.PaintDC);
			m_oldPaint.PaintDC = nullptr;
			m_txtHost->SetClientRect((UIRect*)insetRect);
		}
		if (!m_oldPaint.PaintDC)
		{
			HWND hWnd = (HWND)UINodeBase::m_data.ParentWnd->GetWindowHandle();
			HDC wndDC = GetDC(hWnd);
			m_oldPaint.PaintDC = CreateCompatibleDC(wndDC);
			ReleaseDC(hWnd, wndDC);
			BITMAPINFO bmi = {};
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = curSize.width;
			bmi.bmiHeader.biHeight = -curSize.height;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = curSize.width * curSize.height * 32 * 8;

			LPVOID pBmpBits = nullptr;
			m_oldPaint.PaintBMP = CreateDIBSection(wndDC, &bmi, DIB_RGB_COLORS, &pBmpBits, nullptr, 0);

			if (m_oldPaint.PaintBMP)
			{
				memset(pBmpBits, 0, curSize.width * curSize.height * 4);
				SelectObject(m_oldPaint.PaintDC, m_oldPaint.PaintBMP);
			}
			m_oldPaint.bufferSize = curSize;
		}
		auto& attrib = m_attrib.Get();
		if (attrib.style)
		{
			attrib.style->PaintStyle(render, destRect, cacheCanvas ? 255 : UINodeBase::m_data.AlphaDst,
				IsEnabled() ? m_controlState : UIControlStatus_Disable, 4, GetRectScale().scale());
		}

		ALPHAINFO ai;
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = 0;
		bf.SourceConstantAlpha = cacheCanvas ? 255 : UINodeBase::m_data.AlphaDst;

		auto render_ = render->GetBase<MRender_GDIP>();

		HDC pDC = (HDC)render_->GetDC();

		RECT rc = { 0, 0, curSize.width, curSize.height };
		if (!m_ClearBrush)
			m_ClearBrush = CreateSolidBrush(Color::M_RGBA(0, 0, 0, 0));
		FillRect(m_oldPaint.PaintDC, &rc, m_ClearBrush);

		BitBlt(m_oldPaint.PaintDC, 0, 0, curSize.width, curSize.height, pDC, destRect->left, destRect->top, SRCCOPY);

		GdiAlpha::AlphaBackup(m_oldPaint.PaintDC, &rc, ai);

		rc.right = insetRect->GetWidth();
		rc.bottom = insetRect->GetHeight();

		_m_scale scale = GetRectScale().scale();
		const auto inset = UIScroll::GetAttribute().inset;
		rc.left = _scale_to(inset.left, scale.cx);
		rc.top = _scale_to(inset.top, scale.cy);
		rc.right += rc.left;
		rc.bottom += rc.top;

		HRESULT hr = m_txtHost->GetServices()->TxDraw(
			DVASPECT_CONTENT,
			0,
			nullptr,
			nullptr,
			m_oldPaint.PaintDC,
			nullptr,
			(RECTL*)&rc,
			nullptr,
			nullptr,
			nullptr,
			NULL,
			TXTVIEW_ACTIVE);

		GdiAlpha::AlphaRestore(ai);

		AlphaBlend(pDC, destRect->left, destRect->top, curSize.width, curSize.height,
			m_oldPaint.PaintDC, 0, 0, curSize.width, curSize.height, bf);

		render_->ReleaseDC();
	}

	bool UIEditBox::OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam)
	{
		if (!IsEnabled())
			return false;

		LRESULT lResult = 0;
		if (m_txtHost)
		{
			auto message = dynamic_cast<Window::UIWindowsWnd*>(GetParentWin())->ConvertEventCode(code);
			if ((message >= WM_KEYFIRST && message <= WM_KEYLAST) || message == WM_CHAR)
			{
				if ((int)wParam == 67 && GetKeyState(VK_CONTROL) < 0)//Ctrl + C
				{
					Copy();
				}
				if (m_attrib.Get().number && message == WM_CHAR)
				{
					if (wParam < '0' || wParam > '9')
						return TRUE;
				}
				std::wstring curStr = GetCurTextInternal();
				TxSendMessage(message, (WPARAM)wParam, (LPARAM)lParam, &lResult);
				std::wstring changedStr = GetCurTextInternal();
				if (curStr != changedStr)
					OnTextChanged(std::move(changedStr));

				if (message == WM_KEYDOWN)
					SendEvent(Event_Key_Down, wParam);
				else if (message == WM_KEYUP)
					SendEvent(Event_Key_Up, wParam);

				POINT position = { 0 };
				GetCursorPos(&position);
				ScreenToClient((HWND)GetParentWin()->GetWindowHandle(), &position);

				MTEXTSERV(
					{
						serv->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, 0,
						NULL, 0, position.x, position.y);
					});
				CaretVisiable = true;
				UpdateDisplay(true);
			}
			else if (message == WM_SETFOCUS || message == WM_KILLFOCUS)
			{
				if (message == WM_SETFOCUS)
				{
					if (!m_attrib.Get().pholderText.empty()
						&& GetCurTextInternal() == m_attrib.Get().pholderText.view())
					{
						SetSel(0, -1);
						ReplaceSel(L"");
						MTEXTHOST({ host->SetColor(m_attrib.Get().fontStyle.fontColor); });
					}

					ShowCaret(true);
					m_controlState = UIControlStatus_Pressed;
					m_autoWordSel = false;
					if (m_attrib.Get().wordAutoSel)
						SelAllText();
				}
				else
				{
					if (GetCurTextLength(GTL_DEFAULT) == 0 && !m_attrib.Get().pholderText.empty())
					{
						SetSel(0, -1);
						ReplaceSel(m_attrib.Get().pholderText.view());
						MTEXTHOST({ host->SetColor(m_attrib.Get().pholderTextColor); });
					}

					m_controlState = UIControlStatus_Normal;
					ShowCaret(false);
				}
				UpdateDisplay(true);
				if (!m_txtHost->IsITextHost2())
				{
					MTEXTSERV({ serv->OnTxInPlaceActivate(nullptr); });
				}
				TxSendMessage(message, 0, 0, nullptr);
			}

			if (message == WM_IME_COMPOSITION || message == WM_SETFOCUS)
			{
				const HWND hWnd = (HWND)UINodeBase::m_data.ParentWnd->GetWindowHandle();
				if (const HIMC hIMC = ImmGetContext(hWnd))
				{
					const _m_rect caret = GetCaretRect();
					COMPOSITIONFORM Composition;
					Composition.dwStyle = CFS_POINT;
					Composition.ptCurrentPos.x = caret.left;
					Composition.ptCurrentPos.y = caret.bottom + 10;
					ImmSetCompositionWindow(hIMC, &Composition);

					ImmReleaseContext(hWnd, hIMC);
				}
			}
		}

		return UIScroll::OnWindowMessage(code, wParam, lParam);
	}

	bool UIEditBox::OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam)
	{
		if (!UIScroll::OnMouseMessage(message, wParam, lParam))
		{
			switch (message)
			{
			case M_MOUSE_HOVER:
				SetCursor(IDC_IBEAM);
				if (m_controlState != UIControlStatus_Pressed)
				{
					m_controlState = UIControlStatus_Hover;
					UpdateDisplay(true);
				}
				break;
			case M_MOUSE_LEAVE:
				SetCursor(IDC_ARROW);
				if (m_controlState != UIControlStatus_Pressed)
				{
					m_controlState = UIControlStatus_Normal;
					UpdateDisplay(true);
				}
				break;
			case M_MOUSE_LBDOWN:
			case M_MOUSE_RBDOWN:
				CaretVisiable = true;
				m_mouseDown = true;
				UpdateDisplay(true);
				break;
			case M_MOUSE_WHEEL:
			{
				auto cfg = UIScroll::GetAttribute();
				if (cfg.vertical)
					message = (MEventCodeEnum)WM_VSCROLL;
				else if (cfg.horizontal)
					message = (MEventCodeEnum)WM_HSCROLL;
				wParam = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? SB_LINEUP : SB_LINEDOWN;
				lParam = 0;
			}
			break;
			case M_MOUSE_MOVE:
				if (m_mouseDown)
					UpdateDisplay(true);
				break;
			}
			if (m_mouseInScroll)
			{
				m_mouseInScroll = false;
				SetCursor(IDC_IBEAM);
			}
			if (m_txtHost)
			{
				switch (message)
				{
				case M_MOUSE_LBDOWN:
				case M_MOUSE_RBDOWN:
				case M_MOUSE_MBDOWN:
					if (!m_autoWordSel && m_attrib.Get().wordAutoSel)
					{
						m_autoWordSel = true;
						return false;
					}
					break;
				}
				auto srcCode = dynamic_cast<Window::UIWindowsWnd*>(GetParentWin())->ConvertEventCode(message);
				TxSendMessage(srcCode, wParam, lParam, nullptr);
			}
		}
		else if (!m_mouseInScroll && message != M_SETCURSOR)
		{
			m_mouseInScroll = true;
			SetCursor(IDC_ARROW);
		}
		return message == M_SETCURSOR;
	}

	bool UIEditBox::OnSetCursor(_m_param hCur, _m_param lParam)
	{
		::SetCursor((HCURSOR)hCur);
		SendEvent(Event_Control_SetCursor);

		POINT position = { 0 };
		GetCursorPos(&position);
		ScreenToClient((HWND)GetParentWin()->GetWindowHandle(), &position);

		MTEXTSERV({ serv->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, 0,
				NULL, 0, position.x, position.y); });
		return true;
	}

	void UIEditBox::OnTimer(_m_ptrv idTimer)
	{
		CaretVisiable = !CaretVisiable;
		if (UINodeBase::m_data.ParentWnd->GetFocusControl().curFocus != this)
			CaretVisiable = false;
		m_cacheUpdate = true;
		UpdateDisplay();
	}

	void UIEditBox::OnScale(_m_scale scale)
	{
		UIControl::OnScale(scale);
		if (m_font)
			DeleteObject(m_font);
		m_font = CreateFontObject(m_attrib.Get().fontStyle, GetRectScale().scale());
		MTEXTHOST({ host->SetFont(m_font); });
	}

	void UIEditBox::SetCurText(std::wstring_view text)
	{
		m_attrib.Set().fontStyle.text = text;
		SetSel(0, -1);
		if (text.empty() && !m_attrib.Get().pholderText.empty())
		{
			ReplaceSel(m_attrib.Get().pholderText.view(), false);
			MTEXTHOST({ host->SetColor(m_attrib.Get().pholderTextColor); });
		}
		else
			ReplaceSel(text.data(), false);
		m_cacheUpdate = true;
		UpdateDisplay();
	}

	std::wstring UIEditBox::GetCurText() const
	{
		auto text = GetCurTextInternal();
		if (!m_attrib.Get().pholderText.empty() && text == m_attrib.Get().pholderText.view())
			return {};
		return text;
	}

	std::wstring UIEditBox::GetCurSelText() const
	{
		CHARRANGE cr;
		cr.cpMin = cr.cpMax = 0;
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, nullptr);

		LPWSTR lpText = new wchar_t[cr.cpMax - cr.cpMin + 1];
		ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(wchar_t));
		TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, nullptr);

		std::wstring sText = (LPCWSTR)lpText;
		delete[] lpText;
		return sText;
	}

	int UIEditBox::GetCurTextLength(_m_ulong flag) const
	{
		GETTEXTLENGTHEX textLenEx;
		textLenEx.flags = flag;
		textLenEx.codepage = 1200;
		LRESULT lResult = 0;
		TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
		return (int)lResult;
	}

	void UIEditBox::SelAllText()
	{
		return SetSel(0, -1);
	}

	void UIEditBox::SetFontStyle(const UILabel::Attribute& fontStyle)
	{
		if (m_font)
			DeleteObject(m_font);
		m_font = CreateFontObject(fontStyle, GetRectScale().scale());

		MTEXTHOST(
			{
				host->SetFont(m_font);
				if (m_attrib.Get().pholderText.empty() || GetCurTextInternal() != m_attrib.Get().pholderText.view())
					host->SetColor(fontStyle.fontColor);
			});
		m_attrib.Set().fontStyle = fontStyle;
	}

	void UIEditBox::AddColorText(std::wstring_view text, _m_color color)
	{
		CHARFORMAT2W cf{};
		ZeroMemory(&cf, sizeof(cf));

		LRESULT lResult;
		TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);

		cf.cbSize = sizeof(CHARFORMAT2W);
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(GetRValue(color), GetGValue(color), GetBValue(color));

		ReplaceSel(text.data(), FALSE);
		int len = GetCurTextLength(0UL);
		SetSel(len - (int)text.size(), len);

		TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);

		SetSel(-1, -1);

		TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
		TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
	}

	void UIEditBox::SetPassword(bool password)
	{
		m_attrib.Set().password = password;
		MTEXTSERV({ serv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, password ? TXTBIT_USEPASSWORD : 0); });
	}

	void UIEditBox::SetPasswordChar(wchar_t ch)
	{
		m_attrib.Set().passChar = ch;
		MTEXTHOST({ host->SetPasswordChar(ch); });
	}

	bool UIEditBox::Redo()
	{
		LRESULT lResult;
		TxSendMessage(EM_REDO, 0, 0, &lResult);
		return lResult == TRUE;
	}

	bool UIEditBox::Undo()
	{
		LRESULT lResult;
		TxSendMessage(EM_UNDO, 0, 0, &lResult);
		return lResult == TRUE;
	}

	void UIEditBox::Clear()
	{
		SetCurText(L"");
		//TxSendMessage(WM_CLEAR, 0, 0, nullptr);
	}

	void UIEditBox::Copy() const
	{
		const std::wstring curStr = GetCurSelText();
		if (OpenClipboard((HWND)UINodeBase::m_data.ParentWnd->GetWindowHandle()))
		{
			EmptyClipboard();
			HGLOBAL clipBuffer = GlobalAlloc(GMEM_DDESHARE, 2 * curStr.length() + sizeof(wchar_t));
			wchar_t* buffer = (wchar_t*)GlobalLock(clipBuffer);
			wcscpy_s(buffer, curStr.length() + 1, curStr.c_str());
			GlobalUnlock(clipBuffer);
			SetClipboardData(CF_UNICODETEXT, clipBuffer);
			CloseClipboard();
		}
		// TxSendMessage(WM_COPY, 0, 0, 0);
	}

	void UIEditBox::Cut()
	{
		TxSendMessage(WM_CUT, 0, 0, nullptr);
	}

	void UIEditBox::Paste()
	{
		TxSendMessage(WM_PASTE, 0, 0, nullptr);
	}

	void UIEditBox::OnTextChanged(std::wstring str)
	{
		SendEvent(Event_Edit_TextChanged, (_m_param)str.data());
		m_attrib.Set().fontStyle.text = str;
		mslot.textChanged.Emit(str.data());
	}

	void UIEditBox::CreateCaret(int cx, int cy)
	{
		CaretSize = { cx, cy };
	}

	void UIEditBox::ShowCaret(bool show)
	{
		KillTimer(CaretTimer);
		if (show)
			CaretTimer = SetTimer(500);
		CaretVisiable = show;
	}

	void UIEditBox::SetCaretPos(int x, int y)
	{
		CaretPos = { x, y };
	}

	void UIEditBox::SetCaretColor(_m_color color, MRenderCmd* render)
	{
		CaretColor = color;
		if (CaretBrush)
		{
			CaretBrush->SetColor(color);
		}
		else
		{
			CaretBrush = render->CreateBrush(color);
		}
	}

	_m_rect UIEditBox::GetCaretRect()
	{
		return { CaretPos.x + 1, CaretPos.y, CaretPos.x + CaretSize.width + 1, CaretPos.y + CaretSize.height };
	}

	void UIEditBox::OnScrollView(UIScroll*, int dragValue, bool horizontal)
	{
		ShowCaret(false);

		RECT pRect;
		MTEXTHOST({ host->TxGetClientRect(&pRect); });
		UISize size = { pRect.right - pRect.left, pRect.bottom - pRect.top };
		int value = CalcOffsetDragValue(horizontal, dragValue, horizontal ? size.width : size.height);
		if (!horizontal)
			TxSendMessage(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, value), 0, nullptr);
		else
			TxSendMessage(WM_HSCROLL, MAKELONG(SB_THUMBTRACK, value), 0, nullptr);
	}

	void UIEditBox::SetEnabled(bool enable, bool draw)
	{
		if (!enable)
			ShowCaret(false);
		UIControl::SetEnabled(enable, draw);
	}

	HFONT UIEditBox::GetFont()
	{
		return m_font;
	}

	void UIEditBox::BindAttribute()
	{
		auto list =
		{
			MakeUIAttrib(m_attrib, &Attribute::style, L"style"),
			MakeUIAttrib(m_attrib, &Attribute::fontStyle, L"fontStyle",
			[](decltype(m_attrib)::SetData param)
			{
				if (auto style = param.GetValue<UILabel::Attribute*>())
					param.param->SetFontStyle(*style);
				return true;
			},
			[](decltype(m_attrib)::GetData param)
			{
				return CtrlMgr::ConvertAttribPtr(&param.data->fontStyle);
			}),
			MakeUIAttribEx(m_attrib, &UILabel::Attribute::text, &Attribute::fontStyle,  L"text",
			[](decltype(m_attrib)::SetData param)
			{
				param.param->SetCurText(param.attribValue);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::multiline, L"multiline", param,
			{
				param.param->SetMultiline(param.data->multiline);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::password, L"password", param,
			{
				param.param->SetPassword(param.data->password);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::passChar, L"passChar", param,
			{
				param.param->SetPasswordChar(param.data->passChar);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::readOnly, L"readOnly", param,
			{
				param.param->SetReadOnly(param.data->readOnly);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::isRich, L"isRich", param,
			{
				param.param->SetRichMode(param.data->isRich);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::wordWrap, L"wordWrap", param,
			{
				param.param->SetWordWrap(param.data->wordWrap);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::wordAutoSel, L"wordAutoSel", param,
			{
				param.param->MTEXTSERV({ serv->OnTxPropertyBitsChange(TXTBIT_AUTOWORDSEL, param.data->wordAutoSel); });
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::number, L"number", param,
			{
				param.param->SetNumber(param.data->number);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::scroll, L"scroll", param,
			{
				param.param->MTEXTSERV({ serv->OnTxPropertyBitsChange(TXTBIT_SCROLLBARCHANGE, TXTBIT_SCROLLBARCHANGE); });
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::autoBar, L"autoBar", param,
			{
				param.param->MTEXTSERV({ serv->OnTxPropertyBitsChange(TXTBIT_SCROLLBARCHANGE, TXTBIT_SCROLLBARCHANGE); });
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::number, L"number", param,
			{
				param.param->SetNumber(param.data->number);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::limitText, L"limitText", param,
			{
				param.param->SetLimitText(param.data->limitText);
				return true;
			}),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::editAlign, L"editAlign", param,
			{
				param.param->SetEditAlign(param.data->editAlign);
				return true;
			}),
			MakeUIAttrib(m_attrib, &Attribute::caretColor, L"caretColor"),
			MakeUIAttrib_AfterSetOnly(m_attrib, &Attribute::pholderText, L"pholderText", param,
			{
				if (param.param->GetParentWin()->GetFocusControl().curFocus == param.param)
					return true;
				if (param.data->pholderText.empty())
					return true;

				param.param->SetSel(0, -1);
				param.param->ReplaceSel(param.data->pholderText.view());
				param.param->MTEXTHOST({ host->SetColor(param.data->pholderTextColor); });

				return true;
			}),
			MakeUIAttrib(m_attrib, &Attribute::pholderTextColor, L"pholderTextColor")
		};

		decltype(m_attrib)::RegisterAttrib(list);
	}

	void UIEditBox::TextHost(std::function<void(MTextHost*)> task) const
	{
		if (!m_txtHost) return;
		if (m_render)
			m_render->RunTask([&] { task(m_txtHost); });
		else
			task(m_txtHost);
	}

	UIRect UIEditBox::GetInsetFrame(UIRect src)
	{
		_m_scale scale = GetRectScale().scale();
		const auto inset = UIScroll::GetAttribute().inset;
		UIRect newRect = src;
		newRect.left += _scale_to(inset.left, scale.cx);
		newRect.top += _scale_to(inset.top, scale.cy);
		newRect.right -= _scale_to(inset.right, scale.cx);
		newRect.bottom -= _scale_to(inset.bottom, scale.cy);
		return newRect;
	}

	void UIEditBox::OnInitTextSer()
	{
		m_txtHost = new MTextHost();
		if (!m_txtHost)
			return;

		if (m_txtHost->Init(this)
			&& GetCurTextLength(GTL_DEFAULT) == 0 && !m_attrib.Get().pholderText.empty())
		{
			SetCurText(m_attrib.Get().pholderText.view());
			SetTextColor(m_attrib.Get().pholderTextColor);
		}
	}

	std::wstring UIEditBox::GetCurTextInternal() const
	{
		const int len = GetCurTextLength(GTL_DEFAULT);
		GETTEXTEX gt;
		gt.flags = GT_DEFAULT;
		gt.cb = sizeof(wchar_t) * (len + 1);
		gt.codepage = 1200;
		LPTSTR lpText = new wchar_t[len + 1];
		ZeroMemory(lpText, (len + 1) * sizeof(wchar_t));
		gt.lpDefaultChar = nullptr;
		gt.lpUsedDefChar = nullptr;
		TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, nullptr);
		std::wstring sText = lpText;
		delete[] lpText;
		return sText;
	}

#pragma region MTextHost

	const IID IID_ITextServices = { 0x8d33f740, 0xcf58, 0x11ce, { 0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 } };

	const IID IID_ITextServices2 = { 0x8D33F741, 0xCF58, 0x11CE, { 0xA8, 0x9D, 0x00, 0xAA, 0x00, 0x6C, 0xAD, 0xC5 } };

	const IID IID_ITextHost = { 0xc5bdd8d0,0xd26e,0x11ce, { 0xa8,0x9e,0x00,0xaa,0x00,0x6c,0xad,0xc5 } };

	const IID IID_ITextHost2 = { 0x13e670f5,0x1a5a,0x11cf, { 0xab,0xeb,0x00,0xaa,0x00,0xb6,0x5e,0xa1} };

	ITextServices2* UIEditBox::MTextHost::GetServices()
	{
		return m_services;
	}

	void UIEditBox::MTextHost::SetClientRect(UIRect* prc)
	{
		//HIMETRIC_PER_INCH = 2540
		LONG extX = (LONG)MulDiv(prc->GetWidth(), 2540, 96);
		LONG extY = (LONG)MulDiv(prc->GetHeight(), 2540, 96);

		if (extX == m_sizelExtent.cx && extY == m_sizelExtent.cy)
			return;

		m_sizelExtent = { extX, extY };

		m_services->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
	}

	void UIEditBox::MTextHost::SetWordWrap(bool warp)
	{
		m_services->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, warp ? TXTBIT_WORDWRAP : 0);
	}

	void UIEditBox::MTextHost::SetReadOnly(bool read)
	{
		m_services->OnTxPropertyBitsChange(TXTBIT_READONLY, read ? TXTBIT_READONLY : 0);
	}

	void UIEditBox::MTextHost::SetFont(HFONT hFont)
	{
		if (hFont == nullptr) return;

		LOGFONT lf = { 0 };
		GetObjectW(hFont, sizeof(LOGFONT), &lf);
		//LY_PER_INCH = 1440
		m_charFormat.yHeight = -lf.lfHeight * 1440 / 96;

		if (lf.lfWeight >= FW_BOLD)
			m_charFormat.dwEffects |= CFE_BOLD;
		if (lf.lfItalic)
			m_charFormat.dwEffects |= CFE_ITALIC;
		if (lf.lfUnderline)
			m_charFormat.dwEffects |= CFE_UNDERLINE;
		m_charFormat.bCharSet = lf.lfCharSet;
		m_charFormat.bPitchAndFamily = lf.lfPitchAndFamily;

		wcscpy_s(m_charFormat.szFaceName, lf.lfFaceName);

		m_services->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
	}

	void UIEditBox::MTextHost::SetColor(COLORREF color)
	{
		m_charFormat.crTextColor = RGB(GetRValue(color), GetGValue(color), GetBValue(color));
		m_services->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
	}

	void UIEditBox::MTextHost::LimitText()
	{
		m_services->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
	}

	WORD UIEditBox::MTextHost::GetAlign()
	{
		return m_paraFormat.wAlignment;
	}

	void UIEditBox::MTextHost::SetAlign(WORD wNewAlign)
	{
		m_paraFormat.wAlignment = wNewAlign;
		m_services->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}

	void UIEditBox::MTextHost::SetRichTextFlag(bool rich)
	{
		m_services->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, rich ? TXTBIT_RICHTEXT : 0);
	}

	void UIEditBox::MTextHost::SetPasswordChar(WCHAR chars)
	{
		m_services->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, (chars != 0) ? TXTBIT_USEPASSWORD : 0);
	}

	void UIEditBox::MTextHost::SetMultiline(bool multiline)
	{
		m_services->OnTxPropertyBitsChange(TXTBIT_MULTILINE, multiline ? TXTBIT_MULTILINE : 0);
	}

	void UIEditBox::MTextHost::SetBarWidth(_m_ushort width)
	{
		m_services->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);
	}

	HRESULT __stdcall UIEditBox::MTextHost::QueryInterface(REFIID riid, void** ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		*ppvObject = nullptr;

		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost))
		{
			AddRef();
			*ppvObject = dynamic_cast<ITextHost*>(this);
			hr = S_OK;
		}

		return hr;
	}

	ULONG __stdcall UIEditBox::MTextHost::AddRef()
	{
		InterlockedIncrement(&m_dwRef);
		return m_dwRef;
	}

	ULONG __stdcall UIEditBox::MTextHost::Release()
	{
		ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
		if (ulRefCount == 0)
			delete this;
		return ulRefCount;
	}

	BOOL UIEditBox::MTextHost::TxShowScrollBar(INT fnBar, BOOL fShow)
	{
		auto scroll = dynamic_cast<UIScroll*>(m_editBox);
		if (fnBar == SB_VERT)
		{
			scroll->SetAttributeSrc(L"vertical", (bool)fShow);
		}
		else if (fnBar == SB_HORZ)
		{
			if (!m_editBox->m_attrib.Get().multiline)
				return TRUE;
			scroll->SetAttributeSrc(L"horizontal", (bool)fShow);
		}
		else if (fnBar == SB_BOTH)
		{
			scroll->SetAttributeSrc(L"vertical", (bool)fShow, false);
			scroll->SetAttributeSrc(L"horizontal", (bool)fShow);
		}
		return TRUE;
	}

	BOOL UIEditBox::MTextHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
	{
		auto scroll = dynamic_cast<UIScroll*>(m_editBox);
		UIRect frame = m_editBox->GetInsetFrame(m_editBox->Frame());

		if (fnBar == SB_VERT)
		{
			if (nMaxPos - nMinPos - frame.bottom + frame.top <= 0)
			{
				scroll->SetAttributeSrc(L"vertical", false);
			}
			else
			{
				scroll->SetAttributeSrc(L"vertical", true, false);
				scroll->SetAttributeSrc<int>(L"rangeV", nMaxPos - nMinPos);
			}
		}
		else if (fnBar == SB_HORZ)
		{
			if (nMaxPos - nMinPos - frame.right + frame.left <= 0)
			{
				scroll->SetAttributeSrc(L"horizontal", false);
			}
			else
			{
				scroll->SetAttributeSrc(L"horizontal", true, false);
				scroll->SetAttributeSrc<int>(L"rangeH", nMaxPos - nMinPos);
			}
		}
		return TRUE;
	}

	BOOL UIEditBox::MTextHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
	{
		int range = m_editBox->GetRange(fnBar == SB_HORZ);
		RECT pRect;
		TxGetClientRect(&pRect);
		UISize size = { pRect.right - pRect.left, pRect.bottom - pRect.top };
		int srcRange = range;
		//先得到TxHost的可滚动区域
		if (fnBar == SB_HORZ)
			srcRange -= size.width;
		else
			srcRange -= size.height;
		if (nPos > srcRange)
			nPos = srcRange;
		//算出来给的nPos在滚动区域位置的百分比
		float percentage = (float)nPos / (float)srcRange * 100.f;
		//然后按百分比算这个位置在我们的滚动条里的位置
		int value = range * (int)percentage / 100;
		m_editBox->SetDragValueNoAni(fnBar == SB_HORZ, value, fRedraw);
		return TRUE;
	}

	void UIEditBox::MTextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
	{
		//m_editBox->m_cacheUpdate = true;
		//m_editBox->UpdateDisplay();
	}

	void UIEditBox::MTextHost::TxViewChange(BOOL fUpdate)
	{
		m_editBox->UpdateDisplay(true);
	}

	BOOL UIEditBox::MTextHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
	{
		m_editBox->CreateCaret(xWidth, yHeight);
		return TRUE;
	}

	BOOL UIEditBox::MTextHost::TxSetCaretPos(INT x, INT y)
	{
		m_editBox->SetCaretPos(x, y);
		return TRUE;
	}

	void UIEditBox::MTextHost::TxSetCapture(BOOL fCapture)
	{
		if (fCapture)
		{
			m_editBox->SetCapture();
		}
		else
		{
			m_editBox->ReleaseCapture();
		}
	}

	void UIEditBox::MTextHost::TxSetFocus()
	{
		//m_editBox->SetFocus(true);
	}

	void UIEditBox::MTextHost::TxSetCursor(HCURSOR hcur, BOOL fText)
	{
		::SetCursor(hcur);
	}

	BOOL UIEditBox::MTextHost::TxScreenToClient(LPPOINT lppt)
	{
		return ScreenToClient((HWND)m_editBox->UINodeBase::m_data.ParentWnd->GetWindowHandle(), lppt);
	}

	BOOL UIEditBox::MTextHost::TxClientToScreen(LPPOINT lppt)
	{
		return ClientToScreen((HWND)m_editBox->UINodeBase::m_data.ParentWnd->GetWindowHandle(), lppt);
	}

	HRESULT UIEditBox::MTextHost::TxGetClientRect(LPRECT prc)
	{
		UIRect frame = m_editBox->GetInsetFrame(m_editBox->Frame());
		prc->left = frame.left;
		prc->top = frame.top;
		prc->right = frame.right;
		prc->bottom = frame.bottom;
		return S_OK;
	}

	HRESULT UIEditBox::MTextHost::TxGetViewInset(LPRECT prc)
	{
		*prc = { 0,0,0,0 };
		return S_OK;
	}

	HRESULT UIEditBox::MTextHost::TxGetCharFormat(const CHARFORMATW** ppCF)
	{
		*ppCF = &m_charFormat;
		return NOERROR;
	}

	HRESULT UIEditBox::MTextHost::TxGetParaFormat(const PARAFORMAT** ppPF)
	{
		*ppPF = &m_paraFormat;
		return NOERROR;
	}

	COLORREF UIEditBox::MTextHost::TxGetSysColor(int nIndex)
	{
		return GetSysColor(nIndex);
	}

	HRESULT UIEditBox::MTextHost::TxGetBackStyle(TXTBACKSTYLE* pstyle)
	{
		*pstyle = TXTBACK_TRANSPARENT;
		return NOERROR;
	}

	HRESULT UIEditBox::MTextHost::TxGetMaxLength(DWORD* plength)
	{
		*plength = m_editBox->m_attrib.Get().limitText;
		return NOERROR;
	}

	HRESULT UIEditBox::MTextHost::TxGetScrollBars(DWORD* pdwScrollBar)
	{
		auto& attrib = m_editBox->m_attrib.Get();
		if (attrib.scroll)
		{
			*pdwScrollBar |= WS_VSCROLL | WS_HSCROLL;
			if (attrib.autoBar)
				*pdwScrollBar |= ES_AUTOVSCROLL | ES_AUTOHSCROLL;
		}
		return NOERROR;
	}

	HRESULT UIEditBox::MTextHost::TxGetPasswordChar(TCHAR* pch)
	{
		*pch = m_editBox->m_attrib.Get().passChar;
		return NOERROR;
	}

	HRESULT UIEditBox::MTextHost::TxGetExtent(LPSIZEL lpExtent)
	{
		*lpExtent = m_sizelExtent;
		return S_OK;
	}

	HRESULT UIEditBox::MTextHost::TxGetPropertyBits(DWORD dwMask, DWORD* pdwBits)
	{
		DWORD dwProperties = 0;

		const Attribute& config = m_editBox->m_attrib.Get();

		if (config.isRich)
		{
			dwProperties = TXTBIT_RICHTEXT;
		}

		if (config.multiline)
		{
			dwProperties |= TXTBIT_MULTILINE;
		}

		if (config.readOnly)
		{
			dwProperties |= TXTBIT_READONLY;
		}

		if (config.password)
		{
			dwProperties |= TXTBIT_USEPASSWORD;
		}

		dwProperties |= TXTBIT_HIDESELECTION;

		if (config.wordAutoSel)
		{
			dwProperties |= TXTBIT_AUTOWORDSEL;
		}

		if (config.wordWrap)
		{
			dwProperties |= TXTBIT_WORDWRAP;
		}
		//禁用拖拽组件 启用D2D绘制
		dwProperties |= TXTBIT_DISABLEDRAG;

		if (!IsOldVer)
			dwProperties |= TXTBIT_D2DDWRITE;

		*pdwBits = dwProperties & dwMask;
		return NO_ERROR;
	}

	HRESULT UIEditBox::MTextHost::TxGetSelectionBarWidth(LONG* lSelBarWidth)
	{
		*lSelBarWidth = dynamic_cast<UIScroll*>(m_editBox)->GetAttribute().barWidth;
		return S_OK;
	}

	BOOL UIEditBox::MTextHost::Init(UIEditBox* m_pEdit)
	{
		m_editBox = m_pEdit;

		IUnknown* pUnk = nullptr;
		PCreateTextServices TextServicesProc = nullptr;
		auto& attrib = m_editBox->m_attrib.Get();
		HRESULT hr = S_OK;
		if (FAILED(InitDefaultCharFormat(&m_charFormat, NULL))) goto err;

		memset(&m_paraFormat, 0, sizeof(PARAFORMAT2));
		m_paraFormat.cbSize = sizeof(PARAFORMAT2);
		m_paraFormat.dwMask = PFM_ALL;
		m_paraFormat.wAlignment = PFA_LEFT;
		m_paraFormat.cTabCount = 1;
		m_paraFormat.rgxTabs[0] = lDefaultTab;

		//默认在没有窗口的情况下创建的编辑控件是多行的 可以设置段落
		if (!(attrib.editAlign & ES_LEFT))
		{
			if (attrib.editAlign & ES_CENTER)
				m_paraFormat.wAlignment = PFA_CENTER;
			else if (attrib.editAlign & ES_RIGHT)
				m_paraFormat.wAlignment = PFA_RIGHT;
		}

		IsOldVer = true;

		if (!m_hDll)
			m_hDll = LoadLibraryW(L"msftedit.dll");
		TextServicesProc = (PCreateTextServices)GetProcAddress(m_hDll, "CreateTextServices");
		if (m_hDll && TextServicesProc)
		{
			TextServicesProc(nullptr, dynamic_cast<ITextHost*>(this), &pUnk);
		}

		hr = pUnk->QueryInterface(IID_ITextServices, (void**)&m_services);
		pUnk->Release();

		if (FAILED(hr)) goto err;

		//激活
		m_services->OnTxInPlaceActivate(nullptr);
		//设置文本
		if (!attrib.fontStyle.text.empty())
		{
			if (FAILED(m_services->TxSetText(attrib.fontStyle.text.cstr())))
				goto err;
		}

		return TRUE;

	err:
		if (m_hDll) FreeLibrary(m_hDll);
		m_hDll = nullptr;
		return FALSE;
	}

	HRESULT UIEditBox::MTextHost::InitDefaultCharFormat(CHARFORMAT2W* pcf, HFONT hFont)
	{
		LOGFONT lf = { 0 };

		if (hFont == nullptr)
		{
			hFont = m_editBox->GetFont();
			if (hFont == nullptr) return E_FAIL;
		}

		GetObjectW(hFont, sizeof(LOGFONT), &lf);

		const DWORD dwColor = m_editBox->m_attrib.Get().fontStyle.fontColor;
		pcf->cbSize = sizeof(CHARFORMAT2W);
		pcf->crTextColor = RGB(GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor));
		pcf->yHeight = -lf.lfHeight * 1440 / 96;
		pcf->yOffset = 0;
		pcf->dwEffects = 0;
		pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

		if (lf.lfWeight >= FW_BOLD)
			pcf->dwEffects |= CFE_BOLD;
		if (lf.lfItalic)
			pcf->dwEffects |= CFE_ITALIC;
		if (lf.lfUnderline)
			pcf->dwEffects |= CFE_UNDERLINE;

		pcf->bCharSet = lf.lfCharSet;
		pcf->bPitchAndFamily = lf.lfPitchAndFamily;
		wcscpy_s(pcf->szFaceName, lf.lfFaceName);

		return S_OK;
	}
#pragma endregion

#endif // _WIN32
}
