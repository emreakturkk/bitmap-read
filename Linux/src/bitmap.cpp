#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include "bitmap.h"

using namespace std;

BYTE* LoadBMP(char* bmpfile, int* width, int* height, long* size)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFOHEADER bmpinfo;
    // open the input bmp for reading in binary mode
	FILE* img = fopen(bmpfile, "rb");
	// make sure the file opens properly
	if(img == NULL)
	{
		fputs("Error reading file", stderr);
        fclose(img);
		exit(1);
	}	
	// read the bmp file header
	if(fread(&bmpheader, sizeof(BITMAPFILEHEADER), 1, img) == false)
    {
        fclose(img);
        exit(1);
    }
    // read the bmp info header
	if(fread(&bmpinfo, sizeof(BITMAPINFOHEADER), 1, img) == false)
    {
        fclose(img);
        exit(1);
    }
    // check if file is actually a bmp
	if(bmpheader.bfType != 19778) // hexidecimal address is 0x4D42
	{
		cout << "File is not of bitmap type!" << endl;
        fclose(img);
		exit(1);
	}
    // check if bmp is uncompressed
    if (bmpinfo.biCompress != 0)  
    {
        fclose(img);
        exit(1);
    }
    // check if we have 24 bit bmp
	if (bmpinfo.biBitCount != 24) 
    {
        fclose(img);
        exit(1);
    }
    // get image measurements
	*width = bmpinfo.biWidth;
	*height = abs(bmpinfo.biHeight);
    
    // create buffer to hold the data
	*size = bmpheader.bfSize - bmpheader.bfOffBits;

    BYTE* buffer = new BYTE[*size];

	// move file pointer to start of bitmap data
	fseek(img, bmpheader.bfOffBits, SEEK_SET);
	// read bmp data
    //fread(buffer, sizeof(BYTE),sizeof(*size), img);
    if(fread(buffer, *size ,1 ,img) == false)
    {
        fclose(img);
        exit(1);
    }

    /*
    //swap the r and b values to get RGB (bitmap is BGR)
    BYTE tempRGB;
    for (int i = 0; i < width*height*3; i+=3)
    {
        tempRGB = buffer[i];
        buffer[i] = buffer[i + 2];
        buffer[i + 2] = tempRGB;
    }
    */

    // everything successful here: close file and return buffer
	fclose(img);

	return buffer;
}

bool SaveBMP(char* bmpfile, BYTE* buffer, int width, int height)
{
    BITMAPFILEHEADER bmp_head;
    BITMAPINFOHEADER bmp_info;

    // andinitialize them to zero
	memset(&bmp_head, 0, sizeof (BITMAPFILEHEADER));
	memset(&bmp_info, 0, sizeof (BITMAPINFOHEADER));

    long size = width * height * 3;

    bmp_head.bfType = 0x4D42; // 'BM'
    bmp_head.bfSize= size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // 24 + head + info no quad    
    bmp_head.bfReserved1 = 0;
    bmp_head.bfReserved2 = 0;
    bmp_head.bfOffBits = bmp_head.bfSize - size;
    // finish the initial of head

    bmp_info.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info.biWidth = width;
    bmp_info.biHeight = height;
    bmp_info.biPlanes = 1;
    bmp_info.biBitCount = 24; // bit(s) per pixel, 24 is true color
    bmp_info.biCompress = 0;
    bmp_info.biSizeImage = size;
    bmp_info.biXPelsPerMeter = 0;
    bmp_info.biYPelsPerMeter = 0;
    bmp_info.biClrUsed = 0 ;
    bmp_info.biClrImportant = 0;
    // finish the initial of infohead;

    // copy the data
    FILE *fp;
    if (!(fp = fopen(bmpfile,"wb"))) return 0;

    fwrite(&bmp_head, sizeof(BITMAPFILEHEADER),1 , fp);
    fwrite(&bmp_info, sizeof(BITMAPINFOHEADER), 1, fp);
    fwrite(buffer, size, 1, fp);
    fclose(fp);

    return 1;
}

BYTE* ConvertRGB(BYTE* buffer, int width, int height)
{
    BYTE* tBuffer = new BYTE[height * width * 3];
    
    int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)
		padding++;
    int psw = scanlinebytes + padding;
    
    long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
        for (int column = 0; column < width; column++)  { 
                bufpos = (height - row - 1) * psw + column * 3;
                tBuffer[newpos]     = buffer[bufpos + 2]; // red
                tBuffer[newpos + 1] = buffer[bufpos + 1]; // green
                tBuffer[newpos + 2] = buffer[bufpos];     // blue
                newpos += 3;
            }

	return tBuffer;
}

BYTE* ConvertBGR(BYTE* buffer, int width, int height)
{
   
	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;
	// we can already store the size of the new padded buffer
	long newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[newsize];

	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset(newbuf, 0, sizeof(newsize));

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
        for (int column = 0; column < width; column++){
            newpos = (height - row - 1) * psw + column * 3; // position in padded buffer
            newbuf[newpos]     = buffer[bufpos + 2]; //  blue
            newbuf[newpos + 1] = buffer[bufpos + 1]; //  green
            newbuf[newpos + 2] = buffer[bufpos];     //  red
            bufpos += 3;
        }

	return newbuf;
}

BYTE* ConvertBMPToIntensity(BYTE* Buffer, int width, int height)
{
	// first make sure the parameters are valid
	if ((Buffer == NULL) || (width == 0) || (height == 0))
		return NULL;

	// find the number of padding bytes

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf = new BYTE[width*height];

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
        for (int column = 0; column < width; column++)  {
                newpos = row * width + column;
                bufpos = (height - row - 1) * psw + column * 3;
                newbuf[newpos] = (BYTE)(0.11*Buffer[bufpos + 2] + 0.59*Buffer[bufpos + 1] + 0.3*Buffer[bufpos]);
            }

	return newbuf;
}//ConvetBMPToIntensity

BYTE* ConvertIntensityToBMP(BYTE* Buffer, int width, int height)
{
	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;
	// we can already store the size of the new padded buffer
	long newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[newsize];

	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset(newbuf, 0, sizeof(newsize));

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
	for (int column = 0; column < width; column++)  	{
		bufpos = row * width + column;     // position in original buffer
		newpos = (height - row - 1) * psw + column * 3;           // position in padded buffer
		newbuf[newpos] = Buffer[bufpos];       //  blue
		newbuf[newpos + 1] = Buffer[bufpos];   //  green
		newbuf[newpos + 2] = Buffer[bufpos];   //  red
	}

	return newbuf;
} //ConvertIntensityToBMP