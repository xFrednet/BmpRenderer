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

#include "Bitmap.hpp"

#include <string>
#include "Common.h"
#include "libbmpread/bmpread.h"

namespace bmp_renderer {

	Color::Color(int argb)
		: ARGB(argb)
	{
		
	}
	Color::Color(color_channel r, color_channel g, color_channel b, color_channel a)
		: B(b), G(g), R(r), A(a)
	{
	}

	Color Color::operator+(Color& other)
	{
		return Add(*this, other);
	}
	Color& Color::operator+=(Color& other)
	{
		*this = Add(*this, other);
		return *this;
	}

	Color Add(const Color& a, const Color& b)
	{
		if (b.A == 0xff) 
		{
			return b; 
		}
		else if (b.A == 0x00)
		{
			return a;
		}
		else 
		{
			Color color;
			float alpha = color.A / 255.0f;
			color.R = (color_channel)((a.R * (1 - alpha)) + (b.R * alpha));
			color.G = (color_channel)((a.G * (1 - alpha)) + (b.G * alpha) );
			color.B = (color_channel)((a.B * (1 - alpha)) + (b.B * alpha));
			if (a.A + b.A < 0xff)
				color.A = a.A + b.A;
			else
				color.A = 0xff;

			return color;
		}
	}
}

namespace bmp_renderer {

	Bitmap* CreateBmp(unsigned width, unsigned height)
	{
		Bitmap* bmp = (Bitmap*)malloc(sizeof(Bitmap) + sizeof(Color) * width * height);
		memcpy((void*)&bmp->WIDTH, &width, sizeof(unsigned));
		memcpy((void*)&bmp->HEIGHT, &height, sizeof(unsigned));

		void* data = (void*)(((intptr_t)bmp) + sizeof(Bitmap));
		memcpy(&bmp->Data, &data, sizeof(void*));
		memset(data, 0, sizeof(Color) * width * height);

		return bmp;
	}
	Bitmap* CreateBmp(const char* bmpFile)
	{
		bmpread_t bmpIn;
		if (!bmpread(bmpFile, BMPREAD_TOP_DOWN | BMPREAD_ANY_SIZE | BMPREAD_LOAD_ALPHA, &bmpIn))
		{
			return nullptr;
		}

		Bitmap* bmp = CreateBmp(bmpIn.width, bmpIn.height);
		memset(bmp->Data, 0x00, sizeof(Color) * bmp->WIDTH * bmp->HEIGHT);
		
		int srcIndex = 0;
		for (int destIndex = 0; destIndex < bmp->WIDTH * bmp->HEIGHT; destIndex++, srcIndex += 4)
		{
			bmp->Data[destIndex].R = bmpIn.rgb_data[srcIndex];
			bmp->Data[destIndex].G = bmpIn.rgb_data[srcIndex + 1];
			bmp->Data[destIndex].B = bmpIn.rgb_data[srcIndex + 2];
			bmp->Data[destIndex].A = bmpIn.rgb_data[srcIndex + 3]; //WTF
		}
		
		return bmp;
	}
	Bitmap* CreateSubBitmap(Bitmap const* srcBmp, int srcX0, int srcY0, int srcX1, int srcY1)
	{
		CLAMP_VALUE(srcX0, 0, srcBmp->WIDTH);
		CLAMP_VALUE(srcY0, 0, srcBmp->HEIGHT);
		CLAMP_VALUE(srcX1, 0, srcBmp->WIDTH);
		CLAMP_VALUE(srcY1, 0, srcBmp->HEIGHT);

		int horizontalAdd = (srcX0 < srcX1) ? 1 : -1;
		int verticalAdd   = (srcY0 < srcY1) ? 1 : -1;

		int width  = ((srcX1 - srcX0) * horizontalAdd) + 1;
		int height = ((srcY1 - srcY0) * verticalAdd) + 1;

		Bitmap* destBmp = CreateBmp(width, height);

		int copySrcY = srcY0;
		for (int yOffset = 0; yOffset < height; yOffset++)
		{
			copySrcY += verticalAdd;
			int copySrcYOffset = copySrcY * srcBmp->WIDTH;
			int destYOffset = yOffset * destBmp->WIDTH;

			int copySrcX = srcX0;
			for (int xOffset = 0; xOffset < width; xOffset++)
			{
				copySrcX += horizontalAdd;
				destBmp->Data[xOffset + destYOffset] = srcBmp->Data[copySrcX + copySrcYOffset];
			}
		}

		return destBmp;
	}

	void DeleteBmp(Bitmap* bmp)
	{
		if (!bmp) return;

		free(bmp);
	}

	Color SampleBitmap(Bitmap const* bmp, int srcX0, int srcY0, int srcX1, int srcY1)
	{
		if (srcX1 < srcX0)
		{
			SWAP_INTS(srcX1, srcX0);
		}
		if (srcY1 < srcY0) {
			SWAP_INTS(srcY1, srcY0);
		}

		//bounds check
		if (srcX1 < 0 || srcX0 >= bmp->WIDTH || srcY1 < 0 || srcY0 >= bmp->HEIGHT)
			return Color(0, 0, 0, 0);
		
		//clamp
		if (srcX0 < 0)
			srcX0 = 0;
		if (srcX1 >= bmp->WIDTH)
			srcX1 = bmp->WIDTH - 1;

		if (srcY0 < 0)
			srcY0 = 0;
		if (srcY1 >= bmp->HEIGHT)
			srcY1 = bmp->HEIGHT - 1;

		//sampling
		int totalR = 0;
		int totalG = 0;
		int totalB = 0;
		int totalA = 0;
		int x;
		for (int y = srcY0; y <= srcY1; y++)
		{
			for (x = srcX0; x <= srcX1; x++)
			{
				Color* color = &bmp->Data[x + y * bmp->WIDTH];
				totalR += color->R;
				totalG += color->G;
				totalB += color->B;
				totalA += color->A;
			}
		}

		//result
		int sampleCount = (srcX1 - srcX0 + 1) * (srcY1 - srcY0 + 1);
		return Color(
			totalR / sampleCount,
			totalG / sampleCount,
			totalB / sampleCount,
			totalA / sampleCount);
	}
	Color SampleBitmap(Bitmap const* bmp, float srcX0, float srcY0, float srcX1, float srcY1)
	{
		return SampleBitmap(bmp, (int)floorf(srcX0), (int)floorf(srcY0), (int)floorf(srcX1), (int)floorf(srcY1));
	}

	Bitmap* ReplaceColor(Bitmap const* bmp, Color oldColor, Color newColor)
	{
		Bitmap* newBmp = CreateBmp(bmp->WIDTH, bmp->HEIGHT);

		Color color;
		for (int index = 0; index < bmp->WIDTH * bmp->HEIGHT; index++)
		{
			color = bmp->Data[index];

			if (color.ARGB == oldColor.ARGB)
				newBmp->Data[index] = newColor;
			else
				newBmp->Data[index] = color;
		}

		return newBmp;
	}
}
