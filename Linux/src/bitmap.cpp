#include "bitmap.h"

BYTE* LoadBMP(char *bmpfile, unsigned int *width, unsigned int *height, unsigned int *size)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFOHEADER bmpinfo;

	FILE *bmp = fopen(bmpfile, "rb");

	if(bmp == NULL)
	{
		fputs("OPEN", stderr);
        fclose(bmp);
		exit(1);
	}
	// read the bmp file header
	if(fread(&bmpheader, 1, sizeof(BITMAPFILEHEADER), bmp) != sizeof(BITMAPFILEHEADER))
    {
        fputs("FREAD, BITMAPFILEHEADER", stderr);
        fclose(bmp);
        exit(1);
    }
    // read the bmp info header
	if(fread(&bmpinfo, 1, sizeof(BITMAPINFOHEADER), bmp) != sizeof(BITMAPINFOHEADER))
    {
        fputs("FREAD, BITMAPINFOHEADER", stderr);
        fclose(bmp);
        exit(1);
    }
    // check if file is actually a bmp
	if(bmpheader.bfType != 19778) // hexidecimal address is 0x4D42
	{
		fputs("bmpheader.bfType", stderr);
        fclose(bmp);
        exit(1);
	}
    // check if bmp is uncompressed
    if (bmpinfo.biCompress != 0)
    {
        fputs("bmpinfo.biCompress", stderr);
        fclose(bmp);
        exit(1);
    }
    // check if we have 24 bit bmp
	if (bmpinfo.biBitCount != 24)
    {
        fputs("bmpinfo.biBitCount", stderr);
        fclose(bmp);
        exit(1);
    }
    // get image measurements
	*width  = bmpinfo.biWidth;
	*height = bmpinfo.biHeight;

    // create buffer to hold the data
	*size = bmpheader.bfSize - bmpheader.bfOffBits;

    BYTE* buffer = new BYTE[*size];

	// move file pointer to start of bitmap data
	if (fseek(bmp, bmpheader.bfOffBits, SEEK_SET) != 0)
	{
        fputs("FSEEK", stderr);
        exit(1);
	}

	// read bmp data
    if(fread(buffer, 1, (*size), bmp) != (*size))
    {
        fputs("FREAD, BITMAP SIZE", stderr);
        fclose(bmp);
        exit(1);
    }

	fclose(bmp);

    // find the number of padding bytes
	unsigned int padding = 0;
	unsigned int scanlinebytes = (*width) * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	unsigned int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf = new BYTE[(*width) * (*height) * 3];

	// now we loop trough all bytes of the original buffer,
	// swap the R and B bytes and the scanlines
	unsigned int bufpos = 0;
	unsigned int newpos = 0;
	for (unsigned int row = 0; row < (*height); row++) {
        for (unsigned int column = 0; column < (*width); column++) {
            newpos = row * ((*width) * 3) + column * 3;
            bufpos = ((*height) - row - 1) * psw + column * 3;
            newbuf[newpos]     = buffer[bufpos + 2];
            newbuf[newpos + 1] = buffer[bufpos + 1];
            newbuf[newpos + 2] = buffer[bufpos + 0];
        }
	}

	return newbuf;
}

void SaveBMP(char* bmpfile, BYTE* buffer, unsigned int width, unsigned int height)
{
    BITMAPFILEHEADER bmp_head;
    BITMAPINFOHEADER bmp_info;

    // andinitialize them to zero
	memset(&bmp_head, 0, sizeof (BITMAPFILEHEADER));
	memset(&bmp_info, 0, sizeof (BITMAPINFOHEADER));

    unsigned int size = width * height * 3;

    bmp_head.bfType = 0x4D42; // 'BM'
    bmp_head.bfSize = size + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // 24 + head + info no quad
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
    FILE *bmp = fopen(bmpfile, "wb");

    if (bmp == NULL) {

        fputs("OPEN", stderr);
        fclose(bmp);
		exit(1);
    }

    if (fwrite(&bmp_head, 1, sizeof(BITMAPFILEHEADER), bmp) != sizeof(BITMAPFILEHEADER)) {

        fputs("FWRITE, BITMAPFILEHEADER", stderr);
        fclose(bmp);
		exit(1);
    }

    if (fwrite(&bmp_info, 1, sizeof(BITMAPINFOHEADER), bmp) != sizeof(BITMAPINFOHEADER)) {

        fputs("FWRITE, BITMAPINFOHEADER", stderr);
        fclose(bmp);
		exit(1);
    }

    if(fwrite(buffer, 1, size, bmp) != size) {

        fputs("FWRITE, BITMAP SI", stderr);
        fclose(bmp);
		exit(1);
    }
}

BYTE* ConvertBMPToIntensity(BYTE *buffer, unsigned int width, unsigned int height)
{
    // first make sure the parameters are valid
	if ((buffer == NULL) || (width == 0) || (height == 0))
		return NULL;

	BYTE *newbuf = new BYTE[width * height];

	unsigned int pos = 0, newpos = 0;
	for (unsigned int row = 0; row < height; row++) {
        for (unsigned int column = 0; column < width; column++) {
            pos    = row * (width * 3) + column * 3;
            newpos = row * width + column;
            newbuf[newpos] = (BYTE)( 0.3 * buffer[pos] + 0.59 * buffer[pos + 1] + 0.11 * buffer[pos + 2]);
        }
	}

	return newbuf;
}

BYTE* ConvertIntensityToBMP(BYTE *buffer, unsigned int width, unsigned int height)
{
	// first make sure the parameters are valid
	if ((buffer == NULL) || (width == 0) || (height == 0))
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary

	unsigned int padding = 0;
	unsigned int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	unsigned int psw = scanlinebytes + padding;
	// we can already store the size of the new padded buffer
	unsigned int newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[newsize];

	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset(newbuf, 0, sizeof(newsize));

	// now we loop trough all bytes of the original buffer,
	// swap the R and B bytes and the scanlines
	unsigned int bufpos = 0;
	unsigned int newpos = 0;
	for (unsigned int row = 0; row < height; row++){
        for (unsigned int column = 0; column < width; column++) {
            bufpos = row * width + column;                         // position in original buffer
            newpos = (height - row - 1) * psw + column * 3;        // position in padded buffer
            newbuf[newpos]     = buffer[bufpos];                   //  blue
            newbuf[newpos + 1] = buffer[bufpos];                   //  green
            newbuf[newpos + 2] = buffer[bufpos];                   //  red
        }
	}

	return newbuf;
}
