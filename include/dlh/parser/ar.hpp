#pragma once

#include <dlh/types.hpp>

class AR {
	char * data;
	size_t size;
	const char * extended_names = nullptr;
	size_t extended_names_size = 0;

 public:
	class Entry {
		struct Header;
		friend class AR;

		AR & archive;
		Header * header;

		Entry(AR & archive, char * start);

		void read_extended_filenames();

		bool is_out_of_range() const;

	 public:
		bool is_symbol_table() const;

		bool is_extended_filenames() const;

		bool is_special() const;

		bool is_valid() const;

		bool is_regular() const;

		const char * name() const;

		size_t size() const;

		void * data() const;

		size_t offset() const;

		Entry& operator++();

		Entry operator++(int);

		bool operator!=(const Entry& other) const;

		Entry& operator*();
	};

	AR(char * data, size_t size);

	AR(const char * filename);

	bool is_valid() const;

	Entry begin();

	Entry end();
};
