/*! \file
 *  \brief vector class
 */
#pragma once

#include <dlh/alloc.hpp>
#include <dlh/assert.hpp>
#include <dlh/types.hpp>
#include <dlh/utility.hpp>
#include <dlh/container/optional.hpp>
#include <dlh/container/initializer_list.hpp>
#include <dlh/stream/buffer.hpp>

/*! \brief Vector class influenced by standard [template/cxx] library
 */
template<class T> class Vector {
	/*! \brief Array with entries
	 * dynamically allocated / increased
	 */
	T* _element = nullptr;

	/*! \brief Number of entries
	 */
	int32_t _size = 0;

	/*! \brief Current maximum capacity
	 */
	int32_t _capacity = 0;

	/*! \brief Expand capacity
	 */
	inline void expand() {
		reserve(0 < _capacity ? 2 * _capacity : 16);
	}

	/*! \brief base vector iterator
	 */
	struct BaseIterator {
		friend class Vector<T>;
		const Vector<T> &ref;
		mutable int32_t i;

		BaseIterator(const Vector<T> &ref, int32_t i) : ref(ref), i(i) {}

		inline void next() const {
			i++;
		}

		inline void prev() const {
			i--;
		}

		inline const T& operator*() const {
			assert(i >= 0 && i < ref._size);
			return ref._element[i];
		}

		inline const T* operator->() const {
			assert(i >= 0 && i < ref._size);
			return ref._element + i;
		}

		inline bool operator==(BaseIterator& other) const {
			return &ref == &other.ref && i == other.i;
		}

		inline bool operator!=(BaseIterator& other) const {
			return &ref != &other.ref || i != other.i;
		}

		inline operator bool() const {
			return i >= 0 && i < ref._size;
		}
	};

 public:
	/*! \brief Constructor
	 */
	constexpr Vector() {}

	/*! \brief Constructor with initial _capacity
	 * \param capacity number of initial values
	 * \param init initial values
	 */
	explicit Vector(size_t capacity) : _size(0), _capacity(static_cast<int32_t>(capacity)) {
		if (capacity > 0) {
			assert(capacity <= INT32_MAX);
			_element = reinterpret_cast<T*>(malloc(capacity * sizeof(T)));
			assert(_element != nullptr);
		}
	}

	/*! \brief Range constructor
	 * \param first First element in range
	 * \param last Last element in range
	 */
	template<typename I>
	explicit Vector(I first, I last) {
		for (I i = first; i < last; ++i)
			emplace_back(*i);
	}


	/*! \brief Initializer list constructor
	 * \param flist initializer list
	 */
	template<typename O>
	Vector(const std::initializer_list<O> & list) : Vector(list.size()) {
		for (const auto & arg : list)
			emplace_back(arg);
	}

	/*! \brief Copy constructor
	 * \param other other vector
	 */
	template<typename O>
	Vector(const Vector<O> & other) {
		for (auto & e : other)
			emplace_back(e);
	}

	/*! \brief Copy constructor
	 * \param other other vector
	 */
	Vector(const Vector<T> & other) {
		for (auto & e : other)
			emplace_back(e);
	}

	/*! \brief Move constructor
	 * \param other other vector
	 */
	Vector(Vector<T>&& other) {
		_element = other._element;
		other._element = nullptr;
		_size = other._size;
		other._size = 0;
		_capacity = other._capacity;
		other._capacity = 0;
	}


	/*! \brief Destructor
	 */
	~Vector() {
		resize(0);
		free(_element);
	}

	/*! \brief Vector iterator
	 */
	class Iterator  : public BaseIterator {
		friend class Vector<T>;
		Iterator(Vector<T> &ref, int32_t p) : BaseIterator(ref, p) {}

	public:
		using BaseIterator::operator*;
		using BaseIterator::operator->;
		using BaseIterator::operator==;
		using BaseIterator::operator!=;
		using BaseIterator::operator bool;

		Iterator& operator++() {
			BaseIterator::next();
			return *this;
		}

		inline T& operator*() {
			return const_cast<T&>(BaseIterator::operator*());
		}

		inline T* operator->() {
			return const_cast<T*>(BaseIterator::operator->());
		}
	};

	/*! \brief Constant Vector iterator
	 */
	class ConstIterator  : public BaseIterator {
		friend class Vector<T>;
		ConstIterator(const Vector<T> &ref, int32_t p) : BaseIterator(ref, p) {}

	public:
		using BaseIterator::operator*;
		using BaseIterator::operator->;
		using BaseIterator::operator==;
		using BaseIterator::operator!=;
		using BaseIterator::operator bool;

		ConstIterator& operator++() {
			BaseIterator::next();
			return *this;
		}
	};

	/*! \brief Vector iterator
	 */
	class ReverseIterator  : public BaseIterator {
		friend class Vector<T>;
		ReverseIterator(Vector<T> &ref, int32_t p) : BaseIterator(ref, p) {}

	public:
		using BaseIterator::operator*;
		using BaseIterator::operator->;
		using BaseIterator::operator==;
		using BaseIterator::operator!=;
		using BaseIterator::operator bool;

		ReverseIterator& operator++() {
			BaseIterator::prev();
			return *this;
		}

		inline T& operator*() {
			return const_cast<T&>(BaseIterator::operator*());
		}

		inline T* operator->() {
			return const_cast<T*>(BaseIterator::operator->());
		}
	};

	/*! \brief Constant Vector iterator
	 */
	class ConstReverseIterator  : public BaseIterator {
		friend class Vector<T>;
		ConstReverseIterator(const Vector<T> &ref, int32_t p) : BaseIterator(ref, p) {}

	public:
		using BaseIterator::operator*;
		using BaseIterator::operator->;
		using BaseIterator::operator==;
		using BaseIterator::operator!=;
		using BaseIterator::operator bool;

		ConstReverseIterator& operator++() {
			BaseIterator::prev();
			return *this;
		}
	};

	inline Iterator begin() {
		return { *this, 0 };
	}

	inline ConstIterator begin() const {
		return { *this, 0 };
	}

	inline Iterator end() {
		return { *this, _size };
	}

	inline ConstIterator end() const {
		return { *this, _size };
	}

	inline ReverseIterator rbegin() {
		return { *this, _size - 1};
	}

	inline ConstReverseIterator rbegin() const {
		return { *this, _size - 1};
	}

	inline ReverseIterator rend() {
		return { *this, -1 };
	}

	inline ConstReverseIterator rend() const {
		return { *this, -1 };
	}

	/*! \brief access specified element
	 * \param pos position of the element to return
	 * \return element or T() if out of bounds
	 * \note This function differs to the standard library due to the lack of exceptions
	 */
	inline Optional<T> at(int32_t i) {
		if (i >= 0 && i < _size)
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

	/*! \brief Access the first element
	 * \return first element
	 */
	inline const T& front() const {
		return operator[](0);
	}

	/*! \brief Access the last element
	 * \return last element
	 */
	inline T& back() {
		return operator[](_size - 1);
	}

	/*! \brief Access the last element
	 * \return last element
	 */
	inline const T& back() const {
		return operator[](_size - 1);
	}

	/*! \brief direct access to the underlying array
	 * \return Pointer to data
	 */
	inline T* data() {
		return _element;
	}

	/*! \brief direct access to the underlying array
	 * \return Pointer to data
	 */
	inline const T* data() const {
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
		return static_cast<size_t>(_size);
	}

	/*! \brief Get the number of elements that can be held in currently allocated storage
	 * \return current capacity
	 */
	inline size_t capacity() const {
		return static_cast<size_t>(_capacity);
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
		int32_t c = capacity < INT32_MAX ? static_cast<int32_t>(capacity) : (INT32_MAX - 1);
		if (c > _capacity) {
			_element = reinterpret_cast<T*>(realloc(reinterpret_cast<void *>(_element), c * sizeof(T)));
			_capacity = c;
		}
	}

	/*! \brief Resizes the container
	 * \param count new size of the container
	 */
	inline void resize(size_t count) {
		int32_t c = count < INT32_MAX ? static_cast<int32_t>(count) : (INT32_MAX - 1);

		if (c < _size) {
			// Free items
			for (int32_t i = c; i < _size; ++i)
				_element[i].~T();
			_size = c;
		}
		reserve(count);
	}

	/*! \brief Creats element at the specified location
	 * \param pos position
	 * \param args Arguments to create the value
	 */
	template<typename... ARGS>
	Iterator emplace(int32_t pos, ARGS&&... args) {
		if (pos >= 0 && pos <= _size) {
			if (pos == _capacity)
				expand();

			for (int32_t i = _size; i > pos; --i) {
				new (_element + i) T(move(_element[i - 1]));
				_element[i - 1].~T();
			}

			new (_element + pos) T(forward<ARGS>(args)...);
			_size++;
			return { *this, pos };
		} else {
			return { *this, _size };
		}
	}

	/*! \brief Creates an element at the end
	 * \param args Arguments to create the value
	 */
	template<typename... ARGS>
	Iterator emplace_back(ARGS&&... args) {
		if (_capacity == _size)
			expand();

		new (_element + _size) T(forward<ARGS>(args)...);
		_size++;
		return { *this, _size};
	}

	/*! \brief Adds an element to the end
	 * \param value the value of the element to append
	 */
	inline Iterator push_back(const T& value) {
		return emplace_back(value);
	}

	/*! \brief Adds an element to the end
	 * \param value the value of the element to append
	 */
	inline Iterator push_back(T&& value) {
		return emplace_back(forward<T>(value));
	}

	/*! \brief Creates an element at the front
	 * \param args Arguments to create the value
	 */
	template<typename... ARGS>
	inline Iterator emplace_front(ARGS&&... args) {
		return emplace(0, forward<ARGS>(args)...);
	}

	/*! \brief Adds an element to the front
	 * \param value the value of the element to append
	 */
	inline Iterator push_front(const T& value) {
		return emplace(0, value);
	}

	/*! \brief Adds an element to the front
	 * \param value the value of the element to append
	 */
	inline Iterator push_front(T&& value) {
		return emplace(0, forward<T>(value));
	}

	/*! \brief Creats element at the specified location
	 * \param pos position
	 * \param args Arguments to create the value
	 */
	template<typename... ARGS>
	inline Iterator emplace(size_t pos, ARGS&&... args) {
		if (pos < INT32_MAX)
			return emplace(static_cast<int32_t>(pos), forward<ARGS>(args)...);
		else
			return { *this, _size };
	}

	/*! \brief Inserts element at the specified location
	 * \param pos position
	 * \param value value
	 */
	inline Iterator insert(size_t pos, const T& value) {
		return emplace(pos, value);
	}

	/*! \brief Inserts element at the specified location
	 * \param pos position
	 * \param value value
	 */
	inline Iterator insert(size_t pos, T&& value) {
		return emplace(pos, forward<T>(value));
	}

	/*! \brief Remove the last element
	 * \return last element
	 */
	inline Optional<T> pop_back() {
		return _size > 0 ? Optional<T>(_element[--_size]) : Optional<T>();
	}

	/*! \brief Remove the first element
	 * \return first element
	 */
	inline Optional<T> pop_front() {
		return remove(0);
	}

	/*! \brief Remove element at the specified location
	 * \param pos position
	 * \return value (if available)
	 */
	Optional<T> remove(int32_t pos) {
		if (pos >= 0 && pos < _size) {
			auto r = Optional<T>(_element[pos]);
			for (int32_t i = pos; i < _size - 1; i++) {
				new (_element + i) T(move(_element[i + 1]));
				_element[i + 1].~T();
			}
			_size--;
			return r;
		} else {
			return Optional<T>();
		}
	}

	/*! \brief Remove element at the specified location
	 * \param pos position
	 * \return value (if available)
	 */
	inline Optional<T> remove(size_t pos) {
		if (pos <= INT32_MAX)
			return remove(static_cast<int32_t>(pos));
		else
			return Optional<T>();
	}
	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	inline Optional<T> remove(const BaseIterator & position) {
		return remove(position.i);
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	inline Optional<T> remove(const Iterator & position) {
		return remove(position.i);
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	inline Optional<T> remove(const ReverseIterator & position) {
		return remove(position.i);
	}


	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	inline Iterator erase(int32_t pos) {
		return { *this, erase_helper(pos) };
	}

	/*! \brief Remove element at the specified location
	 * \param pos position
	 * \return Iterator (to the next element)
	 */
	inline Iterator erase(size_t pos) {
		return { *this, pos <= INT32_MAX ? erase_helper(static_cast<int32_t>(pos)) : _size };
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	inline Iterator erase(const Iterator & position) {
		return { *this, erase_helper(position.i) };
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	inline ReverseIterator erase(const ReverseIterator & position) {
		return { *this, erase_helper(position.i) };
	}

	/*! \brief Access Element
	 * \note undefined behaviour if index is out of bounds
	 * \param i index
	 */
	inline T & operator[](int32_t i) {
		assert(i >= 0 && i < _size);
		return _element[i];
	}

	/*! \brief Access Element
	 * \note undefined behaviour if index is out of bounds
	 * \param i index
	 */
	inline const T & operator[](int32_t i) const {
		assert(i >= 0 && i < _size);
		return _element[i];
	}

	/*! \brief Copy Assignment
	 * \param other
	 */
	template<typename O>
	Vector<T>& operator=(const Vector<O>& other) {
		// No self assignment
		if (this != &other) {
			resize(0);
			for (auto & e : other)
				emplace_back(e);
		}
		return *this;
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
	Vector<T>& operator=(Vector<T>&& other) {
		resize(0);
		free(_element);
		_element = other._element;
		other._element = nullptr;
		_size = other._size;
		other._size = 0;
		_capacity = other._capacity;
		other._capacity = 0;
		return *this;
	}

	/*! \brief Concatenate vector
	 * \param other vector
	 * \return reference to vector
	 */
	template<typename O>
	Vector<T> & operator+=(const Vector<O>& other) {
		reserve(_size + other._size);
		for (int32_t i = 0; i < other._size; ++i)
			emplace_back(other._element[i]);

		return *this;
	}

	/*! \brief Concatenate element
	 * \param element element
	 * \return reference to vector
	 */
	template<typename O>
	Vector<T> & operator+=(const O& element) {
		emplace_back(element);
		return *this;
	}

	/*! \brief Concatenate two vectors
	 * \param other vector
	 * \return new vector with all elements of this and other vector
	 */
	template<typename O>
	Vector<T> operator+(const Vector<O>& other) const {
		Vector<T> r = *this;
		r += other;
		return r;
	}

	/*! \brief Concatenate element
	 * \param element element to Concatenate
	 * \return new vector with all elements of this vector and element
	 */
	template<typename O>
	Vector<T> operator+(const O& element) const {
		Vector<T> r = *this;
		r += element;
		return r;
	}

 private:
	/*! \brief Remove element at the specified location
	 * \param pos position
	 * \return Iterator (to the next element)
	 */
	int32_t erase_helper(int32_t pos) {
		if (pos >= 0 && pos < _size) {
			_element[pos].~T();
			for (int32_t i = pos; i < _size - 1; i++) {
				new (_element + i) T(move(_element[i + 1]));
				_element[i + 1].~T();
			}
			_size--;
			return pos;
		} else {
			return _size;
		}
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
