# strlib
Utility library that provides an optimized ```string``` type for C++. 

The ```string``` class that this library provides makes sure no two identical strings are stored
at different places in memory. This means that two identical strings will share the same
memory buffer, therefore forcing this type to be readonly.
There is a plan to add another ```mutablestring``` type which is mutable, but doesn't have the same
optimization as the ```string``` type.
