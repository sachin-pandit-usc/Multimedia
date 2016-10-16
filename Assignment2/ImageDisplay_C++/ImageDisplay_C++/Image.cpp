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
	if (Data)
		delete Data;
}


// Copy constructor
MyImage::MyImage(MyImage *otherImage)
{
	Height = otherImage->Height;
	Width = otherImage->Width;
	QuantValue = otherImage->QuantValue;
	Data = new char[Width*Height * 3];
	strcpy(otherImage->ImagePath, ImagePath);

	for (int i = 0; i<(Height*Width * 3); i++)
	{
		Data[i] = otherImage->Data[i];
	}

}


void MyImage::fillInFinalMatrix(int offsetX, int offsetY) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			finalData[offsetX + i][offsetY + (j * 3)] = idctCoeff[i][j];
		}
	}
}


void MyImage::convertTo1D(float** dataMatrix) {
	outputData = new char[Width*Height * 3];
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width * 3; j++) {
			outputData[i * (Width * 3) + j] = dataMatrix[i][j];
		}
	}
}

void MyImage::YPRPB_to_RGB() {
	float Y, Pb, Pr;
	rbgOutputValues = new char[Width*Height * 3];
	convertTo1D(finalData);

	for (int i = 0; i < Height * Width; i++) {
		Y = outputData[3 * i];
		Pb = outputData[3 * i + 1];
		Pr = outputData[3 * i + 2];
		float r = (float)(1.000 * Y) + (0.000 * Pb) + (1.402 * Pr);
		float g = (float)(1.000 * Y) - (0.344 * Pb) - (0.714 * Pr);
		float b = (float)(1.000 * Y) + (1.772 * Pb) + (0.000 * Pr);
		rbgOutputValues[3 * i] = (int)r;
		rbgOutputValues[3 * i + 1] = (int)g;
		rbgOutputValues[3 * i + 2] = (int)b;
	}
	for (int i = 0; i<(Height*Width * 3); i++)
	{
		int temp = (int)rbgOutputValues[i];
		if (temp > 255) {
			temp = 255;
		}
		Data[i] = temp;
	}
}


float get_y(int r, int g, int b) {
	float temp;

	temp = (float)(0.299 * r) + (0.587 * g) + (0.114 * b);
	return temp;
}


float get_pb(int r, int g, int b) {
	float temp;

	temp = (float)(-0.169 * r) - (0.331 * g) + (0.5 * b);
	return temp;
}

float get_pr(int r, int g, int b) {
	float temp;

	temp = (float)(0.5 * r) - (0.419 * g) - (0.081 * b);
	return temp;
}

void get_RGB(char *Data, int index, int *r, int *g, int *b) {
	*r = (int)Data[3 * index];
	*g = (int)Data[3 * index + 1];
	*b = (int)Data[3 * index + 2];
}

void get_yprpb(float *y, float *pb, float *pr, int r, int g, int b) {
	*y = get_y(r, g, b);
	*pb = get_pb(r, g, b);
	*pr = get_pr(r, g, b);
}

void fill_yprpb(float *yprpb_single, int index, float y, float pb, float pr) {
	yprpb_single[3 * index] = y;
	yprpb_single[3 * index + 1] = pb;
	yprpb_single[3 * index + 2] = pr;
}

void MyImage::RGB_to_YPRPB() {
	int r, g, b;
	float y, pb, pr;
	float *yprpb_single;

	yprpb_single = new float[Width*Height * 3];
	yprpb_matrix = new float*[Height];
	for (int i = 0; i < Height; i++) {
		yprpb_matrix[i] = new float[Width*3];
	}

	for (int i = 0; i < Height * Width; i++) {
		get_RGB(Data, i, &r, &g, &b);
		get_yprpb(&y, &pb, &pr, r, g, b);
		fill_yprpb(yprpb_single, i, y, pb, pr);
	}

	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width * 3; j++) {
			yprpb_matrix[i][j] = (float)yprpb_single[i * (Width * 3) + j];
		}
	}
}

void MyImage::DCT(float** resBlock)
{
	float cu = 0;
	float cv = 0;
	dctCoeff = new float*[8];
	for (int i = 0; i < 8; i++) {
		dctCoeff[i] = new float[8];
	}
	for (int u = 0; u < 8; ++u) {
		for (int v = 0; v < 8; ++v) {
			dctCoeff[u][v] = 0;
			float dct = 0;
			for (int x = 0; x < 8; x++) {
				for (int y = 0; y < 8; y++) {
					dct += resBlock[x][y] * cos(((2 * x + 1) * u * 22) / (float)(16 * 7)) * cos(((2 * y + 1) * v * 22) / (float)(16 * 7));
					/*dctCoeff[u][v] += resBlock[i][j] * cos(M_PI / ((float)8)*(i + 1.0 / 2.0)*u)*cos(M_PI / ((float)8)*(j + 1.0 / 2.0)*v);*/
				}
			}

			if (0 == u) {
				cu = 1 / (float)sqrt(2);
			}
			else {
				cu = 1.0;
			}

			if (0 == v) {
				cv = 1 / (float)sqrt(2);
			}
			else {
				cv = 1.0;
			}

			dctCoeff[u][v] = (1 / (float)4) * cu * cv * dct;
		}
	}
}

void MyImage::idct(float** DCTMatrix) {
	int i, j, u, v;
	idctCoeff = new float*[8];
	for (int i = 0; i < 8; i++) {
		idctCoeff[i] = new float[8];
	}

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			float idct = 0;
			float cu = 0;
			float cv = 0;
			for (int u = 0; u < 8; u++) {
				for (int v = 0; v < 8; v++) {
					if (0 == u) {
						cu = 1 / (float)sqrt(2);
					}
					else {
						cu = 1.0;
					}

					if (0 == v) {
						cv = 1 / (float)sqrt(2);
					}
					else {
						cv = 1.0;
					}
					idct += cu * cv * DCTMatrix[u][v] * cos(((2 * x + 1) * u * 22) / (float)(16 * 7)) * cos(((2 * y + 1) * v * 22) / (float)(16 * 7));
				}
			}
			idctCoeff[x][y] = idct / (float) 4.0;
		}
	}
}

float** MyImage::zigZagTraversal(float** dctCoeff, int diagonal) {

	int dimension = 8;
	quantizedMatrix = new float*[dimension];

	for (int i = 0; i < dimension; i++) {
		quantizedMatrix[i] = new float[dimension];
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%d ", dctCoeff[i][j]);
		}
		printf("\n");
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
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%d ", quantizedMatrix[i][j]);
		}
		printf("\n");
	}

	return quantizedMatrix;
}

float** get_block(float** yprpb_matrix, int start_x, int start_y) {
	float** block = new float*[8];
	for (int i = 0; i < 8; i++) {
		block[i] = new float[8];
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			block[i][j] = yprpb_matrix[start_x + i][start_y + (j * 3)];
		}
	}
	return block;
}

void MyImage::calculate_DCT() {
	float** quantizedMatrix = new float*[8];
	float** block;

	finalData = new float*[Height];
	for (int j = 0; j < Height; j++) {
		finalData[j] = new float[Width * 3];
	}

	for (int i = 0; i < 8; i++) {
		quantizedMatrix[i] = new float[8];
	}

	for (int i = 0; i < Height; i = i + 8) {
		for (int j = 0; j < Width; j = j + 8) {
			for (int k = 0; k < 3; k++) {
				block = get_block (yprpb_matrix, i, 3 * j + k);
				DCT(block);
				quantizedMatrix = zigZagTraversal(dctCoeff, 15);
				idct(quantizedMatrix);
				fillInFinalMatrix(i, 3 * j + k);
			}
		}
	}
}

// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width = otherImage.Width;
	Data = new char[Width*Height * 3];
	strcpy((char *)otherImage.ImagePath, ImagePath);

	for (int i = 0; i<(Height*Width * 3); i++)
	{
		Data[i] = otherImage.Data[i];
	}

	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}

	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if (IN_FILE == NULL)
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width];
	char *Gbuf = new char[Height*Width];
	char *Bbuf = new char[Height*Width];

	for (i = 0; i < Width*Height; i++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}

	// Allocate Data structure and copy
	Data = new char[Width*Height * 3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3 * i] = Bbuf[i];
		Data[3 * i + 1] = Gbuf[i];
		Data[3 * i + 2] = Rbuf[i];
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
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}

	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if (OUT_FILE == NULL)
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
		Bbuf[i] = Data[3 * i];
		Gbuf[i] = Data[3 * i + 1];
		Rbuf[i] = Data[3 * i + 2];
	}


	// Write data to file
	for (i = 0; i < Width*Height; i++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i++)
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
	for (int i = 0; i<Width*Height; i++)
	{
		Data[3 * i] = 0;
		Data[3 * i + 1] = 0;

	}

	return false;
}