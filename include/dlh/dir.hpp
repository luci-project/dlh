// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/syscall.hpp>
#include <dlh/log.hpp>

template<size_t BUFFER_SIZE = 2096>
class Directory {
	char buffer[BUFFER_SIZE];
	int fd = -1;

 public:
	class Entry {
		friend class Directory;
		char * buffer;
		dirent * cur;
		dirent * end;
		const int fd;

		Entry(char * buffer, int fd) : buffer(buffer), cur(nullptr), end(nullptr), fd(fd) {
			if (buffer != nullptr && fd != -1) {
				dirent * init = reinterpret_cast<dirent*>(buffer);
				init->d_ino = 0;
				init->d_off = 0;
				init->d_reclen = 0;
				next();
			}
		}

		bool next() {
			if (buffer == nullptr || fd == -1)
				return false;
			if (auto getdents = Syscall::getdents(fd, buffer, BUFFER_SIZE)) {
				if (getdents.value() > 0) {
					cur = reinterpret_cast<dirent*>(buffer);
					end = reinterpret_cast<dirent*>(buffer + getdents.value());
				} else {
					cur = end = nullptr;
				}
				return true;
			} else {
				LOG_WARNING << "Reading directory entry failed: " << getdents.error_message() << endl;
				cur = end = nullptr;
				return false;
			}
		}

	 public:
		bool valid() const {
			return cur != nullptr;
		}

		const char * name() const {
			return cur == nullptr ? nullptr : cur->d_name;
		}

		dir_type_t type() const {
			return cur == nullptr ? DT_UNKNOWN : cur->d_type;
		}

		inline bool is_block_device() const {
			return type() == DT_BLK;
		}

		inline bool is_char_device() const {
			return type() == DT_CHR;
		}

		inline bool is_directory() const {
			return type() == DT_DIR;
		}

		inline bool is_named_pipe() const {
			return type() == DT_FIFO;
		}

		inline bool is_symlink() const {
			return type() == DT_LNK;
		}

		inline bool is_regular() const {
			return type() == DT_REG;
		}

		inline bool is_socket() const {
			return type() == DT_SOCK;
		}

		inline operator bool() const {
			return valid();
		}

		Entry& operator++() {
			auto ptr = reinterpret_cast<dirent*>(reinterpret_cast<char*>(cur) + cur->d_reclen);
			if (ptr >= end)
				next();
			else
				cur = ptr;
			return *this;
		}

		Entry& operator*() {
			return *this;
		}
	};

	explicit Directory(const char * path) {
		if (auto open = Syscall::open(path, O_RDONLY|O_DIRECTORY|O_CLOEXEC)) {
			fd = open.value();
		} else {
			LOG_WARNING << "Unable to open '" << path << "': " << open.error_message() << endl;
		}
	}

	~Directory() {
		if (fd != -1)
			Syscall::close(fd);
	}

	bool valid() {
		return fd != -1;
	}

	Entry begin() {
		return Entry(buffer, fd);
	}

	Entry end() {
		return Entry(nullptr, -1);
	}
};
