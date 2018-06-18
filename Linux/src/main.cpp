#include <iostream>
#include "bitmap.h"

using namespace std;

#define SAVE 1

int main(int argc, char *argv[])
{
    int height, width;
    long size;
    
    char* in = "/home/ea/Desktop/BitmapRead/Linux/img/a.bmp";
    char* out = "/home/ea/Desktop/BitmapRead/Linux/out/out.bmp";
    BYTE* buffer = LoadBMP(in, &width, &height, &size);

    BYTE* tempBuffer = new BYTE[height*width*3];
    for(int i=0; i<height*width*3; i++)
        tempBuffer[i] = buffer[i];
    BYTE* RGB_Buffer = ConvertRGB(tempBuffer, width, height);

    // print rgb pixel value
    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width*3; j+=3)
            cout<<(int)RGB_Buffer[i*(width*3)+j]<<"\t"<<(int)RGB_Buffer[i*(width*3)+j+1]<<"\t"<<(int)RGB_Buffer[i*(width*3)+j+2]<<"\t";
        cout<<endl;
    }
    cout<<endl;

    // print grey level pixel value
    BYTE* intensity = ConvertBMPToIntensity(buffer, width, height);

    for(int i=0; i<height; i++)
    {
        for(int j=0; j<width; j++)
            cout<<(int)intensity[i*width+j]<<"\t";
        cout<<endl;
    }
    // save
    if(SAVE)
    {
        BYTE *rgb = ConvertBGR(RGB_Buffer, width, height);
        SaveBMP(out, rgb, width, height);
    }
    else
    {
        BYTE* grey_level = ConvertIntensityToBMP(intensity, width, height);
        SaveBMP(out, grey_level, width, height);
    }

    return 0;
}
