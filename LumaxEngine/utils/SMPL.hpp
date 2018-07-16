
#ifndef SMPL_HPP
#define SMPL_HPP

#include <cstddef>

#include <functional>
#include <type_traits>

namespace smpl {
	struct type_list_null {};
	
	template<typename T, typename U>
	struct type_list_impl {
		using Head = T;
		using Tail = U;
	};

	template<typename... Ts>
	struct type_list;
	template<>
	struct type_list<> {
		using type = type_list_null;
	};
	template<typename T, typename... Ts>
	struct type_list<T, Ts...> {
		using type = type_list_impl<T, typename type_list<Ts...>::type>;
	};
	
	// Type list size
	template<typename T>
	struct type_list_size;
	template<>
	struct type_list_size<type_list_null> {
		static constexpr size_t value = 0;
	};
	template<typename T, typename U>
	struct type_list_size<type_list_impl<T, U>> {
		static constexpr size_t value = type_list_size<U>::value + 1;
	};
	template<typename T>
	constexpr size_t getSize() { return type_list_size<T>::value; } 

	// Type list contains
	template<typename T, typename V>
	struct type_list_contains;
	template<typename V>
	struct type_list_contains<type_list_null, V> {
		static constexpr bool value = false;
	};
	template<typename T, typename U>
	struct type_list_contains<type_list_impl<T, U>, T> {
		static constexpr bool value = true;
	};
	template<typename T, typename U, typename V>
	struct type_list_contains<type_list_impl<T, U>, V> {
		static constexpr bool value = type_list_contains<U, V>::value;
	};
	template<typename T, typename V>
	constexpr bool contains() { return type_list_contains<T, V>::value; }

	// Type list index of
	template<typename T, typename V>
	struct type_list_index_of;
	template<typename V>
	struct type_list_index_of<type_list_null, V> {
		static constexpr size_t value = -1;
	};
	template<typename T, typename U>
	struct type_list_index_of<type_list_impl<T, U>, T> {
		static constexpr size_t value = 0;
	};
	template<typename T, typename U, typename V>
	struct type_list_index_of<type_list_impl<T, U>, V> {
		using result = type_list_index_of<U, V>;
		static constexpr size_t value = result::value == -1 ? -1 : result::value + 1;
	};
	template <typename T, typename V>
	constexpr size_t getIndexOf() { return type_list_index_of<T, V>::value; }
	
	// Type list filter
	template <typename T, template<typename> typename Predicate, typename... Ts>
	struct type_list_filter;

	template <bool same, typename T, template<typename> typename Predicate, typename... Ts>
	struct type_list_filter_impl;
	template <bool same, typename T, typename U, template<typename> typename Predicate, typename... Ts>
	struct type_list_filter_impl<same, type_list_impl<T, U>, Predicate, Ts...> {
		using result = typename type_list_filter<U, Predicate, Ts...>::result;
	};
	template <typename T, typename U, template<typename> typename Predicate, typename... Ts>
	struct type_list_filter_impl<true, type_list_impl<T, U>, Predicate, Ts...> {
		using result = typename type_list_filter<U, Predicate, Ts..., T>::result;
	};
	
	template <typename T, typename U, template<typename> typename Predicate, typename... Ts>
	struct type_list_filter<type_list_impl<T, U>, Predicate, Ts...> {
		using result = typename type_list_filter_impl<Predicate<T>::value, type_list_impl<T, U>, Predicate, Ts...>::result;
	};
	template <template<typename> typename Predicate, typename... Ts>
	struct type_list_filter<type_list_null, Predicate, Ts...> {
		using result = type_list<Ts...>;
	};
	
	// Type list repeater
	template <
		template <typename...> class ResultHolder,
		size_t N,
		typename Repeated,
		typename... ArgsList >
	struct repeater {};
	template <
		template <typename...> class ResultHolder,
		size_t N,
		template <typename...> class Holder,
		typename... ToBeRepeated,
		typename... ArgsList >
	struct repeater<ResultHolder, N, Holder<ToBeRepeated...>, ArgsList...> {
		typedef typename repeater<ResultHolder, N-1, Holder<ToBeRepeated...>, ArgsList..., ToBeRepeated...>::type type;
	};
	template <
		template <typename...> class ResultHolder,
		template <typename...> class Holder,
		typename... ToBeRepeated,
		typename... ArgsList >
	struct repeater<ResultHolder, 0U, Holder<ToBeRepeated...>, ArgsList...> {
		typedef ResultHolder<ArgsList...> type;
	};

	// Type wrapper
	template <typename T>
	struct Type {
		using type = T;
	};

	// Type list rename
	template <typename src, template<typename...> typename dst>
	struct rename_impl;
	template <template<typename...> typename src, typename... T, template<typename...> typename dst>
	struct rename_impl<src<T...>, dst>{
		using type = dst<T...>;
	};
	template<typename src, template <typename...> typename dst>
	using rename = typename rename_impl<src, dst>::type;

	// Type list's for types operation (run-time)
	// TODO

	// Iterate through signatures and call init bitsets function
	template <typename T, typename S>
	struct initializeSignatureBitset;
	template <typename T, typename U, typename S>
	struct initializeSignatureBitset<type_list_impl<T, U>, S> {
		static inline void execute(S* SignatureBitsets){
			//SignatureBitsets->initializeBitsets<T>();
			initializeSignatureBitset<U, S>::execute(SignatureBitsets);
		}
	};
	template <typename S>
	struct initializeSignatureBitset<type_list_null, S> {
		static inline void execute(S* SignatureBitsets) {}
	};

	// Iterate through signature's components and set bitsets
	template <typename T, typename B, template<typename> typename Bit>
	struct initializeBitset;
	template <typename T, typename U, typename B, template<typename> typename Bit>
	struct initializeBitset<type_list_impl<T, U>, B, Bit> {
		static inline void execute(B& bitset) {
			bitset[Bit<T>()] = true;
			initializeBitset<U, B, Bit>::execute(bitset);
		}
	};
	template <typename B, template<typename> typename Bit>
	struct initializeBitset<type_list_null, B, Bit> {
		static inline void execute(B& bitset) {
			
		}
	};
};

#endif
