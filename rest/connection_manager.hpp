#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include <map>
#include <functional>
#include <unordered_map>
#include "connection.hpp"
#include "route_manager.hpp"
//#include "regex_orderable.hpp"

namespace http {
	namespace server {
		class connection_manager {
		public:
			connection_manager(const connection_manager&) = delete;
			connection_manager& operator=(const connection_manager&) = delete;

			connection_manager();

			void start(connection_ptr c);
			
			void stop(connection_ptr c);

			void stop_all();

			void add_route(const std::string &method, const RoutePath& path, const std::function<void(const Request&, Response&)>& func);

			std::function<void(const Request&, Response&)> get_route(Request& req) const;

		private:
			std::set<connection_ptr> connections_;
			RouteManager route_manager_;
		};
	}
}

#endif