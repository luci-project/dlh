// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/types.hpp>
#include <dlh/stdarg.hpp>

/*! \brief Sets the base for numbers
 * Shortcut functions are `bin`, `oct`, `dec` and `hex`
 */
struct setbase {
	uint8_t base;
	explicit setbase(uint8_t base) : base(base) {}
};

/*! \brief Sets the field width to be used on output operations.
 * \note use zero to disable field width (no padding)
 */
struct setw {
	uint8_t n;
	explicit setw(uint8_t n = 0) : n(n) {}
};

/*! \brief Sets the fill character
 */
struct setfill {
	const char c;
	explicit setfill(char c) : c(c) {}
};


/*! \brief BufferStream class */
class BufferStream {
	/*! \brief Number system used for printing integral numbers (one of 2,
	 *  8, 10, or 16)
	 */
	uint8_t _base;

	/*! \brief Field width for next input */
	uint8_t _width;

	/*! \brief Fill character */
	char _fill;

	/*! \brief Upper or lowercase hex character */
	char _hexchar;

	/*! Print base prefix */
	bool _prefix;

	/*! Numbers are left aligned */
	bool _left;

	/*! Show sign for numbers (base 10) */
	enum { MINUS_ONLY, PLUS, SPACE } _sign;

	/*! \brief Helper to write a string with fill characters (if necessary) */
	BufferStream& writefill(const char* string, size_t precision);

	/*! \brief Helper to write an number with fill characters (if necessary) */
	BufferStream& writefill(long long ival, size_t precision = SIZE_MAX);

	/*! \brief Helper to write an unsigned number with fill characters (if necessary) */
	BufferStream& writefill(unsigned long long ival, bool minus = false, size_t precision = SIZE_MAX);

 protected:
	/*! \brief Buffer */
	char * _bufptr;

	/*! \brief Length of buffer */
	size_t _len;

	/*! \brief Current position of buffer */
	size_t _pos;

 public:
	/*! \brief Default constructor. Initial number system is decimal. */
	BufferStream(char * buffer, size_t len) : _bufptr(buffer), _len(buffer == nullptr ? 0 : len), _pos(0) {
		clear();
	}

	/*! \brief Constructor implicitly determining size */
	template<size_t L>
	explicit BufferStream(char buffer[L]) : BufferStream(&buffer, L) {}

	/*! \brief Destructor */
	virtual ~BufferStream() { flush(); }

	/*! \brief get current string
	 * \return pointer to current (!) null byte terminated buffer (might be invalid after next stream operation)
	 */
	const char * str();

	/*! \brief get current string
	 * \param len will contain the length of the string (excluding null byte character)
	 * \return pointer to current (!) null byte terminated buffer
	 */
	const char * str(size_t & len) {
		const char * buf = str();  // might change _pos
		len = _len < 1 ? 0 : _pos;
		return buf;
	}


	/*! \brief Flush the buffer.
	 * (only for derived classes, has no effect in BufferStream)
	 */
	virtual void flush() {}

	/*! \brief clear the buffer.
	 */
	void clear() {
		reset();
		_pos = 0;
		if (_len > 0)
			_bufptr[_len - 1] = '\0';
	}

	/*! \brief set alignment for fields with specified width
	 * \param left `true` for right-, `false` for left alignment
	 */
	void align(bool right) {
		_left = !right;
	}

	/*! \brief Enable/disable number prefix (for base != 10)
	 * \param show `true` to enable number prefix
	 */
	void prefix(bool show) {
		_prefix = show;
	}

	/*! \brief Reset modifiers */
	void reset() {
		_base = 10;
		_width = 0;
		_fill = ' ';
		_hexchar = 'a';
		_prefix = false;
		_left = true;
		_sign = MINUS_ONLY;
	}

	/*! \brief Write string (including all characters, even `\0`) into buffer
	 * \param string pointer to string with at least `n` bytes
	 * \param n number of bytes to copy from string into buffer
	 * \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& write(const char* string, size_t size);

	/*! \brief Write padding character multiple times into buffer
	 * \param c padding character
	 * \param n number of times the character should be written into buffer
	 * \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& write(char c, size_t num);


	/*! \brief Write string (up to a given limit) into buffer
	 * \param string pointer to a '\0' terminated string
	 * \param limit maximum number of bytes to copy from string into buffer
	 * \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& write_str(const char* string, size_t limit = SIZE_MAX);

	/*! \brief Print a single character
	 *
	 *  \param c Character to be printed
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(char c);

	/*! \brief Print a single character
	 *
	 *  \param c Character to be printed
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(unsigned char c) {
		return *this << static_cast<char>(c);
	}

	/*! \brief Printing a null-terminated string
	 *
	 *  \param string String to be printed
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(const char* string);

	/*! \brief Print a boolean value
	 *
	 *  \param b Boolean to be printed
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(bool b) {
		return b ? writefill("true", 4) : writefill("false", 5);
	}

	/*! \brief Print an integral number in radix base
	 *
	 *  \param ival Number to be printed
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(short ival) {
		return writefill(static_cast<long long>(ival));
	}

	/// \copydoc BufferStream::operator<<(short)
	BufferStream& operator<<(unsigned short ival) {
		return writefill(static_cast<unsigned long long>(ival));
	}

	/// \copydoc BufferStream::operator<<(short)
	BufferStream& operator<<(int ival) {
		return writefill(static_cast<long long>(ival));
	}

	/// \copydoc BufferStream::operator<<(short)
	BufferStream& operator<<(unsigned int ival) {
		return writefill(static_cast<unsigned long long>(ival));
	}

	/// \copydoc BufferStream::operator<<(short)
	BufferStream& operator<<(long ival) {
		return writefill(static_cast<long long>(ival));
	}

	/// \copydoc BufferStream::operator<<(short)
	BufferStream& operator<<(unsigned long ival) {
		return writefill(static_cast<unsigned long long>(ival));
	}

	/// \copydoc BufferStream::operator<<(short)
	BufferStream& operator<<(long long ival) {
		return writefill(ival);
	}

	/// \copydoc BufferStream::operator<<(short)
	BufferStream& operator<<(unsigned long long ival) {
		return writefill(ival);
	}

	/*! \brief Print a pointer as hexadecimal number
	 *
	 *  \param ptr Pointer to be printed
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(const void* ptr);

	/*! \brief Calls one of the manipulator functions.
	 *
	 *  Method that calls the manipulator functions defined below, which
	 *  allow modifying the stream's behavior by, for instance, changing the
	 *  number system.
	 *  \param f Manipulator function to be called
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(BufferStream& (*f) (BufferStream&)) {
		return f(*this);
	}

	/*! \brief Base manipulator
	 *  \param val Manipulator object to be used
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(const setbase & val);

	/*! \brief Field width manipulator
	 *  \param val Manipulator object to be used
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(const setw & val) {
		_width = val.n;
		return *this;
	}

	/*! \brief Padding character manipulator
	 *  \param val Manipulator object to be used
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(const setfill & val) {
		_fill = val.c;
		return *this;
	}

	/*! \brief Allow Stream embedding
	 *
	 *  \param bs (other) Bufferstream)
	 *  \return Reference to BufferStream; allows operator chaining.
	 */
	BufferStream& operator<<(BufferStream & bs) {
		if (&bs != this)
			*this << bs.str();
		return * this;
	}

	/*! \brief Write format string
	 * \note Supports only a subset of printf format string
	 * \param format Format string
	 * \return Bytes written
	 */
	size_t format(const char * format, ...) {
		va_list args;
		va_start(args, format);
		size_t ret = this->format(format, args);
		va_end(args);
		return ret;
	}

	/*! \brief Write format string
	 * \note Supports only a subset of printf format string
	 * \param format Format string
	 * \param args Argument list
	 * \return Bytes written
	 */
	size_t format(const char * format, va_list args);
};

/*! \brief Enforces a buffer flush.
 *  \param os Reference to stream to be flushed.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& flush(BufferStream& os) {
	os.flush();
	return os;
}

/*! \brief Prints a newline character to the stream and issues a buffer flush.
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& endl(BufferStream& os) {
	return os << '\n' << flush;
}

/*! \brief Print subsequent numbers in binary form.
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& bin(BufferStream& os) {
	return os << setbase(2);
}

/*! \brief Print subsequent numbers in octal form.
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& oct(BufferStream& os) {
	return os << setbase(8);
}

/*! \brief Print subsequent numbers in decimal form.
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& dec(BufferStream& os) {
	return os << setbase(10);
}

/*! \brief Print subsequent numbers in hex form.
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& hex(BufferStream& os) {
	return os << setbase(16);
}

/*! \brief Align output with fixed length to the left
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& left(BufferStream& os) {
	os.align(false);
	return os;
}

/*! \brief Align output with fixed length to the right
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& right(BufferStream& os) {
	os.align(true);
	return os;
}

/*! \brief Enable number prefix (for base != 10)
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& prefix(BufferStream& os) {
	os.prefix(true);
	return os;
}

/*! \brief Disable number prefix
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& noprefix(BufferStream& os) {
	os.prefix(false);
	return os;
}

/*! \brief Set settings to default
 *  \param os Reference to stream to be modified.
 *  \return Reference to BufferStream os; allows operator chaining.
 */
inline BufferStream& reset(BufferStream& os) {
	os.reset();
	return os;
}
