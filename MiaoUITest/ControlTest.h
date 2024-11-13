#pragma once
#include <Control/Mui_Control.h>

namespace Mui::Ctrl
{

	class UITestCtrl : public UIControl
	{
	public:
		UITestCtrl(UIControl* parent);
		~UITestCtrl() override;

		//控件类名和注册方法
		MCTRL_DEFNAME(L"UITestCtrl");

	protected:
		UITestCtrl() = default;

		void OnPaintProc(MPCPaintParam param) override;

	private:
		MPenPtr m_pen = nullptr;
		MGradientBrushPtr m_brush = nullptr;
		MBrushPtr m_staticBrush = nullptr;
		MBitmapPtr m_bitmap = nullptr;
		MGeometryPtr m_geometry = nullptr;
	};
}
