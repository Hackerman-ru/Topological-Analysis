#pragma once

#include <cassert>
#include <vector>
#include <iterator>
#include <ranges>

namespace topa::detail {

template <typename T>
class FlatMatrix {
   public:
    class Row;
    class CRow;
    class iterator;        // NOLINT
    class const_iterator;  // NOLINT

   public:
    explicit FlatMatrix(size_t row_size)
        : row_size_(row_size) {
    }

    explicit FlatMatrix(size_t row_size, size_t rows)
        : row_size_(row_size),
          data_(std::vector<T>(row_size * rows)) {
    }

    void push_back(const std::vector<T>& row) {  // NOLINT
        assert(row.size() == row_size_ &&
               "The row size must match the row_size of the FlatMatrix");
        data_.insert(data_.end(), row.begin(), row.end());
    }

    void push_back(std::vector<T>&& row) {  // NOLINT
        assert(row.size() == row_size_ &&
               "The row size must match the row_size of the FlatMatrix");
        data_.insert(data_.end(), std::make_move_iterator(row.begin()),
                     std::make_move_iterator(row.end()));
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

    Row operator[](size_t row_index) {
        return Row(row_index * row_size_, this);
    }

    CRow operator[](size_t row_index) const {
        return CRow(row_index * row_size_, this);
    }

    iterator begin() {  // NOLINT
        return iterator(0, this);
    }

    const_iterator begin() const {  // NOLINT
        return const_iterator(0, this);
    }

    iterator end() {  // NOLINT
        return iterator(data_.size(), this);
    }

    const_iterator end() const {  // NOLINT
        return const_iterator(data_.size(), this);
    }

    void reserve(size_t rows_number) {  // NOLINT
        data_.reserve(rows_number * row_size_);
    }

    void clear() {  // NOLINT
        data_.clear();
    }

    size_t size() const {  // NOLINT
        if (row_size_ == 0) {
            return 0;
        }
        return data_.size() / row_size_;
    }

    size_t row_size() const {  // NOLINT
        return row_size_;
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
        return begin() + flat_matrix_->row_size_;
    }

    auto begin() const {  // NOLINT
        return flat_matrix_->data_.begin() + offset_;
    }

    auto end() const {  // NOLINT
        return begin() + flat_matrix_->row_size_;
    }

    size_t size() const {  // NOLINT
        return flat_matrix_->row_size_;
    }

    operator std::vector<T>() const {  // NOLINT
        return {begin(), end()};
    }

   private:
    Row(size_t offset, FlatMatrix* flat_matrix)
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
        return begin() + flat_matrix_->row_size_;
    }

    size_t size() const {  // NOLINT
        return flat_matrix_->row_size_;
    }

    operator std::vector<T>() const {  // NOLINT
        return {begin(), end()};
    }

   private:
    CRow(size_t offset, const FlatMatrix* flat_matrix)
        : offset_(offset),
          flat_matrix_(flat_matrix) {
    }
};

// Реализация итераторов FlatMatrix

template <typename T>
class FlatMatrix<T>::iterator {
   public:
    // NOLINTBEGIN
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Row;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
    // NOLINTEND

    iterator(size_t offset, FlatMatrix* matrix)
        : offset_(offset),
          matrix_(matrix) {
    }

    reference operator*() const {
        return Row(offset_, matrix_);
    }

    iterator& operator++() {
        offset_ += matrix_->row_size_;
        return *this;
    }

    iterator operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }

    iterator& operator--() {
        offset_ -= matrix_->row_size_;
        return *this;
    }

    iterator operator--(int) {
        iterator tmp = *this;
        --*this;
        return tmp;
    }

    iterator& operator+=(difference_type n) {
        offset_ += n * matrix_->row_size_;
        return *this;
    }

    iterator& operator-=(difference_type n) {
        offset_ -= n * matrix_->row_size_;
        return *this;
    }

    friend iterator operator+(iterator it, difference_type n) {
        it += n;
        return it;
    }

    friend iterator operator+(difference_type n, iterator it) {
        return it + n;
    }

    friend iterator operator-(iterator it, difference_type n) {
        it -= n;
        return it;
    }

    friend difference_type operator-(const iterator& a, const iterator& b) {
        return (a.offset_ - b.offset_) / a.matrix_->row_size_;
    }

    bool operator==(const iterator& other) const {
        return offset_ == other.offset_ && matrix_ == other.matrix_;
    }

    bool operator!=(const iterator& other) const {
        return !(*this == other);
    }

   private:
    size_t offset_;
    FlatMatrix* matrix_;
};

template <typename T>
class FlatMatrix<T>::const_iterator {
   public:
    // NOLINTBEGIN
    using iterator_category = std::random_access_iterator_tag;
    using value_type = CRow;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type;
    // NOLINTEND

    const_iterator(size_t offset, const FlatMatrix* matrix)
        : offset_(offset),
          matrix_(matrix) {
    }

    reference operator*() const {
        return CRow(offset_, matrix_);
    }

    const_iterator& operator++() {
        offset_ += matrix_->row_size_;
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    const_iterator& operator--() {
        offset_ -= matrix_->row_size_;
        return *this;
    }

    const_iterator operator--(int) {
        const_iterator tmp = *this;
        --*this;
        return tmp;
    }

    const_iterator& operator+=(difference_type n) {
        offset_ += n * matrix_->row_size_;
        return *this;
    }

    const_iterator& operator-=(difference_type n) {
        offset_ -= n * matrix_->row_size_;
        return *this;
    }

    friend const_iterator operator+(const_iterator it, difference_type n) {
        it += n;
        return it;
    }

    friend const_iterator operator+(difference_type n, const_iterator it) {
        return it + n;
    }

    friend const_iterator operator-(const_iterator it, difference_type n) {
        it -= n;
        return it;
    }

    friend difference_type operator-(const const_iterator& a,
                                     const const_iterator& b) {
        return (a.offset_ - b.offset_) / a.matrix_->row_size_;
    }

    bool operator==(const const_iterator& other) const {
        return offset_ == other.offset_ && matrix_ == other.matrix_;
    }

    bool operator!=(const const_iterator& other) const {
        return !(*this == other);
    }

   private:
    size_t offset_;
    const FlatMatrix* matrix_;
};

}  // namespace topa::detail
