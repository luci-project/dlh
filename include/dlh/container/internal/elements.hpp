#pragma once

#include <dlh/types.hpp>
#include <dlh/alloc.hpp>
#include <dlh/assert.hpp>
#include <dlh/utility.hpp>

template<class T>
struct Elements {
	uint32_t _capacity;
	uint32_t _next;
	uint32_t _count;

	struct Node {
		union {
			struct {
				bool active;
				uint32_t prev, next, temp;
			} hash;
			struct {
				bool active;
				int8_t balance;
				uint32_t parent, left, right;
			} tree;
		};

		T data;

		T& value() { return data; }
	} * _node;

	static_assert(sizeof(Node) == 16 + sizeof(T), "Wrong Node size");

	/*! \brief Constructor (empty) element container
	 * \param e Element container to copy
	 */
	constexpr Elements() : _capacity(0), _next(1), _count(0), _node(nullptr) {}

	/*! \brief Copy constructor
	 * \param e Element container to copy
	 */
	Elements(const Elements<T>& e) : _capacity(e._capacity), _next(e._next), _count(e._count), _node(nullptr) {
		if (e._capacity > 0) {
			_node = reinterpret_cast<Node*>(malloc(e._capacity * sizeof(Node)));
			assert(_node != nullptr);
			for (size_t i = 0; i < e._next; i++)
				_node[i] = e._node[i];
		}
	}

	/*! \brief Default move constructor
	 */
	Elements(Elements<T> &&) = default;

	/*! \brief Destructor
	 */
	virtual ~Elements() {
		free(Elements<T>::_node);
	}

	/*! \brief Copy assignment operator
	 * \param e Element container to copy
	 * \return Reference to this instance
	 */
	Elements<T> & operator=(const Elements<T> & e) {
		_next = e._next;
		_count = e._count;
		if ((_capacity = e._capacity) > 0) {
			_node = reinterpret_cast<Node*>(malloc(e._capacity * sizeof(Node)));
			assert(_node != nullptr);
			for (size_t i = 0; i < e._next; i++)
				_node[i] = e._node[i];
		} else {
			_node = nullptr;
		}
		return *this;
	}

	/*! \brief Default move assignment operator
	 */
	Elements<T> & operator=(Elements<T> &&) = default;

	/*! \brief Resize element slots to capacity
	 * \param capacity the new capacity
	 * \return `true` if resize was successful
	 */
	bool resize(uint32_t capacity) {
		if (capacity != _capacity) {
			void * ptr = realloc(reinterpret_cast<void*>(_node), capacity * sizeof(Node));
			if (ptr == nullptr) {
				return false;
			} else {
				_node = reinterpret_cast<Node *>(ptr);
				_capacity = capacity;
			}
		}
		return true;
	}


	/*! \brief check if a given node is part of the element
	 * \param n given node
	 * \param index will contain the index in node array (in case it is a part of it)
	 * \return `true` if given node is partof this element
	 */
	bool is_node(const Node & given_node, size_t & index) {
		auto gnptr = reinterpret_cast<uintptr_t>(&given_node);
		auto _nptr = reinterpret_cast<uintptr_t>(_node);
		if (gnptr >= _nptr) {
			index = (gnptr - _nptr) / sizeof(Node);
			return index < _next;
		} else {
			return false;
		}
	}
};
