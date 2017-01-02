#pragma once

namespace my_type_traits_impl {
	template<typename T, typename = nullptr_t>
	struct has_iterator_impl : std::false_type {};
	template<typename T>
	struct has_iterator_impl < T, std::enable_if_t<dxle::ignore_type<
		typename std::iterator_traits<T>::iterator_category
	>::value, nullptr_t> >
		: std::true_type {};
}
template<typename T>
struct has_iterator : my_type_traits_impl::has_iterator_impl <T>{};
