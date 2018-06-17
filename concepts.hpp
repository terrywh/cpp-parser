#ifndef PARSER_CONCEPTS_H
#define PARSER_CONCEPTS_H

#include <utility>
#include <cctype>

namespace parser {
namespace concepts {
	template <class T>
	struct value_type {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().push_back('c'), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};

	template <class T, class V>
	struct container_type_1 {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().insert(std::move(std::declval<V>())), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};
	template <class T, class V>
	struct container_type_2 {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().push_back(std::move(std::declval<V>())), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};
	
}
}

#endif // PARSER_CONCEPTS_H
