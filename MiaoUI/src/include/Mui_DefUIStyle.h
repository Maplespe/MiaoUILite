/**
 * FileName: Mui_DefUIStyle.h
 * Note: UI默认样式
 *
 * Copyright (C) 2024 Maplespe (mapleshr@icloud.com)
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
 * date: 2024-10-1 Create
*/
#pragma once
#include <Mui_XML.h>

#if MUI_MXML_ENABLE_DEFSTYLE

namespace Mui::DefStyle
{
	constexpr auto _g_mui_default_uistyle = MXMLCODE(
		<DefPropGroup control="UIButton" style="_mui_def_btn" minSize="25,25" inset="12,8,12,8" textAlign="5" />
		<DefPropGroup control="UICheckBox" style="_mui_def_chbox" />

		<DefPropGroup control="UIComBox" style="_mui_def_btn" textAlign="5" listStyle="_mui_def_cmlist"
		itemStyle="_mui_def_cmitem" itemHeight="26" autoSize="false" iTextAlign="5" styleV="_mui_def_scroll"
		button="false" barWidth="6" inset="1,1,1,1" />

		<DefPropGroup control="UIEditBox" style="_mui_def_edit" styleV="_mui_def_scroll" inset="5,5,5,5" button="false" />

		<DefPropGroup control="UIListBox" style="_mui_def_list" itemStyle="_mui_def_listitem" lineSpace="2" 
		drawOffset="10,0" iTextAlign="4" styleV="_mui_def_scroll" button="false" 
		barWidth="6" inset="2,2,2,2" />

		<DefPropGroup control="UISlider" trackInset="0,7,0,7" style="_mui_def_strack" btnStyle="_mui_def_sbutton" />
		<DefPropGroup control="UIProgBar" style="_mui_def_progress" />
	);

	inline auto _g_mui_default_uistyle_loadxml(UIResourceMgr* mgr)
	{
		std::wstring xml = MXMLCODE(
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="235,235,235,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="225,225,225,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="245,245,245,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_btn", xml);

		xml = MXMLCODE(
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="235,235,235,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="225,225,225,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="245,245,245,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
			<fill_ellipse rc="5,5,5,5" color="175,175,175,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="245,245,245,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="225,225,225,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
			<fill_ellipse rc="5,5,5,5" color="190,190,190,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="245,245,245,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
			<fill_ellipse rc="5,5,5,5" color="175,175,175,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
			<fill_ellipse rc="5,5,5,5" color="175,175,175,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_chbox", xml);

		xml = MXMLCODE(
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_cmlist", xml);

		xml = MXMLCODE(
		<part>
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,160" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,160" />
			<draw_round rc="0,0,0,0" value="6.0" color="84,164,227,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,160" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
		</part>
		<part>
			<draw_round rc="0,0,0,0" value="6.0" color="84,164,227,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,160" />
			<draw_round rc="0,0,0,0" value="6.0" color="0,116,181,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,160" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,160" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_cmitem", xml);

		xml = MXMLCODE(
		<part />
		<part />
		<part />
		<part />
		<part />
		<part />
		<part />
		<part />
		<part>
			<fill_round rc="0,0,0,0" value="3.0" color="135,135,135,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="3.0" color="150,150,150,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="3.0" color="140,140,140,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="3.0" color="160,160,160,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="3.0" color="255,255,255,100" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="3.0" color="200,200,200,100" />
		</part>
		<part />
		<part />
		);
		mgr->AddGeometryStyle(L"_mui_def_scroll", xml);

		xml = MXMLCODE(
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="130,130,130,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="245,245,245,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="130,130,130,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="255,255,255,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="130,130,130,255" />
			<fill_rect rc="4,b2,4,1" color="130,130,130,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_edit", xml);

		xml = MXMLCODE(
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="230,230,230,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="220,220,220,255" />
			<draw_round rc="0,0,0,0" value="6.0" color="190,190,190,255" width="1" />
			<fill_rect rc="5,b1,5,0" color="200,200,200,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_list", xml);

		xml = MXMLCODE(
		<part />
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="230,230,230,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="235,235,235,255" />
		</part>
		<part />
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="235,235,235,255" />
			<fill_round rc="0,5,l-5,5" value="2.0" color="92,183,255,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="240,240,240,255" />
			<fill_round rc="0,5,l-5,5" value="2.0" color="92,183,255,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="235,235,235,255" />
			<fill_round rc="0,8,l-5,8" value="2.0" color="92,183,255,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="6.0" color="200,200,200,255" />
			<fill_round rc="0,5,l-5,5" value="2.0" color="180,180,180,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_listitem", xml);

		xml = MXMLCODE(
		<part>
			<fill_round rc="0,0,0,0" value="2.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="2.0" color="230,230,230,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="2.0" color="120,195,255,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="2.0" color="170,170,170,255" />
			<draw_round rc="0,0,0,0" value="2.0" color="200,200,200,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="2.0" color="130,130,130,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_strack", xml);

		xml = MXMLCODE(
		<part>
			<fill_ellipse rc="0,0,0,0" color="250,250,250,255" />
			<draw_ellipse rc="0,0,0,0" color="220,220,220,255" width="1" />
			<fill_ellipse rc="6,6,6,6" color="92,183,255,255" />
		</part>
		<part>
			<fill_ellipse rc="0,0,0,0" color="235,235,235,255" />
			<draw_ellipse rc="0,0,0,0" color="200,200,200,255" width="1" />
			<fill_ellipse rc="5,5,5,5" color="120,195,255,255" />
		</part>
		<part>
			<fill_ellipse rc="0,0,0,0" color="225,225,225,255" />
			<draw_ellipse rc="0,0,0,0" color="200,200,200,255" width="1" />
			<fill_ellipse rc="5,5,5,5" color="120,195,255,255" />
		</part>
		<part>
			<fill_ellipse rc="0,0,0,0" color="220,220,220,255" />
			<draw_ellipse rc="0,0,0,0" color="190,190,190,255" width="1" />
			<fill_ellipse rc="6,6,6,6" color="130,130,130,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_sbutton", xml);

		xml = MXMLCODE(
		<part>
			<fill_round rc="0,0,0,0" value="5.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="5.0" color="230,230,230,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="5.0" color="184,217,251,255" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="5.0" color="250,250,250,255" />
			<draw_round rc="0,0,0,0" value="5.0" color="230,230,230,255" width="1" />
		</part>
		<part>
			<fill_round rc="0,0,0,0" value="5.0" color="184,217,251,255" />
		</part>
		);
		mgr->AddGeometryStyle(L"_mui_def_progress", xml);
	}
}

#endif