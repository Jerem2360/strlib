#include "StringResourceList.hpp"
#include "strhash.h"
#include <iostream>

/*
This class uses a positional stack to keep track of its empty spaces.
Each time a StringResource object is deleted from the list, its
position in the list is pushed to the positional stack. 
Each time a new StringResource object is added to the list, 
its position is popped from the positional stack. If the positional
stack is empty, the object is appended to the list.
Each StringResource object owns a buffer of const chars that stores
a string.
*/


//a

StringResourceList* StringResourceList::cache = nullptr;

StringResourceList& StringResourceList::get() {
	if (!cache) {
		cache = new StringResourceList();
		atexit(freeCache);
	}
	return *cache;
}

void StringResourceList::freeCache() {
	if (cache) {
		delete cache;
		cache = nullptr;
	}
}


void StringResourceList::push_position(resource_t pos) {
	this->positional_stack.push_back(pos);
}

resource_t StringResourceList::pop_position() {
	resource_t res = this->positional_stack[this->positional_stack.size() - 1];
	this->positional_stack.pop_back();
	return res;
}

StringResourceList::StringResourceList() {
	this->resources = std::vector<StringResource>();
	this->positional_stack = std::vector<resource_t>();
}

/*
Create a new string resource by copying the specified data into the list.
The resource is placed at the end of the list if no slots are available
in the middle.
*/
resource_t StringResourceList::createResource(const char* contents, size_t sz) {
	//std::cout << "Positional stack is first " << this->positional_stack.size() << " long.\n";
	//std::cout << "Resources is first " << this->resources.size() << " long.\n";
	if (this->positional_stack.size()) {
		resource_t position = this->pop_position();
		this->resources[position] = StringResource(contents, sz);
		this->resources[position].incref();
		return position;
	}
	resource_t pos = this->resources.size();
	this->resources.push_back(StringResource(contents, sz));
	//std::cout << "Resources is then " << this->resources.size() << " long.\n";
	this->resources[pos].incref();
	return pos;
}

/*
Delete a string resource and free the space it occupies in the list.
The now free position is pushed to the positional stack to notify
the list of its availability.
*/
void StringResourceList::discardResource(resource_t index) {
	this->resources[index].release();
	this->resources[index] = StringResource();
	this->push_position(index);
}


void StringResourceList::decref(resource_t index) {
	this->resources[index].decref();
	if (this->resources[index].getRefCnt() == 0) {
		this->discardResource(index);
	}
}

void StringResourceList::incref(resource_t index) {
	this->resources[index].incref();
}

resource_t StringResourceList::searchForResource(hash_t hash) {
	for (resource_t i = 0; i < (long long)this->resources.size(); i++) {
		if (this->resources[i] && this->resources[i].hash() == hash)
			return i;
	}
	return -1;
}

bool StringResourceList::doesResourceExist(resource_t index) {
	if ((long long)this->resources.size() <= index)  // index out of bounds
		return 0;
	if (index < 0)  // index out of bounds
		return 0;
	if (!this->resources[index])  // 'blank' slot of our list
		return 0;
	return 1;
}

resource_t StringResourceList::find(hash_t hash) {
	resource_t res = this->searchForResource(hash);
	if (res >= 0) this->incref(res);
	return res;
}

resource_t StringResourceList::bind(const char* str, size_t sz) {
	hash_t hash = computeHash(str, sz);
	//std::cout << "Hash is " << hash << "\n";
	resource_t res = this->find(hash);
	if (res < 0) {
		//std::cout << "Not found, creating...\n";
		res = this->createResource(str, sz);
	}
	return res;
}

resource_t StringResourceList::bind(resource_t index) {
	if (!this->doesResourceExist(index))
		return -1;
	//std::cout << "here\n";
	this->incref(index);
	return index;
}

bool StringResourceList::unbind(resource_t* pindex) {
	if (!pindex)
		return 0;
	if (!this->doesResourceExist(*pindex))
		return 0;
	this->decref(*pindex);
	*pindex = -1;
	return 1;
}

bool StringResourceList::get(resource_t index, size_t pos, char* out) {
	if (!this->doesResourceExist(index))
		return 0;
	if (!out)
		return 0;
	int16_t res = this->resources[index].getChar(pos);
	if (res >= CHAR_MAX + 1)
		return 0;
	*out = (char)res;
	return 1;
}

size_t StringResourceList::size(resource_t index) {
	if (!this->doesResourceExist(index))
		return 0;
	return this->resources[index].getSize();
}


bool StringResourceList::copy(resource_t index, char* dst) {
	if (!this->doesResourceExist(index))
		return 0;
	if (!dst)
		return 0;
	for (size_t i = 0; i < this->size(index); i++) {
		char c;
		if (this->get(index, i, &c)) {
			dst[i] = c;
		}
	}
	dst[this->size(index)] = 0;
	return 1;
}

bool StringResourceList::hash(resource_t index, hash_t* out) {
	if (!this->doesResourceExist(index))
		return 0;
	if (!out)
		return 0;
	*out = this->resources[index].hash();
	return 1;
}

const char* StringResourceList::buffer(resource_t index) {
	if (!this->doesResourceExist(index))
		return nullptr;
	return this->resources[index].buffer();
}

