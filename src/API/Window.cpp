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

#include "Window.hpp"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

namespace bmp_renderer
{
	bool DrawOnWindow(void* window, int drawX, int drawY, Bitmap* src)
	{
		HBITMAP winBmp;
		HWND hwnd = (HWND)window;

		if (hwnd && src && src->WIDTH == 0 && src->HEIGHT == 0)
			return false;

		/*
		 * Bitmap
		 */
		{
			BITMAPINFO bmpInfo;
			HDC dc;
			RGBQUAD* pPixels = nullptr;

			memset(&bmpInfo, 0, sizeof(BITMAPINFO));
			bmpInfo.bmiHeader.biSize        = sizeof(BITMAPINFO);
			bmpInfo.bmiHeader.biWidth       = src->WIDTH;
			bmpInfo.bmiHeader.biHeight      = src->HEIGHT;
			bmpInfo.bmiHeader.biPlanes      = 1;
			bmpInfo.bmiHeader.biBitCount    = 32;
			bmpInfo.bmiHeader.biCompression = BI_RGB;

			dc = GetDC(hwnd);
			if (!dc)
				return false;
			winBmp = CreateDIBSection(dc, &bmpInfo, DIB_RGB_COLORS, (void**)&pPixels, NULL, 0);
			if (!winBmp)
				return false;

			SetDIBits(dc, winBmp, 0, src->HEIGHT, src->Data, &bmpInfo, DIB_RGB_COLORS);

			ReleaseDC(hwnd, dc);
		}

		/*
		 * Draw on screen
		 */
		{
			PAINTSTRUCT ps;
			HDC dc, memDC;
			HGDIOBJ memDcOldObject;

			dc = BeginPaint(hwnd, &ps);

			memDC = CreateCompatibleDC(dc);
			memDcOldObject = SelectObject(memDC, winBmp);

			BitBlt(dc, drawX, drawY, src->WIDTH, src->HEIGHT, memDC, 0, 0, SRCCOPY);
		
			SelectObject(memDC, memDcOldObject);
			DeleteDC(memDC);
			EndPaint(hwnd, &ps);
			
			RECT size = {(LONG)drawX, (LONG)drawY, (LONG)src->WIDTH, (LONG)src->HEIGHT};
			RedrawWindow(hwnd, &size, nullptr, RDW_INVALIDATE);
		}

		DeleteObject(winBmp);
		
		return true;
	}
}

#else

namespace bmp_renderer
{
	bool DrawOnWindow(void* window, int drawX, int drawY, Bitmap* src)
	{
		return false;
	}
}

#endif
