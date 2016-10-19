#pragma once
#include <tuple>
#include <type_traits>

namespace ATMPL
{

	namespace functional_helper
	{
		template<typename _T>
		struct func_traits;

		//function
		template<typename _Ret, typename... _Args>
		struct  func_traits<_Ret(_Args...)>
		{
			typedef _Ret ret;
			typedef std::integral_constant<size_t, sizeof...(_Args)> arity;
			template<size_t N>
			struct arg
			{
				static_assert(N < arity::value, "invalid parameter index");
				using type = typename std::tuple_element<N, std::tuple<_Args...>>::type;
			};
		};

		//function pointer
		template<typename _Ret, typename... _Args>
		struct  func_traits<_Ret(*)(_Args...)> :public func_traits<_Ret(_Args...)>
		{
		};

		//member function pointer
		template<typename _Ret, class _Class, typename... _Args>
		struct  func_traits<_Ret(_Class::*)(_Args...)> :public func_traits<_Ret(_Class&, _Args...)>
		{
		};

		template<typename _Ret, class _Class, typename... _Args>
		struct  func_traits<_Ret(_Class::*)(_Args...) const> :public func_traits<_Ret(_Class&, _Args...)>
		{
		};


		//functor and lambda
		template<class _Class>
		struct  func_traits : public func_traits<decltype(&_Class::operator())>
		{
		};

	}


}