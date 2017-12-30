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

#include "Bitmap.hpp"

namespace bmp_renderer {

	/* ====================================== */
	// = utility =
	/* ====================================== */
	void FillBitmap(Bitmap* dest, Color fillColor = 0xffffffff);
	void DrawPixel(Bitmap* dest, int xPixel, int yPixel, Color color);
	void SetPixel(Bitmap* dest, int xPixel, int yPixel, Color color); /* ignores alpha values */
	Color GetPixel(Bitmap const* src, int x, int y);

	/* ====================================== */
	// = lines =
	/* ====================================== */
	void DrawLine(Bitmap* dest, int startX, int startY, int endX, int endY, Color color);
	void DrawHorizontalLine(Bitmap* dest, int startX, int startY, int length, Color color);
	void DrawVerticalLine(Bitmap* dest, int startX, int startY, int length, Color color);

	/* ====================================== */
	// = shapes =
	/* ====================================== */
	void DrawRectangle(Bitmap* dest, int x0, int y0, int x1, int y1, Color color);
	void DrawRectangleFilled(Bitmap* dest, int x0, int y0, int x1, int y1, Color color);

	void DrawCircle(Bitmap* dest, int centerX, int centerY, unsigned radius, Color color);
	void DrawCircleFilled(Bitmap* dest, int centerX, int centerY, unsigned radius, Color color);

	/* ====================================== */
	// = bitmap =
	/* ====================================== */
	void DrawBitmap(Bitmap* dest, Bitmap const* src, int destX, int destY);
	void DrawBitmap(Bitmap* dest, Bitmap const* src,
		int destX0, int destY0, int destX1, int destY1);
	void DrawBitmap(Bitmap* dest, Bitmap const* src,
		int destX0, int destY0, int destX1, int destY1, 
		int srcX0 , int srcY0 , int srcX1 , int srcY1 );

	void DrawRotatedBitmap(Bitmap* dest, Bitmap const* src,
		int destX0, int destY0, float angle, float scale = 1.0f, 
		bool clipBorders = false);
}
