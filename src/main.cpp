/*
============================================================================
= Sobel.c implements the Edge Detection with Sobel Algorithm               =
= ------------------------------------------------------------------------ =
= Inputs (both inputs are hardwired in the source code):                                                                  =
=   - Image: file bmp image                                                =
=   - Desired Threshold: T, integer from 0 to 255                          =
= Output:                                                                  =
=   - Image with Edges Detected: "Output_Sobel_<T>.bmp"                           =
= Run:                                                                     =
=  copy-paste the input image.bmp to _project_name_/_project_name_ folder  =
=  same folder where the .cpp file is located                              = 
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#define THRESHOLD 90   // input threshold for sobel 

// bmp file header 
#pragma pack(push, 1)
typedef struct tagBitmapFileHeader {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} BitmapFileHeader;
#pragma pack(pop)

// bmp extra file header
#pragma pack(push, 1)
typedef struct tagBitmapInfoHeader {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} BitmapInfoHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {                      /**** Colormap entry structure ****/
	unsigned char  rgbBlue;          /* Blue value */
	unsigned char  rgbGreen;         /* Green value */
	unsigned char  rgbRed;           /* Red value */
	unsigned char  rgbReserved;      /* Reserved */
} RGBQUAD;
#pragma pack(pop)

RGBQUAD image[2048][2048];          // Image as input  - just use a big table to store the colors
unsigned char ee_image[2048][2048]; // To store the annotations (output) of the edge detection 
int gray_image[2048][2048];         // To hold the Grayscale image
int grey_image[2048][2048];         // To hold the Grayscale image

// the following function must be implemented in assembly
extern "C" int bmptogray_conversion(int, int, RGBQUAD input_color[2048][2048], int output_gray[2048][2048]);   // first function to be implemented in assembly 
extern "C" int sobel_detection(int, int, int input_gray_image[2048][2048], unsigned char output_ee_image[2048][2048]); // second function to be implemented in assembly 
extern "C" int border_pixel_calculation(int, int, unsigned char ee_image[2048][2048]);  // third  function to be implemented in assembly 

int main()
{
	FILE* fp;
	FILE* wp;
	unsigned int width, height;
	unsigned int x, y;
	BitmapFileHeader bmfh;
	BitmapInfoHeader bmih;

	/*
=================================================================================================
= It seems that visual studio believes that fopen is not safe anymore                           =
= to turn-off this:                                                                             =
=    i) project --> properties                                                                  =
= 	 ii) in the dialog, chose Configuration Properties->C/C++->Preprocessor                     =
= 	 iii) in the field PreprocessorDefinitions append the following: _CRT_SECURE_NO_WARNINGS    = 
= The above step will allow you to use the fopen function                                       =
=================================================================================================
    */

	// Opening the file: using "rb" mode to read this *binary* file
	fp = fopen("input.bmp", "rb");
	if (fp == NULL)
	{
		printf("error when reading the file");
		return 0;
	}

	printf("file opened \n");
	// Reading the file header and any following bitmap information...
	fread(&bmfh, sizeof(BitmapFileHeader), 1, fp);
	fread(&bmih, sizeof(BitmapInfoHeader), 1, fp);

	printf("Header Info\n");
	printf("--------------------\n");
	printf("Size:%i\n", bmfh.bfSize);
	printf("Offset:%i\n", bmfh.bfOffBits);
	printf("--------------------\n");
	printf("Size:%i\n", bmih.biSize);
	printf("biWidth:%i\n", bmih.biWidth);
	printf("biHeight:%i\n", bmih.biHeight);
	printf("biPlanes:%i\n", bmih.biPlanes);
	printf("biBitCount:%i\n", bmih.biBitCount);
	printf("biCompression:%i\n", bmih.biCompression);
	printf("biSizeImage:%i\n", bmih.biSizeImage);
	printf("biXPelsPerMeter:%i\n", bmih.biXPelsPerMeter);
	printf("biYPelsPerMeter:%i\n", bmih.biYPelsPerMeter);
	printf("biClrUsed:%i\n", bmih.biClrUsed);
	printf("biClrImportant:%i\n", bmih.biClrImportant);
	printf("--------------------\n");



	// Extract the width & height from bmp header info
	width = bmih.biWidth; if (width % 4 != 0) width += (4 - width % 4);
	height = bmih.biHeight;
	
	//DEBUG----
	//printf("%i\n", sizeof(image[0]));
	//printf("%i\n", sizeof(gray_image[0]));
	//DEBUG/////

	// Reading the pixels of input image
	for (y = 0; y < height; y++) 
		for (x = 0; x < width; x++) 
		{
			image[x][y].rgbBlue = fgetc(fp);
			image[x][y].rgbGreen = fgetc(fp);
			image[x][y].rgbRed = fgetc(fp);
		}
	
	fclose(fp);

	memset(gray_image, 244, width * height * sizeof(int));    // not really neccesary

	// coverting the input RGB bmp to grayscale image (not black and white)
	bmptogray_conversion(height, width, image, gray_image);
	//bmptogrey_conversion(height, width, image, grey_image);

	// Edge Detection with Sobel  
	sobel_detection(height, width, gray_image, ee_image);

	// Calculating the border pixels with replication
	border_pixel_calculation(height, width, ee_image);

	printf("The edges of the image have been detected with Sobel and a Threshold: %d\n", THRESHOLD);

			
	//-------------------------------------------------------------
	// Creating the final image in (pseudo)bmp format 
	//-------------------------------------------------------------
	char dst_name[80];  // Constructing output image name
	char str_T[3]; // Converting input threshold to string
	sprintf(str_T, "%d", THRESHOLD);

	strcpy(dst_name, "Output_Sobel_");
	strcat(dst_name, str_T);
	strcat(dst_name, ".bmp");

	// Writing new image
	wp = fopen(dst_name, "wb");
	// write the file and the bmp information to the file
	fwrite(&bmfh, 1, sizeof(BitmapFileHeader), wp);
	fwrite(&bmih, 1, sizeof(BitmapInfoHeader), wp);
		
    	// write the sobel annotations pixel-by-pixel
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++) 
		{
			fputc(ee_image[x][y], wp);
			fputc(ee_image[x][y], wp);
			fputc(ee_image[x][y], wp);  // write th same value in all RGB channels 
		}

	fclose(wp);    // I am done here!!! 

	return 0;
}

// first function to be implemented in assembly
// coverting the input RGB bmp to grayscale image (not black and white)
//int bmptogray_conversion(int height, int width, RGBQUAD input_color[2048][2048], int output_gray[2048][2048])   
//{
//	// check for valid inputs -- skipped 
//
//	for (int y = 0; y < height; y++)
//		for (int x = 0; x < width; x++)
//		{
//			// simple formula to generate a gray scale image from a colored image
//			output_gray[x][y] = (int) (0.2989 * input_color[x][y].rgbRed + 
//				                       0.5870 * input_color[x][y].rgbGreen + 
//				                       0.1140 * input_color[x][y].rgbBlue);
//		}
//
//	return 0;
//}

// second function to be implemented in assembly
// Edge Detection with Sobel 
//int sobel_detection(int height, int width, int input_gray_image[2048][2048], unsigned char output_ee_image[2048][2048])
//{
//	// check for valid inputs -- skipped 
//
//	// Sobel kernels
//	double S1[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };    	// mask for x direction
//	double S2[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };       // mask for y direction
//
//	// Scanning the image
//	for (int x = 0; x < width - 2; x++)
//		for (int y = 0; y < height - 2; y++) {
//			//double Gx = S1[0][0] * gray_image[x - 1][y - 1] + S1[0][1] * gray_image[x - 1][y] + S1[0][2] * gray_image[x - 1][y + 1] +
//			//	S1[1][0] * gray_image[x][y - 1] + S1[1][1] * gray_image[x][y] + S1[1][2] * gray_image[x][y + 1] +
//			//	S1[2][0] * gray_image[x + 1][y - 1] + S1[2][1] * gray_image[x + 1][y] + S1[2][2] * gray_image[x + 1][y + 1];
//
//			//double Gy = S2[0][0] * gray_image[x - 1][y - 1] + S2[0][1] * gray_image[x - 1][y] + S2[0][2] * gray_image[x - 1][y + 1] +
//			//	S2[1][0] * gray_image[x][y - 1] + S2[1][1] * gray_image[x][y] + S2[1][2] * gray_image[x][y + 1] +
//			//	S2[2][0] * gray_image[x + 1][y - 1] + S2[2][1] * gray_image[x + 1][y] + S2[2][2] * gray_image[x + 1][y + 1];
//			double Gx=0;
//			for(int i=0; i<3; i++) for(int j=0; j<3; j++) Gx+= S1[i][j]*gray_image[x+i][y+j];
//
//			double Gy=0;
//			for(int i=0; i<3; i++) for(int j=0; j<3; j++) Gy+= S1[i][j]*gray_image[x+i][y+j];
//
//			double e = sqrt(Gx * Gx + Gy * Gy);
//
//			// Thresholding -- just write black and white
//			if (e <= THRESHOLD) ee_image[x+1][y+1] = 0;
//			if (e > THRESHOLD)  ee_image[x+1][y+1] = 255;
//		} // End of image scanning
//	   	 	
//	return 0;
//}

// third function to be implemented in assembly
// Calculating the border pixels with replication
//int border_pixel_calculation(int height, int width, unsigned char ee_image[2048][2048])
//{
//	// check for valid inputs -- skipped 
//	
//	for (int y = 1; y < height - 1; y++)
//	{
//		ee_image[0][y] = ee_image[1][y];
//		ee_image[width - 1][y] = ee_image[width - 2][y];
//	}
//	for (int x = 0; x < width; x++) {
//		ee_image[x][0] = ee_image[x][1];
//		ee_image[x][height - 1] = ee_image[x][height - 2];
//	}
//
//	
//
//	return 0;
//}
