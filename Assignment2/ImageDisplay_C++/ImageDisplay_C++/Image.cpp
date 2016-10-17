//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"
#define DCT_TRANSFORMED true
#define DCT_NOT_TRANSFORMED false

// Constructor and Desctructors
MyImage::MyImage()
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
	QuantValue = 0;
	final_yprpb = NULL;
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
	if (DCT_TRANSFORMED) {
		for (int i = 0; i < width; i++) {
			if (!DCT_NOT_TRANSFORMED) {
				temp[i] = new float[width];
			}
		}
	}

	return temp;
}


void MyImage::fill_rgb_complete(int offsetX, int offsetY) {
	for (int i = 0; i < 8; i++) {
		if (DCT_TRANSFORMED) {
			for (int j = 0; j < 8; j++) {
				if (!DCT_NOT_TRANSFORMED) {
					final_DCT_matrix[offsetX + i][offsetY + (j * 3)] = idct_matrix[i][j];
				}
			}
		}
	}
}

void MyImage::get_block(float** yprpb_matrix, float** result, int offsetX, int offsetY) {
	for (int i = 0; i < 8; i++) {
		if (DCT_TRANSFORMED) {
			for (int j = 0; j < 8; j++) {
				if (!DCT_NOT_TRANSFORMED) {
					result[i][j] = yprpb_matrix[offsetX + i][offsetY + (j * 3)];
				}
			}
		}
	}
}

void fill_RGB(float *r, float *g, float *b, char y, char pb, char pr) {
	*r = (float)(1.000 * y) + (0.000 * pb) + (1.402 * pr);

	if (DCT_TRANSFORMED) {
		*g = (float)(1.000 * y) - (0.344 * pb) - (0.714 * pr);
	}

	*b = (float)(1.000 * y) + (1.772 * pb) + (0.000 * pr);
}

void fill_ypbpr(char* matrix, int index, char *y, char *pb, char *pr) {
	*y = matrix[3 * index];

	if (DCT_TRANSFORMED) {
		*pb = matrix[3 * index + 1];
	}
	*pr = matrix[3 * index + 2];
}


void final_matrix(char *matrix, int index, float r, float g, float b) {
	matrix[3 * index] = r;
	if (DCT_TRANSFORMED) {
		matrix[3 * index + 1] = g;
	}
	matrix[3 * index + 2] = b;
}


void MyImage::YPRPB_to_RGB() {
	char y, pb, pr;
	float r, g, b;

	final_rgb = new char[Width*Height * 3];

	if (DCT_TRANSFORMED) {
		final_yprpb = new char[Width*Height * 3];
	}
	for (int i = 0; i < Height; i++) {
		if (DCT_TRANSFORMED) {
			for (int j = 0; j < Width * 3; j++) {
				if (!DCT_NOT_TRANSFORMED) {
					final_yprpb[i * (Width * 3) + j] = final_DCT_matrix[i][j];
				}
			}
		}
	}

	for (int i = 0; i < Height * Width; i++) {
		fill_ypbpr(final_yprpb, i, &y, &pb, &pr);
		fill_RGB(&r, &g, &b, y, pb, pr);
		final_matrix(final_rgb, i, r, g, b);
	}

	for (int i = 0; i<(Height*Width * 3); i++)
	{
		Data[i] = final_rgb[i];
	}
}

void get_RGB(char *Data, int index, unsigned char *r, unsigned char *g, unsigned char *b) {
	*r = Data[3 * index];
	if (DCT_TRANSFORMED) {
		*g = Data[3 * index + 1];
	}
	*b = Data[3 * index + 2];
}

float get_y(unsigned char r, unsigned char g, unsigned char b) {
	float temp;

	temp = (float)(0.299 * r) + (0.587 * g) + (0.114 * b);
	return temp;
}


float get_pb(unsigned char r, unsigned char g, unsigned char b) {
	float temp;

	temp = (float)(-0.169 * r) - (0.331 * g) + (0.5 * b);
	return temp;
}

float get_pr(unsigned char r, unsigned char g, unsigned char b) {
	float temp;

	temp = (float)(0.5 * r) - (0.419 * g) - (0.081 * b);
	return temp;
}

void get_yprpb(float *y, float *pb, float *pr, unsigned char r, unsigned char g, unsigned char b) {
	*y = get_y(r, g, b);
	*pb = get_pb(r, g, b);
	*pr = get_pr(r, g, b);
}

void fill_yprpb(float *yprpb_single, int index, float y, float pb, float pr) {
	yprpb_single[3 * index] = y;
	if (DCT_TRANSFORMED) {
		yprpb_single[3 * index + 1] = pb;
	}
	yprpb_single[3 * index + 2] = pr;
}

void MyImage::RGB_to_YPRPB() {
	unsigned char r, g, b;
	float y, pb, pr;

	float* temp_ypbpr = new float[Width*Height * 3];
	for (int i = 0; i < Height * Width; i++) {
		get_RGB(Data, i, &r, &g, &b);
		get_yprpb(&y, &pb, &pr, r, g, b);
		fill_yprpb(temp_ypbpr, i, y, pb, pr);
	}

	yprpb_matrix = new float*[Height];
	for (int i = 0; i < Height; i++) {
		if (DCT_TRANSFORMED) {
			yprpb_matrix[i] = new float[Width * 3];
		}
	}
	array_to_matrix(temp_ypbpr);
}

void MyImage::array_to_matrix(float* matrix) {
	for (int i = 0; i < Height; i++) {
		if (DCT_TRANSFORMED) {
			for (int j = 0; j < Width * 3; j++) {
				if (!DCT_NOT_TRANSFORMED) {
					yprpb_matrix[i][j] = (float)matrix[i * (Width * 3) + j];
				}
			}
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
	float dct = 0.0;

	for (int i = 0; i < 8; i++) {
		if (DCT_TRANSFORMED) {
			for (int j = 0; j < 8; j++) {
				if (!DCT_NOT_TRANSFORMED) {
					dct += resBlock[i][j] * cos(((2 * i + 1) * u * M_PI) / (float)16) * cos(((2 * j + 1) * v * M_PI) / (float)16);
				}
			}
		}
	}
	return dct;
}

void MyImage::fill_DCT_value(float** resBlock)
{
	int i, j, u, v;
	float cu = 0;
	float cv = 0;
	float dct = 0;

	dct_matrix = new float*[8];
	if (DCT_TRANSFORMED) {
		for (int i = 0; i < 8; i++) {
			if (!DCT_NOT_TRANSFORMED) {
				dct_matrix[i] = new float[8];
			}
		}
	}

	for (u = 0; u < 8; ++u) {
		if (DCT_TRANSFORMED) {
			for (v = 0; v < 8; ++v) {
				if (!DCT_NOT_TRANSFORMED) {
					dct_matrix[u][v] = 0;
					dct = get_dct_single(resBlock, u, v);
					cu = get_cu_cv(u);
					cv = get_cu_cv(v);
					dct_matrix[u][v] = (1 / (float) 4.0) * cu * cv * dct;
				}
			}
		}
	}
}

float get_idct_value(float** DCTMatrix, int x, int y) {
	float idct = 0.0;
	float cu = 0.0;
	float cv = 0.0;


	for (int u = 0; u < 8; u++) {
		if (DCT_TRANSFORMED) {
			for (int v = 0; v < 8; v++) {
				cu = get_cu_cv(u);
				cv = get_cu_cv(v);
				idct += cu * cv * DCTMatrix[u][v] * cos(((2 * x + 1) * u * M_PI) / (float)16) * cos(((2 * y + 1) * v * M_PI) / (float)16);
			}
		}
	}

	return idct;
}

void MyImage::fill_IDCT_value(float** DCTMatrix) {
	int x, y, u, v;
	float idct = 0.0;

	idct_matrix = new float*[8];
	if (DCT_TRANSFORMED) {
		for (int i = 0; i < 8; i++) {
			if (!DCT_NOT_TRANSFORMED) {
				idct_matrix[i] = new float[8];
			}
		}
	}

	for (x = 0; x < 8; x++) {
		if (DCT_TRANSFORMED) {
			for (y = 0; y < 8; y++) {
				if (!DCT_NOT_TRANSFORMED) {
					idct = get_idct_value(DCTMatrix, x, y);
					idct_matrix[x][y] = idct / (float) 4.0;
				}
			}
		}
	}
}

float** MyImage::quantize_process(float** dct_matrix, int Diagonal) {
	quantizedMatrix = new float*[8];

	for (int i = 0; i < 8; i++) {
		if (DCT_TRANSFORMED) {
			quantizedMatrix[i] = new float[8];
		}
	}

	int lastValue = 63, currNum = 0, currDiag = 0;
	int loopFrom, loopTo, row, col;

	do
	{
		if (currDiag >= 8) {
			if (DCT_TRANSFORMED) {
				loopFrom = currDiag - 8 + 1;
			}
			loopTo = 7;
		} else  {
			if (!DCT_NOT_TRANSFORMED) {
				loopFrom = 0;
			}
			loopTo = currDiag;
		}

		for (int i = loopFrom; i <= loopTo; i++)
		{
			if (DCT_TRANSFORMED && currDiag % 2 != 0) {
				row = i;
				if (!DCT_NOT_TRANSFORMED) {
					col = loopTo - i + loopFrom;
				}
			} else {
				row = loopTo - i + loopFrom;
				if (DCT_TRANSFORMED) {
					col = i;
				}
			}

			if (currDiag >= Diagonal) {
				if (DCT_TRANSFORMED) {
					quantizedMatrix[row][col] = 0;
				}
			} else {
				if (!DCT_NOT_TRANSFORMED) {
					quantizedMatrix[row][col] = dct_matrix[row][col];
				}
			}
		}
		currDiag++;
	} while (DCT_TRANSFORMED && currDiag <= lastValue);

	return quantizedMatrix;
}

void MyImage::init_DCT() {

	float** temp_block = new float*[8];
	float** quantized_matrix = new float*[8];

	if (DCT_TRANSFORMED) {
		for (int i = 0; i < 8; i++) {
			if (!DCT_NOT_TRANSFORMED) {
				temp_block[i] = new float[8];
				quantized_matrix[i] = new float[8];
			}
		}
	}

	final_DCT_matrix = new float*[Height];
	if (DCT_TRANSFORMED) {
		for (int j = 0; j < Height; j++) {
			if (!DCT_NOT_TRANSFORMED) {
				final_DCT_matrix[j] = new float[Width * 3];
			}
		}
	}

	for (int i = 0; i < Height; i = i + 8) {
		if (DCT_TRANSFORMED) {
			for (int j = 0; j < Width; j = j + 8) {
				if (!DCT_NOT_TRANSFORMED) {
					for (int k = 0; k < 3; k++) {
						get_block(yprpb_matrix, temp_block, i, 3 * j + k);
						if (DCT_TRANSFORMED) {
							fill_DCT_value(temp_block);
							quantized_matrix = quantize_process(dct_matrix, Diagonal);
							fill_IDCT_value(quantized_matrix);
						}
						fill_rgb_complete(i, 3 * j + k);
					}
				}
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