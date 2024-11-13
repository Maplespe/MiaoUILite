/**
 * FileName: Mui_EditBox.h
 * Note: UI编辑框控件声明
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

#pragma once
#include <Control/Mui_Label.h>
#include <Control/Mui_Scroll.h>

#ifdef _WIN32

#include <Richedit.h>
#include <TextServ.h>
#include <RichOle.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#elif __ANDROID__
#include <Control/Mui_Panel.h>
#endif // _WIN32

namespace Mui::Ctrl
{

#ifndef UIEDITATTRIB
#define UIEDITATTRIB struct Attribute { \
	UIStylePtr style;				 \
	UILabel::Attribute fontStyle;	 \
	bool multiline = false;          \
	bool password = false;           \
	wchar_t passChar = L'*';		 \
	bool readOnly = false;           \
	bool isRich = false;             \
	bool wordWrap = false;           \
	bool wordAutoSel = false;        \
	bool number = false;             \
	bool scroll = false;             \
	bool autoBar = true;             \
									 \
	int limitText = 0x7fffffff;		 \
	_m_word editAlign = 0;			 \
	_m_color caretColor = 0xff000000;\
									 \
	UIString pholderText;			 \
	_m_color pholderTextColor = 0xFF606060;\
	}
#endif

	/* UIEditBox 控件
	* 属性列表:
	* style (UIStyle*)
	* fontStyle (UILabel::Attribute*)	- 字体样式
	* text (std::wstring_view)			- 当前文本
	* multiline (bool)					- 是否为多行编辑框
	* password (bool)					- 是否为密码输入模式
	* passChar (wchar_t)				- 密码遮掩符
	* readOnly (bool)					- 是否为只读模式
	* isRich (bool)						- 是否为富文本模式
	* wordWrap (bool)					- 是否自动折行
	* wordAutoSel (bool)				- 是否自动选择文本
	* number (bool)						- 是否为数字输入模式
	* scroll (bool)						- 是否有滚动条
	* autoBar (bool)					- 自动显示滚动条
	* limitText (int)					- 最大可输入文本数
	* editAlign (_m_word)				- 编辑框布局样式 ES_XXX
	* caretColor (_m_color)				- 指针颜色
	* pholderText (std::wstring_view)	- 占位符文本
	* pholderTextColor (_m_color)		- 占位符文本颜色
	* ...
	* 还继承UIScroll的属性...
	*/
#ifdef _WIN32
	class UIEditBox : public UIScroll
	{
	public:
		MCTRL_DEFINE_EX
		(
			L"UIEditBox",
			MCTRL_MAKE_ATTRIB
			(
				{ CtrlMgr::AttribType::UIStyle, L"style" },
				{ CtrlMgr::AttribType::labelStyle, L"fontStyle" }
			),
			MCTRL_BASE_ATTRIB(UIScroll)
		);

		MCTRL_DEFSLOT
		(
			CtrlMgr::EventSlot<std::wstring_view> textChanged;//文本已被更改
		);

		UIEDITATTRIB;

		UIEditBox(UIControl* parent, Attribute attrib, UIScroll::Attribute scrollAttrib = {});
		~UIEditBox() override;

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

		template<typename T>
		bool SetAttributeSrc(std::wstring_view attribName, T&& value, bool draw = true)
		{
			if (!m_attrib.SetAttribute<T>(attribName, std::forward<T>(value), this))
				return UIScroll::SetAttributeSrc<T>(attribName, std::forward<T>(value), draw);
			m_cacheUpdate = true;
			if (draw) UpdateDisplay();
			return true;
		}
		[[nodiscard]] const Attribute& GetAttribute() const;

		//设置选中
		void SetSel(int first, int second, bool noScroll = false);

		//替换当前所选内容
		void ReplaceSel(std::wstring_view text, bool canUndo = true);

		//是否自动折行
		bool GetWordWrap() const;

		//设置是否自动折行
		void SetWordWrap(bool wrap);

		//是否只读
		bool GetReadOnly() const;

		//设置只读
		void SetReadOnly(bool read);

		//获取最大文本长度限制
		int GetLimitText() const;

		//设置最大文本长度限制
		void SetLimitText(int lenght);

		//追加文本
		void AppendText(std::wstring_view text, bool canUndo = true);

		//获取对齐方式
		_m_word GetEditAlign() const;

		//设置对齐方式
		void SetEditAlign(_m_word align);

		//是否为富文本模式
		bool IsRichMode() const;

		//设置是否支持富文本
		void SetRichMode(bool rich);

		//设置是否支持多行
		void SetMultiline(bool multiline);

		//设置数字输入模式
		void SetNumber(bool number);

		//设置默认字体颜色
		void SetTextColor(_m_color color);

		//设置当前编辑框内容
		void SetCurText(std::wstring_view text);

		//获取当前内容
		std::wstring GetCurText() const;

		//获取当前选中内容
		std::wstring GetCurSelText() const;

		//获取当前内容长度 GETTEXTLENGTHEX flag
		int GetCurTextLength(_m_ulong flag) const;

		//全选内容
		void SelAllText();

		//设置当前字体样式
		void SetFontStyle(const UILabel::Attribute& fontStyle);

		//添加文本 带颜色 需要富文本支持
		void AddColorText(std::wstring_view text, _m_color color);

		//设置密码输入模式
		void SetPassword(bool password);

		//设置密码遮掩符
		void SetPasswordChar(wchar_t ch = '*');

		//重做
		bool Redo();

		//撤销
		bool Undo();

		//清空
		void Clear();

		//复制
		void Copy() const;

		//剪切
		void Cut();

		//粘贴
		void Paste();

		void SetEnabled(bool enable, bool draw = true) override;

	protected:
		explicit UIEditBox(Attribute attrib, UIScroll::Attribute scrollAttrib = {});

		void OnLoadResource(MRenderCmd* render, bool recreate) override;

		//绘制
		void OnPaintProc(MPCPaintParam param) override;

		//消息处理
		bool OnWindowMessage(MEventCodeEnum code, _m_param wParam, _m_param lParam) override;
		bool OnMouseMessage(MEventCodeEnum message, _m_param wParam, _m_param lParam) override;
		bool OnSetCursor(_m_param hCur, _m_param lParam) override;

		void OnTimer(_m_ptrv idTimer) override;

		void OnScale(_m_scale scale) override;

		void OnTextChanged(std::wstring str);

		//光标相关
		void CreateCaret(int cx, int cy);
		void ShowCaret(bool show);
		void SetCaretPos(int x, int y);
		void SetCaretColor(_m_color color, MRenderCmd* render);
		_m_rect GetCaretRect();

		//滚动条
		void OnScrollView(UIScroll*, int dragValue, bool horizontal);

		HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT* plresult) const;
		void TxServices(std::function<void(ITextServices2*)> task);

		class MTextHost : public ITextHost2
		{
			friend class UIEditBox;
		public:
			virtual ~MTextHost() = default;
			ITextServices2* GetServices();

			void SetClientRect(UIRect* prc);
			void SetWordWrap(bool warp);
			void SetReadOnly(bool read);
			void SetFont(HFONT hFont);
			void SetColor(COLORREF color);
			void LimitText();
			WORD GetAlign();
			void SetAlign(WORD align);
			void SetRichTextFlag(bool rich);
			void SetPasswordChar(WCHAR chars);
			void SetMultiline(bool multiline);
			void SetBarWidth(_m_ushort width);
			bool IsITextHost2() { return !IsOldVer; }

			//IUnknown
			HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
			ULONG __stdcall AddRef() override;
			ULONG __stdcall Release() override;

			//ITextHost
			HDC TxGetDC() override { return m_editBox->m_oldPaint.PaintDC; }
			INT TxReleaseDC(HDC hdc) override { return TRUE; }
			BOOL TxShowScrollBar(INT fnBar, BOOL fShow) override;
			BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override { return TRUE; }
			BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) override;
			BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override;
			void TxInvalidateRect(LPCRECT prc, BOOL fMode) override;
			void TxViewChange(BOOL fUpdate) override;
			BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) override;
			BOOL TxShowCaret(BOOL fShow) override { return TRUE; }
			BOOL TxSetCaretPos(INT x, INT y) override;
			BOOL TxSetTimer(UINT idTimer, UINT uTimeout) override { return TRUE; }
			void TxKillTimer(UINT idTimer) override { }
			void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll) override {}
			void TxSetCapture(BOOL fCapture) override;
			void TxSetFocus() override;
			void TxSetCursor(HCURSOR hcur, BOOL fText) override;
			BOOL TxScreenToClient(LPPOINT lppt) override;
			BOOL TxClientToScreen(LPPOINT lppt) override;
			HRESULT TxActivate(LONG* plOldState) override { return S_OK; }
			HRESULT TxDeactivate(LONG lNewState) override { return S_OK; }
			HRESULT TxGetClientRect(LPRECT prc) override;
			HRESULT TxGetViewInset(LPRECT prc) override;
			HRESULT TxGetCharFormat(const CHARFORMATW** ppCF) override;
			HRESULT TxGetParaFormat(const PARAFORMAT** ppPF) override;
			COLORREF TxGetSysColor(int nIndex) override;
			HRESULT TxGetBackStyle(TXTBACKSTYLE* pstyle) override;
			HRESULT TxGetMaxLength(DWORD* plength) override;
			HRESULT TxGetScrollBars(DWORD* pdwScrollBar) override;
			HRESULT TxGetPasswordChar(TCHAR* pch) override;
			HRESULT TxGetAcceleratorPos(LONG* pcp) override { return S_OK; }
			HRESULT TxGetExtent(LPSIZEL lpExtent) override;

			HRESULT OnTxCharFormatChange(const CHARFORMATW* pcf) override { return S_OK; }
			HRESULT OnTxParaFormatChange(const PARAFORMAT* ppf) override { return S_OK; }
			HRESULT TxGetPropertyBits(DWORD dwMask, DWORD* pdwBits) override;
			HRESULT TxNotify(DWORD iNotify, void* pv) override { return S_OK; }
			HIMC TxImmGetContext() override { return nullptr; }
			void TxImmReleaseContext(HIMC himc) override { }
			HRESULT TxGetSelectionBarWidth(LONG* lSelBarWidth) override;

			//ITextHost2
			HRESULT TxDestroyCaret() override { return S_OK; }
			void TxFreeTextServicesNotification() override {  }
			HRESULT TxGetEastAsianFlags(LONG* pFlags) override { return S_OK; }
			HRESULT TxGetEditStyle(DWORD dwItem, DWORD* pdwData) override { return S_OK; }
			HRESULT TxGetHorzExtent(LONG* plHorzExtent) override { return S_OK; }
			HPALETTE TxGetPalette() override { return nullptr; }
			HRESULT TxGetWindow(HWND* phwnd) override { return S_OK; }
			HRESULT TxGetWindowStyles(DWORD* pdwStyle, DWORD* pdwExStyle) override { return S_OK; }
			BOOL TxIsDoubleClickPending() override { return FALSE; }
			HCURSOR TxSetCursor2(HCURSOR hcur, BOOL bText) override { return nullptr; }
			HRESULT TxSetForegroundWindow() override { return S_OK; }
			HRESULT TxShowDropCaret(BOOL fShow, HDC hdc, LPCRECT prc) override { return S_OK; }

			BOOL Init(UIEditBox* m_pEdit);
			HRESULT InitDefaultCharFormat(CHARFORMAT2W* pcf, HFONT hFont);

			HMODULE m_hDll = nullptr;
			LONG m_dwRef = 0;

			ITextServices2* m_services = nullptr;
			SIZEL m_sizelExtent = { 0,0 };
			CHARFORMAT2W m_charFormat = {};
			PARAFORMAT2	m_paraFormat = {};

			UIEditBox* m_editBox = nullptr;

			bool IsOldVer = false;
		};

		MTextHost* m_txtHost = nullptr;

		void TextHost(std::function<void(MTextHost*)> task) const;

		friend class MTextHost;

		UIRect GetInsetFrame(UIRect src);

		struct OldPaintData
		{
			UISize bufferSize;
			HBITMAP PaintBMP;
			HDC PaintDC;
		} m_oldPaint;
		void GDIPaint(MRenderCmd* render, MPCRect destRect, MPCRect insetRect, bool cacheCanvas);

		//ITextHost服务
		void OnInitTextSer();

		std::wstring GetCurTextInternal() const;

		//字体
		HFONT m_font = nullptr;
		HFONT GetFont();

		HBRUSH m_ClearBrush = nullptr;

		mutable CtrlMgr::UIAttribute<Attribute, UIEditBox*> m_attrib;

		static void BindAttribute();

		//光标
		UISize CaretSize;
		UIPoint CaretPos;
		_m_rect CaretRect;
		_m_color CaretColor = 0xff000000;
		MBrushPtr CaretBrush = nullptr;
		bool CaretVisiable = false;
		MTimers::ID CaretTimer = 0;

		UIControlStatus m_controlState = UIControlStatus_Normal;

		//鼠标是否在滚动条区域
		bool m_mouseInScroll = false;
		bool m_mouseDown = false;

		bool m_autoWordSel = false;
	};
#endif
#ifdef __ANDROID__
        class UIEditBox : public UILabel
        {
        public:
            UIEditBox();
            UIEditBox(UIControl* parent, UIEditBoxConfig config);
            virtual ~UIEditBox();

            M_DEF_CTRL(L"UIEditBox");

             void SetProperty(UIEditBoxConfig config, bool draw = true);
            UIEditBoxConfig GetProperty();

            virtual void SetAttribute(std::wstring attribName, std::wstring attrib, bool draw = true) override;
            virtual std::wstring GetAttribute(std::wstring attribName) override;

            //设置选中
            void SetSel(_m_ulong dwSelection, bool bNoScroll = false) { }

            //替换当前所选内容
            void ReplaceSel(std::wstring text, bool bCanUndo = true) { }

            //是否自动折行
            bool GetWordWrap() { return m_config.WordWrap; }

            //设置是否自动折行
            void SetWordWrap(bool fWordWrap) { m_config.WordWrap = fWordWrap; }

            //是否只读
            bool GetReadOnly() { return m_config.ReadOnly; }

            //设置只读
            bool SetReadOnly(bool bReadOnly) { return m_config.ReadOnly = bReadOnly; }

            //获取最大文本长度
            _m_long GetLimitText() { return m_config.LimitText; }

            //设置最大文本长度
            bool SetLimitText(_m_long nLength) { m_config.LimitText = nLength; }

            //追加文本
            void AppendText(std::wstring text, bool bCanUndo = true);

            //获取对齐方式
            _m_word GetDefaultAlign() { return m_align; }

            //设置对齐方式
            void SetDefaultAlign(_m_word wNewAlign);

            //是否为富文本模式
            bool GetRichTextFlag() { return false; }

            //设置是否支持富文本
            void SetRichTextFlag(bool fRich) { m_config.IsRich = fRich; }

            //设置默认字体颜色
            _m_color SetDefaultTextColor(_m_color cr);

            //设置当前编辑框内容
            void SetCurText(std::wstring text);

            //获取当前内容
            std::wstring GetCurText() { return m_config.m_textConfig.Text; }

            //获取当前选中内容
            std::wstring GetCurSelText() { return L""; }

            //获取当前内容长度
            _m_long GetCurTextLength(_m_ulong flag) { return m_config.m_textConfig.Text.length(); }

            //全选内容
            void SelAllText() {}

            //设置是否支持多行
            void SetMultiline(bool multiline);

            //设置数字输入模式
            void SetNumber(bool number);

            //设置当前字体
            void SetFontStyle(UILabel::Property fontStyle);

            //添加文本 带颜色
            void AddColorText(std::wstring text, _m_color color);

            //取滚动条滚动范围
            int GetScrollShift(bool hBar = false) { return 0; };

            //取滚动条滑块位置
            int GetScrollDragPos(bool hBar = false) { return 0; };

            //设置滚动条滑块位置
            void SetScrollDragPos(int pos, bool hBar = false) {  };

            //设置密码输入模式
            void SetPassword(bool password);

            //恢复
            bool Redo() { return true; }

            //撤销
            bool Undo() { return true; }

            //清空
            void Clear() { SetCurText(L""); }

            //复制
            void Copy() {}

            //剪切
            void Cut() {}

            //粘贴
            void Paste() {}

            //绘制
            virtual void OnPaintProc(MRenderCmd* render, MPCRect clipRect, MPCRect destRect, bool cacheCanvas) override;

        protected:
            virtual bool OnMouseEntered(_m_uint flag, const UIPoint& point) override;
            virtual bool OnMouseExited(_m_uint flag, const UIPoint& point) override;
            virtual bool OnLButtonDown(_m_uint flag, const UIPoint& point) override;
            virtual bool OnLButtonUp(_m_uint flag, const UIPoint& point) override;

            UIEditBoxConfig m_config;
            std::wstring  m_passStr;
            _m_word m_align = 0;
            UIControlStatus m_status = UIControlStatus_Normal;
        };
#endif
}
