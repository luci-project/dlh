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

	constexpr Elements() : _capacity(0), _next(1), _count(0), _node(nullptr) {}

	Elements(const Elements<T>& e) : _capacity(e._capacity), _next(e._next), _count(e._count), _node(malloc(e._capacity * sizeof(Node))) {
		assert(_node != nullptr);
		memcpy(_node, e._node, e._capacity * sizeof(Node));
	}

	Elements(Elements<T>&& e) : _capacity(move(e._capacity)), _next(move(e._next)), _count(move(e._count)), _node(move(e._node)) {
		assert(_node != nullptr);
		e._capacity = 0;
		e._next = 1;
		e._count = 0;
		e._node = 0;
	}

	virtual ~Elements() {
		free(Elements<T>::_node);
	}

	/*! \brief Resize element slots to capacity
	 * \param capacity the new capacity
	 * \return `true` if resize was successful
	 */
	bool resize(uint32_t capacity) {
		if (capacity != _capacity) {
			void * ptr = realloc(_node, capacity * sizeof(Node));
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
