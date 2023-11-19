#include "StringResource.hpp"
#include "strhash.h"
#include <cstdlib>

StringResource::StringResource() {
	this->contents = nullptr;
	this->size = 0;
	this->_hash = 0;
	this->refcnt = 0;
}

StringResource::StringResource(const char* contents, size_t sz) {
	bool isNullTerminated = contents[sz - 1] == 0;

	this->size = isNullTerminated ? sz - 1 : sz;
	char* buf = new char[this->size + 1];
	std::memcpy(buf, contents, this->size);
	buf[this->size] = 0;
	this->contents = buf;

	this->_hash = computeHash(this->contents, this->size);
	this->refcnt = 0;
}

hash_t StringResource::hash() {
	return this->_hash;
}

void StringResource::incref() {
	this->refcnt++;
}

void StringResource::decref() {
	if (this->refcnt > 0)
		this->refcnt--;
}

size_t StringResource::getSize() {
	return this->size;
}

int16_t StringResource::getChar(size_t index) {
	if (index >= this->size)
		return CHAR_MAX + 1;
	return this->contents[index];
}

size_t StringResource::getRefCnt() {
	return this->refcnt;
}

void StringResource::release() {
	if (!this->contents)
		return;
	delete this->contents;
}

StringResource::operator bool() {
	return this->contents != nullptr;
}


