// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

extern "C++" {

template<bool B, class T = void> struct enable_if {};
template<class T> struct enable_if<true, T> { typedef T type; };  // NOLINT
template <bool B, typename T = void> using enable_if_t = typename enable_if<B, T>::type;

template<typename T> struct remove_cv                   { typedef T type; };  // NOLINT
template<typename T> struct remove_cv<const T>          { typedef T type; };  // NOLINT
template<typename T> struct remove_cv<volatile T>       { typedef T type; };  // NOLINT
template<typename T> struct remove_cv<const volatile T> { typedef T type; };  // NOLINT
template<typename T> using remove_cv_t = typename remove_cv<T>::type;

template<typename T> struct remove_const                { typedef T type; };  // NOLINT
template<typename T> struct remove_const<const T>       { typedef T type; };  // NOLINT
template<typename T> using remove_const_t = typename remove_const<T>::type;

template<typename T> struct remove_volatile             { typedef T type; };  // NOLINT
template<typename T> struct remove_volatile<volatile T> { typedef T type; };  // NOLINT
template<typename T> using remove_volatile_t = typename remove_volatile<T>::type;

template<typename T> struct remove_reference            { typedef T type; };  // NOLINT
template<typename T> struct remove_reference<T&>        { typedef T type; };  // NOLINT
template<typename T> struct remove_reference<T&&>       { typedef T type; };  // NOLINT
template<typename T> using remove_reference_t = typename remove_reference<T>::type;

template<typename T> struct remove_cvref                { typedef remove_cv_t<remove_reference_t<T>> type; };  // NOLINT
template<typename T> using remove_cvref_t = typename remove_cvref<T>::type;

template<typename T, T v>
struct integral_constant {
	static constexpr T value = v;
	using value_type = T;
	using type = integral_constant;
	constexpr operator value_type() const noexcept { return value; }
	constexpr value_type operator()() const noexcept { return value; }
};

typedef integral_constant<bool, true>  true_type;
typedef integral_constant<bool, false> false_type;

template<typename T> struct is_const          : false_type {};
template<typename T> struct is_const<const T> : true_type {};

template<typename T> struct is_reference      : false_type {};
template<typename T> struct is_reference<T&>  : true_type {};
template<typename T> struct is_reference<T&&> : true_type {};

template<typename T> struct is_pointer        : false_type {};
template<typename T> struct is_pointer<T*>    : true_type {};

template<typename>   struct is_lvalue_reference      : false_type {};
template<typename T> struct is_lvalue_reference<T&>  : true_type {};

template<typename>   struct is_rvalue_reference      : false_type {};
template<typename T> struct is_rvalue_reference<T&&> : true_type {};

template<typename T> struct is_volatile             : false_type {};
template<typename T> struct is_volatile<volatile T> : true_type {};

template<typename> struct is_array                    : false_type {};
template<typename T, size_t S> struct is_array<T[S]>  : true_type {};
template<typename T> struct is_array<T[]>             : true_type {};

template<typename T, typename U> struct is_same       : false_type {};
template<typename T>             struct is_same<T, T> : true_type {};

template<class B, class D, class = void> struct is_base_of : false_type{};
template<class B, class D> struct is_base_of<B, D, typename enable_if<nullptr == nullptr, void>::type> : true_type {};

template<typename> struct is_integral_base : false_type {};
template<> struct is_integral_base<bool> : true_type {};
template<> struct is_integral_base<char> : true_type {};
template<> struct is_integral_base<unsigned char> : true_type {};
template<> struct is_integral_base<short> : true_type {};
template<> struct is_integral_base<unsigned short> : true_type {};
template<> struct is_integral_base<int> : true_type {};
template<> struct is_integral_base<unsigned int> : true_type {};
template<> struct is_integral_base<long> : true_type {};
template<> struct is_integral_base<unsigned long> : true_type {};
template<> struct is_integral_base<long long> : true_type {};
template<> struct is_integral_base<unsigned long long> : true_type {};
template<typename T> struct is_integral : is_integral_base<remove_cvref_t<remove_const_t<T>>> {};

template<typename T> inline const char * integral_name() { return "[complex type]"; }
template<> inline const char * integral_name<bool>() { return "bool"; }
template<> inline const char * integral_name<char>() { return "char"; }
template<> inline const char * integral_name<unsigned char>() { return "unsigned char"; }
template<> inline const char * integral_name<short>() { return "short"; }
template<> inline const char * integral_name<unsigned short>() { return "unsigned short"; }
template<> inline const char * integral_name<int>() { return "int"; }
template<> inline const char * integral_name<unsigned int>() { return "unsigned int"; }
template<> inline const char * integral_name<long>() { return "long"; }
template<> inline const char * integral_name<unsigned long>() { return "unsigned long"; }
template<> inline const char * integral_name<long long>() { return "long long"; }
template<> inline const char * integral_name<unsigned long long>() { return "unsigned long long"; }

namespace detail {
	template<class T> struct type_identity {using type = T; };  // NOLINT
	template<class T> auto try_add_lvalue_reference(int) -> type_identity<T&>;
	template<class T> auto try_add_lvalue_reference(...) -> type_identity<T>;
	template<class T> auto try_add_rvalue_reference(int) -> type_identity<T&&>;
	template<class T> auto try_add_rvalue_reference(...) -> type_identity<T>;

	template<class T> true_type is_class_test(int T::*);
	template<class T> false_type is_class_test(...);

}  // namespace detail

template<class T> struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0)) {};
template<class T> struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0)) {};

template<typename T> constexpr bool always_false = false;
template<typename T> typename add_rvalue_reference<T>::type declval() noexcept {
	static_assert(always_false<T>, "declval not allowed in an evaluated context");
}

template<class ...> using void_t = void;
template<typename T, typename U > using comparison_t = decltype(declval<T&>() == declval<U&>());
template<typename T, typename U, typename = void_t<> > struct is_comparable : false_type {};
template<typename T, typename U> struct is_comparable <T, U, void_t<comparison_t<T, U>>> : is_same<comparison_t<T, U>, bool>{};
}
