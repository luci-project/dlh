#pragma once

#include <dlh/assert.hpp>
#include <dlh/utility.hpp>

/*! \brief Linked list
 * influenced by standard [template/cxx] librarys `list`
 * \tparam T type for container
 */
template<typename T>
class List {
	struct Node {
		Node * _prev;
		Node * _next;

		T _data;

		Node(const T& value) : _prev(nullptr), _next(nullptr), _data(value) {}

		Node(T&& value) : _prev(nullptr), _next(nullptr), _data(value) {
			value.~T();
		}

		template <class... ARGS>
		explicit Node(const ARGS&... args) : _prev(nullptr), _next(nullptr), _data(args...) {}

		template <class... ARGS>
		explicit Node(ARGS&&... args) : _prev(nullptr), _next(nullptr), _data(forward<ARGS>(args)...) {}
	};

	Node * _head = nullptr;
	Node * _tail = nullptr;
	size_t _size = 0;

	/*! \brief base hash set iterator
	 */
	struct BaseIterator {
		friend class List<T>;
		mutable Node * i;

		BaseIterator(Node * node) : i(node) {}

		inline void next() const {
			assert(i != nullptr);
			i = i->_next;
		}

		inline void prev() const {
			assert(i != nullptr);
			i = i->_prev;
		}

		inline const T& operator*() const {
			assert(i != nullptr);
			return i->_data;
		}

		inline const T* operator->() const {
			assert(i != nullptr);
			return &(i->_data);
		}

		inline bool operator==(BaseIterator& other) const {
			return i == other.i;
		}

		inline bool operator==(const T& other) const {
			return i->_data == other;
		}

		inline bool operator!=(BaseIterator& other) const {
			return i != other.i;
		}

		inline bool operator!=(const T& other) const {
			return i->_data != other;
		}

		inline operator bool() const {
			return i != nullptr;
		}
	};

 public:
	/*! \brief Create new empty list
	 */
	explicit List() {}

	/*! \brief Range constructor
	 * \param begin First element in range
	 * \param end End of range
	 * \param initial_capacity space to reserve
	 */
	template<typename I>
	explicit List(const I & begin, const I & end) {
		for (I i = begin; i != end; ++i)
			emplace_back(*i);
	}

	/*! \brief Destructor
	 */
	virtual ~List() {
		clear();
	}

	/*! \brief binary search tree iterator
	 */
	class Iterator : public BaseIterator {
		friend class List<T>;
		Iterator(Node * p) : BaseIterator(p) {}

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

	/*! \brief constant binary search tree iterator
	 */
	class ConstIterator : public BaseIterator {
		friend class List<T>;
		ConstIterator(Node * p) : BaseIterator(p) {}

	 public:
		using BaseIterator::operator*;
		using BaseIterator::operator->;
		using BaseIterator::operator==;
		using BaseIterator::operator!=;
		using BaseIterator::operator bool;

		const ConstIterator& operator++() const {
			BaseIterator::next();
			return *this;
		}

	};

	/*! \brief Reverse binary search tree iterator
	 */
	class ReverseIterator : public BaseIterator {
		friend class List<T>;
		ReverseIterator(Node * p) : BaseIterator(p) {}

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

	/*! \brief constant binary search tree iterator
	 */
	class ConstReverseIterator : public BaseIterator {
		friend class List<T>;
		ConstReverseIterator(Node * p) : BaseIterator(p) {}

	 public:
		using BaseIterator::operator*;
		using BaseIterator::operator->;
		using BaseIterator::operator==;
		using BaseIterator::operator!=;
		using BaseIterator::operator bool;

		const ConstReverseIterator& operator++() const {
			BaseIterator::prev();
			return *this;
		}
	};

	/*! \brief Get iterator to first element
	 * \return iterator to first valid element (if available) or `end()`
	 */
	inline Iterator begin() {
		return { _head };
	}

	/*! \brief Get iterator to end (post-last-element)
	 * \return iterator to end (first invalid element)
	 */
	inline Iterator end() {
		return { nullptr };
	}

	/*! \brief Get iterator to first element
	 * \return iterator to first valid element (if available) or `end()`
	 */
	inline ConstIterator begin() const {
		return { _head };
	}

	/*! \brief Get iterator to end (post-last-element)
	 * \return iterator to end (first invalid element)
	 */
	inline ConstIterator end() const {
		return { nullptr };
	}

	/*! \brief Get iterator to last element
	 * \return iterator to last valid element (if available) or `rend()`
	 */
	inline ReverseIterator rbegin() {
		return { _tail };
	}

	/*! \brief Get reverse iterator to end (pre-first-element)
	 * \return iterator to end (first invalid element)
	 */
	inline ReverseIterator rend() {
		return { nullptr };
	}

	/*! \brief Get iterator to last element
	 * \return iterator to last valid element (if available) or `rend()`
	 */
	inline ConstReverseIterator rbegin() const {
		return { _tail };
	}

	/*! \brief Get reverse iterator to end (pre-first-element)
	 * \return iterator to end (first invalid element)
	 */
	inline ConstReverseIterator rend() const {
		return { nullptr };
	}

	/*! \brief Create new element before postition
	 * \param position iterator to the element where this element should be created
	 * \param args Arguments to construct element
	 * \return iterator to the new element
	 */
	template<typename... ARGS>
	Iterator emplace(const BaseIterator & position, ARGS&&... args) {
		if (position.i == nullptr)
			return emplace_back(forward<ARGS>(args)...);

		Node * node = new Node(forward<ARGS>(args)...);

		node->_next = position.i;
		if ((node->_prev = position.i->_prev) == nullptr) {
			assert(_head == position.i);
			_head = node;
		} else {
			node->_prev->_next = node;
		}
		position.i->_prev = node;

		assert((node->_next == nullptr && _tail == node) || (node->_next->_prev == node && _tail != node));
		assert((node->_prev == nullptr && _head == node) || (node->_prev->_next == node && _head != node));

		_size++;
		return { node };
	}

	/*! \brief Insert element before postition
	 * \param position iterator to the element where this element should be created
	 * \param value new element to be inserted
	 * \return iterator to the inserted element
	 */
	Iterator insert(const BaseIterator & position, const T & value) {
		return emplace(position, value);
	}

	/*! \brief Create new element at front
	 * \param args Arguments to construct element
	 * \return iterator to the new element
	 */
	template<typename... ARGS>
	Iterator emplace_front(ARGS&&... args) {
		Node * node = new Node(forward<ARGS>(args)...);

		if ((node->_next = _head) != nullptr) {
			assert(_head->_prev == nullptr);
			_head->_prev = node;
		} else if (_tail == nullptr) {
			_tail = node;
		}
		node->_prev = nullptr;
		_head = node;

		assert((node->_next == nullptr && _tail == node) || (node->_next->_prev == node && _tail != node));
		assert((node->_prev == nullptr && _head == node) || (node->_prev->_next == node && _head != node));

		_size++;
		return { node };
	}

	/*! \brief Insert element at front
	 * \param value new element to be inserted
	 * \return iterator to the inserted element
	 */
	Iterator push_front(const T & value) {
		return emplace_front(value);
	}

	/*! \brief Create new element at back
	 * \param args Arguments to construct element
	 * \return iterator to the new element
	 */
	template<typename... ARGS>
	Iterator emplace_back(ARGS&&... args) {
		Node * node = new Node(forward<ARGS>(args)...);

		if ((node->_prev = _tail) != nullptr) {
			assert(_tail->_next == nullptr);
			_tail->_next = node;
		} else if (_head == nullptr) {
			_head = node;
		}
		node->_next = nullptr;
		_tail = node;

		assert((node->_next == nullptr && _tail == node) || (node->_next->_prev == node && _tail != node));
		assert((node->_prev == nullptr && _head == node) || (node->_prev->_next == node && _head != node));

		_size++;
		return { node };
	}

	/*! \brief Insert element at back
	 * \param value new element to be inserted
	 * \return iterator to the inserted element
	 */
	Iterator push_back(const T & value) {
		return emplace_back(value);
	}

	/*! \brief Erase element at position
	 * \param position iterator to the element which should be removed
	 * \return iterator to the next element
	 */
	Iterator erase(const BaseIterator & position) {
		return { erase(position.i) };
	}

	/*! \brief Erase element at front
	 * \return `true` if element was removed
	 */
	bool pop_front() {
		return erase(_head) != nullptr;
	}

	/*! \brief Erase element at back
	 * \return `true` if element was removed
	 */
	bool pop_back() {
		return erase(_tail) != nullptr;
	}

	/*! \brief Access element at front
	 * \return reference to first element
	 */
	T& front() {
		assert(_head != nullptr);
		return *_head;
	}

	/*! \brief Access element at front
	 * \return reference to first element
	 */
	const T& front() const {
		assert(_head != nullptr);
		return *_head;
	}

	/*! \brief Access element at back
	 * \return reference to last element
	 */
	T& back() {
		assert(_tail != nullptr);
		return *_tail;
	}

	/*! \brief Access element at back
	 * \return reference to last element
	 */
	const T& back() const {
		assert(_tail != nullptr);
		return *_tail;
	}

	/*! \brief Test whether container is empty
	 * \return true if set is empty
	 */
	inline bool empty() const {
		return size() == 0;
	}

	/*! \brief Element count
	 * \return Number of (unique) elements in set
	 */
	inline size_t size() const {
		assert((_size == 0 && _head == nullptr && _tail == nullptr) || (_size > 0 && _head != nullptr && _tail != nullptr));
		return _size;
	}

	/*! \brief Remove all elements in list
	 */
	void clear() {
		while(pop_front()) {}
		assert(_size == 0 && _head == nullptr && _tail == nullptr);
	}

 private:
	/*! \brief Erase element at position
	 * \param node Element to remove
	 * \return Poitner to the next element
	 */
	Node * erase(Node * node) {
		if (node == nullptr)
			return nullptr;

		auto next = node->_next;
		auto prev = node->_prev;

		if (next == nullptr) {
			assert(_tail == node);
			_tail = prev;
		} else {
			assert(next->_prev == node);
			next->_prev = prev;
		}

		if (prev == nullptr) {
			assert(_head == node);
			_head = next;
		} else {
			assert(prev->_next == node);
			prev->_next = next;
		}

		delete node;
		_size--;
		return next;
	}
};
