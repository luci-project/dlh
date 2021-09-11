#include <dlh/stream/buffer.hpp>
#include <dlh/types.hpp>
#include <dlh/string.hpp>

const char * BufferStream::str() {
	if (_len < 1)
		return nullptr;
	else if (_pos >= _len)
		_pos = _len - 1;

	_bufptr[_pos] = '\0';

	return _bufptr;
}

BufferStream& BufferStream::writefill(const char* string, size_t n) {
	// Fill left
	if (!_left && _width > n)
		write(_fill, _width - n);

	// Write string
	write(string, n);

	// Fill right
	if (_left && _width > n)
		write(_fill, _width - n);

	_width = 0;

	return *this;
}

// Print a number.
BufferStream& BufferStream::writefill(unsigned long long ival, bool minus) {
	// Determine the largest potency in the number system used, which is
	// still smaller than the number to be printed
	size_t n = 1;
	unsigned long long div;
	for (div = 1; ival/div >= static_cast<unsigned long long>(_base); div *= _base)
		n++;

	// Fill (if necessary)
	if ((_base == 10 && (minus || _sign != MINUS_ONLY)) || (_base == 8 && _prefix))
		n += 1;
	else if ((_base == 2 || _base == 16) && _prefix)
		n += 2;

	size_t pad = _width > n ? _width - n : 0;
	_width = 0;
	if (pad && !_left && (_fill != '0' || _base == 10)) {
		write(_fill, pad);
		pad = 0;
	}

	// Print prefix
	switch(_base) {
		case 2:
			if (_prefix)
				operator<<("0b");
			break;

		case 8:
			if (_prefix)
				operator<<('0');
			break;

		case 16:
			if (_prefix)
				operator<<("0x");
			break;

		default:
			if (minus)
				operator<<('-');
			else if (_sign == PLUS)
				operator<<('+');
			else if (_sign == SPACE)
				operator<<(' ');
	}
	// Special case: Zeros after prefix (for base != 10)
	if (pad && !_left && _fill == '0' && _base != 10)
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
	if (_width > 1)
		write(_fill, _width - 1);
	_width = 0;

	if (_pos + 1 < _len)
		_bufptr[_pos++] = c;
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
			while ((*string) != '\0' && _pos + 1 < _len) {
				_bufptr[_pos++] = *(string++);
				if (_pos + 1 == _len)
					flush();
			}
	}
	return *this;
}


// Print a pointer as hexadecimal number
BufferStream& BufferStream::operator<<(const void* ptr) {
	int oldbase = _base;
	_base = 16;
	*this << reinterpret_cast<unsigned long long>(ptr);
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
	enum {
		NONE,
		MARKER,
		FLAGS,
		WIDTHSTAR,
		WIDTH,
		LENGTH,
		SPECIFIER
	} state = NONE;
	enum {
		SHORT,
		INT,
		LONG,
		LONGLONG,
	} length = INT;
	while (*format) {
		switch (state) {
			case MARKER:
				if (*format == '%') {
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
						_sign = SPACE;
						break;
					case '0':
						_fill = '0';
						break;
					case '#':
						_prefix = true;
						break;
					default:
						state = WIDTHSTAR;
				}
				if (state != WIDTHSTAR)
					break;
				[[fallthrough]];

			case WIDTHSTAR:
				if (*format == '*') {
					_width = va_arg(args, size_t);
					state = LENGTH;
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
					state = LENGTH;
				}
				[[fallthrough]];

			// Precision is not supported

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
						state = NONE;
						break;

					case 's':
						operator<<(va_arg(args, const char *));
						state = NONE;
						break;

					case 'p':
						operator<<(va_arg(args, void *));
						state = NONE;
						break;

					case 'n':
						*va_arg(args, int *) = _pos - start;
						state = NONE;
						break;

					case 'd':
					case 'i':
						switch (length) {
							case SHORT:
								operator<<(static_cast<short>(va_arg(args, int)));
								break;

							case LONG:
								operator<<(va_arg(args, long));
								break;

							case LONGLONG:
								operator<<(va_arg(args, long long));
								break;

							default:
								operator<<(va_arg(args, int));
						}
						state = NONE;
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
						break;

					default:
						state = NONE;
				}
				if (state != NONE)
					switch (length) {
						case SHORT:
							operator<<(static_cast<unsigned short>(va_arg(args, unsigned)));
							break;

						case LONG:
							operator<<(va_arg(args, unsigned long));
							break;

						case LONGLONG:
							operator<<(va_arg(args, unsigned long long));
							break;

						default:
							operator<<(va_arg(args, unsigned));
					}
				state = NONE;
				break;

			default:
				if (*format == '%') {
					state = MARKER;
					_width = 0;
					_fill = ' ';
					_prefix = false;
					_left = false;
					_sign = MINUS_ONLY;
					length = INT;
				} else {
					operator<<(*format);
				}
		}
		format++;
	}

	// Reset modifiers
	reset();
	size_t n = _pos - start;
	// 0 Byte
	str();
	flush();
	return n;
}
