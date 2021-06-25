#pragma once

#include <dlh/types.hpp>
#include <dlh/alloc.hpp>
#include <dlh/assert.hpp>
#include <dlh/utility.hpp>
#include <dlh/type_traits.hpp>

template<class T>
struct Elements {
	uint32_t _capacity;
	uint32_t _next;
	uint32_t _count;

	struct Node {
		union {
			struct __attribute__ ((packed)) {
				uint64_t a,b;
			} mem;
			struct {
				bool active;
			} generic;
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


		~Node() {
			if (generic.active)
				data.~T();
		}

		Node & operator=(const Node& other) {
			mem = other.mem;
			if (generic.active)
				new (&data) T(other.data);
			return *this;
		}
	} * _node;
	static_assert(sizeof(Node) == 16 + sizeof(T), "Wrong Node size");

	/*! \brief Constructor (empty) element container
	 * \param e Element container to copy
	 */
	constexpr Elements() : _capacity(0), _next(1), _count(0), _node(nullptr) {}

	/*! \brief Copy constructor
	 * \param reserve additional space to reserve
	 * \param e Element container to copy
	 */
	Elements(const Elements<T>& e, size_t reserve = 0) : _capacity(e._capacity), _next(e._next), _count(e._count), _node(nullptr) {
		if (e._capacity > 0) {
			auto s = e._capacity * sizeof(Node);
			_node = reinterpret_cast<Node*>(malloc(s + reserve));
			assert(_node != nullptr);
			if (is_integral<T>::value || is_reference<T>::value)
				memcpy(reinterpret_cast<void*>(_node), reinterpret_cast<void*>(e._node), s);
			else
				for (size_t i = 0; i < e._next; i++)
					_node[i] = e._node[i];
		}
	}

	/*! \brief Default move constructor
	 */
	Elements(Elements<T> && e) : _capacity(e._capacity), _next(e._next), _count(e._count), _node(e._node) {
		e._capacity = 0;
		e._next = 1;
		e._count = 0;
		e._node = nullptr;
	}

	/*! \brief Destructor
	 */
	virtual ~Elements() {
		clear();
		if (_node != nullptr)
			free(_node);
	}

	/*! \brief Resize element slots to capacity
	 * \param capacity the new capacity
	 * \param reserve additional space to reserve
	 * \return `true` on success, `false` on error
	 */
	bool resize(uint32_t capacity, size_t reserve = 0) {
		void * ptr = realloc(reinterpret_cast<void*>(_node), capacity * sizeof(Node) + reserve);
		if (ptr == nullptr) {
			return false;
		} else {
			_node = reinterpret_cast<Node *>(ptr);
			_capacity = capacity;
			return true;
		}
	}

	/*! \brief get pointer to the reserved space
	 * \return pointer to end of Element nodes space
	 */
	inline void * reserved() const {
		return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(_node) + _capacity * sizeof(Node));
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

	void clear() {
		if (_count > 0) {
			for (size_t i = 0; i < _next; i++)
				if (_node[i].generic.active) {
					_node[i].data.~T();
					_node[i].generic.active = false;
				}
			_next = 1;
			_count = 0;
		}
	}
};
