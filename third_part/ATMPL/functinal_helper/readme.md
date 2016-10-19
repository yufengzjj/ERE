### functional_helper

include function traits and a enhanced bind template class.  

##### function_traits

 it provide a template class named function_traits<template T>. 
you can obtain type information on compile-time like return type, 
arguments' type. it support function pointer,member function pointer, 
member object pointer,lambda and functor.(no support for bind expression,see next).  

##### bind--enhanced std::bind

 this version of bind also supports whatever std::bind supports for binding. 
it allow you obtain type information of the bonded callable objects as function_traits provided. 
bind::underlying_type is the very original callable type. it can obtain very original callable type information 
of any recursive bind(not std::bind) you have made.  

  

all of it programmed with standard c++11. 
this library supports any compilers as long as they support c++11 standard.

  

examples:

```c++ 
void test(int,char*){}
auto f1 = functional_helper::bind(test, _1,_2);
auto f2 = functional_helper::bind(f1, _1, _2);
auto test_lambda = [](int, char*)->void{};
```

obtain argument: 

```c++
functional_helper::function_traits<decltype(test)>::arg<1>::type//char*
decltype(f1)::underlying_type::arg<1>::type //char*
decltype(f2)::underlying_type::arg<1>::type //char*
decltype(test_lambda)::underlying_type::arg<1>::type //char*
f1('A');
f2('B');
test_lambda(1,'C');
```

