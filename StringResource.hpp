#pragma once
#include <vector>
#include "resource.hpp"

class StringResource
{
	size_t refcnt;
	size_t size;
	hash_t _hash;
	const char* contents;

public:
	StringResource();
	StringResource(const char* contents, size_t sz);

	hash_t hash();
	void incref();
	void decref();
	size_t getRefCnt();
	void release();
	size_t getSize();
	int16_t getChar(size_t index);
	operator bool();
};

