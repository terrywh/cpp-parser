#ifndef PARSER_CONCEPTS_H
#define PARSER_CONCEPTS_H

#include <utility>
#include <cctype>

namespace parser {
namespace concepts {

	template <class T>
	struct has_size {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().size(), std::size_t());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::size_t>::value;
	};

	template <class T>
	struct is_char_pushable {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().push_back('c'), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};

	template <class T, class V>
	struct is_entry_insertable {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().insert(std::move(std::declval<V>())), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};
	template <class T, class V>
	struct is_entry_pushable {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().push_back(std::move(std::declval<V>())), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};
	
}
}

#endif // PARSER_CONCEPTS_H
