// Dirty Little Helper (DLH) - system support library for C/C++
// Copyright 2021-2023 by Bernhard Heinloth <heinloth@cs.fau.de>
// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include <dlh/container/tree.hpp>
#include <dlh/parser/string.hpp>


namespace Parser {

class Config {
 public:
	enum Source {
		CONFIG_ONLY,     ///< Ignore environment variables
		CONFIG_ENV,      ///< Prioritize config over environment variables
		ENV_CONFIG       ///< Prioritize environment variables over config
	};

 private:
	Source source;
	bool consume_env;
	TreeMap<const char *, const char *> contents;

 public:
	explicit Config(const char * file, Source source = CONFIG_ONLY, bool consume_env = false);

	const char * value(const char * name);

	template<typename T>
	Optional<T> value_as(const char * name) {
		return Parser::string_as<T>(value(name));
	}

	template<typename T>
	T value_or_default(const char * name, T def) {
		auto var = Parser::string_as<T>(value(name));
		return var ? var.value() : def;
	}
};

}  // namespace Parser
