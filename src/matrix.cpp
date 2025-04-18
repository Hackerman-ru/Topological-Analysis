#include "matrix.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace topa {

void Clean(Matrix& matrix) {
    for (auto it = matrix.begin(); it != matrix.end();) {
        if (it->second.empty()) {
            it = matrix.erase(it);
        } else {
            ++it;
        }
    }
}

void PrintMatrix(const Matrix& matrix, size_t n, const std::string& title) {
    std::cout << "\n" << title << " (size = " << n << "):\n";

    // Заголовок с номерами столбцов
    std::cout << "     ";
    for (size_t col = 0; col < n; ++col) {
        std::cout << std::setw(3) << col << " ";
    }
    std::cout << "\n";

    // Тело матрицы
    for (size_t row = 0; row < n; ++row) {
        std::cout << std::setw(3) << row << "|";
        for (size_t col = 0; col < n; ++col) {
            if (matrix.find(col) != matrix.end()) {
                const auto& col_data = matrix.at(col);
                bool has_row = std::find(col_data.begin(), col_data.end(),
                                         row) != col_data.end();
                std::cout << (has_row ? "  * " : "  . ");
            } else {
                std::cout << "  . ";
            }
        }
        std::cout << "\n";
    }
}

}  // namespace topa
