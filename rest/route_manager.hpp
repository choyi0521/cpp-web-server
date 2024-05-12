#ifndef HTTP_SERVER_ROUTE_MANAGER_HPP
#define HTTP_SERVER_ROUTE_MANAGER_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include <list>
#include <regex>
#include "connection.hpp"

namespace http {
	namespace server {
		enum MatchType {
			EXACT,
			PARTIAL,
			REGEX
		};

		class RoutePath {
		public:
			std::string path_;
			MatchType match_type_;

			RoutePath(const char* path) : path_(path), match_type_(MatchType::EXACT) {}
			RoutePath(const std::string& path) : path_(path), match_type_(MatchType::EXACT) {}
			RoutePath(const std::string& path, MatchType match_type) : path_(path), match_type_(match_type) {}

			//bool operator==(const RoutePath& other) const {
			//	return path_ == other.path_ && match_type_ == other.match_type_;
			//}
		};

		class RouteManager {
		public:
			RouteManager(const RouteManager&) = delete;
			RouteManager& operator=(const RouteManager&) = delete;
			RouteManager();

			void add_route(
				const std::string& method,
				const RoutePath& path,
				const std::function<void(const Request&, Response&)>& function);

			std::function<void(const Request&, Response&)> get_route(Request& req) const;

		private:
			std::unordered_map<std::string, std::list<std::pair<RoutePath, std::function<void(const Request&, Response&)>>>> route_map_;
		};
	}
}

#endif