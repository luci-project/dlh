#pragma once

extern "C++" {

template<typename T> struct remove_cv                   { typedef T type; };
template<typename T> struct remove_cv<const T>          { typedef T type; };
template<typename T> struct remove_cv<volatile T>       { typedef T type; };
template<typename T> struct remove_cv<const volatile T> { typedef T type; };

template<typename T> struct remove_const                { typedef T type; };
template<typename T> struct remove_const<const T>       { typedef T type; };

template<typename T> struct remove_volatile             { typedef T type; };
template<typename T> struct remove_volatile<volatile T> { typedef T type; };

template<typename T> struct remove_reference            { typedef T type; };
template<typename T> struct remove_reference<T&>        { typedef T type; };
template<typename T> struct remove_reference<T&&>       { typedef T type; };

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

template<typename>   struct is_lvalue_reference      : false_type {};
template<typename T> struct is_lvalue_reference<T&>  : true_type {};

template<typename>   struct is_rvalue_reference      : false_type {};
template<typename T> struct is_rvalue_reference<T&&> : true_type {};

template<typename T> struct is_volatile             : false_type {};
template<typename T> struct is_volatile<volatile T> : true_type {};

template<typename> struct is_array                    : false_type { };
template<typename T, size_t S> struct is_array<T[S]>  : true_type { };
template<typename T> struct is_array<T[]>             : true_type { };

template<typename T, typename U> struct is_same       : false_type {};
template<typename T>             struct is_same<T, T> : true_type {};


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
template<typename T> struct is_integral : is_integral_base<remove_cv<T>> {};

template<typename T> inline const char * integral_name() { return "[complex type]"; };
template<> inline const char * integral_name<bool>() { return "bool"; };
template<> inline const char * integral_name<char>() { return "char"; };
template<> inline const char * integral_name<unsigned char>() { return "unsigned char"; };
template<> inline const char * integral_name<short>() { return "short"; };
template<> inline const char * integral_name<unsigned short>() { return "unsigned short"; };
template<> inline const char * integral_name<int>() { return "int"; };
template<> inline const char * integral_name<unsigned int>() { return "unsigned int"; };
template<> inline const char * integral_name<long>() { return "long"; };
template<> inline const char * integral_name<unsigned long>() { return "unsigned long"; };
template<> inline const char * integral_name<long long>() { return "long long"; };
template<> inline const char * integral_name<unsigned long long>() { return "unsigned long long"; };

}
