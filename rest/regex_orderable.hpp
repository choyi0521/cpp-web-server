#ifndef HTTP_SERVER_REGEX_ORDERABLE_HPP
#define HTTP_SERVER_REGEX_ORDERABLE_HPP

#include <string>
#include <regex>

namespace http {
	namespace server {
		class RegexOrderable : public std::regex {
		public:
			std::string regex_str_;

			RegexOrderable(const RegexOrderable&) = delete;
			RegexOrderable& operator=(const RegexOrderable&) = delete;

			explicit RegexOrderable(std::string regex_str)
				: std::regex(regex_str_), regex_str_(std::move(regex_str)) {}

			bool operator<(const RegexOrderable& other) const {
				return regex_str_ < other.regex_str_;
			}
		};
	}
}

#endif