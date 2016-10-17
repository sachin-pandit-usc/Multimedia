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
	temp_rgb_matrix = NULL;
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

float** allocate_matrix(int height, int width) {
	float** temp;

	temp = new float*[height];
	for (int i = 0; i < width; i++) {
		temp[i] = new float[width];
	}

	return temp;
}

void MyImage::complete_matrix(int offsetX, int offsetY) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			dct_final[offsetX + i][offsetY + (j * 3)] = idct_matrix[i][j];
		}
	}
}

void fill_ypbpr(float *matrix, int index, float *y, float *pb, float *pr) {
	*y = matrix[3 * index];
	*pb = matrix[3 * index + 1];
	*pr = matrix[3 * index + 2];
}

void fill_RGB(float *r, float *g, float *b, float y, float pb, float pr) {
	*r = (float)(1.000 * y) + (0.000 * pb) + (1.402 * pr);
	*g = (float)(1.000 * y) - (0.344 * pb) - (0.714 * pr);
	*b = (float)(1.000 * y) + (1.772 * pb) + (0.000 * pr);
}

void final_matrix(char *matrix, int index, int r, int g, int b) {
	matrix[3 * index] = (int)r;
	matrix[3 * index + 1] = (int)g;
	matrix[3 * index + 2] = (int)b;
}

void MyImage::YPRPB_to_RGB() {
	float y, pb, pr;
	float r, g, b;

	final_rgb_matrix = new char[Width  *Height * 3];
	temp_rgb_matrix = new float[Width*Height * 3];
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width * 3; j++) {
			temp_rgb_matrix[i * (Width * 3) + j] = dct_final[i][j];
		}
	}

	for (int i = 0; i < Height * Width; i++) {
		fill_ypbpr(temp_rgb_matrix, i, &y, &pb, &pr);
		fill_RGB(&r, &g, &b, y, pb, pr);
		final_matrix(final_rgb_matrix, i, r, g, b);
	}
	for (int i = 0; i<(Height*Width * 3); i++)
	{
		int temp = (int)final_rgb_matrix[i];
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

float get_cu_cv(int value) {
	float temp;

	if (value == 0) {
		temp = 1 / (float)sqrt(2);
	}
	else {
		temp = 1.0;
	}
	return temp;
}


float get_dct_single(float** resBlock, int u, int v) {
	float dct_val = 0;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			dct_val += resBlock[x][y] * cos(((2 * x + 1) * u * 22) / (float)(16 * 7)) * cos(((2 * y + 1) * v * 22) / (float)(16 * 7));
		}
	}
	return dct_val;
}

void MyImage::calculate_DCT(float** resBlock)
{
	float cu = 0;
	float cv = 0;
	float dct_val = 0;

	quantizedMatrix = allocate_matrix(8, 8);
	dct_matrix = allocate_matrix(8, 8);
	
	for (int u = 0; u < 8; ++u) {
		for (int v = 0; v < 8; ++v) {
			dct_matrix[u][v] = 0;
			dct_val = get_dct_single(resBlock, u, v);
			cu = get_cu_cv(u);
			cv = get_cu_cv(v);
			dct_matrix[u][v] = (1 / (float)4) * cu * cv * dct_val;
		}
	}
	quantization_process(8);
}

float get_idct_single(float **matrix, int x, int y) {
	float idct_val = 0;
	float cu, cv;

	for (int u = 0; u < 8; u++) {
		for (int v = 0; v < 8; v++) {
			cu = get_cu_cv(u);
			cv = get_cu_cv(v);
			idct_val += cu * cv * matrix[u][v] * cos(((2 * x + 1) * u * 22) / (float)(16 * 7)) * cos(((2 * y + 1) * v * 22) / (float)(16 * 7));
		}
	}
	return idct_val;
}

void MyImage::calculate_idct(float** DCTMatrix) {
	int i, j, u, v;
	float idct;

	idct_matrix = allocate_matrix(8, 8);

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			idct = get_idct_single (DCTMatrix, x, y);
			idct_matrix[x][y] = idct / (float) 4.0;
		}
	}
}

void MyImage::quantization_process(int diagonal) {

	int dimension = 8, lastValue = 63, currNum = 0;
	int currDiag = 0, loopFrom, loopTo, row, col;

	quantizedMatrix = allocate_matrix(8, 8);

	do
	{
		if (currDiag >= dimension) {
			loopFrom = currDiag - dimension + 1;
			loopTo = dimension - 1;
		} else {
			loopFrom = 0;
			loopTo = currDiag;
		}

		for (int i = loopFrom; i <= loopTo; i++)
		{
			if (currDiag % 2 == 0) {
				row = i;
				col = loopTo - i + loopFrom;
			} else {
				row = loopTo - i + loopFrom;
				col = i;
			}

			if (currDiag >= diagonal) {
				quantizedMatrix[row][col] = 0;
			} else {
				quantizedMatrix[row][col] = dct_matrix[row][col];
			}
		}
		currDiag++;
	} while (currDiag <= lastValue);

	calculate_idct (quantizedMatrix);
}

float** get_block(float** yprpb_matrix, int start_x, int start_y) {
	float** block = allocate_matrix(8, 8);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			block[i][j] = yprpb_matrix[start_x + i][start_y + (j * 3)];
		}
	}
	return block;
}

void MyImage::calculate_DCT() {
	float** block;

	dct_final = new float*[Height];
	for (int j = 0; j < Height; j++) {
		dct_final[j] = new float[Width * 3];
	}

	for (int i = 0; i < Height; i = i + 8) {
		for (int j = 0; j < Width; j = j + 8) {
			for (int k = 0; k < 3; k++) {
				block = get_block (yprpb_matrix, i, 3 * j + k);
				calculate_DCT(block);
				complete_matrix(i, 3 * j + k);
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