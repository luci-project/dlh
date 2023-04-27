// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/assert.hpp>
#include <dlh/utility.hpp>

/*! \brief Generic linked list node
 * influenced by standard [template/cxx] librarys `list`
 * \tparam T type for payload
 */
template<typename T>
struct ListNode : public T {
	ListNode * _prev;
	ListNode * _next;

	explicit ListNode(const T& value) : T(value), _prev(nullptr), _next(nullptr) {}

	explicit ListNode(T&& value) : T(move(value)), _prev(nullptr), _next(nullptr) {}

	template <class... ARGS>
	explicit ListNode(const ARGS&... args) : T(args...), _prev(nullptr), _next(nullptr) {}

	template <class... ARGS>
	explicit ListNode(ARGS&&... args) : T(forward<ARGS>(args)...), _prev(nullptr), _next(nullptr) {}

	virtual ~ListNode() {}
};

/*! \brief Linked list
 * \tparam T type for payload
 * \tparam N type for node container
 * \tparam NEXT pointer to member pointing to next element
 * \tparam PREV pointer to member pointing to previous element
 */
template<typename T, typename N = ListNode<T>, N* N::* NEXT = &ListNode<T>::_next, N* N::* PREV = &ListNode<T>::_prev>
class List {
	N * _head = nullptr;
	N * _tail = nullptr;
	size_t _size = 0;

	/*! \brief base hash set iterator
	 */
	struct BaseIterator {
		friend class List<T, N, NEXT, PREV>;
		mutable N * i;

		explicit BaseIterator(N * node) : i(node) {}

		inline void next() const {
			assert(i != nullptr);
			i = i->*NEXT;
		}

		inline void prev() const {
			assert(i != nullptr);
			i = i->*PREV;
		}

		inline const T& operator*() const {
			assert(i != nullptr);
			return *reinterpret_cast<T*>(i);
		}

		inline const T* operator->() const {
			assert(i != nullptr);
			return reinterpret_cast<T*>(i);
		}

		template<typename I, typename enable_if<is_base_of<BaseIterator, I>::value, int>::type = 0>
		inline bool operator==(const I& other) const {
			return i == other.i;
		}

		template<typename X = T, typename enable_if<!is_base_of<BaseIterator, X>::value, int>::type = 0>
		inline bool operator==(const X& other) const {
			assert(i != nullptr);
			return *reinterpret_cast<X*>(i) == other;
		}

		template<typename I, typename enable_if<is_base_of<BaseIterator, I>::value, int>::type = 0>
		inline bool operator!=(const BaseIterator& other) const {
			return i == other.i;
		}

		template<typename X = T, typename enable_if<!is_base_of<BaseIterator, X>::value, int>::type = 0>
		inline bool operator!=(const X& other) const {
			assert(i != nullptr);
			return *reinterpret_cast<X*>(i) != other;
		}

		inline operator bool() const {
			return i != nullptr;
		}
	};

 public:
	/*! \brief Create new empty list
	 */
	List() {}

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
	~List() {
		clear();
	}

	/*! \brief binary search tree iterator
	 */
	class Iterator : public BaseIterator {
		friend class List<T, N, NEXT, PREV>;
		explicit Iterator(N * p) : BaseIterator(p) {}

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
		friend class List<T, N, NEXT, PREV>;
		explicit ConstIterator(N * p) : BaseIterator(p) {}

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
		friend class List<T, N, NEXT, PREV>;
		explicit ReverseIterator(N * p) : BaseIterator(p) {}

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
		friend class List<T, N, NEXT, PREV>;
		explicit ConstReverseIterator(N * p) : BaseIterator(p) {}

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
		return Iterator{_head};
	}

	/*! \brief Get iterator to end (post-last-element)
	 * \return iterator to end (first invalid element)
	 */
	inline Iterator end() {
		return Iterator{nullptr};
	}

	/*! \brief Get iterator to first element
	 * \return iterator to first valid element (if available) or `end()`
	 */
	inline ConstIterator begin() const {
		return ConstIterator{_head};
	}

	/*! \brief Get iterator to end (post-last-element)
	 * \return iterator to end (first invalid element)
	 */
	inline ConstIterator end() const {
		return ConstIterator{nullptr};
	}

	/*! \brief Get iterator to last element
	 * \return iterator to last valid element (if available) or `rend()`
	 */
	inline ReverseIterator rbegin() {
		return ReverseIterator{_tail};
	}

	/*! \brief Get reverse iterator to end (pre-first-element)
	 * \return iterator to end (first invalid element)
	 */
	inline ReverseIterator rend() {
		return ReverseIterator{nullptr};
	}

	/*! \brief Get iterator to last element
	 * \return iterator to last valid element (if available) or `rend()`
	 */
	inline ConstReverseIterator rbegin() const {
		return ConstReverseIterator{_tail};
	}

	/*! \brief Get reverse iterator to end (pre-first-element)
	 * \return iterator to end (first invalid element)
	 */
	inline ConstReverseIterator rend() const {
		return ConstReverseIterator{nullptr};
	}

	/*! \brief Create new element before postition
	 * \param position iterator to the element where this element should be created
	 * \param args Arguments to construct element
	 * \return iterator to the new element
	 */
	template<typename... ARGS>
	Iterator emplace(const BaseIterator & position, ARGS&&... args) {
		return insert(position, new N(forward<ARGS>(args)...));
	}

	/*! \brief Insert node element before postition
	 * \param position iterator to the element where this element should be created
	 * \param node pointer to node element to be inserted
	 * \return iterator to the inserted element
	 */
	Iterator insert(const BaseIterator & position, N * node) {
		if (position.i == nullptr)
			return push_back(node);

		assert(node != nullptr);

		node->*NEXT = position.i;
		if ((node->*PREV = position.i->*PREV) == nullptr) {
			assert(_head == position.i);
			_head = node;
		} else {
			node->*PREV->*NEXT = node;
		}
		position.i->*PREV = node;

		assert((node->*NEXT == nullptr && _tail == node) || (node->*NEXT->*PREV == node && _tail != node));
		assert((node->*PREV == nullptr && _head == node) || (node->*PREV->*NEXT == node && _head != node));

		_size++;
		return Iterator{node};
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
		return push_front(new N(forward<ARGS>(args)...));
	}

	/*! \brief Insert node element at front
	 * \param node pointer to node element to be inserted
	 * \return iterator to the inserted element
	 */
	Iterator push_front(N * node) {
		assert(node != nullptr);

		if ((node->*NEXT = _head) != nullptr) {
			assert(_head->*PREV == nullptr);
			_head->*PREV = node;
		} else if (_tail == nullptr) {
			_tail = node;
		}
		node->*PREV = nullptr;
		_head = node;

		assert((node->*NEXT == nullptr && _tail == node) || (node->*NEXT->*PREV == node && _tail != node));
		assert((node->*PREV == nullptr && _head == node) || (node->*PREV->*NEXT == node && _head != node));

		_size++;
		return Iterator{node};
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
		return push_back(new N(forward<ARGS>(args)...));
	}

	/*! \brief Insert node element at back
	 * \param node pointer to node element to be inserted
	 * \return iterator to the inserted element
	 */
	Iterator push_back(N * node) {
		assert(node != nullptr);

		if ((node->*PREV = _tail) != nullptr) {
			assert(_tail->*NEXT == nullptr);
			_tail->*NEXT = node;
		} else if (_head == nullptr) {
			_head = node;
		}
		node->*NEXT = nullptr;
		_tail = node;

		assert((node->*NEXT == nullptr && _tail == node) || (node->*NEXT->*PREV == node && _tail != node));
		assert((node->*PREV == nullptr && _head == node) || (node->*PREV->*NEXT == node && _head != node));

		_size++;
		return Iterator{node};
	}

	/*! \brief Insert element at back
	 * \param value new element to be inserted
	 * \return iterator to the inserted element
	 */
	Iterator push_back(const T & value) {
		return emplace_back(value);
	}

	/*! \brief Extract node from list
	 * \param position iterator to the element which should be removed
	 * \return pointer to extracted node
	 */
	N * extract(const BaseIterator & position) {
		if (position.i == nullptr)
			return nullptr;
		extract(position.i);
		return position.i;
	}

	/*! \brief Extract node
	 * \param node Element to extract
	 */
	void extract(N * node) {
		assert(node != nullptr);

		auto * next = node->*NEXT;
		auto * prev = node->*PREV;

		if (next == nullptr) {
			assert(_tail == node);
			_tail = prev;
		} else {
			assert(next->*PREV == node);
			next->*PREV = prev;
		}

		if (prev == nullptr) {
			assert(_head == node);
			_head = next;
		} else {
			assert(prev->*NEXT == node);
			prev->*NEXT = next;
		}

		_size--;
	}

	/*! \brief Erase element at position
	 * \param position iterator to the element which should be removed
	 * \return iterator to the next element
	 */
	Iterator erase(const BaseIterator & position) {
		return Iterator{erase(position.i)};
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
		return *reinterpret_cast<T*>(_head);
	}

	/*! \brief Access element at front
	 * \return reference to first element
	 */
	const T& front() const {
		assert(_head != nullptr);
		return *reinterpret_cast<T*>(_head);
	}

	/*! \brief Access element at back
	 * \return reference to last element
	 */
	T& back() {
		assert(_tail != nullptr);
		return *reinterpret_cast<T*>(_tail);
	}

	/*! \brief Access element at back
	 * \return reference to last element
	 */
	const T& back() const {
		assert(_tail != nullptr);
		return *reinterpret_cast<T*>(_tail);
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
	 * \return Pointer to the next element
	 */
	N * erase(N * node) {
		if (node == nullptr)
			return nullptr;

		auto * next = node->*NEXT;
		extract(node);
		delete node;
		return next;
	}
};


/*! \brief Print contents of a List
 *
 *  \param s Target Stream
 *  \param val List to be printed
 *  \return Reference to Stream; allows operator chaining.
 */
template<typename S, typename T, typename N = ListNode<T>, N* N::* NEXT, N* N::* PREV>
static inline S & operator<<(S & s, const List<T, N, NEXT, PREV> & val) {
	s << '[';
	bool p = false;
	for (const auto & v : val) {
		if (p)
			s << ',';
		else
			p = true;
		s << ' ' << v;
	}
	return s << ' ' << ']';
}
