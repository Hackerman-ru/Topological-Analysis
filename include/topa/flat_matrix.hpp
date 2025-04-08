#ifndef TOPA_FLAT_MATRIX_HPP_
#define TOPA_FLAT_MATRIX_HPP_

#include <cassert>
#include <vector>
#include <ranges>

namespace topa::detail {

template <typename T>
class FlatMatrix {
   public:
    class Row;
    class CRow;

   public:
    explicit FlatMatrix(size_t row_size)
        : row_size_(row_size) {
    }

    void push_back(const std::vector<T>& row) {  // NOLINT
        assert(row.size() == row_size_ &&
               "The row size must match the row_size of the FlatMatrix");
        for (const auto& value : row) {
            data_.push_back(value);
        }
    }

    void push_back(std::vector<T>&& row) {  // NOLINT
        assert(row.size() == row_size_ &&
               "The row size must match the row_size of the FlatMatrix");
        for (auto& value : row) {
            data_.push_back(std::move(value));
        }
    }

    void pop_back() {  // NOLINT
        data_.resize(data_.size() - row_size_);
    }

    Row front() {  // NOLINT
        return Row(0, this);
    }

    CRow front() const {  // NOLINT
        return CRow(0, this);
    }

    Row back() {  // NOLINT
        return Row(data_.size() - row_size_, this);
    }

    CRow back() const {  // NOLINT
        return CRow(data_.size() - row_size_, this);
    }

    Row operator[](size_t rows_number) {
        return Row(rows_number * row_size_, this);
    }

    CRow operator[](size_t rows_number) const {
        return CRow(rows_number * row_size_, this);
    }

    auto begin() {  // NOLINT
        return data_.begin();
    }

    auto begin() const {  // NOLINT
        return data_.begin();
    }

    auto end() {  // NOLINT
        return data_.end();
    }

    auto end() const {  // NOLINT
        return data_.end();
    }

    void reserve(size_t rows_number) {  // NOLINT
        data_.reserve(rows_number * row_size_);
    }

    void clear() {  // NOLINT
        data_.clear();
    }

    size_t size() const {  // NOLINT
        return data_.size() / row_size_;
    }

   private:
    size_t row_size_;
    std::vector<T> data_;
};

template <typename T>
class FlatMatrix<T>::Row {
    friend class FlatMatrix;

    size_t offset_;
    FlatMatrix* flat_matrix_;

   public:
    T& operator[](size_t pos) {
        return flat_matrix_->data_[offset_ + pos];
    }

    const T& operator[](size_t pos) const {
        return flat_matrix_->data_[offset_ + pos];
    }

    auto begin() {  // NOLINT
        return flat_matrix_->data_.begin() + offset_;
    }

    auto end() {  // NOLINT
        return flat_matrix_->data_.begin() + offset_ + flat_matrix_->row_size_;
    }

    size_t size() const {  // NOLINT
        return flat_matrix_->row_size_;
    }

    operator std::vector<T>() const {  // NOLINT
        std::vector<T> values;
        values.reserve(flat_matrix_->row_size_);
        for (size_t i = 0; i < flat_matrix_->row_size_; ++i) {
            values.emplace_back(operator[](i));
        }
        return values;
    }

   private:
    explicit Row(size_t offset, FlatMatrix* flat_matrix)
        : offset_(offset),
          flat_matrix_(flat_matrix) {
    }
};

template <typename T>
class FlatMatrix<T>::CRow {
    friend class FlatMatrix;

    size_t offset_;
    const FlatMatrix* flat_matrix_;

   public:
    const T& operator[](size_t pos) const {
        return flat_matrix_->data_[offset_ + pos];
    }

    auto begin() const {  // NOLINT
        return flat_matrix_->data_.begin() + offset_;
    }

    auto end() const {  // NOLINT
        return flat_matrix_->data_.begin() + offset_ + flat_matrix_->row_size_;
    }

    size_t size() const {  // NOLINT
        return flat_matrix_->row_size_;
    }

    operator std::vector<T>() const {  // NOLINT
        std::vector<T> values;
        values.reserve(flat_matrix_->row_size_);
        for (size_t i = 0; i < flat_matrix_->row_size_; ++i) {
            values.emplace_back(operator[](i));
        }
        return values;
    }

   private:
    explicit CRow(size_t offset, const FlatMatrix* flat_matrix)
        : offset_(offset),
          flat_matrix_(flat_matrix) {
    }
};

}  // namespace topa::detail

#endif
