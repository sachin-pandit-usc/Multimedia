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
	char	ImagePath[_MAX_PATH];	// Image location
	char*	Data;					// RGB data of the image
	float** yprpb_matrix;				
	float** dct_matrix;
	float** idctCoeff;				
	float** quantizedMatrix;		
	float**	finalData;				
	float*	outputData;				
	char*	rbgOutputValues;		
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
	void	setQuantValue(const int q) { QuantValue = q; };
	void	setImageData(const char *img) { Data = (char *)img; };
	void	setImagePath(const char *path) { strcpy(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	int		getQuantValue() { return QuantValue; };
	char*	getImageData() { return Data; };
	char*	getImagePath() {
		return ImagePath;
	}

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

	// Modifications
	bool	Modify();

	// Calculations
	void	convertTo1D(float ** convData);
	void	YPRPB_to_RGB();
	void	RGB_to_YPRPB();
	void	calculate_DCT(float ** resBlock);
	void	idct(float ** DCTMatrix);
	void    zigZagTraversal(int diagonal);
	void	calculate_DCT();
	void	complete_matrix(int offsetX, int offsetY);
};

#endif //IMAGE_DISPLAY