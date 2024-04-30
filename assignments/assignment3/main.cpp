#include <iostream>
#include <fstream>
#include <algorithm>

void problem_1();
void problem_2();
void problem_3();
void image2matrix(std::ifstream& image, int** matrix, const int width, const int height);
void matrix2image(std::ofstream& image, int** matrix, const int width, const int height);
void init_matrix(int** matrix, const int width, const int height);
void delete_matrix(int*** matrix, const int height);
void init_kernel(int** kernel);
void gaussian_filter(int** filtered_matrix, int** matrix, int intensity_range, const int width, const int height, int kernel_size, int sigma, double k);
void laplacian_filter(int** filtered_matrix, int** matrix, int intensity_range, const int width, const int height, int** kernel, int kernel_size, double c);
void median_filter(int** filtered_matrix, int** matrix, const int width, const int height, int intensity_range, const int kernel_size);

int main()
{
	problem_1();
	problem_2();
	problem_3();

	return 0;
}

void problem_1()
{
	const int image_width = 512, image_height = 512;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** matrix = new int* [image_height];
	int** filtered_matrix = new int* [image_height];

	std::ifstream image1("../data/lenna-noise.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../result/lena-gaussian.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		std::cout << "problem 1: failed to open one or more image files\n";
		return;
	}

	init_matrix(matrix, image_width, image_height);
	init_matrix(filtered_matrix, image_width, image_height);

	image2matrix(image1, matrix, image_width, image_height);

	gaussian_filter(filtered_matrix, matrix, intensity_range, image_width, image_height, 5, 2, 1.67);

	matrix2image(image2, filtered_matrix, image_width, image_height);

	delete_matrix(&matrix, image_height);
	delete_matrix(&filtered_matrix, image_height);

	image1.close();
	image2.close();

	std::cout << "problem 1: done\n";
}

void problem_2()
{
	const int image_width = 512, image_height = 512;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** matrix = new int* [image_height];
	int** filtered_matrix = new int* [image_height];
	const int kernel_size = 3;
	int** kernel = new int* [kernel_size];

	std::ifstream image1("../data/lena.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../result/lena-sharpen.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		std::cout << "problem 2: failed to open one or more image files\n";
		return;
	}

	init_matrix(matrix, image_width, image_height);
	init_matrix(filtered_matrix, image_width, image_height);
	init_matrix(kernel, kernel_size, kernel_size);
	init_kernel(kernel);

	image2matrix(image1, matrix, image_width, image_height);

	laplacian_filter(filtered_matrix, matrix, intensity_range, image_width, image_height, kernel, kernel_size, -1);

	matrix2image(image2, filtered_matrix, image_width, image_height);

	delete_matrix(&matrix, image_height);
	delete_matrix(&filtered_matrix, image_height);

	image1.close();
	image2.close();

	std::cout << "problem 2: done\n";
}

void problem_3()
{
	const int image_width = 512, image_height = 512;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** matrix = new int* [image_height];
	int** filtered_matrix = new int* [image_height];
	const int kernel_size = 3;

	std::ifstream image1("../data/lenna-noise.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../result/lena-median.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		std::cout << "problem 3: failed to open one or more image files\n";
		return;
	}

	init_matrix(matrix, image_width, image_height);
	init_matrix(filtered_matrix, image_width, image_height);

	image2matrix(image1, matrix, image_width, image_height);

	median_filter(filtered_matrix, matrix, image_width, image_height, intensity_range, kernel_size);

	matrix2image(image2, filtered_matrix, image_width, image_height);

	delete_matrix(&matrix, image_height);
	delete_matrix(&filtered_matrix, image_height);

	image1.close();
	image2.close();

	std::cout << "problem 3: done\n";
}

void image2matrix(std::ifstream& image, int** matrix, const int width, const int height)
{
	if (!image.is_open())
	{
		std::cout << "image2matrix: image is not open\n";
		return;
	}

	for (int i = 0; i < height; i++)
	{
		for (int k = 0; k < width; k++)
		{
			char pxi;
			image.read(&pxi, sizeof(pxi));
			matrix[i][k] = static_cast<unsigned char>(pxi);
		}
	}
}

void matrix2image(std::ofstream& image, int** matrix, const int width, const int height)
{
	if (!image.is_open())
	{
		std::cout << "image2matrix: image is not open\n";
		return;
	}

	for (int i = 0; i < height; i++)
	{
		for (int k = 0; k < width; k++)
		{
			char pxi = static_cast<char>(matrix[i][k]);
			image.write(&pxi, sizeof(pxi));
		}
	}
}

void init_matrix(int** matrix, const int width, const int height)
{
	for (int i = 0; i < height; i++)
	{
		matrix[i] = new int[width];
	}
}

void delete_matrix(int*** matrix, const int height)
{
	for (int i = 0; i < height; i++)
	{
		delete (*matrix)[i];
	}

	delete* matrix;
	*matrix = nullptr;
}

void gaussian_filter(int** filtered_matrix, int** matrix, int intensity_range, const int width, const int height, int kernel_size, int sigma, double k)
{
	for (int i = 0; i < height; i++)
	{
		for (int m = 0; m < width; m++)
		{
			double sum = 0;
			for (int j = 0; j < kernel_size; j++)
			{
				for (int n = 0; n < kernel_size; n++)
				{
					int s = (-1) * kernel_size / 2 + j;
					int t = (-1) * kernel_size / 2 + n;
					sum += (i + s < 0 || m + t < 0 || i + s >= height || m + t >= width) ? 0 
						: matrix[i + s][m + t] * (k * exp((pow(s, 2) + pow(t, 2)) / (2 * pow(sigma, 2)) * (-1)));
				}
			}
			sum = round(sum / pow(kernel_size, 2));
			filtered_matrix[i][m] = std::min(std::max(static_cast<int>(sum), 0), intensity_range - 1);
		}
	}
}

void laplacian_filter(int** filtered_matrix, int** matrix, int intensity_range, const int width, const int height, int** kernel, int kernel_size, double c)
{
	for (int i = 0; i < height; i++)
	{
		for (int k = 0; k < width; k++)
		{
			int gradient_sum = 0;
			for (int j = 0; j < kernel_size; j++)
			{
				for (int m = 0; m < kernel_size; m++)
				{
					int s = (-1) * kernel_size / 2 + j;
					int t = (-1) * kernel_size / 2 + m;
					gradient_sum += (i + s < 0 || m + t < 0 || i + s >= height || m + t >= width) ? 0 : kernel[j][m] * matrix[i + s][k + t];
				}
				filtered_matrix[i][k] = std::min(std::max(static_cast<int>(round(matrix[i][k] + c * (gradient_sum / pow(kernel_size, 2)))), 0), intensity_range - 1);
			}
		}
	}
}

void init_kernel(int** kernel)
{
	kernel[0][0] = 0;
	kernel[0][1] = 1;
	kernel[0][2] = 0;
	kernel[1][0] = 1;
	kernel[1][1] = -4;
	kernel[1][2] = 1;
	kernel[2][0] = 0;
	kernel[2][1] = 1;
	kernel[2][2] = 0;
}

void median_filter(int** filtered_matrix, int** matrix, const int width, const int height, int intensity_range, const int kernel_size)
{
	for (int i = 0; i < height; i++)
	{
		for (int k = 0; k < width; k++)
		{
			int* elements = new int[kernel_size * kernel_size];
			for (int j = 0; j < kernel_size; j++)
			{
				for (int m = 0; m < kernel_size; m++)
				{
					int s = (-1) * kernel_size / 2 + j;
					int t = (-1) * kernel_size / 2 + m;
					elements[j * kernel_size + m] = (i + s < 0 || k + t < 0 || i + s >= height || k + t >= width) ? intensity_range / 2 : matrix[i + s][k + t];
					//std::cout << "( " << k + t << ",  " << i + s << " )" << std::endl;
				}
			}
			std::nth_element(elements, elements + kernel_size * kernel_size / 2, elements + kernel_size * kernel_size);
			filtered_matrix[i][k] = elements[kernel_size * kernel_size / 2];
			delete[] elements;
		}
	}
}