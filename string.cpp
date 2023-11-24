#include "string.hpp"
#include "StringResourceList.hpp"
#include "strhash.h"
#include "StringIndexOutOfBoundsException.hpp"


bool isSingleChar(resource_t resource) {
	resource_t maybe_char = resource + 384;
	return maybe_char < CHAR_MAX && maybe_char > CHAR_MIN;
}

char resourceToSingleChar(resource_t resource) {
	return (char)(resource + 384);
}

resource_t singleCharToResource(char c) {
	return (resource_t)c - 384;
}

constexpr resource_t NULLSTR_resource = -2;
constexpr resource_t EMPTYSTR_resource = -3;


string::ConstIterator::ConstIterator() :
	resource(-1), position(0), current_element(0)
{}

string::ConstIterator::ConstIterator(resource_t resource) :
	resource(resource), position(0)
{
	if (isSingleChar(this->resource)) {
		this->try_set_from_single_char();
		return;
	}
	if (this->resource < 0) {
		this->clear(); // make invalid, we don't want to iterate through empty or null string.
		return;
	}
	// we must have our own reference to the resource in case the last string pointing
	// there is destroyed before us. This also makes the process thread-safe.
	this->try_bind();
	this->try_fetch_char();
}

string::ConstIterator::ConstIterator(const string::ConstIterator& src) :
	resource(src.resource), position(src.position), current_element(src.current_element)
{
	if (isSingleChar(this->resource)) {
		this->try_set_from_single_char();
		return;
	}
	if (this->resource < 0) {
		this->clear();
		return;
	}
	this->try_bind();
	this->try_fetch_char();
}

string::ConstIterator::ConstIterator(string::ConstIterator&& src) noexcept :
	resource(src.resource), position(src.position), current_element(src.current_element)
{
	src.clear();
}

string::ConstIterator& string::ConstIterator::operator=(const string::ConstIterator& src) {
	resource_t previous_resource = this->resource;
	this->resource = src.resource;
	this->current_element = src.current_element;
	this->position = src.position;
	
	if (isSingleChar(this->resource)) {
		this->try_set_from_single_char();
		goto end;
	}
	if (this->resource < 0) {
		this->clear();
		goto end;
	}
	this->try_bind();

end:
	this->try_unbind(&previous_resource);
	return *this;
}

string::ConstIterator& string::ConstIterator::operator=(string::ConstIterator&& src) noexcept {
	resource_t previous_resource = this->resource;
	this->resource = src.resource;
	this->current_element = src.current_element;
	this->position = src.position;

	if (isSingleChar(this->resource)) {
		this->try_set_from_single_char();
		goto end;
	}
	if (this->resource < 0) {
		this->clear();
	}

end:
	this->try_unbind(&previous_resource);
	src.clear();
	return *this;
}

const string::ConstIterator::value_type& string::ConstIterator::operator *() const {
	return this->current_element;
}

const string::ConstIterator::value_type* string::ConstIterator::operator->() {
	return &this->current_element;
}

string::ConstIterator& string::ConstIterator::operator++() {
	if (this->resource == -2) {
		this->clear();
		return *this;
	}
	if (this->resource < 0)
		return *this;
	this->position++;
	this->try_fetch_char();
	return *this;
}

string::ConstIterator string::ConstIterator::operator++(int) {
	ConstIterator res = *this;
	++(*this);
	return res;
}

bool string::ConstIterator::operator==(const string::ConstIterator& other) const {
	if (this->resource == -1) {
		return other.resource == -1;
	}
	return this->resource == other.resource &&
		this->position == other.position;
}

bool string::ConstIterator::operator!=(const string::ConstIterator& other) const {
	return !(*this == other);
}

size_t string::ConstIterator::resource_length() const {
	if (this->resource < 0)
		return isSingleChar(this->resource);
	return StringResourceList::get().size(this->resource);
}

void string::ConstIterator::try_bind() {
	if (StringResourceList::get().bind(this->resource) < 0) {
		this->clear();
	}
}

void string::ConstIterator::try_fetch_char() {
	/*
	this->current_element is a buffer for passing a character to the user which is iterating over
	our resource. No matter how he changes its value, the resource will be unaffected. This makes
	sense because strings are immutable.
	*/
	if (!StringResourceList::get().get(this->resource, this->position, &this->current_element)) {
		this->clear();
	}
}

bool string::ConstIterator::try_unbind(resource_t* presource) {
	if (*presource >= 0) {
		StringResourceList::get().unbind(presource);
		return 1;
	}
	return 0;
}

void string::ConstIterator::try_unbind() {
	if (this->try_unbind(&this->resource)) {
		this->clear();
	}
}

void string::ConstIterator::try_set_from_single_char() {
	this->current_element = resourceToSingleChar(this->resource);
	this->resource = -2;
	this->position = 0;
}

void string::ConstIterator::clear() {
	this->resource = -1;
	this->current_element = 0;
	this->position = 0;
}

string::ConstIterator::~ConstIterator() {
	this->try_unbind();
}


string::string() : string(nullptr) 
{}

string::string(std::nullptr_t) : data(NULLSTR_resource)
{}

string::string(const char* str, size_t sz) {
	if (!sz) {
		this->data = EMPTYSTR_resource;
		return;
	}
	if (sz == 1) {
		this->data = singleCharToResource(str[0]);
		return;
	}
	this->data = StringResourceList::get().bind(str, sz);
}

string::string(const char* str) : string(str, std::strlen(str))
{}

string::string(char c) :
	data(singleCharToResource(c))
{}

string::string(const string& src) {
	resource_t old_resource = this->data;
	this->data = src.data;
	if (this->data >= 0) {
		StringResourceList::get().bind(this->data);
	}
	if (old_resource >= 0) {
		StringResourceList::get().unbind(&old_resource);
	}
}

string::string(string&& src) noexcept {
	if (this->data >= 0) {
		StringResourceList::get().unbind(&this->data);
	}
	this->data = src.data;
	src.data = -1;
}

string& string::operator=(const string& src) {
	resource_t old_resource = this->data;
	this->data = src.data;
	if (this->data >= 0) {
		StringResourceList::get().bind(this->data);
	}
	if (old_resource >= 0) {
		StringResourceList::get().unbind(&old_resource);
	}
	return *this;
}

string& string::operator=(string&& src) noexcept {
	if (this->data >= 0) {
		StringResourceList::get().unbind(&this->data);
	}
	this->data = src.data;
	src.data = -1;
	return *this;
}

string string::operator +(const string other) const {
	if (!this->length())
		return other;
	if (!other.length())
		return *this;
  
	hash_t hash_sum = this->hash() + other.hash();
	resource_t result_data = StringResourceList::get().find(hash_sum);
	if (result_data >= 0) {
		string result;
		result.data = result_data;
		return result;
	}

	size_t res_size = this->length() + other.length();
	
	char s_uchr;
	const char* self_buffer = StringResourceList::get().buffer(this->data);
	if (!self_buffer) {
		if (isSingleChar(this->data)) {
			s_uchr = resourceToSingleChar(this->data);
			self_buffer = &s_uchr;
		} else
			return other;
	}

	char o_uchr;
	const char* other_buffer = StringResourceList::get().buffer(other.data);
	if (!other_buffer) {
		if (isSingleChar(other.data)) {
			o_uchr = resourceToSingleChar(other.data);
			other_buffer = &o_uchr;
		} else
			return *this;
	}

	char* res_str = new char[res_size + 1];
	std::memcpy(res_str, self_buffer, this->length());
	std::memcpy(res_str + this->length(), other_buffer, other.length());

	string res = { res_str, res_size };
	delete[] res_str;
	return res;
}

string string::operator+=(const string other) {
	(*this) = (*this) + other;
	return *this;
}

string::operator const char* () const {
	if (this->data == NULLSTR_resource)
		return nullptr;
	if (this->data == EMPTYSTR_resource)
		return "";
	char* res = new char[this->length() + 1];
	if (isSingleChar(this->data)) {
		res[0] = resourceToSingleChar(this->data);
		res[1] = 0;
		return res;
	}
	if (!StringResourceList::get().copy(this->data, res))
		return nullptr;
	return res;
}

bool string::operator==(std::nullptr_t) const {
	return this->data == NULLSTR_resource;
}

bool string::operator==(const string other) const {
	return this->hash() == other.hash();
}

bool string::operator>=(const string other) const {
	return this->hash() >= other.hash();
}

bool string::operator>(const string other) const {
	return this->hash() > other.hash();
}

bool string::operator<=(const string other) const {
	return this->hash() <= other.hash();
}

bool string::operator<(const string other) const {
	return this->hash() < other.hash();
}

char string::operator [](size_t i) const {
	if (i >= this->length())
		throw StringIndexOutOfBoundsException("Index out of bounds.");
	char res;
	if (isSingleChar(this->data)) {
		return resourceToSingleChar(this->data);
	}
	if (!StringResourceList::get().get(this->data, i, &res))
		throw StringIndexOutOfBoundsException("Index out of bounds.");
	return res;
}

hash_t string::hash() const {
	hash_t hash;
	if (StringResourceList::get().hash(this->data, &hash))
		return hash;
	if (isSingleChar(this->data)) {
		return resourceToSingleChar(this->data);
	}
	if (this->data == NULLSTR_resource) {
		return INT32_MAX;
	}
	return 0;
}

size_t string::length() const {
	if (isSingleChar(this->data))
		return 1;
	if (size_t res = StringResourceList::get().size(this->data))
		return res;
	
	return 0;
}

string::ConstIterator string::begin() const {
	return ConstIterator(this->data);
}

string::ConstIterator string::end() const {
	return ConstIterator();
}

string::~string() {
	if (this->data >= 0) {
		StringResourceList::get().unbind(&this->data);
	}
}

std::ostream& operator <<(std::ostream& fs, const string str) {
	for (char c : str) {
		fs << c;
	}
	return fs;
}

std::istream& operator >>(std::istream& fs, string& str) {
	str = "";
	for (;;) {
		int in = fs.get();
		if (in == '\r')
			continue;
		if (in == '\n' || in == EOF)
			break;
		str += (char)in;
	}
	return fs;
}

