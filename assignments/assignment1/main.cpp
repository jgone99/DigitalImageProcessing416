#include <iostream>
#include <fstream>

using namespace std;

void problem_1();
void problem_2();
void problem_3();
void problem_4();
void image2matrix(ifstream&, int**, const int, const int);
void delete_matrix(int***, int, int);
int T(int, int, int, int);
void normalize();


int main() 
{
	problem_1();
	problem_2();
	problem_3();
	problem_4(); // What value should the slice be set to? Chose 4/5 of the max intensity as per the image provided in instructions.
	normalize(); // Was not sure if normalizing was necessary so just normalized the original 'lena' image to demonstrate how it would be done.

	return 0;
}

void problem_1()
{
	const int image_width = 512, image_height = 512;
	const int pixel_bitsize = 8;

	ifstream image1("../data/WashingtonDC-ver1.raw", ios::binary | ios::in);
	ifstream image2("../data/WashingtonDC-ver2.raw", ios::binary | ios::in);
	ofstream image3("../result/WashingtonDC-subtract.raw", ios::binary | ios::out);

	if (!image1.is_open() || !image2.is_open() || !image3.is_open())
	{
		cout << "failed to open one or more image files\n";
		return;
	}

	for (int i = 0; i < image_width * image_height; i++)
	{
		char px1, px2;
		image1.read(&px1, sizeof(px1));
		image2.read(&px2, sizeof(px2));

		int u_px1 = static_cast<unsigned char>(px1);
		int u_px2 = static_cast<unsigned char>(px2);

		int u_px3 = max(u_px2 - u_px1, 0); // making sure all values >= 0

		char px3 = static_cast<char>(u_px3);

		image3.write(&px3, sizeof(px3));
	}

	image1.close();
	image2.close();
	image3.close();

	cout << "problem 1: done\n";
}

void problem_2()
{
	const int image_width = 512, image_height = 512;
	const int pixel_bitsize = 8;

	ifstream image1("../data/lena.raw", ios::binary | ios::in);
	ofstream image2("../result/lena_negative.raw", ios::binary | ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		cout << "problem 2: failed to open one or more image files\n";
		return;
	}

	for (int i = 0; i < image_width * image_height; i++)
	{
		char px1;
		image1.read(&px1, sizeof(px1));

		int u_px1 = static_cast<unsigned char>(px1);

		int u_px2 = max((int)pow(2, pixel_bitsize) - u_px1 - 1, 0);

		char px2 = static_cast<char>(u_px2);

		image2.write(&px2, sizeof(px2));
	}

	image1.close();
	image2.close();

	cout << "problem 2: done\n";
}

void problem_3()
{
	const int image_width = 512, image_height = 512;
	const int pixel_bitsize = 8;

	ifstream image1("../data/lena.raw", ios::binary | ios::in);
	ofstream image2("../result/lena_translation.raw", ios::binary | ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		cout << "problem 3: failed to open one or more image files\n";
		return;
	}

	int** matrix = new int*[image_height];

	image2matrix(image1, matrix, image_width, image_height);

	for (int i = 0; i < image_height; i++)
	{
		for (int k = 0; k < image_width; k++)
		{
			int x = k + 5;
			int y = i + 5;
			char pxchar = static_cast<char>((x >= 512 || y >= 512) ? 0 : matrix[y][x]);
			image2.write(&pxchar, sizeof(pxchar));
		}
	}

	delete_matrix(&matrix, image_height, image_width);

	image1.close();
	image2.close();

	cout << "problem 3: done\n";
}

void problem_4()
{
	const int image_width = 500, image_height = 500;
	const int pixel_bitsize = 8;

	ifstream image1("../data/pollen.raw", ios::binary | ios::in);
	ofstream image2("../result/pollen_intensity_slicing.raw", ios::binary | ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		cout << "problem 4: failed to open one or more image files\n";
		return;
	}

	for (int i = 0; i < image_width * image_height; i++)
	{
		char px;
		image1.read(&px, sizeof(px));

		int pxint = T(static_cast<unsigned char>(px), 120, 150, ((int)pow(2, pixel_bitsize) - 1) * 4 / 5);
		char pxchar = static_cast<char>(pxint);

		image2.write(&pxchar, sizeof(pxchar));
	}

	image1.close();
	image2.close();

	cout << "problem 4: done\n";
}

void normalize()
{
	const int image_width = 512, image_height = 512;
	const int pixel_bitsize = 8;

	ifstream image1("../data/lena.raw", ios::binary | ios::in);
	ofstream image2("../result/lena_normal.raw", ios::binary | ios::out);

	if (!image1.is_open() || !image2.is_open())
	{
		cout << "normailze: failed to open one or more image files\n";
		return;
	}

	int min_int = INT16_MAX;
	int max_int = 0;

	// Find min and max intenisity values in the image
	for (int i = 0; i < image_width * image_height; i++)
	{
		char px1;
		image1.read(&px1, sizeof(px1));

		int u_px1 = static_cast<unsigned char>(px1);
		min_int = min(min_int, u_px1);
		max_int = max(max_int, u_px1);
	}

	image1.clear();
	image1.seekg(0);

	for (int i = 0; i < image_width * image_height; i++)
	{
		char px1;
		image1.read(&px1, sizeof(px1));
		int u_px1 = static_cast<unsigned char>(px1);
		int u_px2 = (int)(((u_px1 - min_int) / (float)(max_int - min_int)) * 255);
		char px2 = static_cast<char>(u_px2);
		image2.write(&px2, sizeof(px2));
	}

	image1.close();
	image2.close();
}

void image2matrix(ifstream& image, int** matrix, const int width, const int height)
{
	if (!image.is_open())
	{
		cout << "image2matrix: image is not open\n";
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

void delete_matrix(int*** matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		delete (*matrix)[i];
	}

	delete *matrix;
	*matrix = nullptr;
}

int T(int r, int a, int b, int c)
{
	if (r >= a && r <= b)
	{
		return c;
	}
	return r;
}