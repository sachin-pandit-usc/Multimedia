//*****************************************************************************
//
// Image.h : Defines the class operations on images
//
// Author - Parag Havaldar
// Main Image class structure 
//
//*****************************************************************************

#ifndef IMAGE_DISPLAY
#define IMAGE_DISPLAY
#define M_PI 3.14159265


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Class structure of Image 
// Use to encapsulate an RGB image
class MyImage
{

private:
	int		Width;					// Width of Image
	int		Height;					// Height of Image
	int		Diagonal;				// Diaginal
	char	ImagePath[_MAX_PATH];	// Image location
	char*	Data;					// RGB data of the image
	float** convData;				// 2D Matrix of YPbPr data of the image
	float** dctCoeff;				// 8*8 block of DCT Coeffecients
	float** idctCoeff;				// 8*8 block of iDCT Coeffecient
	float** quantizedMatrix;		// 8*8 block that holds quantized values based on input parameter n
	float**	finalData;				// Quantized Data of the image
	char*	final_yprpb;				// Output 1D YPbPr Data values
	char*	final_rgb;		// Output 1D RGB Data values
	int		QuantValue;

public:
	// Constructor
	MyImage();
	// Copy Constructor
	MyImage::MyImage(MyImage *otherImage);

	// Destructor
	~MyImage();

	// operator overload
	MyImage & operator= (const MyImage & otherImage);

	// Reader & Writer functions
	void	setWidth(const int w) { Width = w; };
	void	setHeight(const int h) { Height = h; };
	void	setDiagonal(const int d) { Diagonal = d; };
	void	setQuantValue(const int q) { QuantValue = q; };
	void	setImageData(const char *img) { Data = (char *)img; };
	void	setImagePath(const char *path) { strcpy(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	int		getQuantValue() { return QuantValue; };
	char*	getImageData() { return Data; };
	int	getDiagonal() { return Diagonal; };
	char*	getImagePath() {
		return ImagePath;
	}

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

	// Modifications
	bool	Modify();

	//
	void	YPRPB_to_RGB();
	void	RGB_to_YPRPB();
	void	array_to_matrix(float * matrix);
	void	DCT(float ** resBlock);
	void	idct(float ** DCTMatrix);
	float** zigZagTraversal(float ** dctCoeff, int diagonal);
	void	generateDCT();
	void	fillInFinalMatrix(int offsetX, int offsetY, int xVal, int yVal);
	void	constructBlock(float ** convData, float ** result, int offsetX, int offsetY, int xVal, int yVal);
};

#endif //IMAGE_DISPLAY
