#pragma once
#include <type_traits>
#include <typeinfo>
namespace ATMPL
{
	template<typename _T>
	class optional
	{
	private:
		typedef typename std::decay<_T>::type storage_type;
		typename std::aligned_storage<sizeof(storage_type), alignof(storage_type)>::type storage;
		bool initialized;
	public:
		optional()
			:initialized(false) {}

		optional(storage_type value)
		{
			//std::cout << "convertion\n";
			::new (&storage) storage_type(std::forward<storage_type>(value));
			initialized = true;
		}

		optional(const optional& opl)
		{
			//std::cout << "copy constructor\n";
			initialized = opl.initialized;
			if (initialized)
				::new(&storage) storage_type(*opl);
		}

		optional(optional&& opl)
		{
			//std::cout << "move constructor\n";
			initialized = opl.initialized;
			if (initialized)
				::new(&storage) storage_type(std::move(*opl));
		}

		optional& operator=(optional && opl)
		{
			//std::cout << "asign move\n";
			if (this == &opl)
				return *this;
			initialized = opl.initialized;
			if (initialized)
				::new(&storage) storage_type(std::move(*opl));
			return *this;
		}

		optional& operator=(const optional & opl)
		{
			//std::cout << "asign copy\n";
			if (this == &opl)
				return *this;
			initialized = opl.initialized;
			if (initialized)
				::new(&storage) storage_type(*const_cast<optional &>(opl));
			return *this;
		}

		explicit  operator bool() const
		{
			return initialized;
		}

		bool operator !() const
		{
			return !initialized;
		}

		storage_type operator *() const
		{
			return reinterpret_cast<const storage_type&>(storage);
		}

		template<typename _Type>
		operator optional<_Type>() const
		{
			return static_cast<_Type>(reinterpret_cast<const storage_type&>(storage));
		}

		template<typename _Type>
		operator _Type() const
		{
			return static_cast<_Type>(reinterpret_cast<const storage_type&>(storage));
		}
	};
}