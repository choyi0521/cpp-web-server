#include "route_manager.hpp"
#include <functional>
#include <list>
#include <map>
#include <string>
#include <stdexcept>
#include <regex>
#include <iostream>

namespace http {
	namespace server {
		RouteManager::RouteManager() {}

		void RouteManager::add_route(
			const std::string& method,
			const RoutePath& path,
			const std::function<void(const Request&, Response&)>& function
		) {
			route_map_[method].push_back(std::make_pair(path, function));
		}

		std::function<void(const Request&, Response&)> RouteManager::get_route(Request& req) const {
			auto it = route_map_.find(req.method);
			if (it != route_map_.end()) {
				for (auto& route : it->second) {
					switch (route.first.match_type_) {
						case MatchType::EXACT:
							if (route.first.path_ == req.decoded_uri) {
								return route.second;
							}
							break;
						case MatchType::PARTIAL:
							throw std::runtime_error("Partial match not implemented yet");
							break;
						case MatchType::REGEX:
							{
								std::regex re(route.first.path_);
								std::smatch sm;
								if (std::regex_search(req.decoded_uri, sm, re)) {
									for (int i = 1; i < sm.size(); i++) {
										req.path_match.push_back(sm[i]);
									}
								}
								return route.second;
							}
							break;
						default:
							throw std::runtime_error("Unknown MatchType");
					}
				}
			}
			return nullptr;
		}
	}
}