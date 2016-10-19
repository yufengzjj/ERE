#include <iostream>
#include <string>
#include "any.hpp"
#include <vector>
using namespace ATMPL;

template<typename _T>
void pod_test(any &v)
{
    std::cout << "any cast to " << typeid(_T).name() << " : " << any::cast<_T>(v) << std::endl;
}

any ret()
{
    int i = 1;
    return any(i);
}

int main()
{


    try
    {
        any v(short(1));
        pod_test<short>(v);

        any v_int(int(2));
        pod_test<int>(v_int);

        any v_long(long(3));
        pod_test<long>(v_long);

        any v_longlong((long long)(4));
        pod_test<long long>(v_longlong);

        any v_char(char('5'));
        pod_test<char>(v_char);

        char str[] = "hello world";
        any v_cptr(str);
        pod_test<char const *>(v_cptr);

        any v_string(std::string("HI! string any cast"));
        pod_test<std::string>(v_string);

        any any_0;
        any any_1(ret());
        pod_test<int>(any_1);

        any any_2(any_1);
        pod_test<int>(any_2);

        any any_3 = ret();
        pod_test<int>(any_3);

        any any_4 = any_1;
        pod_test<int>(any_4);

        any any_5 = any(int(5));
        pod_test<int>(any_5);

        std::vector<any> any_vec;
        any_vec.push_back('A');
        any_vec.push_back(short(1));
        any_vec.push_back(int(2));
        any_vec.push_back(str);
        any_vec.push_back(std::string("hello any,world"));
        pod_test<char>(any_vec[0]);
        pod_test<short>(any_vec[1]);
        pod_test<int>(any_vec[2]);
        pod_test<char const *>(any_vec[3]);
        pod_test<std::string>(any_vec[4]);

        printf("test passed\n");
    }
    catch (std::bad_cast&e)
    {
        std::cout << e.what()<<" occured when use any\n";
    }


}