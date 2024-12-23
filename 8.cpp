#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <stdexcept>

// Пути к файлам
const std::string apple_1 = "C:\\Users\\armn\\Downloads\\examples\\apples\\apple_1.pgm";
const std::string apple_2 = "C:\\Users\\armn\\Downloads\\examples\\apples\\apple_2.pgm";
const std::string apple_3 = "C:\\Users\\armn\\Downloads\\examples\\apples\\apple_3.pgm";
const std::string apple_4 = "C:\\Users\\armn\\Downloads\\examples\\apples\\apple_4.pgm";

const std::string banana_1 = "C:\\Users\\armn\\Downloads\\examples\\bananas\\banana_1.pgm";
const std::string banana_2 = "C:\\Users\\armn\\Downloads\\examples\\bananas\\banana_2.pgm";
const std::string banana_3 = "C:\\Users\\armn\\Downloads\\examples\\bananas\\banana_3.pgm";
const std::string banana_4 = "C:\\Users\\armn\\Downloads\\examples\\bananas\\banana_4.pgm";

class PGMImage {
private:
    std::string filename;
    std::vector<std::vector<int>> pixels;
    std::vector<double> normalizedHistogram;
    int width;
    int height;
    int maxGrayValue;

    void readPGMFile() {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        std::string magic;
        file >> magic;
        if (magic != "P2") {
            throw std::runtime_error("Invalid PGM format in file: " + filename);
        }

        char line[1024];
        file.getline(line, 1024);
        while (file.peek() == '#') {
            file.getline(line, 1024);
        }

        file >> width >> height >> maxGrayValue;
        if (width <= 0 || height <= 0) {
            throw std::runtime_error("Invalid image dimensions in file: " + filename);
        }

        pixels.resize(height, std::vector<int>(width));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (!(file >> pixels[i][j])) {
                    throw std::runtime_error("Error reading pixel data from file: " + filename);
                }
            }
        }

        calculateNormalizedHistogram();
    }

    void calculateNormalizedHistogram() {
        std::vector<int> histogram(maxGrayValue + 1, 0);
        for (const auto& row : pixels) {
            for (int pixel : row) {
                histogram[pixel]++;
            }
        }

        normalizedHistogram.resize(maxGrayValue + 1);
        double totalPixels = width * height;
        for (int i = 0; i <= maxGrayValue; i++) {
            normalizedHistogram[i] = histogram[i] / totalPixels;
        }
    }

public:
    explicit PGMImage(const std::string& fname) : filename(fname) {
        readPGMFile();
    }

    double compareWith(const PGMImage& other) const {
        double totalDifference = 0.0;
        for (size_t i = 0; i < normalizedHistogram.size(); i++) {
            totalDifference += std::abs(normalizedHistogram[i] - other.normalizedHistogram[i]);
        }
        return (1.0 - (totalDifference / 2.0)) * 100.0;
    }

    std::string getFilename() const { return filename; }
};

void compareImages(const std::vector<PGMImage>& imageList) {
    int n = imageList.size();
    std::vector<std::vector<double>> similarityTable(n, std::vector<double>(n, 0.0));

    // Заполняем таблицу сравнения
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double similarity = imageList[i].compareWith(imageList[j]);
            similarityTable[i][j] = similarity;
            similarityTable[j][i] = similarity;
        }
    }

    // Выводим таблицу
    std::cout << "Similarity Table (% match):" << std::endl;
    std::cout << std::setw(15) << "";
    for (int i = 0; i < n; ++i) {
        std::string fname = imageList[i].getFilename();
        std::string shortName = fname.substr(fname.find_last_of("\\") + 1);
        std::cout << std::setw(15) << shortName;
    }
    std::cout << std::endl;

    for (int i = 0; i < n; ++i) {
        std::string fname = imageList[i].getFilename();
        std::string shortName = fname.substr(fname.find_last_of("\\") + 1);
        std::cout << std::setw(15) << shortName;
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                std::cout << std::setw(15) << "100.0";
            } else {
                std::cout << std::setw(15) << std::fixed << std::setprecision(1)
                          << similarityTable[i][j];
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    try {
        // Создаем списки изображений
        std::vector<PGMImage> appleList = {
                PGMImage(apple_1),
                PGMImage(apple_2),
                PGMImage(apple_3),
                PGMImage(apple_4)
        };

        std::vector<PGMImage> bananaList = {
                PGMImage(banana_1),
                PGMImage(banana_2),
                PGMImage(banana_3),
                PGMImage(banana_4)
        };

        // Сравниваем изображения в каждом списке
        std::cout << "Comparing apples:" << std::endl;
        compareImages(appleList);

        std::cout << "Comparing bananas:" << std::endl;
        compareImages(bananaList);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}