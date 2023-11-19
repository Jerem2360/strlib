#pragma once
#include "StringResource.hpp"
#include "resource.hpp"
#include <vector>

/*
Class representing the list of currently active string
resources. Each string resource is represented by
a unique integer index of type resource_t inside this 
list. 
An negative index represents an non-existing resource.
The main reason for this class to be, is to avoid to
have multiple identical const string buffers at the 
same time.
String resources stored here are read-only.
If a string resource's number of bindings reaches zero,
it is deleted. This gives more space for future resources
without the need to push_back the list of resources again.
Strings are compared using their hash values.
*/
class StringResourceList
{
	static StringResourceList* cache;
	std::vector<StringResource> resources;
	std::vector<resource_t> positional_stack;

	resource_t pop_position();
	void push_position(resource_t);
	resource_t createResource(const char*, size_t);
	void discardResource(resource_t);
	resource_t searchForResource(hash_t);

	void incref(resource_t);
	void decref(resource_t);

	StringResourceList();
	bool doesResourceExist(resource_t);

	static void freeCache();
public:
	/*
	Returns the currently active list of string resources.
	*/
	static StringResourceList& get();

	/*
	Tries to bind to an existing string resource.
	If the resource is not found, create a new resource
	and bind to it.
	Returns an index that uniquely identifies the resource.
	*/
	resource_t bind(const char* str, size_t sz);
	/*
	Tries to bind to an existing string resource given its hash.
	If the resource is not found, return -1.
	Returns an index that uniquely identifies the resource if found.
	*/
	resource_t bind(size_t hash);
	/*
	Binds to the resource identified by index.
	Returns index on success, -1 otherwise.
	*/
	resource_t bind(resource_t index);
	/*
	Unbinds from the resource identified by *pindex.
	Sets *pindex to -1.
	If *pindex is invalid, its value is unaffected.
	Return whether unbinding was successful.
	*/
	bool unbind(resource_t* pindex);

	/*
	Fills *out with the character at the specified position
	in the string resource identified by index.
	Return 1 on succes, 0 on failure.
	*/
	bool get(resource_t index, size_t pos, char* out);
	/*
	Returns the size of the string resource identified by index,
	or 0 if the resource doesn't exist.
	*/
	size_t size(resource_t index);

	/*
	Copies the contents of the string resource identified by index 
	into the specified destination buffer. The destination buffer 
	is expected to be one byte longer than the size of the string 
	resource.

	Note: The size of the destination buffer is not checked by this 
	function, so make sure your buffer is large enough, or overflow
	will ensue.
	*/
	bool copy(resource_t index, char* dst);
	/*
	Fills *out with the hash value of the string resource identified
	by index, if it exists.
	Returns 1 on success, 0 otherwise.
	*/
	bool hash(resource_t index, hash_t* out);

	StringResourceList(const StringResourceList&) = delete;
	StringResourceList& operator =(const StringResourceList&) = delete;
};

