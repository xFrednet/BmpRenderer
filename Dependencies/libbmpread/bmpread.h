/******************************************************************************
* libbmpread - tiny, fast bitmap (.bmp) image file loader                     *
*              <https://github.com/chazomaticus/libbmpread>                   *
* Copyright (C) 2005, 2012, 2016 Charles Lindsay <chaz@chazomatic.us>         *
*                                                                             *
*  This software is provided 'as-is', without any express or implied          *
*  warranty.  In no event will the authors be held liable for any damages     *
*  arising from the use of this software.                                     *
*                                                                             *
*  Permission is granted to anyone to use this software for any purpose,      *
*  including commercial applications, and to alter it and redistribute it     *
*  freely, subject to the following restrictions:                             *
*                                                                             *
*  1. The origin of this software must not be misrepresented; you must not    *
*     claim that you wrote the original software. If you use this software    *
*     in a product, an acknowledgment in the product documentation would be   *
*     appreciated but is not required.                                        *
*  2. Altered source versions must be plainly marked as such, and must not be *
*     misrepresented as being the original software.                          *
*  3. This notice may not be removed or altered from any source distribution. *
******************************************************************************/


/* bmpread.h
 * version 2.1
 * 2016-07-18
 * 
 * This code was modified by me xFrednet to enable 16 and 32bpp loading.
 */


#ifndef __bmpread_h__
#define __bmpread_h__

#ifdef __cplusplus
extern "C"
{
#endif


/* The struct filled by bmpread.  Holds information about the image's pixels.
 */
typedef struct bmpread_t
{
    int width;  /* width in pixels */
    int height; /* height in pixels */

    /* A buffer holding the pixel data of the image.  Each pixel spans three
     * bytes: the red, green, and blue color components in that order.  The
     * pixels are ordered left to right, bottom line first (unless you passed
     * BMPREAD_TOP_DOWN, in which case the top line is first).  If the image
     * has a width that's not divisible by 4, each line is padded at the end
     * with unused bytes until its effective width is divisible by 4 (this
     * behavior can be turned off by passing BMPREAD_BYTE_ALIGN).
     */
    unsigned char * rgb_data;

} bmpread_t;


/* Flags for bmpread.  Combine with bitwise or: */

/* Output rgb_data as top line first (default is bottom line first). */
#define BMPREAD_TOP_DOWN   1u

/* Don't pad lines so the width is divisible by 4 (default does pad). */
#define BMPREAD_BYTE_ALIGN 2u

/* Allow loading of any size bitmap (default is bitmaps must be 2^n x 2^m). */
#define BMPREAD_ANY_SIZE   4u

/* This loads the alpha channel of the bitmap. 
 * In case of a bitmap that doesn't provide a alpha channel all alpha values will be set to 0xff. */
#define BMPREAD_LOAD_ALPHA 8u

/* Loads the specified bitmap file from disk and fills out a bmpread_t struct
 * with data about it.
 *
 * Inputs:
 * bmp_file - The filename of the bitmap file to load.
 * flags - One or more BMPREAD_* flags (defined above), combined with bitwise
 *         or.  Specify 0 for standard, OpenGL compliant behavior.
 * p_bmp_out - Pointer to a bmpread_t struct to fill with information.
 *
 * Returns:
 * 0 if there's an error (file doesn't exist or is invalid, i/o error, etc.),
 * or nonzero if the file loaded ok.
 *
 * Notes:
 * The file must be a Windows format bitmap file, 1, 4, 8, or 24 bits, and not
 * compressed (no RLE).
 *
 * Default behavior is for bmpread to return rgb_data in a format directly
 * usable by OpenGL texture functions (e.g. glTexImage2D, format GL_RGB, type
 * GL_UNSIGNED_BYTE).  This implies a few oddities:
 *  * Lines are ordered bottom-first.  To return data starting with the top
 *    line like you might otherwise expect, pass BMPREAD_TOP_DOWN in flags.
 *  * Lines are padded to be divisible by 4.  To return data with no padding,
 *    pass BMPREAD_BYTE_ALIGN in flags.
 *  * Images with width or height that aren't an integer power of 2 will fail
 *    to load.  To allow loading images of arbitrary size, pass
 *    BMPREAD_ANY_SIZE in flags.
 * Note that passing any flags may cause the image to be unusable as an OpenGL
 * texture, which may or may not matter to you.
 */
int bmpread(const char * bmp_file, unsigned int flags, bmpread_t * p_bmp_out);


/* Frees memory allocated during bmpread.  Call bmpread_free when you are done
 * using the bmpread_t struct (e.g. after you have passed the data on to
 * OpenGL).
 *
 * Inputs:
 * p_bmp - The pointer you previously passed to bmpread.
 *
 * Returns:
 * void
 */
void bmpread_free(bmpread_t * p_bmp);


#ifdef __cplusplus
}
#endif

#endif
