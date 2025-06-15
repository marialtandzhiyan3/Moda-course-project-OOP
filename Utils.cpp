#include "Utils.h"
#include <cstring>
#pragma warning(disable : 4996)


CharArray::CharArray() : data(nullptr), length(0) {}

CharArray::CharArray(const char* str) {
    if (!str) {
        data = nullptr;
        length = 0;
        return;
    }

    length = strlen(str);
    data = new char[length + 1];
    strcpy(data, str);
}

CharArray::CharArray(const CharArray& other) {
    copyFrom(other);
}

CharArray::~CharArray() {
    freeMemory();
}

CharArray& CharArray::operator=(const CharArray& other) {
    if (this != &other) {
        freeMemory();
        copyFrom(other);
    }
    return *this;
}

bool CharArray::operator==(const CharArray& other) const {
    if (length != other.length) return false;
    return strcmp(data, other.data) == 0;
}

void CharArray::copyFrom(const CharArray& other) {
    length = other.length;
    data = new char[length + 1];
    strcpy(data, other.data);
}

void CharArray::freeMemory() {
    delete[] data;
    data = nullptr;
    length = 0;
}

unsigned int CharArray::getLength() const {
    return length;
}

const char* CharArray::getData() const {
    return data;
}

void CharArray::concat(const CharArray& other) {
    char* newData = new char[length + other.length + 1];
    strcpy(newData, data);
    strcat(newData, other.data);

    delete[] data;
    data = newData;
    length += other.length;
}