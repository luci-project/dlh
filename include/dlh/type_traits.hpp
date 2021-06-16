#pragma once

extern "C++" {

template<class T> struct remove_cv                   { typedef T type; };
template<class T> struct remove_cv<const T>          { typedef T type; };
template<class T> struct remove_cv<volatile T>       { typedef T type; };
template<class T> struct remove_cv<const volatile T> { typedef T type; };

template<class T> struct remove_const                { typedef T type; };
template<class T> struct remove_const<const T>       { typedef T type; };

template<class T> struct remove_volatile             { typedef T type; };
template<class T> struct remove_volatile<volatile T> { typedef T type; };

template<class T> struct remove_reference            { typedef T type; };
template<class T> struct remove_reference<T&>        { typedef T type; };
template<class T> struct remove_reference<T&&>       { typedef T type; };

template<class T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

typedef integral_constant<bool, true>  true_type;
typedef integral_constant<bool, false> false_type;

template <class T> struct is_reference      : false_type {};
template <class T> struct is_reference<T&>  : true_type {};
template <class T> struct is_reference<T&&> : true_type {};

template<class T, class U> struct is_same       : false_type {};
 template<class T>         struct is_same<T, T> : true_type {};


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

}
