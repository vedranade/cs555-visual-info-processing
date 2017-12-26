#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <dos.h>
#include <Windows.h>

using namespace cv;
using namespace std;

int original_image_histogram_values[255];
int modified_image_histogram_values[255];													//Holds histogram data

long double cumulative_histogram[256];																//Variable for storing cumulative histogram data
Mat img;

void generateHistogram(Mat img, int original_image_histogram_values[])
{
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			original_image_histogram_values[int(img.at<uchar>(i, j))]++;						//Generate histogram for original image
		}
	}
}

void generateNegativeImage(Mat img, int original_image_histogram_values[])
{
	for (int i = 0; i < img.rows; i++)															//Loop that computes negative greyscale image
	{
		for (int j = 0; j < img.cols; j++)
		{
			img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);									//Modifies every pixel greyscale value of image
			original_image_histogram_values[int(img.at<uchar>(i, j))]++;						//Generates histogram data for negative greyscale image	
		}
	}
}

void displayHistogram(int hist[], string name_of_window)
{
	int histogram_height = 400;
	int histogram_width = 512;
	int max_intensity_value = 0;
	int binary_width = cvRound((double)histogram_width / 256);
	Mat histogram_image(histogram_height, histogram_width, CV_8UC1, Scalar(255, 255, 255));

	max_intensity_value = hist[0];

	for (int i = 1; i < 256; i++)
	{
		if (hist[i] > max_intensity_value)
		{
			max_intensity_value = hist[i];
		}
	}

	for (int i = 0; i < 256; i++)
	{
		hist[i] = ((double)hist[i] / max_intensity_value) * histogram_image.rows;
	}

	for (int i = 0; i < 256; i++)
	{
		line(histogram_image, Point(binary_width*(i), histogram_height), Point(binary_width*(i), histogram_height - hist[i]), Scalar(0,0,0),1,8,0);
		//line(histogram_image, Point(j, histogram_height-20), Point(j, (histogram_height-20) - original_image_histogram_values[i]), Scalar(0, 0, 0), 1, 8, 0);
	}

	namedWindow(name_of_window, WINDOW_AUTOSIZE);
	imshow(name_of_window, histogram_image);
}


void equalizeHistogram(Mat img)
{
	int size = img.rows*img.cols;
	//displayHistogram(original_image_histogram_values, "Original Histogram");
	cumulative_histogram[0] = original_image_histogram_values[0];
	for (int i = 1; i < 256; i++)
	{
		cumulative_histogram[i] = original_image_histogram_values[i] + cumulative_histogram[i - 1];
	}

	float alpha = 255.0 / size;
	float intensity_probability[256];
	for (int i = 0; i < 256; i++)
	{
		intensity_probability[i] = (double)original_image_histogram_values[i] / size;
	}
	
	int scaled_histogram[256];

	for (int i = 0; i < 256; i++)
	{
		scaled_histogram[i] = cvRound((double)cumulative_histogram[i] * alpha);
	}

	float equalized_histogram[256];
	for (int i = 0; i < 256; i++)
	{
		equalized_histogram[i] = 0;
	}
	for (int i = 0; i < 256; i++)
	{
		equalized_histogram[scaled_histogram[i]] = equalized_histogram[scaled_histogram[i]] + intensity_probability[i];
	}
	int final_histogram[256];
	for (int i = 0; i < 256; i++)
	{
		final_histogram[i] = cvRound(equalized_histogram[i] * 255);
	}
	Mat enhanced_image = img.clone();
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			enhanced_image.at<uchar>(i, j) = saturate_cast<uchar>(scaled_histogram[img.at<uchar>(i, j)]);
		}
	}

	displayHistogram(scaled_histogram, "Scaled Histogram");
	
	namedWindow("Enhanced Image", WINDOW_AUTOSIZE);
	imshow("Enhanced Image", enhanced_image);

}

int main(int argc, char** argv)
{
	
	string filename;
	
	cout << "Enter image name with full path and extension whose negative greyscale is to be generated:" << endl;
	cin >> filename;
	
	img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);											//Reads image from the file name and path entered as the 2nd command line argument
	
	
	int a;
	for (a = 0; a < 256; a++)																	//Setting histogram Y axis values to zero
	{
		original_image_histogram_values[a] = 0;
		modified_image_histogram_values[a] = 0;
	}
	
	namedWindow("Original Image", WINDOW_AUTOSIZE);												//Creating window to display images
	cout << "Original image: " << endl;
	imshow("Original Image", img);																//Show original image
	
	generateHistogram(img, original_image_histogram_values);
	displayHistogram(original_image_histogram_values, "Original Histogram");
	
	generateNegativeImage(img, original_image_histogram_values);
	displayHistogram(original_image_histogram_values, "Negative Image Histogram");
	

	namedWindow("Negative Image", WINDOW_AUTOSIZE);
	imshow("Negative Image", img);
	cout << "Press any key to continue.." << endl;
	waitKey();
	cvDestroyAllWindows();
	
	cout << "Enter image name and full path for generating its enhanced form:" << endl;
	cin >> filename;
	img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

	namedWindow("Original Image", WINDOW_AUTOSIZE);
	cout << "Original Image: " << endl;
	imshow("Original Image", img);

	generateHistogram(img, original_image_histogram_values);
	equalizeHistogram(img);

	cout << "Press any key to continue.." << endl;
	waitKey(0);
	cvDestroyAllWindows();
	return 0;
}