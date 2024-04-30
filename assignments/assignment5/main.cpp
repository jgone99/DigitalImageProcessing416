
#include <iostream>
#include <fstream>

void image2matrix(std::ifstream& image, int** matrix, const int width, const int height);
void matrix2image(std::ofstream& image, int** matrix, const int width, const int height);
void init_matrix(int** matrix, const int width, const int height);
void delete_matrix(int*** matrix, const int height);
void print_matrix(int** matrix, int m_width, int m_height);
void normalize(int** matrix, int m_width, int m_height, int max_intensity);

void problem_1();
void problem_2();
void problem_3();

void erode(int** eroded_matrix, int** input_matrix, int width, int height, int** SE, int SE_width, int SE_height, int intensity);
void dilate(int** dilated_matrix, int** input_matrix, int width, int height, int** SE, int SE_width, int SE_height, int intensity);
void matrix_diff(int** matrix1, int** matrix2, int width, int height);
void co_occurrence(int** co_occurrence_matrix, int offset[2], int** input_matrix, int width, int height, int intensity);

int main(int argc, char* argv[])
{
	problem_1();
	problem_2();
	problem_3();

	return 0;
}

void problem_1()
{
	const int problem_number = 1;
	const int image_width = 221, image_height = 269;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** input_matrix = new int* [image_height];
	int** eroded_matrix = new int* [image_height];
	
	int** SE = new int*[3]{
		new int[3]{1, 1, 1},
		new int[3]{1, 1, 1},
		new int[3]{1, 1, 1}
	};

	std::ifstream image1("../data/lincoln.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../result/lincoln_boundary.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		std::cout << "problem " << problem_number << ": failed to open one or more image files\n";
		return;
	}

	init_matrix(input_matrix, image_width, image_height);
	image2matrix(image1, input_matrix, image_width, image_height);

	init_matrix(eroded_matrix, image_width, image_height);

	erode(eroded_matrix, input_matrix, image_width, image_height, SE, 3, 3, intensity_range - 1);

	matrix_diff(input_matrix, eroded_matrix, image_width, image_height);

	matrix2image(image2, input_matrix, image_width, image_height);
	
	delete_matrix(&input_matrix, image_height);
	delete_matrix(&eroded_matrix, image_height);
	delete_matrix(&SE, 3);

	image1.close();
	image2.close();

	std::cout << "problem " << problem_number << ": done\n";
}

void problem_2()
{
	const int problem_number = 2;
	const int image_width = 315, image_height = 238;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** input_matrix = new int* [image_height];
	int** opening_matrix1 = new int* [image_height];
	int** opening_matrix2 = new int* [image_height];
	int** closing_matrix1 = new int* [image_height];
	int** closing_matrix2 = new int* [image_height];

	int** SE = new int* [3] {
		new int[3] {1, 1, 1},
			new int[3] {1, 1, 1},
			new int[3] {1, 1, 1}
		};

	std::ifstream image1("../data/fingerprint.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../result/fingerprint_opening.raw", std::ios::binary | std::ios::out);
	std::ofstream image3("../result/fingerprint_closing.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open() || !image3.is_open())
	{
		std::cout << "problem " << problem_number << ": failed to open one or more image files\n";
		return;
	}

	init_matrix(input_matrix, image_width, image_height);
	image2matrix(image1, input_matrix, image_width, image_height);

	init_matrix(opening_matrix1, image_width, image_height);
	init_matrix(opening_matrix2, image_width, image_height);
	init_matrix(closing_matrix1, image_width, image_height);
	init_matrix(closing_matrix2, image_width, image_height);

	erode(opening_matrix1, input_matrix, image_width, image_height, SE, 3, 3, intensity_range - 1);
	dilate(opening_matrix2, opening_matrix1, image_width, image_height, SE, 3, 3, intensity_range - 1);

	dilate(closing_matrix1, input_matrix, image_width, image_height, SE, 3, 3, intensity_range - 1);
	erode(closing_matrix2, closing_matrix1, image_width, image_height, SE, 3, 3, intensity_range - 1);

	matrix2image(image2, opening_matrix2, image_width, image_height);
	matrix2image(image3, closing_matrix2, image_width, image_height);

	delete_matrix(&input_matrix, image_height);
	delete_matrix(&opening_matrix1, image_height);
	delete_matrix(&opening_matrix2, image_height);
	delete_matrix(&closing_matrix1, image_height);
	delete_matrix(&closing_matrix2, image_height);
	delete_matrix(&SE, 3);

	image1.close();
	image2.close();
	image3.close();

	std::cout << "problem " << problem_number << ": done\n";
}

void problem_3()
{
	const int problem_number = 3;
	const int image_width = 365, image_height = 120;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** input_matrix = new int* [image_height];
	int** co_occurrence_matrix = new int* [intensity_range];
	int offset[2]{ 1, 0 };

	std::ifstream image1("../data/cktboard.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../result/cktboard_texture.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		std::cout << "problem " << problem_number << ": failed to open one or more image files\n";
		return;
	}

	init_matrix(input_matrix, image_width, image_height);
	init_matrix(co_occurrence_matrix, intensity_range, intensity_range);

	image2matrix(image1, input_matrix, image_width, image_height);

	co_occurrence(co_occurrence_matrix, offset, input_matrix, image_width, image_height, intensity_range);

	//print_matrix(co_occurrence_matrix, intensity_range, intensity_range);

	matrix2image(image2, co_occurrence_matrix, intensity_range, intensity_range);

	delete_matrix(&input_matrix, image_height);
	delete_matrix(&co_occurrence_matrix, intensity_range);

	image1.close();
	image2.close();

	std::cout << "problem " << problem_number << ": done\n";
}

void erode(int** eroded_matrix, int** input_matrix, int width, int height, int** SE, int SE_width, int SE_height, int intensity)
{
	int offset_x = SE_width / 2;
	int offset_y = SE_height / 2;
	for (int y = offset_y; y < height - offset_y; y++) 
	{
		for (int x = offset_x; x < width - offset_x; x++) 
		{
			bool include = true;
			
			for (int se_y = 0; se_y < SE_height; se_y++) 
			{
				for (int se_x = 0; se_x < SE_width; se_x++) 
				{
					int image_x = x - offset_x + se_x;
					int image_y = y - offset_y + se_y;
					include = include && input_matrix[image_y][image_x] || !SE[se_y][se_x];
				}
			}

			eroded_matrix[y][x] = include * intensity;
		}
	}
}

void dilate(int** dilated_matrix, int** input_matrix, int width, int height, int** SE, int SE_width, int SE_height, int intensity)
{
	int offset_x = SE_width / 2;
	int offset_y = SE_height / 2;
	for (int y = offset_y; y < height - offset_y; y++)
	{
		for (int x = offset_x; x < width - offset_x; x++)
		{
			bool include = false;

			for (int se_y = 0; se_y < SE_height; se_y++)
			{
				for (int se_x = 0; se_x < SE_width; se_x++)
				{
					int image_x = x - offset_x + se_x;
					int image_y = y - offset_y + se_y;
					include = include || (input_matrix[image_y][image_x] && SE[se_y][se_x]);
				}
			}

			dilated_matrix[y][x] = include * intensity;
		}
	}
}

void co_occurrence(int** co_occurrence_matrix, int offset[2], int** input_matrix, int width, int height, int intensity)
{
	for (int y = 0; y < height - offset[1]; y++)
	{
		for (int x = 0; x < width - offset[0]; x++)
		{
			int origin_value = input_matrix[y][x];
			int offset_value = input_matrix[y + offset[1]][x + offset[0]];
			//std::cout << "before:" << co_occurrence_matrix[origin_value][offset_value] << std::endl;
			co_occurrence_matrix[origin_value][offset_value] += 1;
			//std::cout << "after:" << co_occurrence_matrix[origin_value][offset_value] << std::endl;
		}
	}
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
		matrix[i] = new int[width]();
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

void normalize(int** matrix, int m_width, int m_height, int max_intensity)
{
	int min_int = INT16_MAX;
	int max_int = 0;

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int value = matrix[y][x];
			min_int = std::min(min_int, value);
			max_int = std::max(max_int, value);
		}
	}

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			int value = matrix[y][x];
			int normalized_value = static_cast<int>((value - min_int) / static_cast<float>(max_int - min_int) * (max_intensity - 1));
			matrix[y][x] = normalized_value;
		}
	}
}

void print_matrix(int** matrix, int m_width, int m_height)
{
	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			std::cout << matrix[y][x] << ' ';
		}
		std::cout << std::endl;
	}
}

void matrix_diff(int** matrix1, int** matrix2, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			matrix1[y][x] -= matrix2[y][x];
		}
	}
}