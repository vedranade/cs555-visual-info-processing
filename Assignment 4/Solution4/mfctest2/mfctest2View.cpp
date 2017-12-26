
// mfctest2View.cpp : implementation of the Cmfctest2View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "mfctest2.h"
#endif
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <afxdlgs.h>
#include<math.h>
#include<iostream>

#include "mfctest2Doc.h"
#include "mfctest2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define SE_SIZE 3
#endif

using namespace std;
using namespace cv;




// Cmfctest2View

IMPLEMENT_DYNCREATE(Cmfctest2View, CView)

BEGIN_MESSAGE_MAP(Cmfctest2View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cmfctest2View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(GreyscaleDilation, &Cmfctest2View::OnGreyscaledilation)
	ON_COMMAND(GreyscaleErosion, &Cmfctest2View::OnGreyscaleerosion)
	ON_COMMAND(Opening, &Cmfctest2View::OnOpening)
	ON_COMMAND(Closing, &Cmfctest2View::OnClosing)
	ON_COMMAND(ToBinary, &Cmfctest2View::OnTobinary)
END_MESSAGE_MAP()


//Function to convert CString data type to simple string
std::string to_string(const CString& cst)
{
	return CT2A(cst.GetString());
}
// Cmfctest2View construction/destruction

Cmfctest2View::Cmfctest2View()
{
	// TODO: add construction code here

}

Cmfctest2View::~Cmfctest2View()
{
}

BOOL Cmfctest2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cmfctest2View drawing

void Cmfctest2View::OnDraw(CDC* /*pDC*/)
{
	Cmfctest2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// Cmfctest2View printing


void Cmfctest2View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cmfctest2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cmfctest2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cmfctest2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void Cmfctest2View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cmfctest2View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Cmfctest2View diagnostics

#ifdef _DEBUG
void Cmfctest2View::AssertValid() const
{
	CView::AssertValid();
}

void Cmfctest2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cmfctest2Doc* Cmfctest2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cmfctest2Doc)));
	return (Cmfctest2Doc*)m_pDocument;
}
#endif //_DEBUG


// Cmfctest2View message handlers

//Self defined function for opening files and loading the image:
Mat ObtainImage()
{
	cvDestroyAllWindows();							//Closes all windows opened by OpenCV
	CFileDialog FileDialog(true);					
	char title[] = { "Open Image" };				//Setting the window name for the file picker
	CString Cfilepath;								

	if ( FileDialog.DoModal() == IDOK )				//Displaying the file picker
	{
		Cfilepath = FileDialog.GetPathName();		//Retrieving the full path of the selected file
	}
	else if ( FileDialog.DoModal() == IDCANCEL )
	{

	}

	string filepath = to_string(Cfilepath);							//Converting to simple string so that it can be used by OpenCV
	Mat src = imread(filepath, CV_LOAD_IMAGE_GRAYSCALE);			//Loading the image
	return src;														
}

//Self defined functions for image processing:

Mat dilation_small(Mat src)											//Dilation using 3x3 structuring element
{
	Mat dst = src.clone();
	int max_value = 0;
	int SE[3][3] =										//Set structuring element here
	{
		1,1,1,
		1,1,1,
		1,1,1,
	};
	int x = 0;
	int c[25];														//Array for temporary computations
	for ( int i = 0; i < 25; i++ )
	{
		c[i] = 0;
	}

	int rows = src.rows;
	int cols = src.cols;

	for ( int i = 1; i < rows - 1; i++ )							//Calculating product of structuring element and image at every point
	{
		for ( int j = 1; j < cols - 1; j++ )
		{
			c[0] = SE[0][0] * src.at<uchar>(i - 1, j + 1);
			max_value = max(c[0], max_value);
			c[1] = SE[0][1] * src.at<uchar>(i, j + 1);
			max_value = max(c[1], max_value);
			c[2] = SE[0][2] * src.at<uchar>(i + 1, j + 1);
			max_value = max(c[2], max_value);
			c[3] = SE[1][0] * src.at<uchar>(i - 1, j);
			max_value = max(c[3], max_value);
			c[4] = SE[1][1] * src.at<uchar>(i, j);
			max_value = max(c[4], max_value);
			c[5] = SE[1][2] * src.at<uchar>(i + 1, j);
			max_value = max(c[5], max_value);
			c[6] = SE[2][0] * src.at<uchar>(i - 1, j - 1);
			max_value = max(c[6], max_value);
			c[7] = SE[2][1] * src.at<uchar>(i, j - 1);
			max_value = max(c[7], max_value);
			c[8] = SE[2][2] * src.at<uchar>(i + 1, j - 1);
			max_value = max(c[8], max_value);

			std::sort(c, c + 9);											//Sorting array c in ascending order using standard library function
			dst.at<uchar>(i, j) = c[8];										//Changing value of pixel in image

		}
	}
	return dst;
}

Mat erosion_small(Mat src)													//Erosion using 3x3 structuring element
{
	Mat dst = src.clone();
	int min_value = 300;
	int SE[3][3] =
	{
		1,1,1,
		1,1,1,
		1,1,1,
	};
	int c[25];
	for ( int i = 0; i < 24; i++ )
	{
		c[i] = 0;
	}
	int x = 0;

	int rows = src.rows;
	int cols = src.cols;

	for ( int i = 2; i < rows - 2; i++ )
	{
		for ( int j = 2; j < cols - 2; j++ )
		{

			c[0] = SE[0][0] * src.at<uchar>(i - 1, j + 1);
			min_value = min(c[0], min_value);
			c[1] = SE[0][1] * src.at<uchar>(i, j + 1);
			min_value = min(c[1], min_value);
			c[2] = SE[0][2] * src.at<uchar>(i + 1, j + 1);
			min_value = min(c[2], min_value);
			c[3] = SE[1][0] * src.at<uchar>(i - 1, j);
			min_value = min(c[3], min_value);
			c[4] = SE[1][1] * src.at<uchar>(i, j);
			min_value = min(c[4], min_value);
			c[5] = SE[1][2] * src.at<uchar>(i + 1, j);
			min_value = min(c[5], min_value);
			c[6] = SE[2][0] * src.at<uchar>(i - 1, j - 1);
			min_value = min(c[6], min_value);
			c[7] = SE[2][1] * src.at<uchar>(i, j - 1);
			min_value = min(c[7], min_value);
			c[8] = SE[2][2] * src.at<uchar>(i + 1, j - 1);
			min_value = min(c[8], min_value);

			std::sort(c, c + 9);
			dst.at<uchar>(i, j) = c[0];
		}
	}
	return dst;
}

Mat erosion(Mat src)													//Erosion using 5x5 structuring element
{
	Mat dst = src.clone();
	int min_value = 300;
	int SE2[5][5] =
	{
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
	};
	int c[25];
	for ( int i = 0; i < 24; i++ )
	{
		c[i] = 0;
	}
	int x = 0;

	int rows = src.rows;
	int cols = src.cols;

	for ( int i = 2; i < rows - 2; i++ )										
	{
		for ( int j = 2; j < cols - 2; j++ )
		{

			c[0] = SE2[0][0] * src.at<uchar>(i - 2, j + 2);
			min_value = min(c[0], min_value);
			c[1] = SE2[0][1] * src.at<uchar>(i - 1, j + 2);
			min_value = min(c[0], min_value);
			c[2] = SE2[0][2] * src.at<uchar>(i, j + 2);
			min_value = min(c[0], min_value);
			c[3] = SE2[0][3] * src.at<uchar>(i + 1, j + 2);
			min_value = min(c[0], min_value);
			c[4] = SE2[0][4] * src.at<uchar>(i + 2, j + 2);
			min_value = min(c[0], min_value);
			c[5] = SE2[1][0] * src.at<uchar>(i - 2, j + 1);
			min_value = min(c[0], min_value);
			c[6] = SE2[1][1] * src.at<uchar>(i - 1, j + 1);
			min_value = min(c[0], min_value);
			c[7] = SE2[1][2] * src.at<uchar>(i, j + 1);
			min_value = min(c[0], min_value);
			c[8] = SE2[1][3] * src.at<uchar>(i + 1, j + 1);
			min_value = min(c[0], min_value);
			c[9] = SE2[1][4] * src.at<uchar>(i + 2, j + 1);
			min_value = min(c[0], min_value);
			c[10] = SE2[2][0] * src.at<uchar>(i - 2, j);
			min_value = min(c[0], min_value);
			c[11] = SE2[2][1] * src.at<uchar>(i, j);
			min_value = min(c[0], min_value);
			c[12] = SE2[2][2] * src.at<uchar>(i + 1, j);
			min_value = min(c[0], min_value);
			c[13] = SE2[2][3] * src.at<uchar>(i + 2, j);
			min_value = min(c[0], min_value);
			c[14] = SE2[2][4] * src.at<uchar>(i - 2, j - 1);
			min_value = min(c[0], min_value);
			c[15] = SE2[3][0] * src.at<uchar>(i - 1, j - 1);
			min_value = min(c[0], min_value);
			c[16] = SE2[3][1] * src.at<uchar>(i, j - 1);
			min_value = min(c[0], min_value);
			c[17] = SE2[3][2] * src.at<uchar>(i + 1, j - 1);
			min_value = min(c[0], min_value);
			c[18] = SE2[3][3] * src.at<uchar>(i + 2, j - 1);
			min_value = min(c[0], min_value);
			c[19] = SE2[3][4] * src.at<uchar>(i - 2, j - 2);
			min_value = min(c[0], min_value);
			c[20] = SE2[4][0] * src.at<uchar>(i - 1, j - 2);
			min_value = min(c[0], min_value);
			c[21] = SE2[4][1] * src.at<uchar>(i, j - 2);
			min_value = min(c[0], min_value);
			c[22] = SE2[4][2] * src.at<uchar>(i + 1, j - 2);
			min_value = min(c[0], min_value);
			c[23] = SE2[4][3] * src.at<uchar>(i + 2, j - 2);
			min_value = min(c[0], min_value);
			c[24] = SE2[4][4] * src.at<uchar>(i - 1, j + 1);
			min_value = min(c[0], min_value);

			std::sort(c, c + 25);
			dst.at<uchar>(i, j) = c[0];
			
		}
	}
	return dst;
}

Mat dilation(Mat src)											//Dilation using 5x5 structuring element
{
	Mat dst = src.clone();
	int max_value = 0;
	int SE2[5][5] =
	{
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
	};
	int x = 0;
	int c[25];
	for ( int i = 0; i < 25; i++ )
	{
		c[i] = 0;
	}

	int rows = src.rows;
	int cols = src.cols;

	for ( int i = 2; i < rows - 2; i++ )										
	{
		for ( int j = 2; j < cols - 2; j++ )
		{
			
			c[0] = SE2[0][0] * src.at<uchar>(i - 2, j + 2);
			max_value = max(c[0], max_value);
			c[1] = SE2[0][1] * src.at<uchar>(i - 1, j + 2);
			max_value = max(c[1], max_value);
			c[2] = SE2[0][2] * src.at<uchar>(i, j + 2);
			max_value = max(c[2], max_value);
			c[3] = SE2[0][3] * src.at<uchar>(i + 1, j + 2);
			max_value = max(c[3], max_value);
			c[4] = SE2[0][4] * src.at<uchar>(i + 2, j + 2);
			max_value = max(c[4], max_value);
			c[5] = SE2[1][0] * src.at<uchar>(i - 2, j + 1);
			max_value = max(c[5], max_value);
			c[6] = SE2[1][1] * src.at<uchar>(i - 1, j + 1);
			max_value = max(c[6], max_value);
			c[7] = SE2[1][2] * src.at<uchar>(i, j + 1);
			max_value = max(c[7], max_value);
			c[8] = SE2[1][3] * src.at<uchar>(i + 1, j + 1);
			max_value = max(c[8], max_value);
			c[9] = SE2[1][4] * src.at<uchar>(i + 2, j + 1);
			max_value = max(c[9], max_value);
			c[10] = SE2[2][0] * src.at<uchar>(i - 2, j);
			max_value = max(c[10], max_value);
			c[11] = SE2[2][1] * src.at<uchar>(i, j);
			max_value = max(c[11], max_value);
			c[12] = SE2[2][2] * src.at<uchar>(i + 1, j);
			max_value = max(c[12], max_value);
			c[13] = SE2[2][3] * src.at<uchar>(i + 2, j);
			max_value = max(c[13], max_value);
			c[14] = SE2[2][4] * src.at<uchar>(i - 2, j - 1);
			max_value = max(c[14], max_value);
			c[15] = SE2[3][0] * src.at<uchar>(i - 1, j - 1);
			max_value = max(c[15], max_value);
			c[16] = SE2[3][1] * src.at<uchar>(i, j - 1);
			max_value = max(c[16], max_value);
			c[17] = SE2[3][2] * src.at<uchar>(i + 1, j - 1);
			max_value = max(c[17], max_value);
			c[18] = SE2[3][3] * src.at<uchar>(i + 2, j - 1);
			max_value = max(c[18], max_value);
			c[19] = SE2[3][4] * src.at<uchar>(i - 2, j - 2);
			max_value = max(c[19], max_value);
			c[20] = SE2[4][0] * src.at<uchar>(i - 1, j - 2);
			max_value = max(c[20], max_value);
			c[21] = SE2[4][1] * src.at<uchar>(i, j - 2);
			max_value = max(c[21], max_value);
			c[22] = SE2[4][2] * src.at<uchar>(i + 1, j - 2);
			max_value = max(c[22], max_value);
			c[23] = SE2[4][3] * src.at<uchar>(i + 2, j - 2);
			max_value = max(c[23], max_value);
			c[24] = SE2[4][4] * src.at<uchar>(i - 1, j + 1);
			max_value = max(c[24], max_value);

			std::sort(c, c + 25);
			dst.at<uchar>(i, j) = c[24];


		}
	}
	return dst;
}

Mat getBinaryImage(Mat src, int threshold)										//Converting grayscale image to binary based on threshold
{
	for ( int i = 0; i < src.rows; i++ )
	{
		for ( int j = 0; j < src.cols; j++ )
		{
			if ( int(src.at<uchar>(i, j)) <= threshold )
			{
				src.at<uchar>(i, j) = 0;																						//If pixel intensity is greater than threshold
			}
			else
			{
				src.at<uchar>(i, j) = 255;																						//If pixel intensity is less than threshold
			}
		}
	}

	return src;
}

int getAverageIntensityValue(int hist[], int a, int b)	
{
	int sum_of_pixels = 0;
	sum_of_pixels = sum_of_pixels + hist[0];
	int sum_product_of_pixels = (hist[0] * 0);

	for ( int i = a; i < b; i++ )
	{
		sum_of_pixels = sum_of_pixels + hist[i];
	}

	for ( int i = a; i < b; i++ )
	{
		sum_product_of_pixels = sum_product_of_pixels + (hist[i] * i);
	}

	int average_intensity_value = sum_product_of_pixels / sum_of_pixels;
	return average_intensity_value;
}



void Cmfctest2View::OnGreyscaledilation()
{

	Mat src = ObtainImage();													//Load the image
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", src);
	
	Mat dst;
	dst = dilation(src);
	dst = dilation(dst);														//Perform dilation twice for evident effect

	namedWindow("Dilated Image", WINDOW_AUTOSIZE);
	imshow("Dilated Image",dst);
	
	// TODO: Add your command handler code here
}

void Cmfctest2View::OnGreyscaleerosion()
{
	
	Mat src = ObtainImage();														//Load the image
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", src);
	Mat dst;

	dst = erosion(src);
	dst = erosion_small(dst);														//Perform erosion twice for evident effect

	namedWindow("Eroded Image", WINDOW_AUTOSIZE);
	imshow("Eroded Image", dst);
	// TODO: Add your command handler code here
}

void Cmfctest2View::OnOpening()
{
	Mat src = ObtainImage();
	namedWindow("Initial Image", WINDOW_AUTOSIZE);
	imshow("Initial Image", src);

	src = erosion(src);
	src = erosion(src);
	src = erosion(src);
	src = dilation_small(src);										//Peform erosion thrice and then perform dilation to separate the objects

	namedWindow("Opened Image", CV_WINDOW_AUTOSIZE);
	imshow("Opened Image", src);
	imwrite("E:/Assignment 4 images/pigs_separated.jpeg", src);

	Mat dst = src.clone();
	int rows = src.rows;
	int cols = src.cols;
	
}


void Cmfctest2View::OnClosing()
{
	Mat src = ObtainImage();
	namedWindow("Initial Image", WINDOW_AUTOSIZE);
	imshow("Initial Image", src);

	src = dilation(src);
	src = erosion(src);

	namedWindow("Closed Image", CV_WINDOW_AUTOSIZE);
	imshow("Closed Image", src);
}


void Cmfctest2View::OnTobinary()								//Calculates threshold using basic global thresholding method
{
	Mat src = ObtainImage();
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", src);
	int histogram[300];
	for ( int a = 0; a < 256; a++ )																			//Setting histogram Y axis values to zero
	{
		histogram[a] = 0;
	}

	for ( int i = 0; i < src.rows; i++ )
	{
		for ( int j = 0; j < src.cols; j++ )
		{
			histogram[int(src.at<uchar>(i, j))]++;										//Generate histogram for original image
		}
	}

	int old_threshold = 128;
	int new_threshold = 128;
	int delta_threshold = 2;
	int m1, m2;

	do									//Calculate image threshold using Basic Global Thresholding method
	{
		old_threshold = new_threshold;
		m1 = getAverageIntensityValue(histogram, 1, old_threshold);
		m2 = getAverageIntensityValue(histogram, old_threshold + 1, 256);
		new_threshold = (m1 + m2) / 2;

	}
	while ( (old_threshold - new_threshold) > delta_threshold );
	src = getBinaryImage(src, 110);								//Changes original grayscale image to binary image

	namedWindow("Binary Image", WINDOW_AUTOSIZE);
	imshow("Binary Image", src);
	imwrite("E:/Assignment 4 images/pigs_binary_custom.jpeg", src);

}
