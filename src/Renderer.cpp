/******************************************************************************
* BmpRenderer - A library that can render and display bitmaps.                *
*               <https://github.com/xFrednet/BmpRenderer>                     *
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

#include "Renderer.hpp"

#include <string>
#include <math.h>

#include "Common.h"

#define MAX(x, y)                      (((x) > (y)) ? (x) : (y)) 
#define MIN(x, y)                      (((x) < (y)) ? (x) : (y)) 
#define ABS(x)                         (((x) <  0 ) ? (x) : -(x))
#define TO_RAD(x)                      ((3.14f / 180) * x)

namespace bmp_renderer {
	
	inline static void DrawPixelWithoutValidation(Bitmap* dest, const int& x, const int& y, const Color& color)
	{
		if (color.A == 0xff) 
		{
			dest->Data[x + y * dest->WIDTH] = color;
		}
		else if (color.A != 0)
		{
			Color* bmpColor = &dest->Data[x + y * dest->WIDTH];
			float  alpha = color.A / 255.0f;
			
			bmpColor->R = (color_channel)((color.R * alpha) + (bmpColor->R * (1 - alpha)));
			bmpColor->G = (color_channel)((color.G * alpha) + (bmpColor->G * (1 - alpha)));
			bmpColor->B = (color_channel)((color.B * alpha) + (bmpColor->B * (1 - alpha)));
			bmpColor->A += color.A;//TODO does this lap?
			
			if (bmpColor->A > 0xff)
				bmpColor->A = 0xff;
		}
	}
	inline static void SetPixelWithoutValidation(Bitmap* dest, const int& x, const int& y, const Color& color)
	{
		dest->Data[x + y * dest->WIDTH] = color;
	}

	/* ====================================== */
	// = utility =
	/* ====================================== */
	void FillBitmap(Bitmap* dest, Color fillColor)
	{
		if (!dest) return;

		for (int index = 0; index < dest->WIDTH * dest->HEIGHT; index++)
		{
			dest->Data[index] = fillColor;
		}
	}
	void DrawPixel(Bitmap* dest, int xPixel, int yPixel, Color color)
	{
		if (!dest ||
			xPixel < 0 || xPixel >= dest->WIDTH ||
			yPixel < 0 || yPixel >= dest->HEIGHT)
			return;
		
		DrawPixelWithoutValidation(dest, xPixel, yPixel, color);
	}
	void SetPixel(Bitmap* dest, int xPixel, int yPixel, Color color)
	{
		if (!dest ||
			xPixel < 0 || xPixel >= dest->WIDTH ||
			yPixel < 0 || yPixel >= dest->HEIGHT)
			return;

		SetPixelWithoutValidation(dest, xPixel, yPixel, color);
	}
	Color GetPixel(Bitmap const* bmp, int xPixel, int yPixel)
	{
		if (!bmp ||
			xPixel < 0 || xPixel >= bmp->WIDTH ||
			yPixel < 0 || yPixel >= bmp->HEIGHT)
			return Color(0, 0, 0, 0);

		return bmp->Data[xPixel + yPixel * bmp->WIDTH];
	}

	/* ======================================*/
	// = lines =
	/* ======================================*/
	void DrawLine(Bitmap* dest, int startX, int startY, int endX, int endY, Color color)
	{
		if (!dest) return;

		if ((startX < 0 && endX < 0) ||
			(startX >= dest->WIDTH && endX >= dest->WIDTH) ||
			(startY < 0 && endY < 0) ||
			(startY >= dest->HEIGHT && endY >= dest->HEIGHT))
			return;

		//Line : a*x + m
		float horizontalDiff = (float)(endX - startX);
		float verticalDiff   = (float)(endY - startY);
		bool  moveAlongXAxis = ABS(horizontalDiff) > ABS(verticalDiff);

		// the start has to be lower than the end along the selected axis 
		if ((moveAlongXAxis && endX < startX) || //if alongXAxis and startX is higher 
			(!moveAlongXAxis && endY < startY))  //if alongYAxis and startY is higher
		{
			SWAP_INTS(startX, endX);
			SWAP_INTS(startY, endY);

			verticalDiff *= -1;
			horizontalDiff *= -1;
		}

		if (moveAlongXAxis)
		{
			float incrementer = verticalDiff / horizontalDiff;
			float y = (float)startY + 0.5f;
			for (int x = startX; x < endX; x++, y += incrementer)
			{
				DrawPixel(dest, x, (int)floor(y), color);
			}
		} 
		else
		{
			float incrementer = horizontalDiff / verticalDiff;
			float x = (float)startX + 0.5f;
			for (int y = startY; y < endY; y++, x += incrementer) 
			{
				DrawPixel(dest, (int)floor(x), y, color);
			}
		}
	}
	void DrawHorizontalLine(Bitmap* dest, int startX, int drawY, int length, Color color)
	{
		if (!dest) return;
		if (drawY < 0 || drawY >= dest->HEIGHT)
			return;

		if (length < 0)
		{
			length *= -1;
			startX -= length;
		}
		if (startX < 0)
		{
			length += startX;
			startX = 0;
		}
		if (startX + length >= dest->WIDTH)
			length = dest->WIDTH - startX;
		if (length < 0)
			return;

		int drawX;
		for (int xOffset = 0; xOffset < length; xOffset++)
		{
			drawX = startX + xOffset;
			DrawPixelWithoutValidation(dest, drawX, drawY, color);
		}
	}
	void DrawVerticalLine(Bitmap* dest, int drawX, int startY, int length, Color color)
	{
		if (!dest) return;
		if (drawX < 0 || drawX >= dest->WIDTH)
			return;
		
		if (length < 0) {
			length *= -1;
			startY -= length;
		}
		if (startY < 0) {
			length += startY;
			startY = 0;
		}
		if (startY + length >= dest->HEIGHT)
			length = dest->HEIGHT - startY;
		if (length < 0)
			return;

		int drawY;
		for (int yOffset = 0; yOffset < length; yOffset++) {
			drawY = startY + yOffset;
			DrawPixelWithoutValidation(dest, drawX, drawY, color);
		}
	}

	/* ======================================*/
	// = shapes =
	/* ======================================*/
	void DrawRectangle(Bitmap* dest, int x0, int y0, int x1, int y1, Color color)
	{
		if (!dest) return;
		
		int width = x1 - x0;
		int height = y1 - y0;
		
		if (width < 0)
		{
			width *= -1;
			SWAP_INTS(x0, x1);
		} // => x0 < x1
		if (height < 0) {
			height *= -1;
			SWAP_INTS(y0, y1);
		} // => y0 < y1

		if (x1 < 0 || x0 >dest->WIDTH || y1 < 0 || y0 >= dest->HEIGHT)
			return;

		// +---1---+
		// |       |
		// 3       4
		// |       |
		// +---2---+
		DrawHorizontalLine(dest, x0, y0, width, color);
		DrawHorizontalLine(dest, x0, y1, width + 1, color);

		DrawVerticalLine(dest, x0, y0, height, color);
		DrawVerticalLine(dest, x1, y0, height, color);
	}
	void DrawRectangleFilled(Bitmap* dest, int x0, int y0, int x1, int y1, Color color)
	{
		if (!dest) return;
		
		if (x0 > x1)
		{
			SWAP_INTS(x0, x1);
		}
		if (y0 > y1)
		{
			SWAP_INTS(y0, y1);
		}
		
		if (x1 < 0 || x0 >dest->WIDTH || y1 < 0 || y0 >= dest->HEIGHT)
			return;
		
		if (x0 < 0)
			x0 = 0;
		if (x1 >= dest->WIDTH)
			x1 = dest->WIDTH - 1;

		if (y0 < 0)
			y0 = 0;
		if (y1 >= dest->HEIGHT)
			y1 = dest->HEIGHT - 1;

		int drawX;
		for (; y0 <= y1; y0++)
		{
			for (drawX = x0; drawX <= x1; drawX++)
			{
				DrawPixelWithoutValidation(dest, drawX, y0, color);
			}
		}
	}

	// The circle will be drawn along both axis. The used axis is always the one where the other 
	// axis one has only one matching value. The calculations will be done for one quadrant and applied
	// in the others
	//
	// In the following example an "x"s means that it would be drawn along the horizontal-axis
	// and the "y"s mean that the pixel would be drawn along the vertical-axis.
	//   
	// +-------------x
	// | .           x
	// |   .        x
	// |     .    xx
	// |       . x
	// |      yy
	// |   yyy
	// yyyy
	// 
	// The magic join point is at 3/4 at the radius.
	//
	// The x value is defined by using cos()
	// The y value is defined by using sin()
	// -> y = sin(arccos(x))
	// -> x = cos(arcsin(y))
	//
	// BTW: no I didn't find a better way than copping the loop code to every circle method -.-.(#define [...] looked terrible)
	void DrawCircle(Bitmap* dest, int centerX, int centerY, unsigned radius, Color color)
	{
		if (!dest) return;
		if ((centerX + (int)radius) < 0 || centerX - (int)radius >= dest->WIDTH ||
			(centerY + (int)radius) < 0 || centerY - (int)radius >= dest->HEIGHT)
			return;

		//this is art
		float fRadius = (float)radius;
		int mainAxisOffset = 0; // offset along the "moveAxis" from origin
		int sideAxisOffset;
		for (float currentUnit = 0, oneRadiusUnit = (1.0f / fRadius); currentUnit <= 0.75f; currentUnit += oneRadiusUnit, mainAxisOffset++)
		{
			//vertical axis
			sideAxisOffset = (int)roundf((fRadius * sinf(acosf(currentUnit)))); // is positive

			DrawPixel(dest, centerX - mainAxisOffset, centerY + sideAxisOffset, color);
			DrawPixel(dest, centerX + mainAxisOffset, centerY + sideAxisOffset, color);
			DrawPixel(dest, centerX + mainAxisOffset, centerY - sideAxisOffset, color);
			DrawPixel(dest, centerX - mainAxisOffset, centerY - sideAxisOffset, color);

			DrawPixel(dest, centerX - sideAxisOffset, centerY + mainAxisOffset, color);
			DrawPixel(dest, centerX + sideAxisOffset, centerY + mainAxisOffset, color);
			DrawPixel(dest, centerX + sideAxisOffset, centerY - mainAxisOffset, color);
			DrawPixel(dest, centerX - sideAxisOffset, centerY - mainAxisOffset, color);
		}
	}
	void DrawCircleFilled(Bitmap* dest, int centerX, int centerY, unsigned radius, Color color)
	{
		if (!dest) return;
		if ((centerX + (int)radius) < 0 || centerX - (int)radius >= dest->WIDTH ||
			(centerY + (int)radius) < 0 || centerY - (int)radius >= dest->HEIGHT)
			return;

		//this is art
		float fRadius = (float)radius;
		int mainAxisOffset = 0; // offset along the "moveAxis" from origin
		int sideAxisOffset;
		for (float currentUnit = 0, oneRadiusUnit = (1.0f / fRadius); currentUnit <= 0.75f; currentUnit += oneRadiusUnit, mainAxisOffset++)
		{
			//vertical axis
			sideAxisOffset = (int)(fRadius * sinf(acosf(currentUnit))); // is positive

			DrawVerticalLine(dest, centerX - mainAxisOffset, centerY - sideAxisOffset, sideAxisOffset * 2, color);
			DrawVerticalLine(dest, centerX + mainAxisOffset, centerY - sideAxisOffset, sideAxisOffset * 2, color);
			//TODO the center is overdrawn multiple times this my case issues with transparent colors

			DrawHorizontalLine(dest, centerX - sideAxisOffset, centerY - mainAxisOffset, sideAxisOffset * 2, color);
			DrawHorizontalLine(dest, centerX - sideAxisOffset, centerY + mainAxisOffset, sideAxisOffset * 2, color);
		}
	}

	/* ====================================== */
	// = bitmap =
	/* ====================================== */
	void DrawBitmap(Bitmap* dest, Bitmap const* src, int destX, int destY)
	{
		if (!src || !dest) return;

		int xa;
		int ya;
		int xx;
		for (int yy = 0; yy < src->HEIGHT; yy++)
		{
			ya = destY + yy;
			
			if (ya < 0)
				continue;
			if (ya >= dest->HEIGHT)
				break;
			
			for (xx = 0; xx < src->WIDTH; xx++)
			{
				xa = destX + xx;

				if (xa < 0)
					continue;
				if (xa >= dest->WIDTH)
					break;

				DrawPixelWithoutValidation(dest, xa, ya, src->Data[xx + yy * src->WIDTH]);
			}
		}
	}
	void DrawBitmap(Bitmap* dest, Bitmap const* src,
		int destX0, int destY0, int destX1, int destY1)
	{
		DrawBitmap(dest, src, 
			destX0	, destY0, destX1	, destY1, 
			0		, 0		, src->WIDTH, src->HEIGHT);
	}
	void DrawBitmap(Bitmap* dest, Bitmap const* src,
		int destX0, int destY0, int destX1, int destY1,
		int srcX0 , int srcY0 , int srcX1 , int srcY1 )
	{
		/*
		* Validation check
		*/
		if (!src || !dest) return;

		if (destX1 < destX0) {
			SWAP_INTS(destX0, destX1);
			SWAP_INTS(srcX0, srcX1);
		}
		if (destY1 < destY0) {
			SWAP_INTS(destY0, destY1);
			SWAP_INTS(srcY0, srcY1);
		}
		if (destX1 < 0 || destX0 >= dest->WIDTH || destY1 < 0 || destY0 >= dest->HEIGHT)
			return;

		//TODO test performance
		if (srcX0 <= srcX1) if (srcX1 < 0 || srcX0 >= src->WIDTH) return;
		if (srcX0 > srcX1)  if (srcX0 < 0 || srcX1 >= src->WIDTH) return;
		
		if (srcY0 <= srcY1) if (srcY1 < 0 || srcY0 >= src->HEIGHT) return;
		if (srcY1 < srcY0)  if (srcY0 < 0 || srcY1 >= src->HEIGHT) return;

		/*
		* src values
		*/
		float srcXStart = (float)srcX0;
		float srcYStart = (float)srcY0;
		float srcXStepps = (float)(srcX1 - srcX0 + 1) / (float)((destX1 - destX0) + 1);
		float srcYStepps = (float)(srcY1 - srcY0 + 1) / (float)((destY1 - destY0) + 1);

		if (destX0 < 0) {
			srcXStart += srcXStepps * -destX0;
			destX0 = 0;
		}
		if (destX1 >= dest->WIDTH)
			destX1 = dest->WIDTH - 1;
		if (destY0 < 0) {
			srcYStart += srcYStepps * -destY0;
			destY0 = 0;
		}
		if (destY1 >= dest->HEIGHT)
			destY1 = dest->HEIGHT - 1;

		/*
		* Drawing... Art... the thing that this method is all about... (useful comment)
		*/
		int x;
		float srcX;
		float srcY = srcYStart;
		for (int y = destY0; y <= destY1; y++) {
			srcY += srcYStepps;

			srcX = srcXStart;
			for (x = destX0; x <= destX1; x++) {
				srcX += srcXStepps;

				DrawPixelWithoutValidation(dest, x, y, GetPixel(src, (int)floor(srcX), (int)floor(srcY)));
			}
		}
	}

	inline float GetRotX(const float& angle, const float& x, const float& y)
	{
		return x * cosf(angle) + y * -sinf(angle);
	}
	inline float GetRotY(const float& angle, const float& x, const float& y)
	{
		return x * sinf(angle) + y *  cosf(angle);
	}
	inline float GetRotX(const float& angle, const int& x, const int& y)
	{
		return GetRotX(angle, (float)x, (float)y);
	}
	inline float GetRotY(const float& angle, const int& x, const int& y)
	{
		return GetRotY(angle, (float)x, (float)y);
	}
	void DrawRotatedBitmap(Bitmap* dest, Bitmap const* src, int drawX, int drawY, float angle, float scale, bool clipBorders)
	{
		if (!src || !dest || scale == 0.0f) return;

		int xStart = dest->WIDTH;
		int yStart = dest->HEIGHT;
		int xEnd = 0;
		int yEnd = 0;

		/*
		* change the offset to rotate the bitmap around the center
		*/
		drawX += (int)floor((GetRotX(angle, -(float)src->WIDTH / 2.0f, -(float)src->HEIGHT / 2.0f) + (float)src->WIDTH  / 2.0f) * scale);
		drawY += (int)floor((GetRotY(angle, -(float)src->WIDTH / 2.0f, -(float)src->HEIGHT / 2.0f) + (float)src->HEIGHT / 2.0f) * scale);

		/*
		 * draw borders
		 */
		if (clipBorders)
		{
			int exXOffset = (int)floor((GetRotX(angle, -(float)src->WIDTH / 2.0f, -(float)src->HEIGHT / 2.0f) + (float)src->WIDTH  / 2.0f) * scale);
			int exYOffset = (int)floor((GetRotY(angle, -(float)src->WIDTH / 2.0f, -(float)src->HEIGHT / 2.0f) + (float)src->HEIGHT / 2.0f) * scale);
			
			xStart = (0 - exXOffset);
			yStart = (0 - exYOffset);
			xEnd   = ((int)(src->WIDTH  * scale) - exXOffset);
			yEnd   = ((int)(src->HEIGHT * scale) - exYOffset);
		} 
		else
		{
			int points[4][2] = {
				{0, 0}							, {(int)(src->WIDTH * scale), 0							},
				{0, (int)(src->HEIGHT * scale)}	, {(int)(src->WIDTH * scale), (int)(src->HEIGHT * scale)},
			};
			for (int nr = 0; nr < 4; nr++)
			{
				int x = (int)GetRotX(angle, points[nr][0], points[nr][1]);
				int y = (int)GetRotY(angle, points[nr][0], points[nr][1]);

				if (x < xStart)
					xStart = x;
				if (x > xEnd)
					xEnd = x;

				if (y < yStart)
					yStart = y;
				if (y > yEnd)
					yEnd = y;
			}
		}

		/*
		 * boundary check for the draw boundaries
		 */
		CLAMP_VALUE(xStart, -drawX, dest->WIDTH  - drawX - 1);
		CLAMP_VALUE(yStart, -drawY, dest->HEIGHT - drawY - 1);
		CLAMP_VALUE(xEnd  , -drawX, dest->WIDTH  - drawX - 1);
		CLAMP_VALUE(yEnd  , -drawY, dest->HEIGHT - drawY - 1);

		/*
		 * drawing
		 */
		int srcX;
		int srcY;
		int x;
		for (int y = yStart; y <= yEnd; y++)
		{
			for (x = xStart; x <= xEnd; x++)
			{
				srcX = (int)(GetRotX(-angle, x, y) / scale);
				srcY = (int)(GetRotY(-angle, x, y) / scale);

				DrawPixelWithoutValidation(dest, x + drawX, y + drawY, GetPixel(src, srcX, srcY));
			}
		}
	}
}
