#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "bitmap.h"

using namespace std;

#define RGB_LEVEL    1
#define GRAY_LEVEL   1
#define SAVE         1

int main(int argc, char *argv[])
{
    unsigned int height, width, size;

    char in[]  = "img/square.bmp";
    char out[] = "img/out.bmp";

    BYTE *buffer = LoadBMP(in, &width, &height, &size);

    printf( "Bitmap: %s\n"
            "Width: %u\n"
            "Height: %u\n"
            "Size: %u\n\n",
            in,
            width,
            height,
            size);

#if RGB_LEVEL
    for(unsigned int i=0; i<height; i++ ) {
        for(unsigned int j=0; j<width; j++) {
            unsigned int pos = i * (width * 3) + j * 3;
            printf("%u\t%u\t%u\t", buffer[pos], buffer[pos + 1], buffer[pos + 2]);
        }
        printf("\n");
    }
    printf("\n");
#endif // RGB_LEVEL

    BYTE *gray = ConvertBMPToIntensity(buffer, width, height);

#if GRAY_LEVEL
    for(unsigned int i=0; i<height; i++ ) {
        for(unsigned int j=0; j<width; j++) {
            unsigned int pos = i * width + j;
            printf("%u\t", gray[pos]);
        }
        printf("\n");
    }
    printf("\n");
#endif // GRAY_LEVEL

#if SAVE
    BYTE *bitmap = ConvertIntensityToBMP(gray, width, height);
    SaveBMP(out, bitmap, width, height);
#endif // SAVE

    return 0;
}
