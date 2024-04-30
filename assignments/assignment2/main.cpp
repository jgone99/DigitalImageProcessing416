#include <iostream>
#include <fstream>

void problem_1();
void problem_2();
void image2matrix(std::ifstream& image, int** matrix, const int width, const int height);
void matrix2image(std::ofstream& image, int** matrix, const int width, const int height);
void delete_matrix(int*** matrix, int rows, int cols);
void create_freq_hist(int** matrix, int* freq, int width, int height);
void create_eq_freq_map(int* freq_map, int* freq_eq_hist, int* freq, const int intensity_range, int freq_sum);
void create_spec_freq_map(int* freq_map, int* freq_spec_hist, int* freq1, int* freq2, const int intensity_range, int freq_sum);
void create_freq_normal_round(int* freq_norm_round, int* freq, const int intensity_range, int freq_sum);
void map_matrix_freq(int** matrix, int width, int height, int* freq_map);

int main(int argc, char** argv)
{
	problem_1();
	problem_2();

	return 0;
}

void problem_1()
{
	const int image_width = 512, image_height = 512;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** matrix = new int* [image_height];
	int freq[intensity_range] = {};
	int freq_map[intensity_range] = {};

	std::ifstream image1("../data/lena.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../result/lena_histequal.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		std::cout << "problem 1: failed to open one or more image files\n";
		return;
	}

	image2matrix(image1, matrix, image_width, image_height);

	create_freq_hist(matrix, freq, image_width, image_height);

	create_eq_freq_map(freq_map, nullptr, freq, intensity_range, image_width * image_height);

	map_matrix_freq(matrix, image_width, image_height, freq_map);

	matrix2image(image2, matrix, image_width, image_height);

	delete_matrix(&matrix, image_height, image_width);
	
	image1.close();
	image2.close();

	std::cout << "problem 1: done\n";
}

void problem_2()
{
	const int image_width = 512, image_height = 512;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** matrix1 = new int* [image_height];
	int** matrix2 = new int* [image_height];
	int freq1[intensity_range] = {};
	int freq2[intensity_range] = {};
	int freq_map[intensity_range] = {};

	std::ifstream image1("../data/lena.raw", std::ios::binary | std::ios::in);
	std::ifstream image2("../result/lena_histequal.raw", std::ios::binary | std::ios::in);
	std::ofstream image3("../result/lena_histspec.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open() || !image3.is_open())
	{
		std::cout << "problem 2: failed to open one or more image files\n";
		return;
	}

	image2matrix(image1, matrix1, image_width, image_height);
	image2matrix(image2, matrix2, image_width, image_height);

	create_freq_hist(matrix1, freq1, image_width, image_height);
	create_freq_hist(matrix2, freq2, image_width, image_height);

	create_spec_freq_map(freq_map, nullptr, freq1, freq2, intensity_range, image_width * image_height);

	map_matrix_freq(matrix1, image_width, image_height, freq_map);

	matrix2image(image3, matrix1, image_width, image_height);

	delete_matrix(&matrix1, image_height, image_width);

	std::cout << "problem 2: done\n";
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
		matrix[i] = new int[width];
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

void delete_matrix(int*** matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		delete (*matrix)[i];
	}

	delete* matrix;
	*matrix = nullptr;
}

void create_freq_hist(int** matrix, int* freq, int width, int height)
{
	for (int i = 0; i < height; i++)
	{
		for (int k = 0; k < width; k++)
		{
			// creating frequency histogram
			freq[matrix[i][k]]++;
		}
	}
}

void create_eq_freq_map(int* freq_map, int* freq_eq_hist, int* freq, const int intensity_range, int freq_sum)
{
	int* freq_norm_round = new int[intensity_range];
	
	create_freq_normal_round(freq_norm_round, freq, intensity_range, freq_sum);

	int cumul_sum = 0;

	for (int k = 0; k < intensity_range; k++)
	{
		// sum is new intensity index for frequency to match identity histogram
		cumul_sum += freq_norm_round[k];

		// creating a frequency map from original intensity index to equalized intensity index
		freq_map[k] = cumul_sum;

		//create equalized frequency histogram
		if (freq_eq_hist != nullptr)
		{
			freq_eq_hist[cumul_sum] += freq[k];
		}
	}
	delete [] freq_norm_round;
}

void create_spec_freq_map(int* freq_map, int* freq_spec_hist, int* freq1, int* freq2, const int intensity_range, int freq_sum)
{
	int* freq_norm_round1 = new int[intensity_range];
	int* freq_norm_round2 = new int[intensity_range];

	create_freq_normal_round(freq_norm_round1, freq1, intensity_range, freq_sum);
	create_freq_normal_round(freq_norm_round2, freq2, intensity_range, freq_sum);

	int cumul_sum1 = freq_norm_round1[0], cumul_sum2 = freq_norm_round2[0];
	int diff, prev_diff = static_cast<int>(abs(cumul_sum1 - cumul_sum2));

	int i = 0, k = 0;
	while (i < intensity_range)
	{
		if (k >= intensity_range)
		{
			freq_map[i++] = k - 1;
			continue;
		}

		diff = static_cast<int>(abs(cumul_sum1 - (cumul_sum2 + freq_norm_round2[k+1])));
		
		if (diff >= prev_diff)
		{
			freq_map[i] = k;
			cumul_sum1 += freq_norm_round1[++i];
			prev_diff = intensity_range;
		}
		else
		{
			cumul_sum2 += freq_norm_round2[k];
			prev_diff = static_cast<int>(abs(cumul_sum1 - (cumul_sum2 + freq_norm_round2[++k])));
		}
	}
}

void create_freq_normal_round(int* freq_norm_round, int* freq, const int intensity_range, int freq_sum)
{
	for (int k = 0; k < intensity_range; k++)
	{
		// normalize frequencies
		float normalized = (intensity_range - 1) * (freq[k] / static_cast<float>(freq_sum));

		// round normalized frequencies
		freq_norm_round[k] = static_cast<int>(roundf(normalized));
	}
}

void map_matrix_freq(int** matrix, int width, int height, int* freq_map)
{
	for (int i = 0; i < height; i++)
	{
		for (int k = 0; k < width; k++)
		{
			// updating frequencies to new equalized indecies
			matrix[i][k] = freq_map[matrix[i][k]];
		}
	}
}