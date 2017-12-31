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

#pragma once

#include "../Window.hpp"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

namespace bmp_renderer { namespace api {
	
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	class WinWindow : public Window
	{
	private:
		friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		HWND m_Hwnd;

		BITMAPINFO m_BmpInfo;
		HBITMAP m_HBitmap;
		unsigned m_BmpWidth;
		unsigned m_BmpHeight;

	public:
		WinWindow(const char* name, unsigned width, unsigned height, WINDOW_ON_EXIT_ACTION onExit);
		~WinWindow();

		bool update() override;
		void loadBitmap(const Bitmap* bitmap) override;

		void setVisibility(bool visible) override;
		bool getVisibility() const override;

		void destroy() override;
		bool isValid() const override;

		inline HWND getHandle() { return m_Hwnd; }
		inline HBITMAP getBitmapHandle() { return m_HBitmap; }
	};

}}
#endif