#pragma once
#include "resource.hpp"
#include <vector>
#include <iostream>


class string
{
	resource_t data;

	struct ConstIterator {
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = char;
		using pointer = value_type*;
		using reference = value_type&;

		ConstIterator(resource_t resource);
		ConstIterator();
		ConstIterator(const ConstIterator&);
		ConstIterator(ConstIterator&&) noexcept;

		ConstIterator& operator =(const ConstIterator&);
		ConstIterator& operator =(ConstIterator&&) noexcept;

		const value_type& operator *() const;
		const value_type* operator ->();

		ConstIterator& operator++();
		ConstIterator operator++(int);

		bool operator ==(const ConstIterator& other) const;
		bool operator !=(const ConstIterator& other) const;

		~ConstIterator();

	private:
		size_t resource_length() const;
		void try_bind();
		bool try_unbind(resource_t*);
		void try_unbind();
		void try_fetch_char();
		void try_set_from_single_char();
		void clear();

		resource_t resource;
		size_t position;
		value_type current_element;
	};
	
public:
	string();
	string(std::nullptr_t);
	string(const char*, size_t);
	string(const char*);
	string(char);

	string(const string&);
	string(string&&) noexcept;

	explicit string(long long);
	explicit string(long double);
	explicit string(bool);

	string& operator =(const string&);
	string& operator =(string&&) noexcept;

	operator const char* () const;
	operator bool() const;

	string operator +(const string) const;
	string operator *(const size_t) const;
	string operator +=(const string);
	string operator *=(const size_t);

	bool operator ==(std::nullptr_t) const;
	bool operator ==(const string) const;

	bool operator >=(const string) const;
	bool operator >(const string) const;
	bool operator <=(const string) const;
	bool operator <(const string) const;

	char operator [](size_t) const;

	size_t length() const;
	std::vector<string> split(const string) const;
	string join(std::vector<string>) const;
	string removePrefix(const string prefix) const;
	string removeSuffix(const string suffix) const;
	bool startsWith(const string) const;
	bool endsWith(const string) const;
	hash_t hash() const;
	bool contains(const string) const;
	string fill(char what, size_t max) const;
	
	ConstIterator begin() const;
	ConstIterator end() const;

	~string();
};


std::ostream& operator <<(std::ostream&, const string);
std::istream& operator >>(std::istream&, string&);

