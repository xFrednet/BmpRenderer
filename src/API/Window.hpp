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

#include "../Bitmap.hpp"

namespace bmp_renderer {
	
	enum WINDOW_ON_EXIT_ACTION
	{
		WINDOW_ON_EXIT_DESTROY = 0,
		WINDOW_ON_EXIT_HIDE,
		WINDOW_ON_EXIT_MINIMIZE,
		WINDOW_ON_EXIT_DO_NOTHING 
	};

	class Window
	{
	public:
		static Window* CreateInstance(char const* name, unsigned width, unsigned height, WINDOW_ON_EXIT_ACTION onExit = WINDOW_ON_EXIT_DESTROY);

	protected:
		WINDOW_ON_EXIT_ACTION m_OnExitAction;
		
		Window(WINDOW_ON_EXIT_ACTION onExit);
	public:
		virtual ~Window() {/* bye bye */}

		virtual bool update() = 0;
		virtual void loadBitmap(const Bitmap* bitmap) = 0;

		virtual void setVisibility(bool visible) = 0;
		virtual bool getVisibility() const = 0;

		virtual void destroy() = 0;
		virtual bool isValid() const = 0;

		WINDOW_ON_EXIT_ACTION getOnExitAction() const;
		void setOnExitAction(WINDOW_ON_EXIT_ACTION action);
	};
}
