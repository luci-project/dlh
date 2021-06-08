#pragma once

// Taken from http://bajamircea.github.io/coding/cpp/2016/04/07/move-forward.html

template<typename T> struct remove_reference { typedef T type; };
template<typename T> struct remove_reference<T&> { typedef T type; };
template<typename T> struct remove_reference<T&&> { typedef T type; };

template<typename T>
constexpr typename remove_reference<T>::type && move(T && arg) noexcept {
  return static_cast<typename remove_reference<T>::type &&>(arg);
}


template<typename T> struct is_lvalue_reference { static constexpr bool value = false; };
template<typename T> struct is_lvalue_reference<T&> { static constexpr bool value = true; };

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type & arg) noexcept {
  return static_cast<T&&>(arg);
}

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type && arg) noexcept {
  static_assert(!is_lvalue_reference<T>::value, "invalid rvalue to lvalue conversion");
  return static_cast<T&&>(arg);
}
    template<typename T>
    class initializer_list
    {
    public:
        using value_type = T;
        using reference = const T&;
        using const_reference = const T&;
        using size_type = size_t;
        using iterator = const T*;
        using const_iterator = const T*;

    private:
        iterator  m_array;
        size_type m_len;

        // The compiler can call a private constructor.
        constexpr initializer_list(const_iterator itr, size_type st)
            : m_array(itr), m_len(st) { }

    public:
        constexpr initializer_list() noexcept : m_array(0), m_len(0) { }

        // Number of elements.
        constexpr size_type size() const noexcept { return m_len; }

        // First element.
        constexpr const_iterator begin() const noexcept { return m_array; }

        // One past the last element.
        constexpr const_iterator end() const noexcept { return begin() + size(); }
    };

/*
template<class T>
class initializer_list {
	const T* _begin;
	size_t    _size;

	inline constexpr initializer_list(const T* begin, size_t size) : _begin(begin), _size(size) {}

 public:
	typedef T        value_type;
	typedef const T& reference;
	typedef const T& const_reference;
	typedef size_t   size_type;

	typedef const T* iterator;
	typedef const T* const_iterator;

	inline constexpr initializer_list() noexcept : _begin(nullptr), _size(0) {}

	inline constexpr size_t size() const {
		return _size;
	}

	inline constexpr const T* begin() const {
		return _begin;
	}

	inline constexpr const T* end() const {
		return _begin + _size;
	}
};

template<class T>
inline constexpr const T* begin(initializer_list<T> i) {
	return i.begin();
}

template<class T>
inline constexpr const T* end(initializer_list<T> i) {
	return i.end();
}
*/
