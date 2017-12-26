#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <dos.h>
#include <Windows.h>
#include<math.h>


using namespace cv;
using namespace std;

int getAverageIntensityValue(int hist[], int a, int b)													//Function to find average_intensity_value of histogram based on algorithm provided in the README file
{
	int sum_of_pixels=0;
	sum_of_pixels = sum_of_pixels + hist[0];
	int sum_product_of_pixels = (hist[0] * 0);

	for (int i = a; i < b; i++)
	{
		sum_of_pixels = sum_of_pixels + hist[i];
	}

	for (int i = a; i < b; i++)
	{
		sum_product_of_pixels = sum_product_of_pixels + (hist[i] * i) ;
	}

	int average_intensity_value = sum_product_of_pixels / sum_of_pixels;														//average_intensity_value calculation code ends here
	return average_intensity_value;
}

Mat getBinaryImage(Mat img, int threshold)
{
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			if (int(img.at<uchar>(i, j)) <= threshold)
			{
				img.at<uchar>(i, j) = 0;																						//If pixel intensity is greater than threshold
			}
			else
			{
				img.at<uchar>(i, j) = 255;																						//If pixel intensity is less than threshold
			}
		}
	}

	return img;
}

int main(int argc, char** argv)
{

	if (argc != 2)																							//If command line arguments are insufficient
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	Mat img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);														//Load the image
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", img);
	int rows = img.rows;
	int cols = img.cols;
	int original_image_histogram_values[255];
	for (int a = 0; a < 256; a++)																			//Setting histogram Y axis values to zero
	{
		original_image_histogram_values[a] = 0;
	}

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			original_image_histogram_values[int(img.at<uchar>(i, j))]++;										//Generate histogram for original image
		}
	}

	int old_threshold = 128;
	int new_threshold = 128;
	int delta_threshold = 2;
	int m1, m2;
	
	do
	{
		old_threshold = new_threshold;
		m1 = getAverageIntensityValue(original_image_histogram_values, 1, old_threshold);
		m2 = getAverageIntensityValue(original_image_histogram_values, old_threshold + 1, 256);
		new_threshold = (m1 + m2) / 2;
		
	} 
	while ((old_threshold-new_threshold) > delta_threshold);
	img = getBinaryImage(img, new_threshold);
	cout << "Threshold value T: " << new_threshold << endl;
	namedWindow("Binary Image", WINDOW_AUTOSIZE);
	imshow("Binary Image", img);
	
	waitKey(0);
	cvDestroyWindow("Binary Image");
	return 0;
}

