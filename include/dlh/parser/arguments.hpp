// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/container/initializer_list.hpp>
#include <dlh/container/vector.hpp>
#include <dlh/parser/string.hpp>
#include <dlh/string.hpp>
#include <dlh/log.hpp>

namespace Parser {

template <class Opts>
struct Arguments : Opts {
	typedef bool (*validate_t)(const char *);

	class Member {
		const enum { M_STR, M_S, M_US, M_I, M_U, M_L, M_UL, M_LL, M_ULL, M_B, M_VSTR, M_VS, M_VUS, M_VI, M_VU, M_VL, M_VUL, M_VLL, M_VULL } t;

		const union Helper {
			const char * Opts::* str;
			short Opts::* s;
			unsigned short Opts::* us;
			int Opts::* i;
			unsigned Opts::* u;
			long Opts::* l;
			unsigned long Opts::* ul;
			long long Opts::* ll;
			unsigned long long Opts::* ull;
			bool Opts::* b;
			Vector<const char *> Opts::* vstr;
			Vector<short> Opts::* vs;
			Vector<unsigned short> Opts::* vus;
			Vector<int> Opts::* vi;
			Vector<unsigned int> Opts::* vu;
			Vector<long> Opts::* vl;
			Vector<unsigned long> Opts::* vul;
			Vector<long long> Opts::* vll;
			Vector<unsigned long long> Opts::* vull;
			void * ptr;

			Helper(const char * Opts::* str)                     : str(str) {}  // NOLINT
			Helper(short Opts::* s)                              : s(s)     {}  // NOLINT
			Helper(unsigned short Opts::* us)                    : us(us)   {}  // NOLINT
			Helper(int Opts::* i)                                : i(i)     {}  // NOLINT
			Helper(unsigned Opts::* u)                           : u(u)     {}  // NOLINT
			Helper(long Opts::* l)                               : l(l)     {}  // NOLINT
			Helper(unsigned long Opts::* ul)                     : ul(ul)   {}  // NOLINT
			Helper(long long Opts::* ll)                         : ll(ll)   {}  // NOLINT
			Helper(unsigned long long Opts::* ull)               : ull(ull) {}  // NOLINT
			Helper(bool Opts::* b)                               : b(b)     {}  // NOLINT
			Helper(Vector<const char *> Opts::* vstr)            : vstr(vstr) {}  // NOLINT
			Helper(Vector<short> Opts::* vs)                     : vs(vs)     {}  // NOLINT
			Helper(Vector<unsigned short> Opts::* vus)           : vs(vus)    {}  // NOLINT
			Helper(Vector<int> Opts::* vi)                       : vi(vi)     {}  // NOLINT
			Helper(Vector<unsigned> Opts::* vu)                  : vu(vu)     {}  // NOLINT
			Helper(Vector<long> Opts::* vl)                      : vl(vl)     {}  // NOLINT
			Helper(Vector<unsigned long> Opts::* vul)            : vul(vul)   {}  // NOLINT
			Helper(Vector<long long> Opts::* vll)                : vll(vll)   {}  // NOLINT
			Helper(Vector<unsigned long long> Opts::* vull)      : vull(vull) {}  // NOLINT
		} m;

	 public:
		const bool is_vector;

		Member(const char * Opts::* str)                     : t(M_STR),  m(str),  is_vector(false) {}  // NOLINT
		Member(short Opts::* s)                              : t(M_S),    m(s),    is_vector(false) {}  // NOLINT
		Member(unsigned short Opts::* us)                    : t(M_US),   m(us),   is_vector(false) {}  // NOLINT
		Member(int Opts::* i)                                : t(M_I),    m(i),    is_vector(false) {}  // NOLINT
		Member(unsigned Opts::* u)                           : t(M_U),    m(u),    is_vector(false) {}  // NOLINT
		Member(long Opts::* l)                               : t(M_L),    m(l),    is_vector(false) {}  // NOLINT
		Member(unsigned long Opts::* ul)                     : t(M_UL),   m(ul),   is_vector(false) {}  // NOLINT
		Member(long long Opts::* ll)                         : t(M_LL),   m(ll),   is_vector(false) {}  // NOLINT
		Member(unsigned long long Opts::* ull)               : t(M_ULL),  m(ull),  is_vector(false) {}  // NOLINT
		Member(bool Opts::* b)                               : t(M_B),    m(b),    is_vector(false) {}  // NOLINT
		Member(Vector<const char *> Opts::* vstr)            : t(M_VSTR), m(vstr), is_vector(true) {}  // NOLINT
		Member(Vector<short> Opts::* vs)                     : t(M_VS),   m(vs),   is_vector(true) {}  // NOLINT
		Member(Vector<unsigned short> Opts::* vus)           : t(M_VUS),  m(vus),  is_vector(true) {}  // NOLINT
		Member(Vector<int> Opts::* vi)                       : t(M_VI),   m(vi),   is_vector(true) {}  // NOLINT
		Member(Vector<unsigned> Opts::* vu)                  : t(M_VU),   m(vu),   is_vector(true) {}  // NOLINT
		Member(Vector<long> Opts::* vl)                      : t(M_VL),   m(vl),   is_vector(true) {}  // NOLINT
		Member(Vector<unsigned long> Opts::* vul)            : t(M_VUL),  m(vul),  is_vector(true) {}  // NOLINT
		Member(Vector<long long> Opts::* vll)                : t(M_VLL),  m(vll),  is_vector(true) {}  // NOLINT
		Member(Vector<unsigned long long> Opts::* vull)      : t(M_VULL), m(vull), is_vector(true) {}  // NOLINT

		bool is_bool() const {
			return t == M_B;
		}

		bool set(Opts * opts, const char * value) const {
			switch (t) {
				case M_STR:  return string(opts->*(m.str), value);
				case M_S:    return string(opts->*(m.s), value);
				case M_US:   return string(opts->*(m.us), value);
				case M_I:    return string(opts->*(m.i), value);
				case M_U:    return string(opts->*(m.u), value);
				case M_L:    return string(opts->*(m.l), value);
				case M_UL:   return string(opts->*(m.ul), value);
				case M_LL:   return string(opts->*(m.ll), value);
				case M_ULL:  return string(opts->*(m.ull), value);
				case M_B:    return string(opts->*(m.b), value);
				case M_VSTR: return string(opts->*(m.vstr), value);
				case M_VS:   return string(opts->*(m.vs), value);
				case M_VUS:  return string(opts->*(m.vus), value);
				case M_VI:   return string(opts->*(m.vi), value);
				case M_VU:   return string(opts->*(m.vu), value);
				case M_VL:   return string(opts->*(m.vl), value);
				case M_VUL:  return string(opts->*(m.vul), value);
				case M_VULL: return string(opts->*(m.vull), value);
				default:     return false;
			}
		}

		bool is_default(Opts * opts) const {
			switch (t) {
				case M_STR:  return opts->*(m.str) == nullptr;
				case M_S:    return opts->*(m.s) == 0;
				case M_US:   return opts->*(m.us) == 0;
				case M_I:    return opts->*(m.i) == 0;
				case M_U:    return opts->*(m.u) == 0;
				case M_L:    return opts->*(m.l) == 0;
				case M_UL:   return opts->*(m.ul) == 0;
				case M_LL:   return opts->*(m.ll) == 0;
				case M_ULL:  return opts->*(m.ull) == 0;
				case M_B:    return true;
				case M_VSTR: return (opts->*(m.vstr)).empty();
				case M_VS:   return (opts->*(m.vs)).empty();
				case M_VUS:  return (opts->*(m.vus)).empty();
				case M_VI:   return (opts->*(m.vi)).empty();
				case M_VU:   return (opts->*(m.vu)).empty();
				case M_VL:   return (opts->*(m.vl)).empty();
				case M_VUL:  return (opts->*(m.vul)).empty();
				case M_VLL:  return (opts->*(m.vll)).empty();
				case M_VULL: return (opts->*(m.vull)).empty();
				default:     return false;
			}
		}

		BufferStream & output(Opts * opts, BufferStream & out) const {
			switch (t) {
				case M_STR:  return out << opts->*(m.str);
				case M_S:    return out << opts->*(m.s);
				case M_US:   return out << opts->*(m.us);
				case M_I:    return out << opts->*(m.i);
				case M_U:    return out << opts->*(m.u);
				case M_L:    return out << opts->*(m.l);
				case M_UL:   return out << opts->*(m.ul);
				case M_LL:   return out << opts->*(m.ll);
				case M_ULL:  return out << opts->*(m.ull);
				case M_B:    return out << opts->*(m.b);
				case M_VSTR: return out << opts->*(m.vstr);
				case M_VS:   return out << opts->*(m.vs);
				case M_VUS:  return out << opts->*(m.vus);
				case M_VI:   return out << opts->*(m.vi);
				case M_VU:   return out << opts->*(m.vu);
				case M_VL:   return out << opts->*(m.vl);
				case M_VUL:  return out << opts->*(m.vul);
				case M_VLL:  return out << opts->*(m.vll);
				case M_VULL: return out << opts->*(m.vull);
				default:     return out;
			}
		}

		bool operator==(const Member & other) const {
			return t == other.t && m.ptr == other.m.ptr;
		}
	};

	struct Parameter {
		const char name_short;
		const char * name_long;
		const char * name_arg;
		Member member;
		bool required;
		const char * help_text;
		validate_t validate;
		bool present;

		constexpr Parameter(const char name_short, const char * name_long, const char * name_arg, Member member, bool required, const char * help_text, validate_t validate = nullptr)
		: name_short(name_short), name_long(name_long), name_arg(name_arg), member(member), required(required), help_text(help_text), validate(validate), present(false) {}

		bool matches(const char * name, const char ** appending = nullptr) {
			if (name[0] == '-') {
				if (name_short != '\0' && name[1] == name_short) {
					if (appending != nullptr)
						*appending = name[2] != '\0' ? name + 2 : nullptr;
					return true;
				} else if (name_long != nullptr && name[1] == '-') {
					auto name_len = String::len(name_long);
					if (String::compare_case(name + 2, name_long, name_len) == 0) {
						if (name[2 + name_len] == '=') {
							if (appending != nullptr)
								*appending = name + 3 + name_len;
							return true;
						} else if (name[2 + name_len] == '\0') {
							if (appending != nullptr)
								*appending = nullptr;
							return true;
						}
					}
				}
			}
			return false;
		}

		void help(BufferStream & out, size_t par_len, size_t line_len, Opts * opts = nullptr) {
			size_t pos = 0;
			auto text = [this, &out, par_len, line_len, &pos, &opts](size_t len) -> bool {
				size_t l = 0;
				bool e = false;
				while (help_text[pos] == ' ')
					pos++;
				for (size_t i = 0; i < line_len - par_len; i++)
					if (help_text[pos + i] == '\0') {
						e = opts == nullptr;
						l = i;
						break;
					} else if (help_text[pos + i] == ' ') {
						l = i;
					}
				if (l > 0) {
					if (len < par_len)
						out.write(' ', par_len - len);
					out.write(help_text + pos, l);
					pos += l;
				} else if (opts != nullptr) {
					if (len < par_len)
						out.write(' ', par_len - len);
					out << "(default: ";
					member.output(opts, out);
					out << ")" << endl;
					opts = nullptr;
				}
				if (l > 0 || len > 0)
					out << endl;
				return e;
			};

			if (name_short != '\0') {
				out << "    -" << name_short;
				if (name_arg == nullptr) {
					text(6);
				} else {
					out << ' ' << name_arg;
					text(7 + String::len(name_arg));
				}
			}
			if (name_long != nullptr) {
				out << "    --" << name_long;
				if (name_arg == nullptr) {
					text(6 + String::len(name_long));
				} else {
					out << ' ' << name_arg;
					text(7 + String::len(name_long) + String::len(name_arg));
				}
			}
			while (!text(0)) {}

			out << endl;
		}
	};

 private:
	Vector<Parameter> args;
	Vector<const char *> positional, terminal;
	validate_t validate_positional, validate_terminal;
	const size_t line_len = 80;

	Arguments() = default;
	Arguments(const Arguments&) = delete;
	Arguments(Arguments&&) = delete;
	Arguments& operator=(const Arguments&) = delete;
	Arguments& operator=(Arguments&&) = delete;

	bool set(const Parameter & arg, const char * value) {
		return arg.validate != nullptr && !arg.validate(value) ? false : arg.member.set(this, value);
	}

 public:
	Arguments(const std::initializer_list<Parameter> & list, validate_t validate_positional, validate_t validate_terminal = nullptr, size_t line_len = 80) : validate_positional(validate_positional), validate_terminal(validate_terminal), line_len(line_len) {
		for (const auto & arg : list) {
			// Check if parameter is unqiue
			bool skip = false;
			if (arg.name_short == '\0' && arg.name_long == nullptr) {
				LOG_FATAL << "Parameter has neither short nor long name!" << endl;
				skip = true;
			} else if (arg.name_short != '\0' && (arg.name_short < '0' || arg.name_short > '9') && (arg.name_short < 'a' || arg.name_short > 'z') && (arg.name_short < 'A' || arg.name_short > 'Z')) {
				LOG_FATAL << "Short parameter '-" << arg.name_long << "' is not a valid alpha-numeric value!" << endl;
				skip = true;
			} else {
				for (const auto & other : args)
					if (arg.name_short != '\0' && arg.name_short == other.name_short) {
						LOG_FATAL << "Parameter '-" << arg.name_short << "' is not unqiue!" << endl;
						skip = true;
						break;
					} else if (arg.name_long != nullptr && other.name_long != nullptr && String::compare_case(arg.name_long, other.name_long) == 0) {
						LOG_FATAL << "Parameter '--" << arg.name_long << "' is not unqiue!" << endl;
						skip = true;
						break;
					} else if (arg.member == other.member) {
						if (arg.name_long != nullptr) {
							LOG_FATAL << "Member of parameter '--" << arg.name_long << "' is not unqiue!" << endl;
						} else {
							LOG_FATAL << "Member of parameter '-" << arg.name_short << "' is not unqiue!" << endl;
						}
						skip = true;
						break;
					}
			}

			// valid parameter.
			if (!skip)
				args.push_back(arg);
		}
	}

	Arguments(const std::initializer_list<Parameter> & list)
	 : Arguments(
			list,
			[](const char * str) -> bool {
				if (str != nullptr && *str == '-') {
					LOG_ERROR << "Positional parameter '" << str << "' looks like its not meant to be positional!" << endl;
					return false;
				} else {
					return true;
				}
			},
			[](const char *) -> bool {
				return true;
			}
		) {}

	~Arguments() = default;

	void help(BufferStream & out, const char * header, const char * file, const char * footer, const char * positional = nullptr, const char * terminal = nullptr) {
		// Usage line
		if (header != nullptr)
			out << endl << header << endl;
		out << endl << " Usage: " << endl << "    " << file;

		bool hasRequired = false;
		bool hasOptional = false;
		size_t file_len = 4 + String::len(file);
		size_t cur_len = file_len;
		size_t arg_len_max = 16;

		auto check_newline = [this, &out, &cur_len, file_len](size_t len) {
			if (cur_len + len > this->line_len) {
				out << endl;
				out.write(' ', file_len);
				cur_len = file_len;
			}
			cur_len += len;
		};

		for (auto arg : args) {
			size_t len_name_long = arg.name_long != nullptr ? String::len(arg.name_long) : 0;
			size_t len_name_arg = arg.name_long != nullptr ? 1 + String::len(arg.name_arg) : 0;
			arg_len_max = Math::max(arg_len_max, 2 + len_name_long + len_name_arg);
			check_newline((arg.required ? 3 : 5) + len_name_long + len_name_arg);
			if (arg.required) {
				if (arg.name_long != nullptr)
					out << " --" << arg.name_long;
				else
					out << " -" << arg.name_short;
				if (arg.name_arg != nullptr)
					out << " " << arg.name_arg;
				hasRequired = true;
			} else {
				if (arg.name_long != nullptr)
					out << " [--" << arg.name_long;
				else
					out << " [-" << arg.name_short;
				if (arg.name_arg != nullptr)
					out << " " << arg.name_arg;
				out << "]";
				hasOptional = true;
			}
		}
		if (positional != nullptr) {
			check_newline(1 + String::len(positional));
			out << " " << positional;
		}
		if (terminal != nullptr) {
			check_newline(4 + String::len(terminal));
			out << " -- " << terminal;
		}
		out << endl;

		// Parameter details
		if (hasRequired) {
			out << endl << " Required parameters:" << endl;
			for (auto arg : args)
				if (arg.required)
					arg.help(out, arg_len_max + 5, line_len);
		}

		if (hasOptional) {
			out << endl << " Optional parameters:" << endl;
			for (auto arg : args)
				if (!arg.required)
					arg.help(out, arg_len_max + 5, line_len, arg.member.is_default(this) ? nullptr : this);
		}

		if (footer != nullptr)
			out << endl << footer << endl;

		out << endl;
	}

	bool parse(int argc, char* argv[]) {
		bool terminator = false;
		bool error = false;

		for (int idx = 1; idx < argc; ++idx) {
			char * current = argv[idx];

			if (terminator) {
				// Terminal argument
				if (validate_terminal == nullptr || validate_terminal(current)) {
					terminal.push_back(current);
				} else {
					LOG_ERROR << "Terminal argument '" << current << "' is not valid!" << endl;
					error = true;
				}
			} else if (current[0] == '-' && current[1] == '-' && current[2] == '\0') {
				terminator = true;
				continue;
			} else {
				bool found = false;
				// Check all parameter names
				for (auto & arg : args) {
					const char * appendix = nullptr;
					if (arg.matches(current, &appendix)) {
						found = true;
						bool valid;
						if (appendix != nullptr) {
							if (!(valid = set(arg, appendix))) {
								LOG_ERROR << "Invalid appended value '" << appendix << "' for parameter " << current << "!" << endl;
							}
						} else if (arg.member.is_bool()) {
							valid = set(arg, "1");
						} else {
							const char * next = idx < argc - 1 ? argv[idx + 1] : nullptr;
							if (next != nullptr) {
								idx += 1;
								if (!(valid = set(arg, next))) {
									LOG_ERROR << "Invalid value '" << next << "' for parameter " << current << "!" << endl;
								}
							} else {
								LOG_ERROR << "Missing value for parameter " << current << "!" << endl;
								valid = false;
							}
						}
						if (!valid) {
							error = true;
						} else if (arg.present && !arg.member.is_vector) {
							LOG_ERROR << "Parameter " << current << " occured multiple times (= reassigned)!" << endl;
							error = true;
						} else {
							arg.present = true;
						}
						break;
					}
				}

				// Positional argument
				if (!found) {
					if (validate_positional == nullptr || validate_positional(current)) {
						positional.push_back(current);
					} else {
						LOG_ERROR << "Positional argument '" << current << "' is not valid!" << endl;
						error = true;
					}
				}
			}
		}

		for (auto arg : args)
			if (arg.required && !arg.present) {
				if (arg.name_long != nullptr) {
					LOG_ERROR << "Parameter '--" << arg.name_long << "' is required!" << endl;
				} else {
					LOG_ERROR << "Parameter '-" << arg.name_short << "' is required!" << endl;
				}
				error = true;
			}

		return !error;
	}

	bool has(const char * name) const {
		for (auto arg : args)
			if (arg.matches(name))
				return arg.present;
		return false;
	}

	bool has(Member & member) const {
		for (auto arg : args)
			if (member == arg.member)
				return arg.present;
		return false;
	}

	bool has_positional() {
		return positional.size() > 0;
	}

	Vector<const char *> get_positional() {
		return positional;
	}

	const Vector<const char *> & get_positional() const {
		return positional;
	}

	bool has_terminal() {
		return terminal.size() > 0;
	}

	Vector<const char *> get_terminal() {
		return terminal;
	}

	const Vector<const char *> & get_terminal() const {
		return terminal;
	}

	bool set(const char * name, const char * value) {
		for (auto & arg : args)
			if (arg.matches(name))
				return set(arg, value);
	}

	bool set(Member & member, const char * value) {
		for (auto & arg : args)
			if (member == arg.member)
				return set(arg, value);
		return false;
	}

	Opts get_all() {
		return static_cast<Opts>(*this);
	}

	const Opts & get_all() const {
		return static_cast<Opts>(*this);
	}
};

}  // namespace Parser
