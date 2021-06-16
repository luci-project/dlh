/*! \file
 *  \brief vector class
 */
#pragma once

#include <dlh/alloc.hpp>
#include <dlh/assert.hpp>
#include <dlh/types.hpp>
#include <dlh/utility.hpp>
#include <dlh/container/optional.hpp>
#include <dlh/stream/buffer.hpp>

/*! \brief Vector class influenced by standard [template/cxx] library
 */
template<class T> class Vector {
	/*! \brief Array with entries
	 * dynamically allocated / increased
	 */
	T* _element;

	/*! \brief Number of entries
	 */
	size_t _size;

	/*! \brief Current maximum capacity
	 */
	size_t _capacity;

	/*! \brief Expand capacity
	 */
	inline void expand() {
		reserve(0 < _capacity ? 2 * _capacity : 16);
	}

 public:
	Vector(const Vector&) = default;
	Vector(Vector&&) = default;

	/*! \brief Constructor
	 */
	Vector() : _element(nullptr), _size(0), _capacity(0) {}

	/*! \brief Constructor with initial _capacity
	 * \param capacity number of initial values
	 * \param init initial values
	 * \param invalid object for invalid returns
	 */
	explicit Vector(size_t capacity) : _element(nullptr), _size(capacity), _capacity(capacity) {
		if (capacity > 0) {
			_element = reinterpret_cast<T*>(malloc(capacity * sizeof(T)));
			assert(_element != nullptr);
		}
	}

	/*! \brief Range constructor
	 * \param first First element in range
	 * \param last Last element in range
	 * \param invalid object for invalid returns
	 */
	template<typename I>
	explicit Vector(I first, I last) : Vector(0) {
		for (I i = first; i < last; ++i)
			emplace_back(*i);
	}

	/*! \brief Copy constructor
	 * \param other other vector
	 */
	template<typename I>
	explicit Vector(const Vector & other) : Vector(other.begin(), other.end()) {}


	/*! \brief Destructor
	 */
	~Vector() {
		resize(0);
		free(_element);
	}

	/*! \brief access specified element
	 * \param pos position of the element to return
	 * \return element or T() if out of bounds
	 * \note This function differs to the standard library due to the lack of exceptions
	 */
	inline Optional<T> at(int i) {
		if (i < _size)
			return { _element[i] };
		else
			return {};
	}

	/*! \brief Access the first element
	 * \return first element
	 */
	inline T& front() {
		return operator[](0);
	}

	/*! \brief Access the last element
	 * \return last element
	 */
	inline T& back() {
		return operator[](_size - 1);
	}

	/*! \brief direct access to the underlying array
	 * \return Pointer to data
	 */
	inline T* data() {
		return _element;
	}

	/*! \brief Checks if the container has no element
	 */
	inline bool empty() const {
		return _size == 0;
	}

	/*! \brief Get the number of elements
	 * \return number of entires
	 */
	inline size_t size() const {
		return _size;
	}

	/*! \brief Get the number of elements that can be held in currently allocated storage
	 * \return current capacity
	 */
	inline size_t capacity() const {
		return _capacity;
	}

	/*! \brief Erases all elements from the container
	 */
	inline void clear() {
		_size = 0;
	}

	/*! \brief Increase the capacity of the vector
	 * \param capacity new capacity
	 */
	inline void reserve(size_t capacity) {
		if (capacity > this->_capacity) {
			_element = reinterpret_cast<T*>(realloc(reinterpret_cast<void *>(_element), capacity * sizeof(T)));
			this->_capacity = capacity;
		}
	}

	/*! \brief Resizes the container
	 * \param count new size of the container
	 */
	inline void resize(size_t count) {
		if (count < _size) {
			// Free items (
			for (size_t i = count; i < _size; ++i)
				_element[i].~T();
			_size = count;
		}
		reserve(count);
	}

	/*! \brief Creates an element at the end
	 * \param args Arguments to create the value
	 */
	template<typename... ARGS>
	inline void emplace_back(ARGS&&... args) {
		if (_capacity == _size)
			expand();

		new (_element + _size) T(forward<ARGS>(args)...);
		_size++;
	}

	/*! \brief Adds an element to the end
	 * \param value the value of the element to append
	 */
	inline void push_back(const T& value) {
		emplace_back(value);
	}

	/*! \brief Adds an element to the end
	 * \param value the value of the element to append
	 */
	inline void push_back(T&& value) {
		emplace_back(forward<T>(value));
	}

	/*! \brief Creats element at the specified location
	 * \param pos position
	 * \param args Arguments to create the value
	 */
	template<typename... ARGS>
	inline void emplace(size_t pos, ARGS&&... args) {
		if (pos == _capacity)
			expand();

		if (pos <= _size) {
			for (size_t i = _size; i > pos; --i)
				new (_element + i) T(move(_element[i - 1]));

			new (_element + pos) T(forward<ARGS>(args)...);
			_size++;
		}
	}

	/*! \brief Inserts element at the specified location
	 * \param pos position
	 * \param value value
	 */
	inline void insert(size_t pos, const T& value) {
		emplace(pos, value);
	}

	/*! \brief Inserts element at the specified location
	 * \param pos position
	 * \param value value
	 */
	inline void insert(size_t pos, T&& value) {
		emplace(pos, forward<T>(value));
	}

	/*! \brief Remove the last element
	 * \return last element
	 */
	inline Optional<T> pop_back() {
		return 0 == _size ? Optional<T>() : Optional<T>(_element[--_size]);
	}

	/*! \brief Remove element at the specified location
	 * \param pos position
	 * \return value (if available)
	 */
	inline Optional<T> remove(size_t pos) {
		if (pos < _size) {
			auto r = Optional<T>(_element[pos]);
			for (size_t i = pos; i < _size - 1; i++)
				new (_element + i) T(move(_element[i - 1]));

			_size--;
			return r;
		} else {
			return Optional<T>();
		}
	}

	/*! \brief Access Element
	 * \note undefined behaviour if index is out of bounds
	 * \param i index
	 */
	inline T & operator[](int i) {
		assert(i >= 0 && static_cast<size_t>(i) < _size);
		return _element[i];
	}

	/*! \brief Copy Assignment
	 * \param other
	 */
	Vector<T>& operator=(const Vector<T>& other) {
		// No self assignment
		if (this != &other) {
			resize(0);
			for (auto & e : other)
				emplace_back(e);
		}
		return *this;
	}

	/*! \brief Move Assignment
	 * \param other
	 */
	Vector<T>& operator=(Vector<T>&& other) = default;

	/*! \brief Concatenate vector
	 * \param other vector
	 * \return reference to vector
	 */
	Vector<T> & operator+=(const Vector<T>& other) {
		reserve(_size + other._size);
		for (size_t i = 0; i < other._size; ++i)
			push_back(i);

		return *this;
	}

	/*! \brief Concatenate element
	 * \param element element
	 * \return reference to vector
	 */
	Vector<T> & operator+=(const T& element) {
		push_back(element);
		return *this;
	}

	/*! \brief Concatenate two vectors
	 * \param other vector
	 * \return new vector with all elements of this and other vector
	 */
	Vector<T> operator+(const Vector<T>& other) const {
		Vector<T> r = *this;
		r += other;
		return r;
	}

	/*! \brief Concatenate element
	 * \param element element to Concatenate
	 * \return new vector with all elements of this vector and element
	 */
	Vector<T> operator+(const T& element) const {
		Vector<T> r = *this;
		r += element;
		return r;
	}

	/*! \brief Vector iterator
	 */
	class Iterator {
		T* i;

	 public:  //NOLINT
		explicit Iterator(T* p) : i(p) {}

		Iterator& operator++() {
			i++;
			return *this;
		}

		Iterator& operator--() {
			i--;
			return *this;
		}

		T& operator*() {
			return *i;
		}

		bool operator==(const Vector<T>::Iterator& other) const {
			return i == other.i;
		}

		bool operator!=(const Vector<T>::Iterator& other) const {
			return i != other.i;
		}
	};

	inline Vector<T>::Iterator begin() const {
		return Vector<T>::Iterator(_element);
	}

	inline Vector<T>::Iterator end() const {
		return Vector<T>::Iterator(_element + _size);
	}
};


/*! \brief Print contents of a vector
 *
 *  \param bs Target Bufferstream
 *  \param val Vector to be printed
 *  \return Reference to BufferStream os; allows operator chaining.
 */
template<typename T>
static inline BufferStream& operator<<(BufferStream& bs, Vector<T> & val) {
	bs << "{ ";
	bool p = false;
	for (const auto & v : val) {
		if (p)
			bs << ", ";
		p = true;
		bs << v;
	}
	return bs << '}';
}
