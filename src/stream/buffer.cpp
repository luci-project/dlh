// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/buffer.hpp>
#include <dlh/string.hpp>
#include <dlh/types.hpp>

const char * BufferStream::str() {
	if (_len < 1)
		return nullptr;
	else if (_pos >= _len)
		_pos = _len - 1;

	_bufptr[_pos] = '\0';

	return _bufptr;
}

BufferStream& BufferStream::writefill(const char* string, size_t precision) {
	// Fill left
	if (!_left && _width > precision)
		write(_fill, _width - precision);

	// Write string
	write_str(string, precision);

	// Fill right
	if (_left && _width > precision)
		write(_fill, _width - precision);

	_width = 0;

	return *this;
}

BufferStream& BufferStream::writefill(long long ival, size_t precision) {
	if ((ival < 0) && (_base == 10))
		return writefill(static_cast<unsigned long long>(-ival), true, precision);
	else
		return writefill(static_cast<unsigned long long>(ival), false, precision);
}

// Print a number.
BufferStream& BufferStream::writefill(unsigned long long ival, bool minus, size_t precision) {
	if (ival == 0 && precision == 0) {
		return write(_fill, _width);
	}

	// Determine the largest potency in the number system used, which is
	// still smaller than the number to be printed
	size_t n = 1;
	unsigned long long div;
	for (div = 1; ival/div >= static_cast<unsigned long long>(_base); div *= _base)
		n++;

	size_t ival_precision = n;
	if (precision != SIZE_MAX && n < precision)
		n = precision;

	// Fill (if necessary)
	bool ignore_oct_prefix = false;
	if (_base == 10 && (minus || _sign != MINUS_ONLY)) {
		n += 1;
	} else if (_base == 8 && _prefix) {
		if (ival_precision != n || ival == 0)
			// special case: we already have a leading zero
			ignore_oct_prefix = true;
		else
			n += 1;
	} else if ((_base == 2 || _base == 16) && _prefix) {
		n += 2;
	}

	size_t pad = _width > n ? _width - n : 0;
	_width = 0;
	if (pad && !_left && (_fill != '0' || (!minus && _sign != PLUS && _base == 10))) {
		write(_fill, pad);
		pad = 0;
	}

	// Print prefix
	switch(_base) {
		case 2:
			if (_prefix) {
				operator<<('0');
				operator<<('b');
			}
			break;

		case 8:
			if (_prefix && !ignore_oct_prefix)
				operator<<('0');
			break;

		case 16:
			if (_prefix) {
				operator<<('0');
				operator<<(_hexchar == 'A' ? 'X' : 'x');
			}
			break;

		default:
			if (minus)
				operator<<('-');
			else if (_sign == PLUS)
				operator<<('+');
			else if (_sign == SPACE)
				operator<<(' ');
	}
	if (precision != SIZE_MAX && ival_precision < precision)
		write('0', precision - ival_precision);

	// Special case: Zeros after prefix (for base != 10)
	if (pad && !_left && _fill == '0' && (minus || _sign == PLUS || _base != 10))
		write(_fill, pad);

	// print number char by char
	for (; div > 0; div /= static_cast<unsigned long long>(_base)) {
		auto digit = ival / div;
		if (digit < 10)
			operator<<(static_cast<char>('0' + digit));
		else
			operator<<(static_cast<char>(_hexchar + digit - 10));

		ival %= div;
	}

	// Fill right
	if (pad && _left)
		write(_fill, pad);

	return *this;
}

BufferStream& BufferStream::write_str(const char* string, size_t limit) {
	for (size_t i = 0; i < limit && string[i] != '\0' ; i++)
		if (_pos + 1 < _len) {
			_bufptr[_pos++] = string[i];
			if (_pos + 1 == _len)
				flush();
		}

	return *this;
}

BufferStream& BufferStream::write(const char* string, size_t n) {
	for (size_t i = 0; i < n; i++)
		if (_pos + 1 < _len) {
			_bufptr[_pos++] = string[i];
			if (_pos + 1 == _len)
				flush();
		}

	return *this;
}

BufferStream& BufferStream::write(char c, size_t n) {
	for (size_t i = 0; i < n; i++)
		if (_pos + 1 < _len) {
			_bufptr[_pos++] = c;
			if (_pos + 1 == _len)
				flush();
		}
	return *this;
}

// Print a single character (trivial)
BufferStream& BufferStream::operator<<(char c) {
	if (_width > 1 && !_left)
		write(_fill, _width - 1);

	if (_pos + 1 < _len)
		_bufptr[_pos++] = c;

	if (_width > 1 && _left)
		write(_fill, _width - 1);
	_width = 0;

	if (_pos + 1 >= _len)
		flush();

	return *this;
}

// Printing a null-terminated string
BufferStream& BufferStream::operator<<(const char* string) {
	if (string != nullptr) {
		if (_width > 0)
			return writefill(string, String::len(string));
		else  // slightly faster
			return write_str(string);
	}
	return *this;
}


// Print a pointer as hexadecimal number
BufferStream& BufferStream::operator<<(const void* ptr) {
	int oldbase = _base;
	_base = 16;
	writefill(reinterpret_cast<unsigned long long>(ptr));
	_base = oldbase;
	return *this;
}


// Calls one of the manipulator functions
BufferStream& BufferStream::operator<<(const setbase & val) {
	switch(val.base) {
		case 2:
		case 8:
		case 10:
		case 16:
			this->_base = val.base;
			break;

		default:
			this->_base = 10;
	}
	return *this;
}

size_t BufferStream::format(const char * format, va_list args) {
	size_t start = _pos;
	auto cache_base = _base;
	auto cache_width = _width;
	auto cache_fill = _fill;
	auto cache_hexchar = _hexchar;
	auto cache_prefix = _prefix;
	auto cache_left = _left;
	auto cache_sign = _sign;

	enum {
		NONE,
		MARKER,
		FLAGS,
		WIDTHSTAR,
		WIDTH,
		PRECISIONDOT,
		PRECISIONSTAR,
		PRECISION,
		LENGTH,
		SPECIFIER
	} state = NONE;

	enum {
		SHORT,
		INT,
		LONG,
		LONGLONG,
	} length = INT;

	size_t precision = SIZE_MAX;
	const char * format_start = nullptr;

	while (*format) {
		switch (state) {
			case MARKER:
				if (*format == '%') {
					format_start = nullptr;
					state = NONE;
					operator<<(*format);
					break;
				} else {
					state = FLAGS;
				}
				[[fallthrough]];

			case FLAGS:
				switch (*format) {
					case '+':
						_sign = PLUS;
						break;
					case '-':
						_left = true;
						break;
					case ' ':
						if (_sign != PLUS)
							_sign = SPACE;
						break;
					case '0':
						_fill = '0';
						break;
					case '#':
						_prefix = true;
						break;
					case 'I':
						/* localization ignored */
						break;
					default:
						if (_left)
							_fill = ' ';  // at "%-0..." and "%0-..." the filler is ignored
						state = WIDTHSTAR;
				}
				if (state != WIDTHSTAR)
					break;
				[[fallthrough]];

			case WIDTHSTAR:
				if (*format == '*') {
					int tmp = va_arg(args, int);
					if (tmp < 0) {
						_width = -tmp;
						_left = true;
					} else {
						_width = tmp;
					}
					state = PRECISIONDOT;
					break;
				} else {
					state = WIDTH;
				}
				[[fallthrough]];

			case WIDTH:
				if (*format >= '0' && *format <= '9') {
					_width = _width * 10 + *format - '0';
					break;
				} else {
					state = PRECISIONDOT;
				}
				[[fallthrough]];

			case PRECISIONSTAR:
				if (*format == '*') {
					int tmp = va_arg(args, int);
					precision = tmp < 0 ? -tmp : tmp;
					state = LENGTH;
					break;
				} else {
					state = PRECISION;
				}
				[[fallthrough]];

			case PRECISION:
				if (*format >= '0' && *format <= '9') {
					precision = precision * 10 + *format - '0';
					break;
				} else {
					state = LENGTH;
				}
				[[fallthrough]];

			case PRECISIONDOT:
				if (*format == '.') {
					precision = 0;
					_fill = ' ';
					state = PRECISIONSTAR;
					break;
				} else {
					state = LENGTH;
				}
				[[fallthrough]];

			case LENGTH:
				switch (*format) {
					case 'h':
						// hh not supported
						length = SHORT;
						break;
					case 'l':
						length = length != LONG ? LONG : LONGLONG;
						break;
					case 'j':
						length = sizeof(__INTMAX_TYPE__) == sizeof(long) ? LONG : LONGLONG;
						break;
					case 'z':
						length = sizeof(size_t) == sizeof(long) ? LONG : LONGLONG;
						break;
					case 't':
						length = sizeof(ptrdiff_t) == sizeof(long) ? LONG : LONGLONG;
						break;
					default:
						state = SPECIFIER;
				}
				if (state != SPECIFIER)
					break;
				[[fallthrough]];

			case SPECIFIER:
				switch (*format) {
					case 'c':
						operator<<(static_cast<char>(va_arg(args, int)));
						format_start = nullptr;
						state = NONE;
						break;

					case 's':
					 {
						const char * str = va_arg(args, const char *);
						if (str == nullptr)
							str = "(null)";
						if (precision != SIZE_MAX)
							writefill(str, precision);
						else
							operator<<(str);
						format_start = nullptr;
						state = NONE;
						break;
					 }

					case 'p':
						if (void * ptr = va_arg(args, void *)) {
							_prefix = true;
							_base = 16;
							writefill(reinterpret_cast<unsigned long long>(ptr));
						} else {
							_fill = ' ';
							operator<<("(nil)");
						}
						format_start = nullptr;
						state = NONE;
						break;

					case 'n':
						*va_arg(args, int *) = _pos - start;
						format_start = nullptr;
						state = NONE;
						break;

					case 'd':
					case 'i':
						switch (length) {
							case SHORT:
							 {
								auto val = static_cast<short>(va_arg(args, int));
								writefill(static_cast<long long>(val), precision);
								break;
							 }

							case LONG:
								writefill(static_cast<long>(va_arg(args, int)), precision);
								break;

							case LONGLONG:
								writefill(va_arg(args, long long), precision);
								break;

							default:
								writefill(static_cast<long long>(va_arg(args, int)), precision);
						}
						format_start = nullptr;
						state = NONE;
						break;

					case 'b':
						_base = 2;
						break;

					case 'o':
						_base = 8;
						break;

					case 'x':
						_hexchar = 'a';
						_base = 16;
						break;

					case 'X':
						_hexchar = 'A';
						_base = 16;
						break;

					case 'u':
						_sign = MINUS_ONLY;
						break;

					default:
						state = NONE;
				}
				if (state != NONE) {
					switch (length) {
						case SHORT:
						 {
							auto val = static_cast<unsigned short>(va_arg(args, unsigned));
							writefill(static_cast<unsigned long long>(val), false, precision);
							break;
						 }

						case LONG:
							writefill(static_cast<unsigned long long>(va_arg(args, unsigned long)), false, precision);
							break;

						case LONGLONG:
							writefill(va_arg(args, unsigned long long), false, precision);
							break;

						default:
							writefill(static_cast<unsigned long long>(va_arg(args, unsigned)), false, precision);
					}
					format_start = nullptr;
					state = NONE;
				}
				break;

			default:
				// Output invalid format strings as text
				while (format_start != nullptr && format_start != format)
					operator<<(*(format_start++));
				if (*format == '%') {
					format_start = format;
					state = MARKER;
					_base = 10;
					_width = 0;
					_fill = ' ';
					_hexchar = 'a';
					_prefix = false;
					_left = false;
					_sign = MINUS_ONLY;
					precision = SIZE_MAX;
					length = INT;
				} else {
					operator<<(*format);
					format_start = nullptr;
				}
		}
		format++;
	}
	// Output remaining invalid format strings as text
	while (format_start != nullptr && format_start != format)
		operator<<(*(format_start++));

	// Reset modifiers
	_base = cache_base;
	_width = cache_width;
	_fill = cache_fill;
	_hexchar = cache_hexchar;
	_prefix = cache_prefix;
	_left = cache_left;
	_sign = cache_sign;

	size_t n = _pos - start;
	// 0 Byte
	str();
	flush();
	return n;
}
