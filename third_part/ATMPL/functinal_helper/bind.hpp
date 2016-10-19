#include <tuple>
#include <type_traits>
#include <functional>
#include "func_traits.hpp"
namespace ATMPL
{
    

namespace functional_helper
{
    template<class BindExpr, class F, class...Args>
    struct bind_holder;

    template<class F>
    struct is_bind_holder
        :public std::false_type
    {};
    template<class BindExpr, class F, class...Args>
    struct is_bind_holder<bind_holder<BindExpr,F,Args...>>
        :public std::true_type
    {};

    template<class BindExpr, class F, class...Args>
    struct is_bind_holder<const bind_holder<BindExpr, F, Args...>>
        :public std::true_type
    {};

    template<class BindExpr, class F, class...Args>
    struct is_bind_holder<volatile bind_holder<BindExpr, F, Args...>>
        :public std::true_type
    {};

    template<class BindExpr, class F, class...Args>
    struct is_bind_holder<const volatile bind_holder<BindExpr, F, Args...>>
        :public std::true_type
    {};

    

    template<class F,bool Is_bind_holder>
    struct bind_holder_stripper_helper;

    template<class F>
    struct bind_holder_stripper_helper<F, false>
        : public func_traits<F>
    {};
    
    template<class F>
    struct bind_holder_stripper_helper<F, true>
        :public bind_holder_stripper_helper<typename F::bind_type, is_bind_holder<typename F::bind_type>::value>
    {};

    template<class F>
    struct bind_holder_stripper
        :public bind_holder_stripper_helper<F, is_bind_holder<F>::value>
    {};

    template<class BindExpr, class F, class...Args>
    struct bind_holder
    {
    private:
        BindExpr _bind_expr;
    public:

        typedef F bind_type;
        typedef bind_holder_stripper<F> underlying_type;
        typedef std::integral_constant<size_t, sizeof...(Args)> bind_arity;
        explicit bind_holder(BindExpr&& _functor)
            :_bind_expr(std::move(std::forward<BindExpr>(_functor)))
        {
        }

        template<size_t N>
        struct bind_arg
        {
            static_assert(N < bind_arity::value, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
        };

        template<typename... _Args>
        auto operator()(_Args&&...params)
            ->decltype(this->_bind_expr(std::forward<_Args>(params)...))
        {
            return this->_bind_expr(std::forward<_Args>(params)...);
        }
    };

	template <typename T>
	struct remove_cvr
	{
		using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
	};

    template<class F, typename...Args>
    auto bind(F&& f, Args&&... args)
        ->decltype((bind_holder<decltype(std::bind(std::forward<F>(f), std::forward<Args>(args)...)),
        typename remove_cvr<F>::type, Args...>
                (std::bind(std::forward<F>(f), std::forward<Args>(args)...))))
    {
        return (bind_holder<decltype(std::bind(std::forward<F>(f), std::forward<Args>(args)...)),
            typename remove_cvr<F>::type, Args...>
                        (std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
    }
}

}