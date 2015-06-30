#include "stdafx.h"
#include "hc3dToolkit.h"
#include <iostream>

using namespace hc3d;

void hc3dToolkit::XYZtoBMP() {
	std::string xyzName = "canada.XYZ"; std::string bmpName = "bmpFile.bmp"; size_t w = 25000; size_t h = 25000;
	FILE *pFile = fopen(bmpName.c_str(), "wb");
	FILE *xyz = fopen(xyzName.c_str(), "r");

	if (pFile == NULL)
	{
		return;
	}

	BITMAPINFOHEADER BMIH;

	BMIH.biSize = sizeof(BITMAPINFOHEADER);

	BMIH.biSizeImage = w * h * 3;

	// Create the bitmap for this OpenGL context

	BMIH.biSize = sizeof(BITMAPINFOHEADER);

	BMIH.biWidth = w;

	BMIH.biHeight = h;

	BMIH.biPlanes = 1;

	BMIH.biBitCount = 24;

	BMIH.biCompression = BI_RGB;

	BMIH.biSizeImage = w * h * 3;

	BITMAPFILEHEADER bmfh;

	int nBitsOffset = sizeof(BITMAPFILEHEADER)+BMIH.biSize;

	LONG lImageSize = BMIH.biSizeImage;

	LONG lFileSize = nBitsOffset + lImageSize;

	bmfh.bfType = 'B' + ('M' << 8);

	bmfh.bfOffBits = nBitsOffset;

	bmfh.bfSize = lFileSize;

	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

	//Write the bitmap file header

	UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1,

		sizeof(BITMAPFILEHEADER), pFile);

	//And then the bitmap info header

	UINT nWrittenInfoHeaderSize = fwrite(&BMIH,

		1, sizeof(BITMAPINFOHEADER), pFile);

	//Finally, write the image data itself 

	//-- the data represents our drawing

	int counter = 0;
	float curB = -1;
	int fileSize = 0;
	for (;;) {
		float a, b, c;
		if (counter == 1) std::cout << ((float)fileSize / float(w*h)) * 100.0 << "%\n";
		if (counter >= w) {
			while (1) {
				fscanf(xyz, "%f%f%f", &a, &b, &c);
				if (curB != b) {
					curB = b;
					counter = 1;
					auto vec = heightToRgb(c);
					char buf[3];
					buf[0] = vec.z;
					buf[1] = vec.y;
					buf[2] = vec.x;
					fwrite(&buf, 3, 1, pFile);
					fileSize++;
					break;
				}
			}
			continue;
		}
		fscanf(xyz, "%f%f%f", &a, &b, &c);
		if (curB == -1) curB = b;
		auto vec = heightToRgb(c);
		char buf[3];
		buf[0] = vec.z;
		buf[1] = vec.y;
		buf[2] = vec.x;
		fwrite(&buf, 3, 1, pFile);
		fileSize++;
		if (fileSize >= w*h) break;
		if (curB != b) {
			curB = b;
	//		std::cout << counter << std::endl;
			counter = 0;
		}
		else counter++;
		
	}

	fclose(pFile);



}