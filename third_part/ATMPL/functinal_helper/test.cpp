#include <type_traits>
#include <functional>
#include "bind.hpp"
using namespace ATMPL::functional_helper;
int test(int, long*)
{
	return 0;
}
class test_class
{
public:
    test_class(){}
    ~test_class(){}
    void test(int, char*){}
};
using namespace std::placeholders;
using namespace ATMPL;
void bind_holder_test()
{
	auto f1 = functional_helper::bind(test, _1, _2);
	auto f2 = functional_helper::bind(f1, _1, _2);
	auto f3 = functional_helper::bind(f1, _1, _2);
	f1(1, nullptr);
	f2(1, nullptr);
	f3(1, nullptr);
	static_assert(std::is_same<decltype(f1), decltype(f2)::bind_type>::value, "");
	static_assert(std::is_same<decltype(f1)::underlying_type::arg<1>::type, long*>::value, "");
	static_assert(std::is_same<bind_holder_stripper<decltype(f3)>::arg<1>::type, long*>::value, "");
	static_assert(std::is_same<bind_holder_stripper<decltype(f3)>::arg<1>::type, decltype(f3)::underlying_type::arg<1>::type>::value, "");
}


class T
{
public:
	short test(long) const
	{
		return 1;
	}
	static void test1(char) {}
	long long operator() (int) const
	{
		return 1;
	}
};

void function_traits_test()
{
	T t;
	auto p = [](int, const char*, long*)->int {
		return 0;
	};
	auto bind_func = std::bind(test, 1, std::placeholders::_1, std::placeholders::_2);
	typedef decltype(&T::test)  mem_func_pointer;
	typedef decltype(test)  func;
	typedef decltype(&test)  func_pointer;
	typedef decltype(t)  functor;
	typedef decltype(p)  lambda_type;
	typedef decltype(&lambda_type::operator())  lambda;
	typedef decltype(bind_func) bind_expr;
	printf("%s\n", typeid(func_traits<mem_func_pointer>::arg<1>::type).name());
	printf("%s\n", typeid(func_traits<func>::ret).name());
	printf("%s\n", typeid(func_traits<func_pointer>::arg<0>::type).name());
	printf("%s\n", typeid(func_traits<functor>::arg<1>::type).name());
	printf("%s\n", typeid(func_traits<lambda>::arg<2>::type).name());
	printf("%s\n", typeid(func_traits<lambda_type>::arg<3>::type).name());
	printf("%s\n", typeid(lambda_type).name());
	printf("%s\n", typeid(lambda).name());
	printf("%s\n", typeid(bind_expr).name());



	static_assert(std::is_same<func_traits<mem_func_pointer>::arg<1>::type, long>::value, "");
	static_assert(std::is_same<func_traits<func>::ret, int>::value, "");
	static_assert(std::is_same<func_traits<func_pointer>::arg<0>::type, int>::value, "");
	static_assert(std::is_same<func_traits<functor>::arg<1>::type, int>::value, "");
	static_assert(std::is_same<func_traits<lambda>::arg<2>::type, const char*>::value, "");
}