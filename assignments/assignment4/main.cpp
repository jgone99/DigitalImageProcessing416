
#include <iostream>
#include <fstream>
#include <limits>

typedef struct Position {
	int best_row;
	int best_col;
	double best_ssd;
	double best_cc;
} Position;

void problem_1();
void problem_2();
void problem_3();
void image2matrix(std::ifstream& image, int** matrix, const int width, const int height);
void matrix2image(std::ofstream& image, int** matrix, const int width, const int height);
void init_matrix(int** matrix, const int width, const int height);
void delete_matrix(int*** matrix, const int height);
void x_gradient(int** x_filtered_matrix, int** matrix, const int* gradient, int m_width, int m_height);
void y_gradient(int** x_filtered_matrix, int** matrix, const int* gradient, int m_width, int m_height);
void normalize(int** matrix, int m_width, int m_height, int max_intensity);
void print_matrix(int** matrix, int m_width, int m_height);
void grad_mag(int** grad_mag_matrix, int** x_matrix, int** y_matrix, int m_width, int m_height);
void image_reg_ssd(int** matched_matrix, int** search_matrix, int** template_matrix, int search_width, int search_height, int template_width, int template_height);
void image_reg_cc(int** matched_matrix, int** search_matrix, int** template_matrix, int search_width, int search_height, int template_width, int template_height);

int main(int argc, char** argv)
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
	int** x_matrix = new int* [image_height];
	int** y_matrix = new int* [image_height];
	int** grad_mag_matrix = new int* [image_height];

	const int grad[2]{ -1, 1 };

	std::ifstream image1("../../data/lena.raw", std::ios::binary | std::ios::in);
	std::ofstream image2("../../result/lena_derivX.raw", std::ios::binary | std::ios::out);
	std::ofstream image3("../../result/lena_derivY.raw", std::ios::binary | std::ios::out);
	std::ofstream image4("../../result/lena_gradient.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open() || !image3.is_open() || !image4.is_open())
	{
		std::cout << "problem 1: failed to open one or more image files\n";
		return;
	}

	init_matrix(matrix, image_width, image_height);
	init_matrix(x_matrix, image_width, image_height);
	init_matrix(y_matrix, image_width, image_height);
	init_matrix(grad_mag_matrix, image_width, image_height);

	image2matrix(image1, matrix, image_width, image_height);

	x_gradient(x_matrix, matrix, grad, image_width, image_height);
	normalize(x_matrix, image_width, image_height, intensity_range);

	y_gradient(y_matrix, matrix, grad, image_width, image_height);
	normalize(y_matrix, image_width, image_height, intensity_range);	
	
	grad_mag(grad_mag_matrix, x_matrix, y_matrix, image_width, image_height);
	normalize(grad_mag_matrix, image_width, image_height, intensity_range);

	matrix2image(image2, x_matrix, image_width, image_height);
	matrix2image(image3, y_matrix, image_width, image_height);
	matrix2image(image4, grad_mag_matrix, image_width, image_height);

	delete_matrix(&matrix, image_height);
	delete_matrix(&x_matrix, image_height);
	delete_matrix(&y_matrix, image_height);
	delete_matrix(&grad_mag_matrix, image_height);

	image1.close();
	image2.close();
	image3.close();
	image4.close();

	std::cout << "problem 1: done\n";
}

void problem_2()
{
	const int image1_width = 400, image1_height = 300;
	const int image2_width = 42, image2_height = 45;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** search_matrix = new int* [image1_height];
	int** template_matrix = new int* [image2_height];
	int** matched_matrix = new int* [image1_height];

	std::ifstream image1("../../data/flowers.raw", std::ios::binary | std::ios::in);
	std::ifstream image2("../../data/flowers-template.raw", std::ios::binary | std::ios::out);
	std::ofstream image3("../../result/flowers_squared_diff.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open() || !image3.is_open())
	{
		std::cout << "problem 2: failed to open one or more image files\n";
		return;
	}

	init_matrix(search_matrix, image1_width, image1_height);
	init_matrix(template_matrix, image2_width, image2_height);
	init_matrix(matched_matrix, image1_width, image1_height);

	image2matrix(image1, search_matrix, image1_width, image1_height);
	image2matrix(image2, template_matrix, image2_width, image2_height);

	image_reg_ssd(matched_matrix, search_matrix, template_matrix, image1_width, image1_height, image2_width, image2_height);

	matrix2image(image3, matched_matrix, image1_width, image1_height);

	delete_matrix(&search_matrix, image1_height);
	delete_matrix(&template_matrix, image2_height);
	delete_matrix(&matched_matrix, image1_height);

	image1.close();
	image2.close();
	image3.close();

	std::cout << "problem 2: done\n";
}

void problem_3()
{
	const int image1_width = 400, image1_height = 300;
	const int image2_width = 42, image2_height = 45;
	const int intensity_range = 256;
	const int pixel_bitsize = 8;
	int** search_matrix = new int* [image1_height];
	int** template_matrix = new int* [image2_height];
	int** matched_matrix = new int* [image1_height];

	std::ifstream image1("../../data/flowers.raw", std::ios::binary | std::ios::in);
	std::ifstream image2("../../data/flowers-template.raw", std::ios::binary | std::ios::out);
	std::ofstream image3("../../result/flowers_correlation_coeff.raw", std::ios::binary | std::ios::out);

	if (!image1.is_open() || !image2.is_open() || !image3.is_open())
	{
		std::cout << "problem 3: failed to open one or more image files\n";
		return;
	}

	init_matrix(search_matrix, image1_width, image1_height);
	init_matrix(template_matrix, image2_width, image2_height);
	init_matrix(matched_matrix, image1_width, image1_height);

	image2matrix(image1, search_matrix, image1_width, image1_height);
	image2matrix(image2, template_matrix, image2_width, image2_height);

	image_reg_cc(matched_matrix, search_matrix, template_matrix, image1_width, image1_height, image2_width, image2_height);

	matrix2image(image3, matched_matrix, image1_width, image1_height);

	delete_matrix(&search_matrix, image1_height);
	delete_matrix(&template_matrix, image2_height);
	delete_matrix(&matched_matrix, image1_height);

	image1.close();
	image2.close();
	image3.close();

	std::cout << "problem 3: done\n";
}

void x_gradient(int** x_filtered_matrix, int** matrix, const int* gradient, int m_width, int m_height)
{
	for (int k = 0; k < m_width * m_height; k++)
	{
		int x = k % m_width;
		int y = k / m_width;
		//std::cout << matrix[y][x] << std::endl;
		//std::cout << '(' << x << ',' << y << ")\n";
		//std::cout << ((x + 1 == m_width) ? matrix[y][x] : (gradient[0] * matrix[y][k] + gradient[1] * matrix[y][x + 1])) << std::endl;
		x_filtered_matrix[y][x] = (x + 1 == m_width) ? matrix[y][x] : (gradient[0] * matrix[y][x] + gradient[1] * matrix[y][x + 1]);
	}
}

void y_gradient(int** x_filtered_matrix, int** matrix, const int* gradient, int m_width, int m_height)
{
	for (int k = 0; k < m_width * m_height; k++)
	{
		int x = k % m_width;
		int y = k / m_width;
		//std::cout << matrix[y][x] << std::endl;
		//std::cout << '(' << x << ',' << y << ")\n";
		//std::cout << ((x + 1 == m_width) ? matrix[y][x] : (gradient[0] * matrix[y][k] + gradient[1] * matrix[y][x + 1])) << std::endl;
		x_filtered_matrix[y][x] = (y + 1 == m_height) ? matrix[y][x] : (gradient[0] * matrix[y][x] + gradient[1] * matrix[y + 1][x]);
	}
}

void grad_mag(int** grad_mag_matrix, int** x_matrix, int** y_matrix, int m_width, int m_height)
{
	for (int k = 0; k < m_width * m_height; k++)
	{
		int x = k % m_width;
		int y = k / m_width;
		grad_mag_matrix[y][x] = static_cast<int>(round(sqrt(pow(x_matrix[y][x], 2) + pow(y_matrix[y][x], 2))));
	}
}

void image_reg_ssd(int** matched_matrix, int** search_matrix, int** template_matrix, int search_width, int search_height, int template_width, int template_height)
{
	Position position{};

	constexpr double max_double = std::numeric_limits<double>::max();

	double min_ssd = max_double;

	for (int y_s = 0; y_s < search_height - template_height; y_s++)
	{
		for (int x_s = 0; x_s < search_width - template_width; x_s++)
		{
			double ssd = 0.0;

			for (int y_t = 0; y_t < template_height; y_t++)
			{
				for (int x_t = 0; x_t < template_width; x_t++)
				{
					int search_int = search_matrix[y_s + y_t][x_s + x_t];
					int template_int = template_matrix[y_t][x_t];

					ssd += pow(search_int - template_int, 2);
				}
			}

			if (ssd < min_ssd)
			{
				min_ssd = ssd;
				position.best_col = x_s;
				position.best_row = y_s;
				position.best_ssd = ssd;
			}
		}
	}

	int template_index = 0;

	for (int y_s = 0; y_s < search_height; y_s++)
	{
		for (int x_s = 0; x_s < search_width; x_s++)
		{
			if (y_s >= position.best_row && x_s >= position.best_col && y_s < position.best_row + template_height && x_s < position.best_col + template_width)
			{
				matched_matrix[y_s][x_s] = template_matrix[template_index / template_width][template_index % template_width];
				template_index++;
			}
			else
			{
				matched_matrix[y_s][x_s] = search_matrix[y_s][x_s];
			}
		}
	}
}

void image_reg_cc(int** matched_matrix, int** search_matrix, int** template_matrix, int search_width, int search_height, int template_width, int template_height)
{
	Position position{};

	double E_I = 0;
	double E_R = 0;

	for (int y = 0; y < search_height; y++)
	{
		for (int x = 0; x < search_width; x++)
		{
			E_I += search_matrix[y][x];
		}
	}
	E_I /= search_width * search_height;

	for (int y = 0; y < template_height; y++)
	{
		for (int x = 0; x < template_width; x++)
		{
			E_R += template_matrix[y][x];
		}
	}
	E_R /= template_width * template_height;

	double* R_std_devs = new double[template_width * template_height];
	double R_var_sum = 0;

	for (int y_t = 0; y_t < template_height; y_t++)
	{
		for (int x_t = 0; x_t < template_width; x_t++)
		{
			R_std_devs[y_t * template_width + x_t] = template_matrix[y_t][x_t] - E_R;
			R_var_sum += pow(R_std_devs[y_t * template_width + x_t], 2);
		}
	}

	double max_cc = 0;

	for (int y_s = 0; y_s < search_height - template_height; y_s++)
	{
		for (int x_s = 0; x_s < search_width - template_width; x_s++)
		{
			double sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0;

			for (int y_t = 0; y_t < template_height; y_t++)
			{
				for (int x_t = 0; x_t < template_width; x_t++)
				{
					sum3 += (search_matrix[y_s + y_t][x_s + x_t] - E_I) * R_std_devs[y_t * template_width + x_t];
					sum4 += pow(search_matrix[y_s + y_t][x_s + x_t] - E_I, 2);
				}
			}

			double cc = sum3 / (sqrt(sum4) * sqrt(R_var_sum));

			if (cc > max_cc)
			{
				max_cc = cc;
				position.best_col = x_s;
				position.best_row = y_s;
				position.best_ssd = cc;
			}
		}
	}

	int template_index = 0;

	for (int y_s = 0; y_s < search_height; y_s++)
	{
		for (int x_s = 0; x_s < search_width; x_s++)
		{
			if (y_s >= position.best_row && x_s >= position.best_col && y_s < position.best_row + template_height && x_s < position.best_col + template_width)
			{
				matched_matrix[y_s][x_s] = template_matrix[template_index / template_width][template_index % template_width];
				template_index++;
			}
			else
			{
				matched_matrix[y_s][x_s] = search_matrix[y_s][x_s];
			}
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

void normalize(int** matrix, int m_width, int m_height, int max_intensity)
{
	int min_int = INT16_MAX;
	int max_int = 0;

	for (int i = 0; i < m_width * m_height; i++)
	{
		int x = i % m_width;
		int y = i / m_width;
		int value = matrix[y][x];
		min_int = std::min(min_int, value);
		max_int = std::max(max_int, value);
	}

	for (int i = 0; i < m_width * m_height; i++)
	{
		int x = i % m_width;
		int y = i / m_width;
		int value = matrix[y][x];
		int new_value = (int)(((value - min_int) / (float)(max_int - min_int)) * (max_intensity - 1));
		matrix[y][x] = new_value;
	}
}

void print_matrix(int** matrix, int m_width, int m_height)
{
	for (int y = 0; y < m_width; y++)
	{
		for (int x = 0; x < m_height; x++)
		{
			std::cout << matrix[y][x] << ' ';
		}
		std::cout << std::endl;
	}
}