#include <dlh/parser/config.hpp>
#include <dlh/environ.hpp>
#include <dlh/string.hpp>
#include <dlh/file.hpp>

namespace Parser {

Config::Config(const char * path, Source source, bool consume_env) : source(source), consume_env(consume_env) {
	char * value;
	for (const auto & line : File::lines(path))
		if (line != nullptr && line[0] != '\0' && line[0] != '#' && (value = const_cast<char*>(String::find(line, '='))) != nullptr && reinterpret_cast<uintptr_t>(value) > reinterpret_cast<uintptr_t>(line)) {
			*value = '\0';
			value++;
			contents.insert(line, value);
		}
}

const char * Config::value(const char * name) {
	const char * env = nullptr;
	auto cfg = contents.at(name);
	if (source != CONFIG_ONLY)
		env = Environ::variable(name, consume_env);
	return (cfg.has_value() && (env == nullptr || source == CONFIG_ENV)) ? cfg.value() : env;
}

}  // namespace Parser
