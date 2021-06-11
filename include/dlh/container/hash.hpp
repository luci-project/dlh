#pragma once


#include <dlh/assert.hpp>
#include <dlh/string.hpp>
#include <dlh/utility.hpp>
#include <dlh/utils/comparison.hpp>
#include <dlh/container/optional.hpp>
#include <dlh/container/pair.hpp>
#include <dlh/container/internal/elements.hpp>
#include <dlh/container/internal/keyvalue.hpp>

/*! \brief Hash set
 * influenced by standard [template/cxx] librarys `unordered_set`
 * \tparam T type for container
 * \tparam C structure with comparison (`bool equal(const T&, const T&)`)
 *           and hash (`uint32_t hash(const T&)`) functions
 * \tparam L percentage of hash buckets (compared to element capacity)
 */
template<typename T, typename C = Comparison, size_t L = 150>
class HashSet : protected Elements<T> {
 protected:
	using typename Elements<T>::_capacity;
	using typename Elements<T>::_next;
	using typename Elements<T>::_count;
	using typename Elements<T>::_node;

	/*! \brief Hash bucket capacity */
	uint32_t _bucket_capacity = 0;

	/*! \brief Pointer to start of hash bucket array */
	uint32_t * _bucket = nullptr;

 public:
	using typename Elements<T>::Node;

	/*! \brief Create new hash set
	 * \return capacity initial capacity
	 */
	explicit HashSet(size_t capacity = 0) {
		resize(capacity);
		assert(empty() || !Elements<T>::_node[0].hash.active);
	}

	/*! \brief Convert to hash set
	 * \return set Elements container
	 */
	HashSet(const HashSet<T>& set) : Elements<T>(set), _bucket_capacity(set._bucket_capacity), _bucket(nullptr) {
		const size_t size = _bucket_capacity * sizeof(uint32_t);
		if (size > 0) {
			_bucket = reinterpret_cast<uint32_t *>(malloc(size));
			assert(_bucket != nullptr);
			memcpy(_bucket, set._bucket, size);
			assert(!Elements<T>::_node[0].hash.active);
		}
	}

	/*! \brief Convert to hash set
	 * \return elements Elements container
	 */
	HashSet(const Elements<T>& elements) : Elements<T>(elements) {
		rehash();
		assert(empty() || !Elements<T>::_node[0].hash.active);
	}

	/*! \brief Convert to hash set
	 * \return elements Elements container
	 */
	HashSet(HashSet<T, C, L> &&) = default;

	/*! \brief Convert to hash set
	 * \return elements Elements container
	 */
	HashSet(Elements<T>&& elements) : Elements<T>(move(elements)) {
		rehash();
		assert(empty() || !Elements<T>::_node[0].hash.active);
	}

	/*! \brief Copy assignment operator
	 * \param set HashSet to copy
	 */
	HashSet<T, C, L> & operator=(const HashSet<T, C, L> & set) {
		Elements<T>::operator=(set);
		if ((_bucket_capacity = set._bucket_capacity) > 0) {
			const size_t size = _bucket_capacity * sizeof(uint32_t);
			_bucket = reinterpret_cast<uint32_t *>(malloc(size));
			assert(_bucket != nullptr);
			memcpy(_bucket, set._bucket, size);
			assert(!Elements<T>::_node[0].hash.active);
		} else {
			_bucket = nullptr;
		}
		return *this;
	}

	/*! \brief Default move assignment operator
	 */
	HashSet<T, C, L> & operator=(HashSet<T, C, L> &&) = default;

	/*! \brief Range constructor
	 * \param first First element in range
	 * \param last Last element in range
	 * \param initial_capacity space to reserve
	 */
	template<typename I>
	explicit HashSet(I first, I last, size_t initial_capacity = 0) : HashSet(initial_capacity) {
		for (I i = first; i < last; ++i)
			emplace(*i);
		assert(empty() || !Elements<T>::_node[0].hash.active);
	}

	/*! \brief Destructor
	 */
	virtual ~HashSet() {
		clear();
		free(_bucket);
	}

	/*! \brief hash set iterator
	 */
	class Iterator {
		friend class HashSet<T, C>;
		HashSet<T, C> &ref;
		uint32_t i;

		Iterator(HashSet<T, C> &ref, uint32_t p) : ref(ref), i(p) {}

	 public:
		Iterator& operator++() {
			do {
				i++;
			} while (i < ref._next && !ref._node[i].hash.active);
			return *this;
		}

		inline T& operator*() {
			assert(ref._node[i].hash.active);
			return ref._node[i].data;
		}

		inline const T& operator*() const {
			assert(ref._node[i].hash.active);
			return ref._node[i].data;
		}

		inline T* operator->() {
			assert(ref._node[i].hash.active);
			return &(ref._node[i].data);
		}

		inline const T* operator->() const {
			assert(ref._node[i].hash.active);
			return &(ref._node[i].data);
		}

		inline bool operator==(const Iterator& other) const {
			return &ref == &other.ref && i == other.i;
		}

		template<typename O>
		inline bool operator==(const O& other) const {
			return C::equal(ref.element[i].data, other);
		}

		inline bool operator!=(const Iterator& other) const {
			return &ref != &other.ref || i != other.i;
		}

		template<typename O>
		inline bool operator!=(const T& other) const {
			return !C::equal(ref.element[i].data, other);
		}

		inline operator bool() const {
			return i != ref._next;
		}
	};

	/*! \brief hash set iterator
	 */
	class ConstIterator {
		friend class HashSet<T, C>;
		const HashSet<T, C> &ref;
		mutable uint32_t i;

		ConstIterator(const HashSet<T, C> &ref, uint32_t p) : ref(ref), i(p) {}

	 public:
		const ConstIterator& operator++() const {
			do {
				i++;
			} while (i < ref._next && !ref._node[i].hash.active);
			return *this;
		}

		inline const T& operator*() const {
			assert(ref._node[i].hash.active);
			return ref._node[i].data;
		}

		inline const T* operator->() const {
			assert(ref._node[i].hash.active);
			return &(ref._node[i].data);
		}

		inline bool operator==(const Iterator& other) const {
			return &ref == &other.ref && i == other.i;
		}

		inline bool operator==(const ConstIterator& other) const {
			return &ref == &other.ref && i == other.i;
		}

		template<typename O>
		inline bool operator==(const O& other) const {
			return C::equal(ref.element[i].data, other);
		}

		inline bool operator!=(const Iterator& other) const {
			return &ref != &other.ref || i != other.i;
		}

		inline bool operator!=(const ConstIterator& other) const {
			return &ref != &other.ref || i != other.i;
		}

		template<typename O>
		inline bool operator!=(const T& other) const {
			return !C::equal(ref.element[i].data, other);
		}

		inline operator bool() const {
			return i != ref._next;
		}
	};


	/*! \brief Get iterator to first element
	 * \return iterator to first valid element (if available) or `end()`
	 */
	inline Iterator begin() {
		if (empty()) {
			return end();
		} else {
			uint32_t i = 1;
			while (i < Elements<T>::_next && !Elements<T>::_node[i].hash.active)
				i++;
			return { *this, i };
		}
	}

	/*! \brief Get iterator to end (post-last-element)
	 * \return iterator to end (first invalid element)
	 */
	inline Iterator end() {
		return { *this, Elements<T>::_next };
	}

	/*! \brief Get iterator to first element
	 * \return iterator to first valid element (if available) or `end()`
	 */
	inline ConstIterator begin() const {
		if (empty()) {
			return end();
		} else {
			uint32_t i = 1;
			while (i < Elements<T>::_next && !Elements<T>::_node[i].hash.active)
				i++;
			return { *this, i };
		}
	}

	/*! \brief Get iterator to end (post-last-element)
	 * \return iterator to end (first invalid element)
	 */
	inline ConstIterator end() const {
		return { *this, Elements<T>::_next };
	}

	/*! \brief Create new element into set
	 * \param args Arguments to construct element
	 * \return iterator to the new element (`first`) and
	 *         indicator if element was created (`true`) or has already been in the set (`false`)
	 */
	template<typename... ARGS>
	Pair<Iterator,bool> emplace(ARGS&&... args) {
		increase();

		// Create local element (not active yet!)
		auto & next = Elements<T>::_node[Elements<T>::_next];
		new (&next.data) T(forward<ARGS>(args)...);
		uint32_t * b = bucket(next.hash.temp = C::hash(next.data));

		// Check if already in set
		uint32_t i = find_in(b, next.data);
		if (i != Elements<T>::_next)
			return { Iterator(*this, i), false };

		// Insert
		return insert(Elements<T>::_next++, b);
	}

	/*! \brief Insert element into set
	 * \param value new element to be inserted
	 * \return iterator to the inserted element (`first`) and
	 *         indicator (`second`) if element was created (`true`) or has already been in the set (`false`)
	 */
	Pair<Iterator,bool> insert(const T &value) {
		increase();

		// Get Bucket
		uint32_t h = C::hash(value);
		uint32_t * b = bucket(h);

		// Check if already in set
		uint32_t i = find_in(b, value);
		if (i != Elements<T>::_next)
			return { Iterator(*this, i), false };

		// Insert at position
		auto & next = Elements<T>::_node[Elements<T>::_next];
		next.hash.temp = h;
		new (&next.data) T(value);
		return insert(Elements<T>::_next++, b);
	}

	/*! \brief Insert element node into set
	 * \param value Node of element to be inserted
	 * \return iterator to the inserted element (`first`) and
	 *         indicator (`second`) if element was created (`true`) or has already been in the set (`false`)
	 */
	Pair<Iterator,bool> insert(Node &&value) {
		uint32_t h = C::hash(value.data);
		uint32_t * b = bucket(h);

		uint32_t i = find_in(b, value.data);
		if (i != Elements<T>::_next) {
			return { Iterator(*this, i), false };
		} else {
			size_t n = 0;
			if (!Elements<T>::is_node(value, n) || n == 0)
				new (&Elements<T>::_node[n = Elements<T>::_next++].data) T(move(value.data));
			Elements<T>::_node[n].hash.temp = h;
			return insert(n, b);
		}
	}


	/*! \brief Get iterator to specific element
	 * \param value element
	 * \return iterator to element (if found) or `end()` (if not found)
	 */
	template<typename U>
	inline Iterator find(const U& value) {
		return empty() ? end() : Iterator(*this, find_in(bucket(C::hash(value)), value));
	}

	/*! \brief check if set contains element
	 * \param value element
	 * \return `true` if element is in set
	 */
	template<typename U>
	inline bool contains(const U& value) const {
		return empty() ? false : (find_in(bucket(C::hash(value)), value) != Elements<T>::_next);
	}


	/*! \brief Extract value from set
	 * \param position iterator to element (must be valid)
	 * \return removed Node
	 */
	Node && extract(const Iterator & position) {
		assert(position.i >= 1 && position.i < Elements<T>::_next && Elements<T>::_node[position.i].hash.active);
		auto & e = Elements<T>::_node[position.i];

		e.hash.active = false;

		uint32_t * b = bucket(e.hash.temp);
		assert(*b != 0);

		auto next = e.hash.next;
		auto prev = e.hash.prev;

		if (next != 0) {
			assert(Elements<T>::_node[next].hash.active);
			assert(Elements<T>::_node[next].hash.prev == position.i);
			Elements<T>::_node[next].hash.prev = prev;
		}

		if (*b == position.i) {
			assert(prev == 0);
			*b = next;
		} else if (prev != 0) {
			assert(Elements<T>::_node[prev].hash.active);
			assert(Elements<T>::_node[prev].hash.next == position.i);
			Elements<T>::_node[prev].hash.next = next;
		}
		Elements<T>::_count--;
		return move(Elements<T>::_node[position.i]);
	}

	/*! \brief Remove value from set
	 * \param position iterator to element
	 * \return removed value (if valid iterator)
	 */
	Optional<T> erase(const Iterator & position) {
		if (position.i >= 1 && position.i < Elements<T>::_next && Elements<T>::_node[position.i].hash.active)
			return { move(extract(position).data) };
		else
			return {};
	}

	/*! \brief Remove value from set
	 * \param value element to be removed
	 * \return removed value (if found)
	 */
	template<typename U>
	inline Optional<T> erase(const U& value) {
		auto position = find(value);
		if (position.i >= 1 && position.i < Elements<T>::_next && Elements<T>::_node[position.i].hash.active)
			return { move(extract(position).data) };
		else
			return {};
	}

	/*! \brief Recalculate hash values
	 */
	inline void rehash() {
		if (!empty())
			bucketize(false, true);
	}

	/*! \brief Reorganize Elements
	 * Fill gaps emerged from erasing elments
	 */
	inline void reorganize() {
		if (!empty() && reorder())
			bucketize();
	}

	/*! \brief Resize set capacity
	 * \param capacity new capacity (has to be equal or greater than `size()`)
	 * \return `true` if resize was successfully, `false` otherwise
	 */
	bool resize(size_t capacity) {
		if (capacity <= Elements<T>::_count || capacity > UINT32_MAX)
			return false;

		// reorder node slots
		bool need_bucketize = reorder();
		bool buckets_zeroed = false;

		bool s = capacity == Elements<T>::_capacity;
		// Resize element container
		if (!s && Elements<T>::resize(capacity)) {
			// NULL Element (let's waste it)
			Elements<T>::_node[0].hash.active = false;

			// resize hash buckets
			size_t bucket_cap = capacity * L / 100;
			if (bucket_cap >= UINT32_MAX)
				bucket_cap = UINT32_MAX - 1;

			auto bucket_ptr = reinterpret_cast<uint32_t *>(calloc(sizeof(uint32_t), bucket_cap));
			if (bucket_ptr != nullptr) {
				free(_bucket);
				_bucket = bucket_ptr;
				_bucket_capacity = bucket_cap;
				s = buckets_zeroed = need_bucketize = true;
			}

		}

		// Bucketize (if either reordering or resizing of hash buckets was successful)
		if (need_bucketize)
			bucketize(buckets_zeroed);

		return s;
	}

	/*! \brief Test whether container is empty
	 * \return true if set is empty
	 */
	inline bool empty() const {
		return Elements<T>::_count == 0;
	}

	/*! \brief Element count
	 * \return Number of (unique) elements in set
	 */
	inline size_t size() const {
		return Elements<T>::_count;
	}

	/*! brief Used buckets
	 * \return Number of non-empty hash buckets (<= `size()`)
	 */
	size_t bucket_count() const {
		size_t i = 0;
		for (size_t c = 0; c < _bucket_capacity; c++)
			if (_bucket[c] != 0)
				i++;
		return i;
	}

	/*! \brief Available buckets
	 * \return Number of available buckets
	 */
	inline size_t bucket_size() const {
		return _bucket_capacity;
	}

	/*! \brief Clear all elements in set */
	void clear() {
		for (size_t i = 1; i < Elements<T>::_next; i++)
			if (Elements<T>::_node[i].hash.active)
				Elements<T>::_node[i].data.~T();

		Elements<T>::_next = 1;
		Elements<T>::_count = 0;

		memset(_bucket, 0, sizeof(uint32_t) * _bucket_capacity);
	}

 private:
	/*! \brief Increase capacity (by reordering or resizing) if required
	 * \return `false` on error
	 */
	inline bool increase() {
		if (Elements<T>::_capacity == 0) {
			if (!resize(16))
				return false;
		} else if (Elements<T>::_next >= Elements<T>::_capacity) {
			if (Elements<T>::_count * 2 <= Elements<T>::_capacity)
				reorganize();
			else if (!resize(Elements<T>::_capacity * 2))
				return false;
		}
		return true;
	}

	/*! \brief Get bucket for hash value
	 * \param h hash value
	 * \return pointer to bucket
	 */
	inline uint32_t * bucket(uint32_t h) const {
		return _bucket + (h % _bucket_capacity);
	}

	/*! \brief Reorder elements
	 * \return `true` if elements are in a different order (and have to be `bucketize`d again!)
	 */
	bool reorder() {
		if (Elements<T>::_count + 1 < Elements<T>::_next) {
			size_t j = Elements<T>::_next - 1;
			for (size_t i = 1; i <= Elements<T>::_count; i++) {
				if (!Elements<T>::_node[i].hash.active) {
					for (; !Elements<T>::_node[j].hash.active; --j)
						assert(j > i);
					new (&Elements<T>::_node[i].data) T(move(Elements<T>::_node[j].data));
					Elements<T>::_node[i].hash.active = true;
					Elements<T>::_node[j--].hash.active = false;
				}
			}
			Elements<T>::_next = Elements<T>::_count + 1;

			return true;
		} else {
			return false;
		}
	}

	/*! \brief Reorganize buckets
	 * Required if bucket capacity has changed
	 * \param is_zero are the bucket slots already zeroed?
	 * \param rehash calculate hash value (replacing cached value)
	 */
	void bucketize(bool is_zero = false, bool rehash = false) {
		if (!is_zero)
			memset(_bucket, 0, _bucket_capacity * sizeof(uint32_t));

		for (size_t i = 1; i <= Elements<T>::_count; i++) {
			if (Elements<T>::_node[i].hash.active) {
				if (rehash)
					Elements<T>::_node[i].hash.temp = C::hash(Elements<T>::_node[i].data);

				uint32_t * b = bucket(Elements<T>::_node[i].hash.temp);
				assert(*b <= Elements<T>::_count);
				Elements<T>::_node[i].hash.prev = 0;
				if ((Elements<T>::_node[i].hash.next = *b) != 0) {
					assert(Elements<T>::_node[*b].hash.active);
					assert(Elements<T>::_node[*b].hash.prev == 0);
					Elements<T>::_node[*b].hash.prev = i;
				}
				*b = i;
			}
		}
	}

	/*! \brief Find value in bucket (helper)
	 * \param bucket bucket determined by value hash
	 * \param value the value we are looking for
	 * \return index of target value or `Elements<T>::_next` if not found
	 */
	template<typename U>
	inline uint32_t find_in(uint32_t * bucket, const U &value) const {
		// Find
		for (uint32_t i = *bucket; i != 0; i = Elements<T>::_node[i].hash.next) {
			assert(Elements<T>::_count < Elements<T>::_next);
			assert(i < Elements<T>::_next);
			assert(Elements<T>::_node[i].hash.active);
			if (C::equal(Elements<T>::_node[i].data, value))
				return i;
		}
		// End (not found)
		return Elements<T>::_next;
	}


	/*! \brief Insert helper
	 * \param element index of element to insert
	 * \param b pointer to target bucket
	 */
	inline Pair<Iterator,bool> insert(uint32_t element, uint32_t * b) {
		Elements<T>::_node[element].hash.active = true;
		Elements<T>::_node[element].hash.prev = 0;

		Elements<T>::_count++;

		// Bucket not empty?
		if ((Elements<T>::_node[element].hash.next = *b) != 0) {
			assert(Elements<T>::_node[*b].hash.active);
			assert(Elements<T>::_node[*b].hash.prev == 0);
			Elements<T>::_node[*b].hash.prev = element;
		}

		// Assign bucket
		*b = element;

		// return Iterator
		return { Iterator(*this, element), true };
	}

};

/*! \brief Hash map
 * \tparam K type for key
 * \tparam V type for value
 * \tparam C structure with comparison (`bool equal(const K&, const K&)`)
 *           and hash (`uint32_t hash(const K&)`) functions
 * \tparam L percentage of hash buckets (compared to element capacity)
 */
template<typename K, typename V, typename C = Comparison, size_t L = 150>
class HashMap : protected HashSet<KeyValue<K,V>, C, L> {
	using Base = HashSet<KeyValue<K,V>, C, L>;

 public:
	using typename Base::Iterator;
	using typename Base::begin;
	using typename Base::end;
	using typename Base::find;
	using typename Base::contains;
	using typename Base::resize;
	using typename Base::rehash;
	using typename Base::empty;
	using typename Base::size;
	using typename Base::bucket_size;
	using typename Base::bucket_count;
	using typename Base::clear;

	/*! \brief Insert element */
	inline Pair<Iterator,bool> insert(const K& key, const V& value) {
		return Base::insert(KeyValue<K,V>(key, value));
	}

	inline Pair<Iterator,bool> insert(K&& key, V&& value) {
		return Base::insert(KeyValue<K,V>(move(key), move(value)));
	}

	inline Optional<V> erase(const Iterator & position) {
		auto i = Base::erase(position);
		if (i)
			return { move(i.value().value) };
		else
			return {};
	}

	template<typename O>
	inline Optional<V> erase(const O& key) {
		auto i = Base::erase(key);
		if (i)
			return { move(i.value().value) };
		else
			return {};
	}

	template<typename O>
	inline Optional<V> at(const O& key) {
		auto i = Base::find(key);
		if (i)
			return { move(i.value().value) };
		else
			return {};
	}

	template<typename O>
	inline Optional<V> at(O&& key) {
		auto i = Base::find(move(key));
		if (i)
			return { move(i.value().value) };
		else
			return {};
	}

	template<typename O>
	inline V & operator[](const O& key) {
		return (*(Base::insert(key).first)).value;
	}

	template<typename O>
	inline V & operator[](O&& key) {
		return (*(Base::insert(move(key).first))).value;
	}
};
