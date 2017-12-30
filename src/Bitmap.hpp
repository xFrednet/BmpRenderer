/******************************************************************************
* BmpRenderer - A library that can render and display bitmaps.                *
*                                                                             *
* =========================================================================== *
* Copyright (C) 2017, xFrednet <xFrednet@gmail.com>                           *
*                                                                             *
* This software is provided 'as-is', without any express or implied warranty. *
* In no event will the authors be held liable for any damages arising from    *
* the use of this software.                                                   *
*                                                                             *
* Permission is hereby granted, free of charge, to anyone to use this         *
* software for any purpose(including commercial applications), including the  *
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or *
* sell copies of this software, subject to the following conditions:          *
*                                                                             *
*   1.  The origin of this software must not be misrepresented; you           *
*       must not claim that you wrote the original software. If you           *
*       use this software in a product, an acknowledgment in the              *
*       product documentation would be greatly appreciated but is not         *
*       required                                                              *
*                                                                             *
*   2.  Altered source versions should be plainly marked as such, and         *
*       must not be misrepresented as being the original software.            *
*                                                                             *
*   3.  This code should not be used for any military or malicious            *
*       purposes.                                                             *
*                                                                             *
*   4.  This notice may not be removed or altered from any source             *
*       distribution.                                                         *
*                                                                             *
******************************************************************************/

#pragma once

#define BMP_RENDERER_BITMAP_BASE_COLOR 0xffff00ff

namespace bmp_renderer {
	typedef unsigned char color_channel;

	struct Color {
		union {
			int ARGB;
			struct {
				color_channel B;
				color_channel G;
				color_channel R;
				color_channel A;
			};
		};

		Color(int argb = 0xff00000);
		Color(color_channel r, color_channel g, color_channel b, color_channel a = 0xff);

		Color operator+(Color& other);
		Color& operator+=(Color& other);
	};

	Color Add(const Color& a, const Color& b);

}
namespace bmp_renderer {

	struct Bitmap
	{
		Bitmap() = delete;

		const int WIDTH;
		const int HEIGHT;
		
		Color* Data;
	};
	
	Bitmap* CreateBmp(unsigned width, unsigned height);
	Bitmap* CreateBmp(const char* bmpFile);
	Bitmap* CreateSubBitmap(Bitmap const* src, int srcX0, int srcY0, int srcX1, int srcY1);
	void DeleteBmp(Bitmap* bmp);

	Color SampleBitmap(Bitmap const* bmp, int srcX0, int srcY0, int srcX1, int srcY1);
	Color SampleBitmap(Bitmap const* bmp, float srcX0, float srcY0, float srcX1, float srcY1);

	Bitmap* ReplaceColor(Bitmap const* bmp, Color oldColor, Color newColor);
}
