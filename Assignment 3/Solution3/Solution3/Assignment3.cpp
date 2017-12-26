#include<iostream>
#include<opencv2/opencv.hpp>
#include<math.h>

#define PI 3.141592653
#define N 8

using namespace std;
using namespace cv;

double alpha(int i)
{
	if (i == 0)
		return (1 / sqrt(2));
	return 1;
}

//Computes the cosine values
float cosineY(int i, int x)
{
	return cos((2 * x + 1) * i * PI) / (2 * 8);
}

float cosineX(int j, int y)
{
	return cos((2 * y + 1) * j * PI) / (2 * 2);
}

//Displays the image and saves the image to disk
void printImage(Mat src_image, string filename)
{
	namedWindow(filename, CV_WINDOW_AUTOSIZE);
	imshow(filename, src_image);
	waitKey();
	imwrite("E:/Assignment 3 outputs/"+filename, src_image);
	cvDestroyAllWindows();
}

//Computes the inverse discrete cosine transform
Mat IDCTImage(Mat src_image)
{
	int height = src_image.rows - (src_image.rows % 8);
	int width = src_image.cols - (src_image.cols % 8);
	float temp;
	//Mat dst_image(src_image.rows, src_image.cols, src_image.type());
	Mat dst_image = src_image.clone();

	for (int m = 0; m < height; m += 8)
	{
		for (int n = 0; n < width; n += 8)					//These two loops divide the image into 8x8 blocks
		{
			for (int i = m; i < m + 8; i++)
			{
				for (int j = n; j < n + 8; j++)
				{
					temp = 0.0;
					for (int x = m; x < m + 8; x++)
					{
						for (int y = n; y < n + 8; y++)				//These four loops calculate the IDCT coefficients for each 8x8 block
						{
							temp += (src_image.at<Vec3b>(x, y)[2]) *
								(cos((((2 * x) + 1)) * ((i * PI)) / (2 * 8))) *
								(cos((((2 * y) + 1)) * ((j * PI)) / (2 * 8))) *
								(alpha(x) * alpha(y));			//Formula for calculating IDCT
						}
					}
					//temp *= 1 / 4;
					dst_image.at<Vec3b>(i, j)[2] = int(temp);				//Assigning the calculated IDCT coefficient to each intensity pixel in I

				}
			}
		}
	}

	return dst_image;
}

//Keeps the first 9 frequency coefficients of each DCT block
Mat DCComponent_nine(Mat src_image)
{
	int height = src_image.rows - (src_image.rows % 8);
	int width = src_image.cols - (src_image.cols % 8);
	Mat dst_image = src_image.clone();


	for (int m = 0; m < height; m += 8)
	{
		for (int n = 0; n < width; n += 8)
		{
			for (int i = m; i < m + 8; i++)
			{
				for (int j = n; j < n + 8; j++)
				{
					if (i == m && j == n)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if(i == m && j == n+1)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if (i == m && j == n + 2)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if (i == m && j == n + 3)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if (i == m + 1 && j == n)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if (i == m + 1 && j == n+1)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if (i == m + 1 && j == n+2)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if (i == m + 2 && j == n)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else if (i == m + 2 && j == n + 1)
					{
						dst_image.at<Vec3b>(i, j)[2] = dst_image.at<Vec3b>(i, j)[2];
					}
					else
					{
						dst_image.at<Vec3b>(i, j)[2] = 0;
					}
					//cout << endl << dst_image.at<Vec3b>(i, j);
				}
			}
		}
	}
	return dst_image;
}

//Keeps the DC frequency component of each DCT block
Mat DCComponent(Mat src_image)
{
	int height = src_image.rows - (src_image.rows % 8);
	int width = src_image.cols - (src_image.cols % 8);
	//Mat dst_image(src_image.rows, src_image.cols, src_image.type());
	Mat dst_image = src_image.clone();

	for (int m = 0; m < height; m += 8)
	{
		for (int n = 0; n < width; n += 8)					
		{
			for (int i = m; i < m + 8; i++)
			{
				for (int j = n; j < n + 8; j++)
				{
					if (i == m && j == n)
					{
						//Do nothing
					}
					else
					{
						dst_image.at<Vec3b>(i, j)[2] = 0;
					}
					//cout << endl << dst_image.at<Vec3b>(i, j);
				}
			}
		}
	}
	return dst_image;
}

//Computes the DCT of the whole image
Mat DCTImage(Mat src_image)
{
	int height = src_image.rows - (src_image.rows % 8);
	int width = src_image.cols - (src_image.cols % 8);
	float temp;
	Mat dst_image = src_image.clone();
	for (int m = 0; m < height; m += 8)
	{
		for (int n = 0; n < width; n += 8)					//These two loops divide the image into 8x8 blocks
		{
			for (int i = m; i < m + 8; i++)
			{
				for (int j = n; j < n + 8; j++)
				{
					temp = 0.0;
					for (int x = m; x < m + 8; x++)
					{
						for (int y = n; y < n + 8; y++)				//These four loops calculate the DCT coefficients for each 8x8 block
						{
							temp += (src_image.at<Vec3b>(x, y)[2]) *
								(cos((((2 * x) + 1)) * ((i * PI)) / (2 * 8))) *
								(cos((((2 * y) + 1)) * ((j * PI)) / (2 * 8)));			//Formula for calculating DCT
						}
					}
					temp *= ((1 / sqrt(2 * 8))) * (alpha(i) * alpha(j));
					dst_image.at<Vec3b>(i, j)[2] = int(temp);				//Assigning the calculated DCT coefficient to each intensity pixel in I
				}

			}
		}
	}
	return dst_image;
}

//Converts RGB image to HSI
Mat toHSI(Mat src_image)
{
	Mat hsi_image(src_image.rows, src_image.cols, src_image.type());

	float red, green, blue, hue, sat, in;

	for (int i = 0; i < src_image.rows; i++)
	{
		for (int j = 0; j < src_image.cols; j++)
		{
			blue = src_image.at<Vec3b>(i, j)[0];			//Obtain blue channel of pixel
			green = src_image.at<Vec3b>(i, j)[1];			//Obtain green channel of pixel
			red = src_image.at<Vec3b>(i, j)[2];				//Obtain red channel of pixel

			in = (blue + green + red) / 3;			//Calculating the intensity value

			int min_value = 0;
			min_value = std::min(red, std::min(blue, green));
			sat = 1 - 3 * (min_value / (blue + green + red));		//Calculating the saturation value
			if (sat < 0.00001)
			{
				sat = 0;
			}
			else if (sat > 0.99999)
			{
				sat = 1;
			}

			if (sat != 0)
			{
				//Calculating the hue value
				hue = 0.5 * ((red - green) + (red - blue)) / sqrt(((red - green)*(red - green)) + ((red - blue)*(green - blue)));
				hue = acos(hue);

				if (blue <= green)
				{
					hue = hue;
				}
				else
				{
					hue = ((360 * 3.14159265) / 180.0) - hue;
				}
			}

			//hsi_image.at<Vec3b>(i, j)[0] = (hue * 180) / 3.14159265;
			//hsi_image.at<Vec3b>(i, j)[1] = sat * 100;
			hsi_image.at<Vec3b>(i, j)[2] = in;					//Obtaining the intensity image
		}

	}

	namedWindow("RGB image", CV_WINDOW_AUTOSIZE);
	imshow("RGB image", src_image);

	namedWindow("HSI image", CV_WINDOW_AUTOSIZE);
	imshow("HSI image", hsi_image);

	waitKey(0);
	
	cvDestroyAllWindows();
	return hsi_image;

}

int main(int argc, char *argv[])
{
	Mat src_image = imread(argv[1], 1);		//Load input image in RGB format
	Mat i_image;
	float temp;
	int ch;
	if (src_image.empty())					//If no command line arguments are passed
	{
		cout << "Error loading image" << endl;
	}

	i_image = toHSI(src_image);				//Converting RGB image to HSI and displaying intensity image I
	Mat F;			
	F = DCTImage(i_image);
	
	printImage(F, "F Image.bmp");		//Calculating DCT coefficients of image and printing image
	Mat D1;

	D1 = DCComponent(F);				//Keeping DC component and printing image
	printImage(D1, "D1 Image.bmp");

	Mat R1;
	R1 = IDCTImage(D1);
	printImage(R1, "R1 Image.bmp");

	Mat D2;
	D2 = DCComponent_nine(F);
	printImage(D2, "D2 Image.bmp");

	Mat R2;
	R2 = IDCTImage(D2);
	printImage(R2, "R2 Image.bmp");

	Mat OriginalImage;
	OriginalImage = IDCTImage(F);
	printImage(OriginalImage, "Original Image.bmp");

	return 0;

}