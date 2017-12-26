
// Solution5View.cpp : implementation of the CSolution5View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Solution5.h"
#endif
#include "Solution5Doc.h"
#include "Solution5View.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2\imgproc\imgproc.hpp>
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace cv;



// CSolution5View

IMPLEMENT_DYNCREATE(CSolution5View, CView)

BEGIN_MESSAGE_MAP(CSolution5View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSolution5View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(Divide, &CSolution5View::OnDivide)
	//ON_COMMAND(KMeans, &CSolution5View::OnKmeans)
	ON_COMMAND(ImageN1, &CSolution5View::OnImagen1)
	ON_COMMAND(ImageN2, &CSolution5View::OnImagen2)
	ON_COMMAND(MotionVectors, &CSolution5View::OnMotionvectors)
	ON_COMMAND(ImageN3, &CSolution5View::OnImagen3)
	ON_COMMAND(ImageN4, &CSolution5View::OnImagen4)
	ON_COMMAND(ImageT1, &CSolution5View::OnImaget1)
END_MESSAGE_MAP()

//Function to convert CString data type to simple string
std::string to_string(const CString& cst)
{
	return CT2A(cst.GetString());
}

// CSolution5View construction/destruction

CSolution5View::CSolution5View()
{
	// TODO: add construction code here

}

CSolution5View::~CSolution5View()
{
}

BOOL CSolution5View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSolution5View drawing

void CSolution5View::OnDraw(CDC* /*pDC*/)
{
	CSolution5Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CSolution5View printing


void CSolution5View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSolution5View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSolution5View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSolution5View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CSolution5View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSolution5View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSolution5View diagnostics

#ifdef _DEBUG
void CSolution5View::AssertValid() const
{
	CView::AssertValid();
}

void CSolution5View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSolution5Doc* CSolution5View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSolution5Doc)));
	return (CSolution5Doc*)m_pDocument;
}
#endif //_DEBUG

//Self defined function for getting 4x4 image blocks
int A[3000][4][4];						//Stores 4x4 blocks of top half of image in M1
int B[3000][4][4];						//Stores 4x4 blocks of bottom half of image in N1

int training_blocks[2400][4][4];
int testing_blocks[2400][4][4];
int no_of_blocks;
//Used for maintaining class information
struct class_info
{
	int label, average;
};

class_info training_info[2400];
class_info testing_info[2400];

//Function to generate M1 image
Mat GenerateM1(Mat src)
{
	int temp;
	int height = src.rows - (src.rows % 4);
	int ht = src.rows;
	int wt = src.cols;
	int width = src.cols - (src.cols % 4);
	int sum, avg,label;
	int p = 0;
	Mat dst = src.clone();
	for ( int m = 0; m < height / 2; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					avg = 0, temp = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						for ( int y = n; y < n + 4; y++ )
						{
							temp += src.at<uchar>(x, y);
						}
					}
					avg = temp / 16;
					training_info[p].average = avg;

					if ( avg < 125 )
					{
						label = 0;
						training_info[p].label = label;
					}
					else if ( avg >= 125 && avg < 175 )
					{
						label = 128;
						training_info[p].label = label;
					}
					else if ( avg >= 175 )
					{
						label = 255;
						training_info[p].label = label;
					}

					training_info[p].label = label;
					for ( int x = m; x < m + 4; x++ )
					{
						for ( int y = n; y < n + 4; y++ )
						{
							dst.at<uchar>(x, y) = label;

						}
					}
					
				}
			}
			p++;
		}
	}
	return dst;
}

Mat GenerateT1(Mat src)
{
	int temp;
	int height = src.rows - (src.rows % 4);
	int ht = src.rows;
	int wt = src.cols;
	int width = src.cols - (src.cols % 4);
	int sum, avg, label;
	int p = 0;
	Mat dst = src.clone();
	for ( int m = src.rows/2; m < height; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					avg = 0, temp = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						for ( int y = n; y < n + 4; y++ )
						{
							temp += src.at<uchar>(x, y);
						}
					}
					avg = temp / 16;
					testing_info[p].average = avg;

					if ( avg < 125 )
					{
						label = 0;
						testing_info[p].label = label;
					}
					else if ( avg >= 125 && avg < 175 )
					{
						label = 128;
						testing_info[p].label = label;
					}
					else if ( avg >= 175 )
					{
						label = 255;
						testing_info[p].label = label;
					}

					testing_info[p].label = label;
					for ( int x = m; x < m + 4; x++ )
					{
						for ( int y = n; y < n + 4; y++ )
						{
							dst.at<uchar>(x, y) = label;

						}
					}

				}
			}
			p++;
		}
	}
	return dst;
}

void ObtainTopBlocks(Mat src, int A[2400][4][4])
{
	int temp;
	int height = src.rows - (src.rows % 4);
	int ht = src.rows;
	int wt = src.cols;
	int x1, y1;
	int width = src.cols - (src.cols % 4);
	int sum, avg, label;
	int p = 0;
	Mat dst = src.clone();
	for ( int m = 0; m < height / 2; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					avg = 0, temp = 0, x1 = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						y1 = 0;
						for ( int y = n; y < n + 4; y++ )
						{
							A[p][x1][y1] = src.at<uchar>(x, y);
							y1++;
							temp += src.at<uchar>(x, y);
						}
						x1++;
					}
					avg = temp / 16;
					training_info[p].average = avg;

					if ( avg < 125 )
					{
						label = 0;
						training_info[p].label = label;
					}
					else if ( avg >= 125 && avg < 175 )
					{
						label = 128;
						training_info[p].label = label;
					}
					else if ( avg >= 175 )
					{
						label = 255;
						training_info[p].label = label;
					}

					training_info[p].label = label;
				}
			}
			p++;
		}
	}
	cout << "" << endl;
}

void ObtainBottomBlocks(Mat src, int A[2400][4][4])
{
	int temp;
	int height = src.rows - (src.rows % 4);
	int ht = src.rows;
	int wt = src.cols;
	int x1, y1;
	int width = src.cols - (src.cols % 4);
	int sum, avg, label;
	int p = 0;
	Mat dst = src.clone();
	for ( int m = src.rows/2; m < height; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					avg = 0, temp = 0, x1 = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						y1 = 0;
						for ( int y = n; y < n + 4; y++ )
						{
							A[p][x1][y1] = src.at<uchar>(x, y);
							y1++;
							temp += src.at<uchar>(x, y);
						}
						x1++;
					}
					avg = temp / 16;
					testing_info[p].average = avg;

					if ( avg < 125 )
					{
						label = 0;
						testing_info[p].label = label;
					}
					else if ( avg >= 125 && avg < 175 )
					{
						label = 128;
						testing_info[p].label = label;
					}
					else if ( avg >= 175 )
					{
						label = 255;
						testing_info[p].label = label;
					}

					testing_info[p].label = label;
				}
			}
			p++;
		}
	}
	cout << "" << endl;

}

Mat GenerateN1(Mat src, int A[2400][4][4], int B[2400][4][4], int no_of_blocks)
{
	Mat dst = src.clone();
	int mean, avg, temp, sum, classID, p;

	for ( int i = 0; i < no_of_blocks; i++ )
	{
		mean = 255;
		for ( int j = 0; j < no_of_blocks; j++ )
		{
			avg = 0, temp = 0;
			for ( int k = 0; k < 4; k++ )
			{
				for ( int l = 0; l < 4; l++ )
				{
					temp += pow(A[j][k][l] - B[i][k][l], 2);
				}
			}
			sum = sqrt(temp);
			avg = sum / 16;
			if ( avg < mean )
			{
				mean = avg;
				classID = j;
			}

		}
		for ( int x = 0; x < 4; x++ )
		{
			for ( int y = 0; y < 4; y++ )
			{
				B[i][x][y] = A[classID][x][y];		//Setting class label to bottom half
			}
		}
	}

	int xx, yy;
	int height = src.rows - (src.rows % 4);
	int width = src.cols - (src.cols % 4);

	p = 0;
	for ( int m = height / 2; m < height; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					xx = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						yy = 0;
						for ( int y = n; y < n + 4; y++ )
						{
							if ( B[p][xx][yy] == 128 )
							{
								dst.at<uchar>(x, y) = 0;
							}
							else
								dst.at<uchar>(x, y) = B[p][xx][yy];

							//dst.at<uchar>(x, y) = B[p][xx][yy];
						}
					}
				}
			}
			p++;
		}
	}
	return dst;
}

Mat GenerateN2(Mat src, int A[2400][4][4], int B[2400][4][4], int no_of_blocks)
{
	Mat dst = src.clone();
	int classID=0, count=0;
	unsigned int distance, new_distance = 255;
	for ( int i = 0; i < no_of_blocks; i++ )
	{
		distance = 255;
		for ( int j = 0; j < no_of_blocks; j++ )
		{
			new_distance = 255;
			new_distance = training_info[j].average - testing_info[i].average;
			if ( new_distance < distance )
			{
				distance = new_distance;
				classID = j;
			}

		}

		for ( int k = 0; k < 4; k++ )
		{
			for ( int l = 0; l < 4; l++ )
			{
				B[i][k][l] = A[classID][k][l];
				//Hello
			}
		}
	}

	int height = src.rows - (src.rows % 4);
	int width = src.cols - (src.cols % 4);
	int xx, yy;
	int p = 0;
	for ( int m = height / 2; m < height; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					xx = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						yy = 0;
						for ( int y = n; y < n + 4; y++ )
						{
							dst.at<uchar>(x, y) = B[p][xx][yy];
						}
					}
				}
			}
			p++;
		}
	}
	
	return dst;
}

Mat GenerateN3(Mat src, int A[2400][4][4], int B[2400][4][4], int no_of_blocks)
{
	Mat dst = src.clone();
	int classID = 0, count = 0;
	unsigned int distance, new_distance = 255;
	for ( int i = 0; i < no_of_blocks; i++ )
	{
		distance = 255;
		for ( int j = 0; j < no_of_blocks; j++ )
		{
			new_distance = 255;
			new_distance = training_info[j].average - testing_info[i].average;
			if ( new_distance < distance )
			{
				distance = new_distance;
				classID = j;
			}

		}

		for ( int k = 0; k < 4; k++ )
		{
			for ( int l = 0; l < 4; l++ )
			{
				B[i][k][l] = training_info[classID].average;
				//Hello
			}
		}
	}

	int height = src.rows - (src.rows % 4);
	int width = src.cols - (src.cols % 4);
	int xx, yy;
	int p = 0;
	for ( int m = height / 2; m < height; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					xx = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						yy = 0;
						for ( int y = n; y < n + 4; y++ )
						{
							dst.at<uchar>(x, y) = B[p][xx][yy];
						}
					}
				}
			}
			p++;
		}
	}

	return dst;
}

int getAverageOfClasses(int matching_label)
{
	int i = 0;
	int temp = 0, avg;
	while ( i < 2400 )
	{
		if ( training_info[i].label == matching_label )
		{
			temp += training_info[i].average;
		}
		i++;
	}
	avg = temp / i;
	return avg;
}

Mat GenerateN4(Mat src, int A[2400][4][4], int B[2400][4][4], int no_of_blocks)
{
	Mat dst = src.clone();
	int classID = 0, count = 0, matching_label, cumulative_avg;
	unsigned int distance, new_distance = 255;
	for ( int i = 0; i < no_of_blocks; i++ )
	{
		distance = 255;
		for ( int j = 0; j < no_of_blocks; j++ )
		{
			new_distance = 255;
			new_distance = training_info[j].average - testing_info[i].average;
			if ( new_distance < distance )
			{
				distance = new_distance;
				classID = j;
			}

		}
		
		matching_label = training_info[classID].label;
		cumulative_avg = getAverageOfClasses(matching_label);

		for ( int k = 0; k < 4; k++ )
		{
			for ( int l = 0; l < 4; l++ )
			{
				B[i][k][l] = cumulative_avg;
			}
		}
	}

	int height = src.rows - (src.rows % 4);
	int width = src.cols - (src.cols % 4);
	int xx, yy;
	int p = 0;
	for ( int m = height / 2; m < height; m += 4 )
	{
		for ( int n = 0; n < width; n += 4 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 4; i++ )
			{
				for ( int j = n; j < n + 4; j++ )
				{
					xx = 0;
					for ( int x = m; x < m + 4; x++ )
					{
						yy = 0;
						for ( int y = n; y < n + 4; y++ )
						{
							dst.at<uchar>(x, y) = B[p][xx][yy];
						}
					}
				}
			}
			p++;
		}
	}

	return dst;
}

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


// CSolution5View message handlers


void CSolution5View::OnDivide()											//Main function
{
	Mat src = ObtainImage();
	//Mat src = imread("E:\\Assignment 4 images\\TestImage-even-width.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat M1 = src.clone();
	int temp;
	int height = src.rows - (src.rows % 4);
	int ht = src.rows;
	int wt = src.cols;
	int width = src.cols - (src.cols % 4);
	int sum, avg;

	namedWindow("Input", CV_WINDOW_AUTOSIZE);
	imshow("Input", src);

	//For creating image M1
	M1 = GenerateM1(src);

	namedWindow("M1", CV_WINDOW_AUTOSIZE);
	imshow("M1", M1);

	/*Mat T1 = src.clone();
	T1 = GenerateT1(src);

	namedWindow("T1", CV_WINDOW_AUTOSIZE);
	imshow("T1", T1);*/
	
}


//void CSolution5View::OnKmeans()
//{
//	Mat src = ObtainImage();
//	Mat dst = src.clone();
//	int height = src.rows;
//	int width = src.cols;
//
//	/*for ( int i = 0; i < height/2; i++ )
//	{
//		for ( int j = 0; j < width; j++ )
//		{
//			if ( src.at<uchar>(i, j) > 0 && src.at<uchar>(i, j) <= 85 )
//				dst.at<uchar>(i, j) = 0;
//			else if ( src.at<uchar>(i, j) > 85 && src.at<uchar>(i, j) <= 170 )
//				dst.at<uchar>(i, j) = 128;
//			else if( src.at<uchar>(i, j) > 170 && src.at<uchar>(i, j) <= 255 )
//				dst.at<uchar>(i, j) = 255;
//		}
//	}*/
//
//	namedWindow("DST", CV_WINDOW_AUTOSIZE);
//	imshow("DST", dst);
//
//
//	
//}


void CSolution5View::OnImagen1()
{
	Mat src = ObtainImage();
	Mat M1 = src.clone();
	M1 = GenerateM1(src);

	no_of_blocks = ((src.rows / 2) / 4) * (src.cols / 4);
	//Generating image N1
	ObtainTopBlocks(M1, training_blocks);
	ObtainBottomBlocks(M1, testing_blocks);
	Mat N1 = M1.clone();
	N1 = GenerateN1(M1, training_blocks, testing_blocks, no_of_blocks);

	namedWindow("N1", CV_WINDOW_AUTOSIZE);
	imshow("N1", N1);
	
}


void CSolution5View::OnImagen2()
{
	Mat src = ObtainImage();
	no_of_blocks = ((src.rows / 2) / 4) * (src.cols / 4);
	//Generating image N2
	ObtainTopBlocks(src, training_blocks);
	ObtainBottomBlocks(src, testing_blocks);
	Mat N2 = src.clone();
	N2 = GenerateN2(src, training_blocks, testing_blocks, no_of_blocks);

	namedWindow("N2", CV_WINDOW_AUTOSIZE);
	imshow("N2", N2);
}


void CSolution5View::OnMotionvectors()
{
	Mat src1 = ObtainImage();
	Mat src2 = ObtainImage();

	Mat dst1 = src1.clone();
	Mat dst2 = src2.clone();
	unsigned int difference;
	int avg, temp;

	for ( int i = 0; i < dst1.rows; i++ )
	{
		for ( int j = 0; j < dst1.cols; j++ )
		{
			dst1.at<uchar>(i, j) = 0;
		}
	}

	for ( int i = 0; i < dst1.rows; i++ )
	{
		for ( int j = 0; j < dst1.cols; j++ )
		{
			dst2.at<uchar>(i, j) = 0;
		}
	}

	for ( int m = 0; m < src1.rows; m += 8 )
	{
		for ( int n = 0; n < src1.cols; n += 8 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 8; i++ )
			{
				for ( int j = n; j < n + 8; j++ )
				{
					avg = 0, temp = 0;
					for ( int x = m; x < m + 8; x++ )
					{
						for ( int y = n; y < n + 8; y++ )
						{
							temp += src1.at<uchar>(x, y);
						}
					}

					avg = temp / 64;

					if ( avg < 125 )
						avg = 0;
					else if ( avg >= 125 && avg < 175 )
						avg = 128;
					else if ( avg >= 175 )
						avg = 255;

					for ( int x = m; x < m + 8; x++ )
					{
						for ( int y = n; y < n + 8; y++ )
						{
							dst1.at<uchar>(x, y) = avg;

						}
					}
				}
			}
		}
	}
	
	for ( int m = 0; m < src1.rows; m += 8 )
	{
		for ( int n = 0; n < src1.cols; n += 8 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 8; i++ )
			{
				for ( int j = n; j < n + 8; j++ )
				{
					avg = 0, temp = 0;
					for ( int x = m; x < m + 8; x++ )
					{
						for ( int y = n; y < n + 8; y++ )
						{
							temp += src2.at<uchar>(x, y);
						}
					}

					avg = temp / 64;

					if ( avg < 125 )
						avg = 0;
					else if ( avg >= 125 && avg < 175 )
						avg = 128;
					else if ( avg >= 175 )
						avg = 255;

					for ( int x = m; x < m + 8; x++ )
					{
						for ( int y = n; y < n + 8; y++ )
						{
							dst2.at<uchar>(x, y) = avg;
						}
					}
				}
			}
		}
	}

	//int difference;
	for ( int m = 0; m < src1.rows; m += 8 )
	{
		for ( int n = 0; n < src1.cols; n += 8 )					//These two loops divide the image into 4x4 blocks
		{
			for ( int i = m; i < m + 8; i++ )
			{
				for ( int j = n; j < n + 8; j++ )
				{
					for ( int x = m; x < m + 8; x++ )
					{
						for ( int y = n; y < n + 8; y++ )
						{
							difference = dst1.at<uchar>(x, y) - dst2.at<uchar>(x, y);
							dst2.at<uchar>(x, y) = difference;
						}
					}
				}
			}
		}
	}

	namedWindow("DST1", CV_WINDOW_AUTOSIZE);
	imshow("DST1", dst1);

	namedWindow("DST2", CV_WINDOW_AUTOSIZE);
	imshow("DST2", dst2);
}


void CSolution5View::OnImagen3()
{
	Mat src = ObtainImage();
	no_of_blocks = ((src.rows / 2) / 4) * (src.cols / 4);
	//Generating image N3
	ObtainTopBlocks(src, training_blocks);
	ObtainBottomBlocks(src, testing_blocks);
	Mat N3 = src.clone();
	N3 = GenerateN3(src, training_blocks, testing_blocks, no_of_blocks);

	namedWindow("N3", CV_WINDOW_AUTOSIZE);
	imshow("N3", N3);
}


void CSolution5View::OnImagen4()
{
	Mat src = ObtainImage();
	no_of_blocks = ((src.rows / 2) / 4) * (src.cols / 4);
	//Generating image N4
	ObtainTopBlocks(src, training_blocks);
	ObtainBottomBlocks(src, testing_blocks);
	Mat N4 = src.clone();
	N4 = GenerateN4(src, training_blocks, testing_blocks, no_of_blocks);

	namedWindow("N4", CV_WINDOW_AUTOSIZE);
	imshow("N4", N4);
}


void CSolution5View::OnImaget1()
{
	Mat src = ObtainImage();
	Mat T1 = src.clone();
	T1 = GenerateT1(src);

	int testing_blocksA[2400][4][4];
	int testing_blocksB[2400][4][4];

	namedWindow("T1", CV_WINDOW_AUTOSIZE);
	imshow("T1", T1);

	Mat M1 = src.clone();
	M1 = GenerateM1(src);

	no_of_blocks = ((src.rows / 2) / 4) * (src.cols / 4);
	
	//Generating and displaying image N1
	ObtainTopBlocks(M1, training_blocks);
	ObtainBottomBlocks(M1, testing_blocks);
	Mat N1 = src.clone();
	N1 = GenerateN1(M1, training_blocks, testing_blocks, no_of_blocks);
	namedWindow("N1", CV_WINDOW_AUTOSIZE);
	imshow("N1", N1);

	ObtainBottomBlocks(N1, testing_blocksB);

	float no_of_different_averages = 0;
	float no_of_same_averages = 0;
	float no_of_different_labels = 0;
	float no_of_same_labels = 0;

	float error_rate = 0;
	
	//Calculating similar and different class averages and labels between training and testing vectors
	for ( int i = 0; i < no_of_blocks; i++ )
	{
		
		if ( training_info[i].average != testing_info[i].average )
			no_of_different_averages++;
		else if ( training_info[i].average == testing_info[i].average )
			no_of_same_averages++;

		if ( training_info[i].label != testing_info[i].label )
			no_of_different_labels++;
		else if ( training_info[i].label == testing_info[i].label )
			no_of_same_labels++;

	}

	//Calculating and printing the error rate
	error_rate = (no_of_different_labels / no_of_blocks) * 100;
	std::string error = std::to_string(error_rate);
	MessageBoxA(NULL, error.c_str(), "Error Rate", MB_OK);

}
