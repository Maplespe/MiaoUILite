#include "ControlTest.h"
#include "MiaoUITest.h"

namespace Mui::Ctrl
{

	UITestCtrl::UITestCtrl(UIControl* parent)
	{
		M_ASSERT(parent)
		parent->AddChildren(this);
		m_cacheSupport = true;
	}

	UITestCtrl::~UITestCtrl()
	{
	}

	void UITestCtrl::OnPaintProc(MPCPaintParam param)
	{
		if (!m_pen)
			m_pen = param->render->CreatePen(10, Color::M_RGBA(255, 0, 0, 255));
		if(!m_staticBrush)
			m_staticBrush = param->render->CreateBrush(Color::M_GREEN);
		if (!m_bitmap)
			m_bitmap = param->render->CreateBitmap(FS::MGetCurrentDir() + L"\\IMG_0065.PNG");

		UIRect dst = UIRect(param->destRect->left, param->destRect->top, 200, 200);
			
		dst.Offset(10, 10);
			
		m_pen->SetWidthAndColor(1, Color::M_RED);
		param->render->DrawLine({ dst.left, dst.top }, { dst.left + 100, dst.top + 100 }, m_pen);

		dst.ResetOffset();
		dst.Offset(200, 10);

		std::pair<_m_color, float> vertex[4];
		vertex[0] = std::make_pair(Color::M_White, 0.f);
		vertex[1] = std::make_pair(Color::M_RED, 1.f);

		m_brush = param->render->CreateGradientBrush(vertex, 2, { 0, 100 }, { 200, 100 });

		param->render->FillRectangle(dst, m_brush);

		vertex[0] = std::make_pair(0, 0.f);
		vertex[1] = std::make_pair(Color::M_Black, 1.f);

		m_brush = param->render->CreateGradientBrush(vertex, 2, { 100, 0 }, { 100, 200 });

		param->render->FillRectangle(dst, m_brush);

		dst.ResetOffset();
		dst.Offset(10, 240);
		param->render->FillEllipse(dst, m_staticBrush);

		dst.ResetOffset();
		dst.Offset(440, 10);
		UIRect geometryRc = dst;
		if (param->cacheCanvas)
		{
			auto sub = UINodeBase::m_data.SubAtlas->GetSubRect();
			geometryRc.Offset(sub.left, sub.top);
		}
		m_geometry = param->render->CreateRoundGeometry(geometryRc, 20.f);

		param->render->PushClipGeometry(m_geometry);

		param->render->DrawBitmap(m_bitmap, 255, *param->destRect);

		//m_pen->SetWidthAndColor(1, Color::M_RGBA(255, 0, 0, 255));
		//render->DrawEllipse(dst_, m_pen);
		//render->FillEllipse(dst_, m_brush);
		param->render->PopClipGeometry();

		dst.ResetOffset();
		dst.Offset(480, 240);
		m_pen->SetWidthAndColor(4, Color::M_RGBA(0, 255, 0, 255));
		//param->render->DrawRoundedRect(dst, 50.f, m_pen);
		param->render->FillRoundedRect(dst, 50.f, m_staticBrush);

		dst.ResetOffset();
		dst.Offset(240, 240);
		geometryRc = dst;
		if (param->cacheCanvas)
		{
			auto sub = UINodeBase::m_data.SubAtlas->GetSubRect();
			geometryRc.Offset(sub.left, sub.top);
		}

		m_geometry = param->render->CreateEllipseGeometry(geometryRc);

		param->render->PushClipGeometry(m_geometry);

		param->render->DrawBitmap(m_bitmap, 255, *param->destRect);

		param->render->PopClipGeometry();

		dst.ResetOffset();
		dst.Offset(720, 240);
		m_pen->SetWidthAndColor(4, Color::M_BLUE);
		param->render->DrawEllipse(dst, m_pen);
	}

	void UITestCtrl::Register()
	{
		static auto method = [](UIControl* parent) {
			return new UITestCtrl(parent);
		};
		CtrlMgr::RegisterControl(ClassName, method);
	}

}
