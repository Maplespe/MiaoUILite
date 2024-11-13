/**
 * FileName: Mui_ImgBox.h
 * Note: UI图片框控件声明
 *
 * Copyright (C) 2020-2023 Maplespe (mapleshr@icloud.com)
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
 * date: 2020-10-23 Create
*/

#pragma once
#include <Control/Mui_Control.h>

namespace Mui::Ctrl
{
	/* UIButton 控件
	* 属性列表:
	* img (UIBitmap*)		- 图片
	* imgStyle (ImgStyle)	- 图片样式
	*/
	class UIImgBox : public UIControl
	{
	public:
		MCTRL_DEFINE
		(
			L"UIImgBox",
			{ CtrlMgr::AttribType::UIBitmap, L"img" }
		);

		UIImgBox(UIControl* parent);

		//图片样式
		enum ImgStyle
		{
			ImageBoxStyle_Zoom,		//缩放
			ImageBoxStyle_Original, //原始尺寸
			ImageBoxStyle_Center,	//居中
			ImageBoxStyle_ZoomFill	//缩放并填充
		};

		//取图像尺寸
		UISize GetImageSize() const;

		//设置图像
		void SetImage(UIBitmapPtr img, bool lowQuality = false, bool draw = true);

		//设置图像呈现样式
		void SetImageStyle(ImgStyle style, bool draw = true);

		//设置图像质量
		void SetQuality(bool lowQuality, bool draw = true);

		void SetAttribute(std::wstring_view attribName, std::wstring_view attrib, bool draw = true) override;
		std::wstring GetAttribute(std::wstring_view attribName) override;

	protected:
		UIImgBox() { m_cacheSupport = true; }

		void OnPaintProc(MPCPaintParam param) override;

		_m_sizef GetContentSize() override;

		ImgStyle m_style = ImageBoxStyle_Zoom;
		UIBitmapPtr m_image = nullptr;
		bool m_isLowQuality = false;
	};

}
