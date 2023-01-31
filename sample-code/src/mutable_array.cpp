// mutable_string_array.cpp
#include <cstring>
#include "mutable_array.h"

MutableStringArray::MutableStringArray() : size_(0), capacity_(10), data_(new char*[capacity_]) {}

MutableStringArray::~MutableStringArray() {
  for (int i = 0; i < size_; i++) {
    delete[] data_[i];
  }
  delete[] data_;
}

void MutableStringArray::AddString(const char *str) {
  if (size_ == capacity_) {
    capacity_ *= 2;
    char **new_data = new char*[capacity_];
    for (int i = 0; i < size_; i++) {
      new_data[i] = data_[i];
    }
    delete[] data_;
    data_ = new_data;
  }
  int len = strlen(str);
  char *copy = new char[len + 1];
  for (int i = 0; i < len + 1; i++) {
    copy[i] = str[i];
  }
  data_[size_++] = copy;
}

void MutableStringArray::Remove(int index) {
  if (index < 0 || index >= size_) {
    return;
  }
  delete[] data_[index];
  for (int i = index; i < size_ - 1; i++) {
    data_[i] = data_[i + 1];
  }
  --size_;
}

int MutableStringArray::Length() const { return size_; }

const char* MutableStringArray::operator[](int index) const { return data_[index]; }
