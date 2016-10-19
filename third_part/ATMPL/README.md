# ATMPL

**all library components write in c++11 without any other dependency.**

at present, this ATMPL--Another Template Meta Programming Library has a little supported  

features as follow:  

### functional_helper  
include function traits and a enhanced bind template class.  

### Any

Safe, generic container for single values of different value types. **similar to [boost.Any](http://www.boost.org/doc/libs/1_60_0/doc/html/any.html)** but c++11 version.

### Optional

A value-semantic, type-safe wrapper for representing 'optional' (or 'nullable') objects of a given type. An optional object may or may not contain a value of the underlying type. [boost.Optional](http://www.boost.org/doc/libs/1_61_0/libs/optional/doc/html/index.html) C++11 version

**all test on VS2015/GCC 5.3.0/Clang 3.8.0**

