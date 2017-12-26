#include<iostream>
#include<cmath>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#define MASK_SIZE 11
#define PI 3.14

using namespace std;
using namespace cv;

//Calculates X gradient
int calculateXGradient(Mat image, int i, int j)
{
	int gx = (image.at<uchar>(i - 1, j - 1) + (2 * image.at<uchar>(i, j - 1)) + image.at<uchar>(i + 1, j - 1)) - (image.at<uchar>(i - 1, j + 1) + (2 * image.at<uchar>(i, j + 1)) + image.at<uchar>(i + 1, j + 1));
	return gx;
}

//Calculates Y gradient
int calculateYGradient(Mat image, int i, int j)
{
	int gy = (image.at<uchar>(i + 1, j + 1) + (2 * image.at<uchar>(i + 1, j)) + image.at<uchar>(i + 1, j - 1)) - (image.at<uchar>(i - 1, j + 1) + (2 * image.at<uchar>(i - 1, j)) + image.at<uchar>(i - 1, j - 1));
	return gy;
}

//Applies Sobel operator on image
void sobel_operation(Mat initial_image)
{
	Mat final_image;
	int Gx, Gy, G;

	final_image = initial_image.clone();																				//Make a copy of loaded image

	for (int i = 0; i < final_image.rows; i++)																			//Setting all pixel values to zero in cloned image
	{
		for (int j = 0; j < final_image.cols; j++)
		{
			final_image.at<uchar>(i, j) = 0.0;
		}
	}

	for (int i = 1; i < final_image.rows - 1; i++)																		//We ignore the boundary of the initial image of thickness 1 pixel
	{
		for (int j = 1; j < final_image.cols - 1; j++)
		{
			Gx = calculateXGradient(initial_image, i, j);
			Gy = calculateYGradient(initial_image, i, j);
			G = abs(Gx) + abs(Gy);																						//Calculate gradient magnitude
			
			if (G > 255)
			{
				G = 255;
			}
			if (G < 0)
			{
				G = 0;
			}

			final_image.at<uchar>(i, j) = G;																			//Assigning gradient value to each pixel

		}
	}

	namedWindow("Initial Image");
	imshow("Initial Image", initial_image);																				//Display initial image

	namedWindow("Final Image");
	imshow("Final Image", final_image);																					//Display final image

	waitKey();

	cvDestroyAllWindows();
}

//Adds two images
Mat add_images(Mat src, Mat dest)
{
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			dest.at<uchar>(i, j) = src.at<uchar>(i, j) + dest.at<uchar>(i, j);
		}
	}

	return dest;
}

//Subtracts two images
Mat subtract_images(Mat src, Mat dest)
{
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			dest.at<uchar>(i, j) = src.at<uchar>(i, j) - dest.at<uchar>(i, j);
		}
	}

	return dest;
}

//Performs Unsharp masking on image
void unsharp_masking(Mat initial_image)
{
	Mat final_image = initial_image.clone();
	int sum;

	for (int i = 0; i < final_image.rows; i++)
	{
		for (int j = 0; j < final_image.cols; j++)
		{
			final_image.at<uchar>(i, j) = 0.0;
		}
	}

	//Setting the Gaussian mask for blurring
	float GaussianMask[3][3] = 
	{
		{1/16.0, 2/16.0, 1/16.0},
		{2/16.0, 4/16.0, 2/16.0},
		{1/16.0, 2/16.0, 1/16.0}
	};

	//Performing convolution operation on the image using the Gaussian mask
	for (int i = 1; i < final_image.rows-1; i++)
	{
		for (int j = 1; j < final_image.cols-1; j++)
		{
			final_image.at<uchar>(i, j) = (GaussianMask[2][2] * initial_image.at<uchar>(i - 1, j + 1)) + (GaussianMask[1][2] * initial_image.at<uchar>(i, j + 1)) + (GaussianMask[0][2] * initial_image.at<uchar>(i + 1, j + 1)) + (GaussianMask[2][1] * initial_image.at<uchar>(i - 1, j)) + (GaussianMask[1][1] * initial_image.at<uchar>(i, j)) + (GaussianMask[0][1] * initial_image.at<uchar>(i + 1, j)) + (GaussianMask[2][0] * initial_image.at<uchar>(i - 1, j - 1)) + (GaussianMask[1][0] * initial_image.at<uchar>(i, j - 1)) + (GaussianMask[0][0] * initial_image.at<uchar>(i + 1, j - 1));
		}
	}

	namedWindow("Initial");
	imshow("Initial", initial_image);										//Original image
	namedWindow("Final");
	imshow("Final", final_image);											//Blurred image using Gaussian mask

	final_image = subtract_images(initial_image, final_image);
	final_image = add_images(initial_image, final_image);

	namedWindow("Final after addition");									//Enhanced image
	imshow("Final after addition", final_image);

	waitKey();
	cvDestroyAllWindows();
}

//Calculates and prints Laplacian of Gaussian mask
void calculate_LoG(Mat initial_image)
{
	int mask[MASK_SIZE][MASK_SIZE];
	long float sigma;
	int n;																	//Size of mask

	cout << "Enter size of mask: ";
	cin >> n;

	cout << "Enter value of sigma: ";
	cin >> sigma;

	float s2 = sigma * sigma;
	long float s4 = sigma * sigma * sigma * sigma;

	int min = int(ceil(-n / 2));
	int max = (ceil(n / 2));

	int l = 0;

	//Calculating the Laplacian of Gaussian mask values
	if (sigma == 5)
	{
		for (int i = min; i <= max; ++i)
		{
			int m = 0;
			for (int j = min; j <= max; ++j)
			{
				float value = (float)(-1.0 / (PI * (s4))) * (1.0 - ((i * i + j * j) / (2.0 * (s2)))) * (float)pow(2.7, ((-(i * i + j * j) / (2.0 * (s2)))));
				mask[l][m] = int(value * 40000);
				++m;
			}
			++l;
		}
	}
	else
	{
		for (int i = min; i <= max; ++i)
		{
			int m = 0;
			for (int j = min; j <= max; ++j)
			{
				float value = (float)(-1.0 / (PI * (s4))) * (1.0 - ((i * i + j * j) / (2.0 * (s2)))) * (float)pow(2.7, ((-(i * i + j * j) / (2.0 * (s2)))));
				mask[l][m] = int(value * 483);
				++m;
			}
			++l;
		}
	}

	//Print out the LoG mask
	cout << endl << "Laplacian of Gaussian:" << endl;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << mask[i][j] << "\t";
		}
		cout << endl;
	}
	//Initialize final image values to zero
	Mat final_image = initial_image.clone();
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			final_image.at<uchar>(i, j) = 0;
		}
	}

	//Applying the Laplacian of Gaussian mask to the image using correlation
	for (int x = max; x < initial_image.rows - max; x++)
	{
		for (int y = max; y < initial_image.cols - max; y++)
		{
			int p = 0, q = 0;
			int value = 0;
			for (int s = min; s <= max; s++)
			{
				for (int t = min; t <= max; t++)
				{
					
					value = value + mask[p][q] * initial_image.at<uchar>(x + s, y + t);
					q++;
				}
				p++;
			}
			if (value < 0)
			{
				value = 0;
			}
			else
			{
				final_image.at<uchar>(x, y) = int(value) / 10000000;
			}
		}
	}
	
	namedWindow("Initial Image");
	imshow("Initial Image", initial_image);

	namedWindow("Final Image");
	imshow("Final Image", final_image);
	
	waitKey();
	cvDestroyAllWindows();

}

int main(int argc, char *argv[])
{
	Mat initial_image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	if (!initial_image.data)
	{
		cout << "ERROR! You need to enter the full path of an image as a command line argument";
		return -1;
	}
	int ch;


	do
	{
		//Interactive menu for programming questions
		cout << endl << "Assignment 2 Programming Section:" << endl << "1. Apply Unsharp Masking method" << endl << "2. Apply Sobel operator" << endl << "3. Calculate Laplacian of Gaussian" << endl << "4. Exit" << endl << "Enter a choice:";
		cin >> ch;
		switch (ch)
		{
			case 2: sobel_operation(initial_image);
				break;
			case 1: unsharp_masking(initial_image);
				break;
			case 3: calculate_LoG(initial_image);
				break;
			default: break;
		}
	}
	while (ch != 4);

	waitKey();
	return 0;

}