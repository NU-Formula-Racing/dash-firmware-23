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
  data_[size_] = copy;
  size_++;
  // ++size_;
}

void MutableStringArray::Remove(const char* str) {
  for (int i = 0; i < size_; i++) {
    const char* cur_str = data_[i];
    int j = 0;
    while (cur_str[j] != '\0' && str[j] != '\0' && cur_str[j] == str[j]) {
      j++;
    }
    if (cur_str[j] == '\0' && str[j] == '\0') {
      delete[] data_[i];
      for (int k = i; k < size_ - 1; k++) {
        data_[k] = data_[k + 1];
      }
      --size_;
      return;
    }
  }
}


bool MutableStringArray::Contains(const char* str) const {
  for (int i = 0; i < size_; i++) {
    const char* s = data_[i];
    int j = 0;
    while (s[j] != '\0' && str[j] != '\0' && s[j] == str[j]) {
      j++;
    }
    if (s[j] == '\0' && str[j] == '\0') {
      return true;
    }
  }
  return false;
}

int MutableStringArray::Length() const { return size_; }

const char* MutableStringArray::operator[](int index) const { return data_[index]; }
