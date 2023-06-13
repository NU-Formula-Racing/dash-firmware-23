// mutable_string_array.hxx
#ifndef MUTABLE_STRING_ARRAY_H
#define MUTABLE_STRING_ARRAY_H

class MutableStringArray {
 public:
  MutableStringArray();
  ~MutableStringArray();
  void AddString(const char *str);
  void Remove(const char* str);
  int Length() const;
  bool Contains(const char* str) const;
  const char* operator[](int index) const;

 private:
  int size_;
  int capacity_;
  char** data_;
};

#endif
