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

#include "Bitmap.hpp"

#include "Common.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fstream>

#include "../Dependencies/libbmpread/bmpread.h"

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
	Bitmap* LoadBmp(const char* bmpFile)
	{
		bmpread_t bmpIn;
		if (!bmpread(bmpFile, BMPREAD_TOP_DOWN | BMPREAD_ANY_SIZE | BMPREAD_ALPHA, &bmpIn))
		{
			return 0;
		}

		Bitmap* bmp = CreateBmp(bmpIn.width, bmpIn.height);
		memset(bmp->Data, 0x00, sizeof(Color) * bmp->WIDTH * bmp->HEIGHT);
		
		int srcIndex = 0;
		for (int destIndex = 0; destIndex < bmp->WIDTH * bmp->HEIGHT; destIndex++, srcIndex += 4)
		{
			bmp->Data[destIndex].R = bmpIn.data[srcIndex];
			bmp->Data[destIndex].G = bmpIn.data[srcIndex + 1];
			bmp->Data[destIndex].B = bmpIn.data[srcIndex + 2];
			bmp->Data[destIndex].A = bmpIn.data[srcIndex + 3]; //WTF
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

	/* 
	 * Most information was taken from: "http://www.fileformat.info/format/bmp/egff.htm"
	 */
	typedef struct BMP_FILE_HEADER_ {
		uint8_t  Magic[2];    /* Magic bytes 'B' and 'M'. */
		uint32_t FileSize;    /* Size of whole file. */
		uint32_t Reserved;
		uint32_t DataOffset; /* Offset from beginning of file to bitmap data. */
	} BMP_FILE_HEADER;
	typedef struct BMP_FILE_BMP_HEADER_ {
		uint32_t Size;            /* Size of this header in bytes */
		int32_t  Width;           /* Image width in pixels */
		int32_t  Height;          /* Image height in pixels */
		uint16_t Planes;          /* Number of color planes */
		uint16_t BitsPerPixel;    /* Number of bits per pixel */
		uint32_t Compression;     /* Compression methods used */
		uint32_t SizeOfBitmap;    /* Size of bitmap in bytes */
		int32_t  HorzResolution;  /* Horizontal resolution in pixels per meter */
		int32_t  VertResolution;  /* Vertical resolution in pixels per meter */
		uint32_t ColorsUsed;      /* Number of colors in the image */
		uint32_t ColorsImportant; /* Minimum number of important colors */

		uint32_t RedMask;       /* Mask identifying bits of red component */
		uint32_t GreenMask;     /* Mask identifying bits of green component */
		uint32_t BlueMask;      /* Mask identifying bits of blue component */
		uint32_t AlphaMask;     /* Mask identifying bits of alpha component */
	} BMP_FILE_BMP_HEADER;
	
	/*
	 * Why do I need to write extra write methods? well the structs may use padding that
	 * isn't supported by loaders
	 */
	inline int WriteData(std::ofstream* file, void* data, size_t size)
	{
		file->write((const char*)data, size);
		return file->good();
	}
	int WriteBmpFileHeader(std::ofstream* file, BMP_FILE_HEADER* header)
	{
		if (!WriteData(file, &header->Magic[0]  , 1)) return 0;
		if (!WriteData(file, &header->Magic[1]  , 1)) return 0;
		if (!WriteData(file, &header->FileSize  , 4)) return 0;
		if (!WriteData(file, &header->Reserved  , 4)) return 0;
		if (!WriteData(file, &header->DataOffset, 4)) return 0;
		return 1;
	}
	int WriteBmpFileBmpHeader(std::ofstream* file, BMP_FILE_BMP_HEADER* header)
	{
		if (!WriteData(file, &header->Size           , 4)) return 0;
		if (!WriteData(file, &header->Width          , 4)) return 0;
		if (!WriteData(file, &header->Height         , 4)) return 0;
		if (!WriteData(file, &header->Planes         , 2)) return 0;
		if (!WriteData(file, &header->BitsPerPixel   , 2)) return 0;
		if (!WriteData(file, &header->Compression    , 4)) return 0;
		if (!WriteData(file, &header->SizeOfBitmap   , 4)) return 0;
		if (!WriteData(file, &header->HorzResolution , 4)) return 0;
		if (!WriteData(file, &header->VertResolution , 4)) return 0;
		if (!WriteData(file, &header->ColorsUsed     , 4)) return 0;
		if (!WriteData(file, &header->ColorsImportant, 4)) return 0;

		if (!WriteData(file, &header->RedMask        , 4)) return 0;
		if (!WriteData(file, &header->GreenMask      , 4)) return 0;
		if (!WriteData(file, &header->BlueMask       , 4)) return 0;
		if (!WriteData(file, &header->AlphaMask      , 4)) return 0;
		return 1;
	}
	int SaveBitmap(Bitmap const* src, const char* fileName)
	{
		std::ofstream file;
		file.open(fileName, std::ios_base::out | std::ios_base::binary);
		if (!file.is_open())
			return 0;
		
		BMP_FILE_HEADER fileHeader;
		fileHeader.Magic[0] = 'B';
		fileHeader.Magic[1] = 'M';
		fileHeader.FileSize = 
			/*sizeof(BMP_FILE_HEADER) without padding     */ 14 + 
			/*sizeof(BMP_FILE_BMP_HEADER) without padding */ 56 +
			src->WIDTH * src->HEIGHT * sizeof(Color);
		fileHeader.Reserved = 0;
		fileHeader.DataOffset = 14 + 56;
		WriteBmpFileHeader(&file, &fileHeader);

		BMP_FILE_BMP_HEADER bmpHeader;
		bmpHeader.Size            = 56; /*sizeof(BMP_FILE_BMP_HEADER) without padding */
		bmpHeader.Width           = src->WIDTH;
		bmpHeader.Height          = src->HEIGHT;
		bmpHeader.Planes          = 1; /*BMP files contain only one color plane, so this value is always 1. */
		bmpHeader.BitsPerPixel    = 32; /* sizeof(Color) * 8*/
		bmpHeader.Compression     = 3; /* 32 bit bitmaps use bit fields */
		bmpHeader.SizeOfBitmap    = 0; /* This value is typically zero when the bitmap data is uncompressed. */
		bmpHeader.HorzResolution  = 1 * 1000; /* 1px per millimeter */
		bmpHeader.VertResolution  = 1 * 1000; /* 1px per millimeter */
		bmpHeader.ColorsUsed      = 0; /* No color palette */
		bmpHeader.ColorsImportant = 0; /* No color palette */

		bmpHeader.RedMask   = 0x00ff0000;
		bmpHeader.GreenMask = 0x0000ff00;
		bmpHeader.BlueMask  = 0x000000ff;
		bmpHeader.AlphaMask = 0xff000000;
		WriteBmpFileBmpHeader(&file, &bmpHeader);
		
		/* "Scan lines are stored from the bottom up if the value of the Height field in the bitmap header is a positive value[...]
		 * The bottom-up configuration is the most common." <http://www.fileformat.info/format/bmp/egff.htm>
		 * 
		 * So I'll store the bitmap upside down .
		 */
		for (int height = src->HEIGHT - 1; height >= 0; height--)
		{
			if (!WriteData(&file, &src->Data[height * src->WIDTH], src->WIDTH * sizeof(Color))) return 0;
		}

		file.close();

		return 1;
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
