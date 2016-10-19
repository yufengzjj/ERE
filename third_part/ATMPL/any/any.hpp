//
// Created by bhzyf on 2016/5/2.
//

#ifndef CLIONSCIENCE_ANY_HPP
#define CLIONSCIENCE_ANY_HPP
#include <type_traits>
#include <typeinfo>

namespace ATMPL
{
	class any
	{
	private:
		class placeholder
		{
		public: // structors

			virtual ~placeholder()
			{
			}

		public: // queries

			virtual placeholder *clone() const = 0;

		};

		template<typename _ValueType>
		class holder : public placeholder
		{
		public:
			holder(const _ValueType &v)
				: _hold(v)
			{
			}

			placeholder *clone() const override
			{
				return new holder(_hold);
			}

		public:
			_ValueType _hold;
		};

	public:
		any() : _holder(nullptr)
		{
		}

		template<typename _ValueType>
		any(const _ValueType &v)
			: _holder(new holder<typename std::remove_cv<typename std::decay<const _ValueType>::type>::type>(v))
		{
		}

		// Perfect forwarding of _ValueType
		template<typename _ValueType>
		any(_ValueType &&v
			, typename std::enable_if<!std::is_same<any, typename std::decay<_ValueType>::type>::value, _ValueType>::type* = nullptr
			, typename std::enable_if<!std::is_const<_ValueType>::value>::type* = nullptr)
			:_holder(new holder< typename std::decay<_ValueType>::type >(std::forward<_ValueType>(v)))
		{
		}

		//copy constructor
		any(const any &other)
			: _holder(other._holder != nullptr ? other._holder->clone() : nullptr)
		{
		}

		//move constructor
		any(any && other)
			:_holder(other._holder)
		{
			other._holder = nullptr;
		}

		//assignment constructor
		any & operator=(const any& other_any)
		{
			_holder = other_any._holder->clone();
			return *this;
		}

		//move assignment constructor 
		any & operator=(any&& other_any)
		{
			_holder = other_any._holder;
			other_any._holder = nullptr;
			return *this;
		}

		~any()
		{
			delete _holder;
			_holder = nullptr;
		}

		template<typename _ValueType>
		static _ValueType cast(any &operand)
		{
			typedef typename std::remove_reference<_ValueType>::type nonref;
			nonref * op = cast<nonref>(&operand);
			typedef typename std::conditional<std::is_reference<_ValueType>::value, _ValueType,
				typename std::add_lvalue_reference<_ValueType>::type>::type ref_type;
			return static_cast<ref_type>(*op);
		}

	private:
		template<typename _ValueType>
		static _ValueType *cast(any *operand)
		{
			auto _holder = dynamic_cast<holder<_ValueType> *>(operand->_holder);
			if (_holder == nullptr)
				throw std::bad_cast();
			return &_holder->_hold;
		}

		template<typename _ValueType>
		static const _ValueType *cast(const any *operand)
		{
			return cast<_ValueType>(const_cast<any *>(operand));
		}


	public:
		placeholder *_holder;
	};


}
#endif //CLIONSCIENCE_ANY_HPP
