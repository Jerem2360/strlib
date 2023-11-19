#pragma once
#include <exception>

class StringIndexOutOfBoundsException : public std::exception
{
	using std::exception::exception;
};

