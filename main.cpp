#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <omp.h>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

typedef vector<float> Array;
typedef vector<Array> Matrix;

// Получить ядро

Matrix getGaussian(int size, double sigma) {
    Matrix kernel(size, Array(size));
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            kernel[i][j] = exp(-(i * i + j * j) / (2 * sigma * sigma))
                           / (2 * M_PI * sigma * sigma);
            sum += kernel[i][j];
        }
    }

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            kernel[i][j] /= sum;

    return kernel;
}

// Пременить фильтр

void applyFilter(Matrix &filter, const char *filename) {
    CImg<float> img(filename); // Входное изображение
    CImg<float> endImg(img.width(), img.height(), 1, 3); // Выходное

// Заполняем изображение на выход нулями(черным)
    endImg.fill(0);

    int height = img.height();
    int width = img.width();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;

// Устанавливаем количество потоков
// 3 потока, по одному на компоненту(r, g, b)
    omp_set_num_threads(3);

// Распараллеливаем цикл
#pragma omp parallel for
    for (int d = 0; d < 3; d++) {
        // int id = omp_get_thread_num();
        // cout << "ID: " << id << endl;
        for (int i = 0; i < newImageWidth; i++) {
            for (int j = 0; j < newImageHeight; j++) {
                for (int w = i; w < i + filterWidth; w++) {
                    for (int h = j; h < j + filterHeight; h++) {
                        endImg(i, j, 0, d) += filter[w - i][h - j] * img(w, h, 0, d);
                    }
                }
            }
        }
    }

// Выводим изображение
    endImg.display("Gaussian blur");
}

int main(int argc, char *argv[]) {
    char *path = argv[1];
    int radius = atoi(argv[2]);
    float sigma = atoi(argv[3]);
    Matrix filter = getGaussian(radius, sigma);
    applyFilter(filter, path);
    return 0;
}