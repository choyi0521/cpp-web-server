#include <regex>
#include <functional>
#include "connection_manager.hpp"

namespace http {
	namespace server {
		connection_manager::connection_manager() {}

		void connection_manager::start(connection_ptr c) {
			connections_.insert(c);
			c->start();
		}

		void connection_manager::stop(connection_ptr c) {
			connections_.erase(c);
			c->stop();
		}

		void connection_manager::stop_all() {
			for (auto c: connections_)
				c->stop();
			connections_.clear();
		}

		void connection_manager::add_route(const std::string& url, std::function<void(const request&, reply&)> func) {
			route_map_[url] = func;
		}

		std::function<void(const request&, reply&)> connection_manager::get_route(const std::string& url) const {
			auto it = route_map_.find(url);
			if (it != route_map_.end())
				return it->second;
			return nullptr;
		}
	}
}