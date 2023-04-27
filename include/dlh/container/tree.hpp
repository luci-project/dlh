// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/assert.hpp>
#include <dlh/string.hpp>
#include <dlh/utility.hpp>
#include <dlh/comparison.hpp>
#include <dlh/container/pair.hpp>
#include <dlh/container/optional.hpp>
#include <dlh/container/initializer_list.hpp>
#include <dlh/container/internal/elements.hpp>
#include <dlh/container/internal/keyvalue.hpp>


/*! \brief Tree set class
 * influenced by standard [template/cxx] librarys `set`
 * \tparam T type for container
 * \tparam C structure with comparison functions (compare())
 */
template<typename T, typename C = Comparison>
class TreeSet : public Elements<T> {
 protected:
	uint32_t _root = 0;

	/*! \brief base binary search tree iterator
	 */
	struct BaseIterator {
		friend class TreeSet<T, C>;
		const TreeSet<T, C> &ref;
		mutable uint32_t i;

		BaseIterator(const TreeSet<T, C> &ref, uint32_t p) : ref(ref), i(p) {}

		inline void next() const {
			const uint32_t right = ref._node[i].tree.right;
			if (right != 0) {
				i = ref.min_node(right);
			} else {
				while(i != 0) {
					const uint32_t old = i;
					i = ref._node[old].tree.parent;
					if (ref._node[i].tree.left == old)
						break;
				}
			}
		}

		inline void prev() const {
			const uint32_t left = ref._node[i].tree.left;
			if (left != 0) {
				i = ref.max_node(left);
			} else {
				while(i != 0) {
					const uint32_t old = i;
					i = ref._node[old].tree.parent;
					if (ref._node[i].tree.right == old)
						break;
				}
			}
		}

		inline const T& operator*() const {
			assert(ref._node[i].tree.active);
			return ref._node[i].data;
		}

		inline const T* operator->() const {
			assert(ref._node[i].tree.active);
			return &(ref._node[i].data);
		}

		template<typename I, typename enable_if<is_base_of<BaseIterator, I>::value, int>::type = 0>
		inline bool operator==(const I& other) const {
			return &ref == &other.ref && i == other.i;
		}

		template<typename X = T, typename enable_if<!is_base_of<BaseIterator, X>::value, int>::type = 0>
		inline bool operator==(const X& other) const {
			return C::compare(ref._node[i].data, other) == 0;
		}

		template<typename I, typename enable_if<is_base_of<BaseIterator, I>::value, int>::type = 0>
		inline bool operator!=(const BaseIterator& other) const {
			return &ref != &other.ref || i != other.i;
		}

		template<typename X = T, typename enable_if<!is_base_of<BaseIterator, X>::value, int>::type = 0>
		inline bool operator!=(const X& other) const {
			return C::compare(ref._node[i].data, other) != 0;
		}

		inline operator bool() const {
			return i != 0;
		}
	};

 public:
	using typename Elements<T>::Node;

	/*! \brief Create new balanced binary search tree
	 * \param capacity initial capacity
	 */
	explicit TreeSet(size_t capacity = 0) {
		if (capacity > 0)
			resize(capacity + 1);
		assert(empty() || !Elements<T>::_node[0].tree.active);
	}

	/*! \brief Copy constructor for a binary search tree from a tree set
	 * \param other Tree Set
	 */
	TreeSet(const TreeSet<T, C> & other) : Elements<T>(other), _root(other._root) {}

	/*! \brief Copy constructor for a binary search tree from an element container
	 * \param elements Elements container
	 */
	explicit TreeSet(const Elements<T>& elements) : Elements<T>(elements) {
		Elements<T>::_count = 0;
		for (size_t i = 1; i < Elements<T>::_next; i++) {
			if (Elements<T>::_node[i].tree.active && !insert(0, i, 0).second)
				Elements<T>::_node[i].tree.active = false;
		}
		assert(empty() || !Elements<T>::_node[0].tree.active);
		assert(Elements<T>::_count <= elements._count);
		assert(Elements<T>::_next == elements._next);
	}

	/*! \brief Move constructor for a binary search tree from a tree set
	 * \param other Tree Set
	 */
	TreeSet(TreeSet<T, C> && other) : Elements<T>(move(other)), _root(other._root) {
		other._root = 0;
	}

	/*! \brief Move constructor for a binary search tree from an element container
	 * \param elements container
	 */
	explicit TreeSet(Elements<T>&& elements) : Elements<T>(move(elements)) {
		Elements<T>::_count = 0;
		for (size_t i = 1; i < Elements<T>::_next; i++) {
			if (Elements<T>::_node[i].tree.active && !insert(0, i, 0).second)
				Elements<T>::_node[i].tree.active = false;
		}
		assert(empty() || !Elements<T>::_node[0].tree.active);
		assert(Elements<T>::_count <= elements._count);
		assert(Elements<T>::_next == elements._next);
	}

	/*! \brief Range constructor
	 * \param begin First element in range
	 * \param end End of range
	 * \param initial_capacity space to reserve (or determined automatically if zero)
	 */
	template<typename I>
	TreeSet(const I & begin, const I & end, size_t initial_capacity = 0) {
		if (initial_capacity == 0) {
			for (I i = begin; i != end; ++i)
				initial_capacity++;
		}

		resize(initial_capacity + 1);

		for (I i = begin; i != end; ++i)
			emplace(*i);

		assert(empty() || !Elements<T>::_node[0].tree.active);
	}

	/*! \brief Initializer list constructor
	 * \param flist initializer list
	 */
	template<typename I>
	TreeSet(const std::initializer_list<I> & list) {
		if (list.size() > 0) {
			resize(list.size() + 1);
			for (const auto & arg : list)
				emplace(arg);
		}
		assert(empty() || !Elements<T>::_node[0].tree.active);
	}

	/*! \brief Destructor
	 */
	virtual ~TreeSet() {}

	/*! \brief binary search tree iterator
	 */
	class Iterator : public BaseIterator {
		friend class TreeSet<T, C>;
		Iterator(TreeSet<T, C> &ref, uint32_t p) : BaseIterator(ref, p) {}

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
		friend class TreeSet<T, C>;
		ConstIterator(const TreeSet<T, C> &ref, uint32_t p) : BaseIterator(ref, p) {}

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
		friend class TreeSet<T, C>;
		ReverseIterator(TreeSet<T, C> &ref, uint32_t p) : BaseIterator(ref, p) {}

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
		friend class TreeSet<T, C>;
		ConstReverseIterator(const TreeSet<T, C> &ref, uint32_t p) : BaseIterator(ref, p) {}

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

	/*! \brief Create new element into set
	 * \param args Arguments to construct element
	 * \return iterator to the new element (`first`) and
	 *         indicator if element was created (`true`) or has already been in the set (`false`)
	 */
	template<typename... ARGS>
	Pair<Iterator, bool> emplace(ARGS&&... args) {
		increase();

		// Create local element
		new (&Elements<T>::_node[Elements<T>::_next].data) T(forward<ARGS>(args)...);

		int c = 0;
		uint32_t i = _root;
		if (contains_node(Elements<T>::_node[Elements<T>::_next].data, i, c))
			return Pair<Iterator, bool>{Iterator(*this, i), false};
		else
			return insert(i, Elements<T>::_next++, c);
	}

	/*! \brief Insert element into set
	 * \param value new element to be inserted
	 * \return iterator to the inserted element (`first`) and
	 *         indicator (`second`) if element was created (`true`) or has already been in the set (`false`)
	 */
	Pair<Iterator, bool> insert(const T &value) {
		increase();

		int c = 0;
		uint32_t i = _root;
		if (contains_node(value, i, c)) {
			return Pair<Iterator, bool>{Iterator(*this, i), false};
		} else {
			new (&Elements<T>::_node[Elements<T>::_next].data) T(value);
			return insert(i, Elements<T>::_next++, c);
		}
	}

	/*! \brief Insert element node into set
	 * \param value Node of element to be inserted
	 * \return iterator to the inserted element (`first`) and
	 *         indicator (`second`) if element was created (`true`) or has already been in the set (`false`)
	 */
	Pair<Iterator, bool> insert(Node &&value) {
		int c = 0;
		uint32_t i = _root;
		if (contains_node(value.data, i, c)) {
			return Pair<Iterator, bool>{Iterator(*this, i), false};
		} else {
			size_t n = 0;
			if (!Elements<T>::is_node(value, n) || n == 0)
				new (&Elements<T>::_node[n = Elements<T>::_next++].data) T(move(value.data));
			return insert(i, n, c);
		}
	}

	/*! \brief Extract value from set
	 * \note must be re-inserted before performing any other operations on the set
	 * \param position iterator to element (must be valid)
	 * \return removed Node
	 */
	Node && extract(const BaseIterator & position) {
		assert(position.i >= 1 && position.i < Elements<T>::_next && Elements<T>::_node[position.i].tree.active);
		auto & e = Elements<T>::_node[position.i].tree;

		if (e.left != 0 && e.right != 0) {
			uint32_t node = min_node(e.right);
			erase(node);
			replace_node(position.i, node, true);
			e.active = false;
		} else {
			erase(position.i);
		}

		e.active = false;
		Elements<T>::_count--;

		return move(Elements<T>::_node[position.i]);
	}

	/*! \brief Extract value from set
	 * \note must be re-inserted before performing any other operations on the set
	 * \param position iterator to element (must be valid)
	 * \return removed Node
	 */
	Node && extract(const Iterator & position) {
		return move(extract(reinterpret_cast<const BaseIterator &>(position)));
	}

	/*! \brief Extract value from set
	 * \note must be re-inserted before performing any other operations on the set
	 * \param position iterator to element (must be valid)
	 * \return removed Node
	 */
	Node && extract(const ReverseIterator & position) {
		return move(extract(reinterpret_cast<const BaseIterator &>(position)));
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	Optional<T> erase(const BaseIterator & position) {
		if (position.i >= 1 && position.i < Elements<T>::_next && Elements<T>::_node[position.i].tree.active)
			return Optional<T>{move(extract(position).data)};
		else
			return Optional<T>{};
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	Optional<T> erase(const Iterator & position) {
		return erase(reinterpret_cast<const BaseIterator &>(position));
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	Optional<T> erase(const ReverseIterator & position) {
		return erase(reinterpret_cast<const BaseIterator &>(position));
	}

	/*! \brief Remove value from set
	 * \param value element to be removed
	 * \return removed value (if found)
	 */
	template<typename O>
	inline Optional<T> erase(const O & value) {
		auto position = find(value);
		if (position.i >= 1 && position.i < Elements<T>::_next && Elements<T>::_node[position.i].tree.active) {
			return Optional<T>{move(extract(position).data)};
		} else {
			return Optional<T>{};
		}
	}

	/*! \brief Get iterator to specific element
	 * \param value element
	 * \return iterator to element (if found) or `end()` (if not found)
	 */
	template<typename O>
	inline Iterator find(const O& value) {
		uint32_t r = _root;
		if (!contains_node(value, r))
			r = 0;
		return Iterator{*this, r};
	}

	/*! \brief Get iterator to specific element
	 * \param value element
	 * \return iterator to element (if found) or `end()` (if not found)
	 */
	template<typename O>
	inline ConstIterator find(const O& value) const {
		uint32_t r = _root;
		if (!contains_node(value, r))
			r = 0;
		return ConstIterator{*this, r};
	}

	/*! \brief check if set contains element
	 * \param value element
	 * \return `true` if element is in set
	 */
	template<typename O>
	inline bool contains(const O& value) const {
		uint32_t r = _root;
		return contains_node(value, r);
	}

	/*! \brief Iterator to the lowest element in this set
	 * \return Iterator to the lowest element
	 */
	inline Iterator begin() {
		return Iterator{*this, min_node(_root)};
	}

	/*! \brief Constant iterator to the lowest element in this set
	 * \return Iterator to the lowest element
	 */
	inline ConstIterator begin() const {
		return ConstIterator{*this, min_node(_root)};
	}

	/*! \brief Iterator refering to the past-the-end element in this set
	 * \return Iterator to the element past the end of this set
	 */
	inline Iterator end() {
		return Iterator{*this, 0};
	}

	/*! \brief Constant iterator refering to the past-the-end element in this set
	 * \return Iterator to the element past the end of this set
	 */
	inline ConstIterator end() const {
		return ConstIterator{*this, 0};
	}

	/*! \brief Iterator to the highest element in this set
	 * \return Iterator to the highest element
	 */
	inline ReverseIterator rbegin() {
		return ReverseIterator{*this, max_node(_root)};
	}

	/*! \brief Constant iterator to the highest element in this set
	 * \return Iterator to the highest element
	 */
	inline ConstReverseIterator rbegin() const {
		return ConstReverseIterator{*this, max_node(_root)};
	}

	/*! \brief Iterator refering to the before-the-start element in this set
	 * \return Iterator to the element before the begin of this set
	 */
	inline ReverseIterator rend() {
		return ReverseIterator{*this, 0};
	}

	/*! \brief Constant iterator refering to thebefore-the-start element in this set
	 * \return Iterator to the element past the begin of this set
	 */
	inline ConstReverseIterator rend() const {
		return ConstReverseIterator{*this, 0};
	}

	/*! \brief Get the lowest element in this set
	 * \note alias for `begin()`
	 * \return Iterator to the lowest element
	 */
	inline Iterator lowest() {
		return Iterator{*this, min_node(_root)};
	}

	/*! \brief Get the lowest element in this set
	 * \note alias for `begin()`
	 * \return Iterator to the lowest element
	 */
	inline ConstIterator lowest() const {
		return ConstIterator{*this, min_node(_root)};
	}

	/*! \brief Get the greatest element in this set less than the given element
	 * \param value element
	 * \return Iterator to the greatest element less than the given element
	 */
	template<typename O>
	inline Iterator lower(const O& value) {
		return Iterator{*this, lower_node(value)};
	}

	/*! \brief Get the greatest element in this set less than the given element
	 * \param value element
	 * \return Iterator to the greatest element less than the given element
	 */
	template<typename O>
	inline ConstIterator lower(const O& value) const {
		return ConstIterator{*this, lower_node(value)};
	}

	/*! \brief Get the greatest element in this set less than or equal to the given element
	 * \param value element
	 * \return Iterator to the greatest element less than or equal to the given element
	 */
	template<typename O>
	inline Iterator floor(const O& value) {
		return Iterator{*this, floor_node(value)};
	}

	/*! \brief Get the greatest element in this set less than or equal to the given element
	 * \param value element
	 * \return Iterator to the greatest element less than or equal to the given element
	 */
	template<typename O>
	inline ConstIterator floor(const O& value) const {
		return ConstIterator{*this, floor_node(value)};
	}

	/*! \brief Get the smallest element in this set greater than or equal to the given element
	 * \param value element
	 * \return Iterator to the smallest element greater than or equal to the given element
	 */
	template<typename O>
	inline Iterator ceil(const O& value) {
		return Iterator{*this, ceil_node(value)};
	}

	/*! \brief Get the smallest element in this set greater than or equal to the given element
	 * \param value element
	 * \return Iterator to the smallest element greater than or equal to the given element
	 */
	template<typename O>
	inline ConstIterator ceil(const O& value) const {
		return ConstIterator{*this, ceil_node(value)};
	}

	/*! \brief Get the smallest element in this set greater than the given element
	 * \param value element
	 * \return Iterator to the smallest element greater than the given element
	 */
	template<typename O>
	inline Iterator higher(const O& value) {
		return Iterator{*this, higher_node(value)};
	}

	/*! \brief Get the smallest element in this set greater than the given element
	 * \param value element
	 * \return Iterator to the smallest element greater than the given element
	 */
	template<typename O>
	inline ConstIterator higher(const O& value) const {
		return ConstIterator{*this, higher_node(value)};
	}

	/*! \brief Get the highest element in this set
	 * \return Iterator to the highest element
	 */
	inline Iterator highest() {
		return Iterator{*this, max_node(_root)};
	}

	/*! \brief Get the highest element in this set
	 * \return Iterator to the highest element
	 */
	inline ConstIterator highest() const {
		return ConstIterator{*this, max_node(_root)};
	}

	/*! \brief Reorganize Elements
	 * Fill gaps emerged from erasing elments
	 */
	inline void reorganize() {
		if (!empty())
			reorder();
	}

	/*! \brief Resize set capacity
	 * \note first element is reserved for NULL, hence the usable capacity is one less
	 * \note minimum capacity is 16
	 * \param capacity new capacity (has to be equal or greater than `size()`)
	 * \return `true` if resize was successfully, `false` otherwise
	 */
	bool resize(size_t capacity) {
		if (capacity <= Elements<T>::_count || capacity > UINT32_MAX)
			return false;

		if (capacity < 16)
			capacity = 16;

		// reorder node slots
		reorder();

		// Resize
		if (capacity != Elements<T>::_capacity) {
			if (Elements<T>::resize(capacity))
				Elements<T>::_node[0].tree.active = false;
			else
				return false;
		}
		return true;
	}

	/*! \brief Test whether container is empty
	 * \return true if set is empty
	 */
	bool empty() const {
		return Elements<T>::_count == 0;
	}

	/*! \brief Element count
	 * \return Number of (unique) elements in set
	 */
	size_t size() const {
		return Elements<T>::_count;
	}

	/*! \brief Clear all elements in set */
	void clear() {
		Elements<T>::clear();
		_root = 0;
	}

#ifndef NDEBUG

 private:
	int check_node(uint32_t node, uint32_t parent) { // NOLINT misc-no-recursion
		if (node == 0) {
			return 0;
		} else {
			assert(Elements<T>::_node[node].tree.parent == parent);
			auto & n = Elements<T>::_node[node].tree;
			int l = check_node(n.left, node);
			int r = check_node(n.right, node);
			assert(r - l == static_cast<int>(n.balance));
			return 1 + (l > r ? l : r);
		}
	}

 public:
	/*! \brief Check if balanced */
	void check() {
		// Elements
		assert(!Elements<T>::_node[0].tree.active);
		uint32_t c = 0;
		for (size_t i = 1; i < Elements<T>::_next; i++)
			if (Elements<T>::_node[i].tree.active)
				c++;
		assert(c == Elements<T>::_count);

		// Order
		auto i = begin();
		if (i) {
			c = 1;
			auto s = i.operator->();
			for (++i; i != end(); ++i) {
				assert(C::compare(*s, *i) < 0);
				s = i.operator->();
				c++;
			}
			assert(c == Elements<T>::_count);
		}

		// Balance
		if (_root == 0) {
			assert(Elements<T>::_count == 0);
		} else {
			assert(Elements<T>::_node[_root].tree.parent == 0);
			check_node(_root, 0);
		}
	}
#endif

 private:
	/*! \brief Get the lowest element in the subtree
	 * \param i node definining the subtree
	 * \return minimum value of the subtree
	 */
	inline uint32_t min_node(uint32_t i) const {
		if (i != 0)
			while (Elements<T>::_node[i].tree.left != 0)
				i = Elements<T>::_node[i].tree.left;
		return i;
	}


	/*! \brief Get the highest element in the subtree
	 * \param i node definining the subtree
	 * \return max value of the subtree
	 */
	inline uint32_t max_node(uint32_t i) const {
		if (i != 0)
			while (Elements<T>::_node[i].tree.right != 0)
				i = Elements<T>::_node[i].tree.right;
		return i;
	}

	/*! \brief Get the greatest element in this set less than the given element
	 * \param value element
	 * \return Node ID of the greatest element less than the given element
	 */
	template<typename O>
	uint32_t lower_node(const O& value) const {
		uint32_t r = 0;
		uint32_t i = _root;
		while (i != 0) {
			auto & e = Elements<T>::_node[i];
			int c = C::compare(e.data, value);
			if (c == 0) {
				if (e.tree.left != 0)
					r = max_node(e.tree.left);
				break;
			} else if (c < 0) {
				r = i;
				i = e.tree.right;
			} else /* if (c > 0) */ {
				i = e.tree.left;
			}
		}
		return r;
	}

	/*! \brief Get the greatest element in this set less than or equal to the given element
	 * \param value element
	 * \return Node ID of the greatest element less than or equal to the given element
	 */
	template<typename O>
	uint32_t floor_node(const O& value) const {
		uint32_t r = 0;
		uint32_t i = _root;
		while (i != 0) {
			auto & e = Elements<T>::_node[i];
			int c = C::compare(e.data, value);
			if (c == 0) {
				r = i;
				break;
			} else if (c < 0) {
				r = i;
				i = e.tree.right;
			} else /* if (c > 0) */ {
				i = e.tree.left;
			}
		}
		return r;
	}

	/*! \brief Get the smallest element in this set greater than or equal to the given element
	 * \param value element
	 * \return Node ID of the smallest element greater than or equal to the given element
	 */
	template<typename O>
	uint32_t ceil_node(const O& value) const {
		uint32_t r = 0;
		uint32_t i = _root;
		while (i != 0) {
			auto & e = Elements<T>::_node[i];
			int c = C::compare(e.data, value);
			if (c == 0) {
				r = i;
				break;
			} else if (c < 0) {
				i = e.tree.right;
			} else /* if (c > 0) */ {
				r = i;
				i = e.tree.left;
			}
		}
		return r;
	}

	/*! \brief Get the smallest element in this set greater than the given element
	 * \param value element
	 * \return Node ID of the smallest element greater than the given element
	 */
	template<typename O>
	uint32_t higher_node(const O& value) const {
		uint32_t r = 0;
		uint32_t i = _root;
		while (i != 0) {
			auto & e = Elements<T>::_node[i];
			int c = C::compare(e.data, value);
			if (c == 0) {
				if (e.tree.right != 0)
					r = min_node(e.tree.right);
				break;
			} else if (c < 0) {
				i = e.tree.right;
			} else /* if (c > 0) */ {
				r = i;
				i = e.tree.left;
			}
		}
		return r;
	}

	/*! \brief Check if set contains node
	 * \param value Value to search
	 * \param i root, will contain index of nearest node
	 * \param c reference to last comparison result
	 * \return true if found, false otherwise
	 */
	template<typename O>
	bool contains_node(const O& value, uint32_t & i, int & c) const {
		if (i != 0)
			while (true) {
				auto & e = Elements<T>::_node[i];
				c = C::compare(e.data, value);
				if (c == 0) {
					return true;
				} else if (c < 0) {
				 	if (e.tree.right == 0)
						break;
					i = e.tree.right;
				} else /* if (c > 0) */ {
				 	if (e.tree.left == 0)
						break;
					i = e.tree.left;
				}
			}

		return false;
	}

	/*! \brief Check if set contains node
	 * \param value Value to search
	 * \param i root, will contain index of nearest node
	 * \return true if found, false otherwise
	 */
	template<typename O>
	inline bool contains_node(const O& value, uint32_t & i) const {
		int c;
		return contains_node(value, i, c);
	}

	/*! \brief Increase capacity (by reordering or resizing) if required
	 * \return `false` on error
	 */
	inline bool increase() {
		if (Elements<T>::_capacity == 0) {
			if (!resize(16))
				return false;
		} else if (Elements<T>::_next >= Elements<T>::_capacity) {
			if (Elements<T>::_count * 2 <= Elements<T>::_capacity)
				reorder();
			else if (!resize(Elements<T>::_capacity * 2))
				return false;
		}
		return true;
	}

	/*! \brief Reorder elements
	 * \return `true` if element positions have changed
	 */
	bool reorder() {
		if (Elements<T>::_count + 1 < Elements<T>::_next) {
			size_t j = Elements<T>::_next - 1;
			for (size_t i = 1; i <= Elements<T>::_count; i++) {
				if (!Elements<T>::_node[i].tree.active) {
					for (; !Elements<T>::_node[j].tree.active; --j)
						assert(j > i);
					new (&Elements<T>::_node[i].data) T(move(Elements<T>::_node[j].data));

					replace_node(j, i, true);

					Elements<T>::_node[i].tree.active = true;
					Elements<T>::_node[j].tree.active = false;
				}
			}

			Elements<T>::_next = Elements<T>::_count + 1;
			assert(j >= Elements<T>::_next);
			return true;
		} else {
			return false;
		}
	}

	/*! \brief Insert helper
	 * \param parent index of parent node or 0 if unknown (or root)
	 * \param element index of element to insert
	 * \param c comparison result of element with parent
	 */
	Pair<Iterator, bool> insert(uint32_t parent, uint32_t element, int c) {
		assert(element != 0);
		auto & e = Elements<T>::_node[element];
		if (_root == 0) {
			assert(parent == 0);
			assert(Elements<T>::_count == 0);
			_root = element;
		} else if ((parent == 0 && contains_node(e.data, parent = _root, c)) || c == 0) {
			assert(Elements<T>::_node[parent].tree.active);
			return Pair<Iterator, bool>{Iterator(*this, parent), false};
		} else if (c < 0) {
			assert(Elements<T>::_node[parent].tree.right == 0);
			Elements<T>::_node[parent].tree.right = element;
		} else /* if (c > 0) */ {
			assert(Elements<T>::_node[parent].tree.left == 0);
			Elements<T>::_node[parent].tree.left = element;
		}
		e.tree.active = true;
		e.tree.balance = 0;
		e.tree.left = 0;
		e.tree.right = 0;
		e.tree.parent = parent;

		// rebalance
		uint32_t node = element;
		while (parent != 0) {
			assert(Elements<T>::_node[node].tree.parent == parent);
			auto & p = Elements<T>::_node[parent].tree;
			if (node == p.right) {
				if (p.balance < 0) {
				 	p.balance = 0;
					break;
				} else if (p.balance > 0) {
					rotate(node, parent, true);
					break;
				} else {
					p.balance = 1;
				}
			} else {
				assert(node == p.left);
				if (p.balance > 0) {
				 	p.balance = 0;
					break;
				} else if (p.balance < 0) {
					rotate(node, parent, false);
					break;
				} else {
					p.balance = -1;
				}
			}
			node = parent;
			parent = p.parent;
		}

		Elements<T>::_count++;
		return Pair<Iterator, bool>{Iterator(*this, element), true};
	}

	/*! \brief Remove node (with at most one child)
	 * \note element is not marked as inactive, neither is the element count modified
	 * \param element element to remove
	 */
	void erase(uint32_t element) {
		assert(element != 0);
		auto & n = Elements<T>::_node[element].tree;
		assert(n.left == 0 || n.right == 0);

		// rebalance
		uint32_t node = element;
		uint32_t parent = n.parent;
		while (parent != 0) {
			auto & p = Elements<T>::_node[parent].tree;
			uint32_t grandparent = p.parent;
			if (p.left == node) {
				if (p.balance < 0) {
				 	p.balance = 0;
				} else if (p.balance == 0) {
					p.balance = 1;
					break;
				} else if (rotate(p.right, parent, true)) {
					break;
				}
			} else {
				assert(p.right == node);
				if (p.balance > 0) {
				 	p.balance = 0;
				} else if (p.balance == 0) {
					p.balance = -1;
					break;
				} else if (rotate(p.left, parent, false)) {
					break;
				}
			}
			node = parent;
			parent = grandparent;
		}
		assert(n.left == 0 || n.right == 0);
		replace_node(element, n.left != 0 ? n.left : n.right, false);
	}

	/*! \brief Helper to perform rotation
	 * \param node Node to rotate
	 * \param parent reference which will contain the new subtree root node
	 * \param left `true` for left direction, `false` for right
	 * \return `true` if there was no height change
	 */
	bool rotate(const uint32_t node, uint32_t & parent, bool left) {
		assert(node != 0);
		auto & n = Elements<T>::_node[node].tree;
		assert(n.active);

		assert(parent != 0);
		auto & p = Elements<T>::_node[parent].tree;
		assert(p.active);

		const uint32_t grandparent = p.parent;

		const int8_t b = n.balance;
		uint32_t subroot;

		if (left ? (b >= 0) : (b <= 0)) {
			// Simple rotation
			if (left) {
				// Rotate left
				if ((p.right = n.left) != 0) {
					assert(Elements<T>::_node[p.right].tree.parent == node);
					Elements<T>::_node[p.right].tree.parent = parent;
				}
				n.left = parent;
			} else {
				// Rotate right
				if ((p.left = n.right) != 0) {
					assert(Elements<T>::_node[p.left].tree.parent == node);
					Elements<T>::_node[p.left].tree.parent = parent;
				}
				n.right = parent;
			}
			p.parent = node;

			// Adjust balance
			p.balance = n.balance == 0 ? (left ?  1 : -1) : 0;
			n.balance = n.balance == 0 ? (left ? -1 :  1) : 0;

			subroot = node;
		} else {
			// Double rotation
			subroot = left ? n.left : n.right;
			assert(subroot != 0);
			auto & s = Elements<T>::_node[subroot].tree;
			assert(s.active);

			if (left) {
				// Rotate right and then left
				if ((n.left = s.right) != 0)
					Elements<T>::_node[n.left].tree.parent = node;
				s.right = node;

				if ((p.right = s.left) != 0)
					Elements<T>::_node[p.right].tree.parent = parent;
				s.left = parent;
			} else {
				// Rotate left and then right
				if ((n.right = s.left) != 0)
					Elements<T>::_node[n.right].tree.parent = node;
				s.left = node;

				if ((p.left = s.right) != 0)
					Elements<T>::_node[p.left].tree.parent = parent;
				s.right = parent;
			}
			p.parent = n.parent = subroot;

			// Adjust balance
			(left ? p : n).balance = s.balance > 0 ? -1 : 0;
			(left ? n : p).balance = s.balance < 0 ?  1 : 0;
			s.balance = 0;
		}

		replace_node(grandparent, parent, subroot, false);  // NOLINT

		parent = subroot;

		return b == 0;
	}

	/*! \brief Replace a node
	 * \param parent the parent node (of target)
	 * \param target the node to be replaced
	 * \param replacement the node to replace target
	 * \param include_children copy children (and balance)
	 */
	void replace_node(uint32_t parent, uint32_t target, uint32_t replacement, bool include_children) {
		if (parent == 0) {
			assert(_root == target);
			_root = replacement;
		} else {
			auto & p = Elements<T>::_node[parent].tree;
			assert(p.active);
			if (target == p.left)
				p.left = replacement;
			else if (target == p.right)
 				p.right = replacement;
			else
				assert(false);
		}

		if (replacement != 0) {
			auto & r = Elements<T>::_node[replacement].tree;
			r.parent = parent;

			if (include_children) {
				assert(target != 0);
				auto & t = Elements<T>::_node[target].tree;

				r.balance = t.balance;

				if ((r.left = t.left) != 0) {
					auto & p = Elements<T>::_node[r.left].tree.parent;
					assert(p == target);
					p = replacement;
				}
				if ((r.right = t.right) != 0) {
					auto & p = Elements<T>::_node[r.right].tree.parent;
					assert(p == target);
					p = replacement;
				}
			}
		}
	}

	/*! \brief Replace a node
	 * \param target the node to be replaced
	 * \param replacement the node to replace target
	 * \param include_children copy children (and balance)
	 */
	inline void replace_node(uint32_t target, uint32_t replacement, bool include_children) {
		replace_node(Elements<T>::_node[target].tree.parent, target, replacement, include_children);
	}
};


/*! \brief Tree set
 * \tparam K type for key
 * \tparam V type for value
 * \tparam C structure with comparison functions (compare())
 */
template<typename K, typename V, typename C = Comparison>
class TreeMap : protected TreeSet<KeyValue<K, V>, C> {
	using Base = TreeSet<KeyValue<K, V>, C>;
	using typename Base::BaseIterator;

 public:
	using typename Base::Iterator;
	using typename Base::ConstIterator;
	using typename Base::ReverseIterator;
	using typename Base::ConstReverseIterator;
	using Base::begin;
	using Base::lowest;
	using Base::lower;
	using Base::floor;
	using Base::ceil;
	using Base::higher;
	using Base::highest;
	using Base::end;
	using Base::rbegin;
	using Base::rend;
	using Base::find;
	using Base::contains;
	using Base::resize;
	using Base::empty;
	using Base::size;
	using Base::clear;
#ifndef NDEBUG
	using Base::check;
#endif

	inline Pair<Iterator, bool> insert(const K& key, const V& value) {
		return Base::emplace(key, value);
	}

	inline Pair<Iterator, bool> insert(K&& key, V&& value) {
		return Base::emplace(move(key), move(value));
	}

	inline Optional<V> erase(const BaseIterator & position) {
		auto i = Base::erase(position);
		if (i)
			return Optional<V>{move(i->value)};
		else
			return Optional<V>{};
	}

	inline Optional<V> erase(const Iterator & position) {
		return erase(reinterpret_cast<const BaseIterator &>(position));
	}

	inline Optional<V> erase(const ConstIterator & position) {
		return erase(reinterpret_cast<const BaseIterator &>(position));
	}

	inline Optional<V> erase(const ReverseIterator & position) {
		return erase(reinterpret_cast<const BaseIterator &>(position));
	}

	inline Optional<V> erase(const ConstReverseIterator & position) {
		return erase(reinterpret_cast<const BaseIterator &>(position));
	}

	template<typename O>
	inline Optional<V> erase(const O& key) {
		auto i = Base::erase(key);
		if (i)
			return Optional<V>{move(i->value)};
		else
			return Optional<V>{};
	}

	template<typename O>
	inline Optional<V> at(const O& key) {
		auto i = Base::find(key);
		if (i)
			return Optional<V>{move(i->value)};
		else
			return Optional<V>{};
	}

	template<typename O>
	inline Optional<V> at(O&& key) {
		auto i = Base::find(move(key));
		if (i)
			return Optional<V>{move(i->value)};
		else
			return Optional<V>{};
	}

	template<typename O>
	inline V & operator[](const O& key) {
		return (*(Base::emplace(key).first)).value;
	}

	template<typename O>
	inline V & operator[](O&& key) {
		return (*(Base::emplace(move(key)).first)).value;
	}
};


/*! \brief Print contents of a TreeSet
 *
 *  \param s Target Stream
 *  \param set TreeSet to be printed
 *  \return Reference to Stream; allows operator chaining.
 */
template<typename S, typename T, typename C>
static inline S & operator<<(S & s, const TreeSet<T, C> & set) {
	s << '{';
	bool p = false;
	for (const auto & entry : set) {
		if (p)
			s << ',';
		else
			p = true;
		s << ' ' << entry;
	}
	return s << ' ' << '}';
}

/*! \brief Print contents of a TreeSet
 *
 *  \param s Target Stream
 *  \param map TreeMap to be printed
 *  \return Reference to Stream; allows operator chaining.
 */
template<typename S, typename K, typename V, typename C>
static inline S & operator<<(S & s, const TreeMap<K, V, C> & map) {
	s << '{';
	bool p = false;
	for (const auto & entry : map) {
		if (p)
			s << ',';
		else
			p = true;
		s << ' ' << entry;
	}
	return s << ' ' << '}';
}
