#include <iostream>
#include <windows.h>

#include "bitmap.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	LPCTSTR input, output;
	int Width, Height;
	long Size, new_size;

	input = L"/home/ea/Desktop/BitmapRead/Windows/a.bmp";
	
	BYTE* buffer = LoadBMP(&Width, &Height, &Size, input);
	BYTE* raw_intensity = ConvertBMPToIntensity(buffer, Width, Height);
		
	char ch;
	cout << "Sonucu diske kaydetsin mi? E/H:"; cin >> ch;
	if ((ch == 'E') || (ch == 'e')) {

		BYTE* display_imge = ConvertIntensityToBMP(raw_intensity, Width, Height, &new_size);
		output = L"/home/ea/Desktop/BitmapRead/Windows/b.bmp";
		if (SaveBMP(display_imge, Width, Height, new_size, output))
			cout << " Output Image was successfully saved" << endl;
		else cout << "Error on saving image" << endl;
		delete[] display_imge;
	}//


	delete[] buffer;
	delete[] raw_intensity;
	
	return 0;
}