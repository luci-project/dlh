// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <dlh/stream/output.hpp>
#include <dlh/string.hpp>
#include <dlh/mem.hpp>
#include <dlh/error.hpp>

int main(int argc, const char *argv[]) {
	(void) argc;
	(void) argv;

	const char * s = "Rhabarberbarbarabarbarbaren!";

	cout << "String::find(s, 'b') = " << String::find(s, 'b') << endl;
	cout << "String::find_or_end(s, 'b') = " << String::find_or_end(s, 'b') << endl;
	cout << "String::find_last(s, 'b') = " << String::find_last(s, 'b') << endl;

	cout << "String::find(s, \"barbar\") = " << String::find(s, "barbar") << endl;
	cout << "String::find_or_end(s, \"barbar\") = " << String::find_or_end(s, "barbar") << endl;
	cout << "String::find_last(s, \"barbar\") = " << String::find_last(s, "barbar") << endl;

	cout << "String::find(s, 'y') = " << String::find(s, 'y') << endl;
	cout << "String::find_or_end(s, 'y') = " << String::find_or_end(s, 'y') << endl;
	cout << "String::find_last(s, 'y') = " << String::find_last(s, 'y') << endl;

	cout << "String::find(s, \"barabara\") = " << String::find(s, "barabara") << endl;
	cout << "String::find_or_end(s, \"barabara\") = " << String::find_or_end(s, "barabara") << endl;
	cout << "String::find_last(s, \"barabara\") = " << String::find_last(s, "barabara") << endl;

	cout << "String::starts_with(s, \"Rha\") = " << String::starts_with(s, "Rha") << endl;
	cout << "String::starts_with(s, \"bar\") = " << String::starts_with(s, "bar") << endl;
	cout << "String::ends_with(s, \"bar\") = " << String::ends_with(s, "bar") << endl;
	cout << "String::ends_with(s, \"en!\") = " << String::ends_with(s, "en!") << endl;

	cout << "String::compare(s, \"Rhabarber\", 8) -> " << (String::compare(s, "Rhabarber", 8) == 0) << endl;
	cout << "String::compare(s, \"Rhabarber\", 9) -> " << (String::compare(s, "Rhabarber", 9) == 0) << endl;
	cout << "String::compare(s, \"Rhabarber!\", 9) -> " << (String::compare(s, "Rhabarber!", 9) == 0) << endl;
	cout << "String::compare(s, \"RHABARBER\", 9) -> " << (String::compare(s, "RHABARBER", 9) == 0) << endl;
	cout << "String::compare_case(s, \"RHABARBER\", 9) -> " << (String::compare_case(s, "RHABARBER", 9) == 0) << endl;
	cout << "String::compare_case(s, \"RHABARBER!\", 9) -> " << (String::compare_case(s, "RHABARBER!", 9) == 0) << endl;
	cout << "String::compare_case(s, \"BARBERA!\", 8) -> " << (String::compare_case(s, "BARBERA!", 8) == 0) << endl;

	 {
		cout << "String::replace(s, \"bar\", \" BAR \", 5) = ";
		const char * r = String::replace(s, "bar", " BAR ", 5);
		cout << r << endl;
		Memory::free(r);
	 }
	 {
		char t[String::len(s)];
		cout << "String::replace_inplace(t, \"bar\", \"...\", 5) = ";
		const char * r = String::replace_inplace(String::copy(t, s), "bar", "...", 5);
		cout << r << endl;
		assert(r == t);
	 }
	 {
		cout << "String::replace(s, \"barbar\", \" BARBAR \") = ";
		const char * r = String::replace(s, "barbar", " BARBAR ");
		cout << r << endl;
		Memory::free(r);
	 }
	 {
		char t[String::len(s)];
		cout << "String::replace_inplace(t, \"barbar\", \"...\") = ";
		const char * r = String::replace_inplace(String::copy(t, s), "barbar", "...");
		cout << r << endl;
		assert(r == t);
	 }
	 {
		cout << "String::replace(s, \"bax\", \" BAR \") = ";
		const char * r = String::replace(s, "bax", " BAR ");
		cout << r << endl;
		Memory::free(r);
	 }
	 {
		char t[String::len(s)];
		cout << "String::replace_inplace(t, \"bax\", \"...\") = ";
		const char * r = String::replace_inplace(String::copy(t, s), "bax", "...");
		cout << r << endl;
		assert(r == t);
	 }
	 {
		cout << "String::split(s, 'b' , 4):" << endl;
		for (const char * p : String::split(s, 'b', 4)) {
			cout << " - " << p << endl;
			Memory::free(p);
		}
	 }
	 {
		char t[String::len(s)];
		cout << "String::split_inplace(t, 'b', 4):" << endl;
		for (const char * p : String::split_inplace(String::copy(t, s), 'b', 4))
			cout << " - " << p << endl;
	 }
	 {
		cout << "String::split(s, \"b\" , 4):" << endl;
		for (const char * p : String::split(s, "b", 4)) {
			cout << " - " << p << endl;
			Memory::free(p);
		}
	 }
	 {
		char t[String::len(s)];
		cout << "String::split_any_inplace(t, \"hb\", 4):" << endl;
		for (const char * p : String::split_any_inplace(String::copy(t, s), "hb", 4))
			cout << " - " << p << endl;
	 }
	 {
		cout << "String::split_any(s, \"hb\"):" << endl;
		for (const char * p : String::split_any(s, "hb")) {
			cout << " - " << p << endl;
			Memory::free(p);
		}
	 }
	 {
		char t[String::len(s)];
		cout << "String::split_inplace(t, \"b\", 4):" << endl;
		for (const char * p : String::split_inplace(String::copy(t, s), "b", 4))
			cout << " - " << p << endl;
	 }
	 {
		cout << "String::split(s, \"bar\"):" << endl;
		for (const char * p : String::split(s, "bar")) {
			cout << " - " << p << endl;
			Memory::free(p);
		}
	 }
	 {
		char t[String::len(s)];
		cout << "String::split_inplace(t, \"bar\"):" << endl;
		for (const char * p : String::split_inplace(String::copy(t, s), "bar"))
			cout << " - " << p << endl;
	 }
	 {
		char t[String::len(s)];
		cout << "String::copy(t, s) = " << String::copy(t, s) << endl;
		cout << "String::copy(t, \"BAR\\0BAR\", 3, true) = " << String::copy(t, "BAR\0BAR", 3, true) << endl;
		cout << "String::copy(t, \"bar\\0bar\", 12, true) = " << String::copy(t, "bar\0bar", 12, true) << endl;
		for (size_t i = 0; i < String::len(s); i++)
			cout << (i == 0 ? " - '" : ", '")  << t[i] << "'";
		cout << endl;
	 }
	 {
		char * t = String::duplicate(s);
		cout << "String::duplicate(s) = " << t << endl;
		Memory::free(t);
	 }
	 {
		char * t = String::duplicate(s, 9);
		cout << "String::duplicate(s, 9) = " << t << endl;
		Memory::free(t);
	 }
	return 0;
}
