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

#ifndef __BMPRENDERER_COMMON_H__
#define __BMPRENDERER_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define CLAMP_VALUE(x, min, max) \
if (min <= max) {\
	if (x < min)\
		x = min;\
	else if (x > max)\
		x = max;\
}
#define SWAP_INTS(x, y) \
{\
	int oldValue = x;\
	x = y;\
	y = oldValue; \
}
#define SWAP_FLOATS(x, y) \
{\
	float oldValue = x;\
	x = y;\
	y = oldValue; \
}

#ifdef __cplusplus
}
#endif

#endif /* __BMPRENDERER_COMMON_H__ */