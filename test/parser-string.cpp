// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/output.hpp>
#include <dlh/parser/string.hpp>
#include <dlh/type_traits.hpp>
#include <dlh/syscall.hpp>

template<typename T>
void test(const char * str, T value) {
	T tmp;
	cout << "Parser::string(" << integral_name<T>() << ", \"" << str << "\") " << flush;
	if (Parser::string(tmp, str)) {
		cout << "= " << tmp << endl;
		if (tmp != value) {
			cerr << "Wrong value: " << tmp << " != " << value << endl;
			Syscall::exit(1);
		}
	} else {
		cerr << "failed!" << endl;
		Syscall::exit(1);
	}
}

template<typename T>
void testfail(const char * str) {
	T tmp;
	cout << "Parser::string(" << integral_name<T>() << ", \"" << str << "\") " << flush;
	if (Parser::string(tmp, str)) {
		cout << "= " << tmp << flush;
		cerr << " should fail!" << endl;
		Syscall::exit(1);
	} else {
		cout << "failed (as expected)!" << endl;
	}
}

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	test<int>("0", 0);
	test<int>("+0", 0);
	test<int>("-0", 0);
	test<unsigned>("0", 0);
	test<unsigned>("+0", 0);

	test<int>("42", 42);
	test<int>("+42", 42);
	test<int>("+ 42", 42);
	test<int>(" + 42", 42);
	test<int>(" +42", 42);
	test<int>("-42", -42);
	test<int>(" - 42", -42);
	test<int>("123'000'000", 123'000'000);
	test<int>("123,000,000", 123'000'000);
	test<int>("123 000 000", 123'000'000);
	testfail<int>("123-000");
	testfail<int>("123'000,000");
	testfail<int>("123.000.000");


	test<unsigned>("42", 42);
	test<unsigned>("+42", 42);
	test<unsigned>("+ 42", 42);
	test<unsigned>(" + 42", 42);
	testfail<unsigned>("-42");
	testfail<unsigned>(" - 42");
	test<unsigned>("123'000'000", 123'000'000);
	test<unsigned>("123,000,000", 123'000'000);
	test<unsigned>("123 000 000", 123'000'000);

	test<int>("023", 023);
	test<int>("+023", 023);
	test<int>(" + 023", 023);
	test<int>("-023", -023);
	test<int>(" - 023", -023);

	test<int>("0x23", 0x23);
	test<int>("+0x23", 0x23);
	test<int>(" + 0x23", 0x23);
	test<int>("-0x23", -0x23);
	test<int>(" - 0x23", -0x23);

	test<int>("0XAFfe", 0xaffe);
	testfail<int>("affe");
	testfail<int>("xaffe");

	test<int>("0b10100101", 0b10100101);
	test<int>("0B1101", 0b1101);
	testfail<int>("b1101");

	test<int>("023", 023);
	test<int>("0x23", 0x23);

	test<int8_t>("0x7f", 0x7f);
	test<int8_t>("-0x80", -0x80);
	testfail<int8_t>("0x80");
	testfail<int8_t>("-0x81");
	test<uint8_t>("0xff", 0xff);
	testfail<uint8_t>("0x100");

	test<int16_t>("0x7fff", 0x7fff);
	test<int16_t>("-0x8000", -0x8000);
	testfail<int16_t>("0x8000");
	testfail<int16_t>("-0x8001");
	test<uint16_t>("0xffff", 0xffff);
	testfail<uint16_t>("0x10000");

	test<int32_t>("0x7fffffff", 0x7fffffff);
	test<int32_t>("-0x80000000", -0x80000000);  // NOLINT
	testfail<int32_t>("0x80000000");
	testfail<int32_t>("-0x80000001");
	test<uint32_t>("0xffffffff", 0xffffffff);
	testfail<uint32_t>("0x100000000");

	test<int64_t>("0x7fffffffffffffff", 0x7fffffffffffffffL);
	test<int64_t>("-0x8000000000000000", -0x8000000000000000L);  // NOLINT
	testfail<int64_t>("0x8000000000000000");
	testfail<int64_t>("-0x8000000000000001");
	test<uint64_t>("0xffffffffffffffff", 0xffffffffffffffffUL);
	testfail<uint64_t>("0x10000000000000000");

	test<bool>("f", false);
	test<bool>("False", false);
	test<bool>("0", false);
	test<bool>("no", false);
	test<bool>("Nope", false);

	test<bool>("t", true);
	test<bool>("True", true);
	test<bool>("1", true);
	test<bool>("yes", true);
	test<bool>("Yay", true);

	return 0;
}
