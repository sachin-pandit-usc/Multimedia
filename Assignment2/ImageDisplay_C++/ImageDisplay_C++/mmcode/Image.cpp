//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"


// Constructor and Desctructors
MyImage::MyImage() 
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
	QuantValue = 0;
	outputData = NULL;
}

MyImage::~MyImage()
{
	if ( Data )
		delete Data;
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
	Height = otherImage->Height;
	Width  = otherImage->Width;
	QuantValue = otherImage->QuantValue;
	Data   = new char[Width*Height*3];
	strcpy(otherImage->ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}

}


void MyImage::fillInFinalMatrix(int offsetX, int offsetY, int xVal, int yVal) {
	for (int i = offsetX; i < offsetX + xVal; i++) {
		for (int j = offsetY; j < offsetY + yVal; j++) {
			finalData[i][j] = quantizedMatrix[i][j];
		}
	}
}

void MyImage::constructBlock(float** convData, float** result, int offsetX, int offsetY, int xVal, int yVal) {
	for (int i = offsetX; i < offsetX + xVal; i++) {
		for (int j = offsetY; j < offsetY + yVal; j++) {
			result[i][j] = convData[i][j];
		}
	}
}

void MyImage::convertTo1D(float** dataMatrix) {
	outputData = new char[Width*Height * 3];
	for (int i = 0; i < Height; i++){
		for (int j = 0; j < Width*3; j++) {
			outputData[i * Height + j] = dataMatrix[i][j];
		}
	}
}

void MyImage::convertYPbPrtoRGB() {
	float Y, Pb, Pr;
	rbgOutputValues = new char[Width*Height * 3];
	convertTo1D(finalData);
	
	for (int i = 0; i < Height * Width; i++) {
		Y = outputData[3 * i];
		Pb = outputData[3 * i + 1];
		Pr = outputData[3 * i + 2];
		float r = (float)(1.000 * Y) + (0.000 * Pb) + (1.402 * Pr);
		float g = (float)(1.000 * Y) - (0.334 * Pb) - (0.714 * Pr);
		float b = (float)(1.000 * Y) + (1.772 * Pb) + (0.000 * Pr);
		rbgOutputValues[3 * i] = r;
		rbgOutputValues[3 * i + 1] = g;
		rbgOutputValues[3 * i + 2] = b;
	}
}

void MyImage::convertRGBToYPbPr() {
	float r, g, b;
	char* newData = new char[Width*Height*3];
	for (int i = 0; i < Height * Width ; i++) {		
			r = Data[3 * i];
			g = Data[3 * i + 1];
			b = Data[3 * i + 2];
			float Y = (float)(0.299 * r) + (0.587 * g) + (0.114 * b);
			float Pb = (float)(-0.169 * r) - (0.331 * g) + (0.5 * b);
			float Pr = (float)(0.5 * r) - (0.419 * g) - (0.081 * b);
			newData[3 * i] = Y;
			newData[3 * i + 1] = Pb;
			newData[3 * i + 2] = Pr;
	}	
	convData = convertTo2D(newData,Height, Width);
}

float** MyImage::convertTo2D(char* matrix, int height, int width) {

	int val = width * 3;
	convData = new float* [height];
	for (int i = 0; i < height; i++) {
		convData[i] = new float[val];
	}
	
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			convData[i][j] = (float)matrix[i * width + j];
		}
	}
	return convData;
}

void MyImage::DCT(float** resBlock)
{
	int i, j, u, v;
	dctCoeff = new float*[8];
	for (int i = 0; i < 8; i++) {
		dctCoeff[i] = new float[8];
	}
	for (u = 0; u < 8; ++u) {
		for (v = 0; v < 8; ++v) {
			dctCoeff[u][v] = 0;
			for (i = 0; i < 8; i++) {
				for (j = 0; j < 8; j++) {
					dctCoeff[u][v] += resBlock[i][j] * cos(M_PI / ((float)8)*(i + 1.0 / 2.0)*u)*cos(M_PI / ((float)8)*(j + 1.0 / 2.0)*v);
				}
			}
		}
	}
}

void MyImage::idct(float** DCTMatrix) {
	int i, j, u, v;
	idctCoeff = new float*[8];
	for (int i = 0; i < 8; i++) {
		idctCoeff[i] = new float[8];
	}

	for (u = 0; u < 8; ++u) {
		for (v = 0; v < 8; ++v) {
			idctCoeff[u][v] = 1 / 4.*DCTMatrix[0][0];
			for (i = 1; i < 8; i++) {
				idctCoeff[u][v] += 1 / 2.*DCTMatrix[i][0];
			}
			for (j = 1; j < 8; j++) {
				idctCoeff[u][v] += 1 / 2.*DCTMatrix[0][j];
			}

			for (i = 1; i < 8; i++) {
				for (j = 1; j < 8; j++) {
					idctCoeff[u][v] += DCTMatrix[i][j] * cos(M_PI / ((float)8)*(u + 1. / 2.)*i)*cos(M_PI / ((float)8)*(v + 1. / 2.)*j);
				}
			}
			idctCoeff[u][v] *= 2. / ((float)8)*2. / ((float)8);
		}
	}
}

void MyImage::zigZagTraversal(float** dctCoeff, int diagonal) {

	int dimension = 8;
	quantizedMatrix = new float*[dimension];

	for (int i = 0; i < dimension; i++) {
		quantizedMatrix[i] = new float[dimension];
	}

	int lastValue = dimension * dimension - 1;
	int currNum = 0;
	int currDiag = 0;
	int loopFrom;
	int loopTo;
	int i;
	int row;
	int col;

	do
	{
		if (currDiag < dimension) // if doing the upper-left triangular half
		{
			loopFrom = 0;
			loopTo = currDiag;
		}
		else // doing the bottom-right triangular half
		{
			loopFrom = currDiag - dimension + 1;
			loopTo = dimension - 1;
		}

		for (i = loopFrom; i <= loopTo; i++)
		{
			if (currDiag % 2 == 0) // want to fill upwards
			{
				row = loopTo - i + loopFrom;
				col = i;
			}
			else // want to fill downwards
			{
				row = i;
				col = loopTo - i + loopFrom;
			}
			if (currDiag < diagonal)
				quantizedMatrix[row][col] = dctCoeff[row][col];
			else
				quantizedMatrix[row][col] = 0;
		}

		currDiag++;
	} while (currDiag <= lastValue);
}

void MyImage::generateDCT() {

	float** resBlock = new float*[8];
	float** quantizedMatrix = new float*[8];
	finalData = new float* [Height];
	for (int j = 0; j < Height; j++) {
		finalData[j] = new float[Width * 3];
	}
	
	for (int i = 0; i < 8; i++) {
		resBlock[i] = new float[8];
		quantizedMatrix[i] = new float[8];	
	}
	
	for (int i = 0; i < Height; i = i + 8) {
		for (int j = 0; j < Width * 3; j = j + 8) {
			constructBlock(convData,resBlock,i, j, 8, 8);
			DCT(resBlock);
			zigZagTraversal(dctCoeff,2);
			idct(dctCoeff);
			fillInFinalMatrix(i, j, 8, 8);
		}
	}
}

// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width  = otherImage.Width;
	Data   = new char[Width*Height*3];
	strcpy( (char *)otherImage.ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}
	
	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if ( IN_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Width*Height; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	// Allocate Data structure and copy
	Data = new char[Width*Height*3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;

}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if ( OUT_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3*i];
		Gbuf[i] = Data[3*i+1];
		Rbuf[i] = Data[3*i+2];
	}

	
	// Write data to file
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}
	
	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}




// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
bool MyImage::Modify()
{

	// TO DO by student
	
	// sample operation
	for ( int i=0; i<Width*Height; i++ )
	{
		Data[3*i] = 0;
		Data[3*i+1] = 0;

	}

	return false;
}