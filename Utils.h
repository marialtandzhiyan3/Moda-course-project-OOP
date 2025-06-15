#pragma once
#ifndef UTILS_H
#define UTILS_H
#pragma warning(disable : 4996)
#include <cstring>

class CharArray {
private:
    char* data;
    unsigned int length;

    void copyFrom(const CharArray& other);
    void freeMemory();

public:
    CharArray();
    CharArray(const char* str);
    CharArray(const CharArray& other);
    ~CharArray();

    CharArray& operator=(const CharArray& other);
    bool operator==(const CharArray& other) const;

    unsigned int getLength() const;
    const char* getData() const;

    void concat(const CharArray& other);
};

#endif
