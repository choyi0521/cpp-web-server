#include <regex>
#include <functional>
#include "connection_manager.hpp"

namespace http {
	namespace server {
		connection_manager::connection_manager() : route_manager_() {}

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

		void connection_manager::add_route(const std::string &method, const RoutePath& path, const std::function<void(const Request&, Response&)>& func) {
			route_manager_.add_route(method, path, func);
		}

		std::function<void(const Request&, Response&)> connection_manager::get_route(Request& req) const {
			return route_manager_.get_route(req);
		}
	}
}