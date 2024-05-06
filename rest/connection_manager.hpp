#ifndef HTTP_SERVER_CONNECTION_MANAGER_HPP
#define HTTP_SERVER_CONNECTION_MANAGER_HPP

#include <set>
#include <map>
#include <functional>
#include "connection.hpp"
//#include "regex_orderable.hpp"

namespace http {
	namespace server {
		typedef std::map<std::string, std::function<void(const request&, reply&)>> RouteMap;

		class connection_manager {
		public:
			connection_manager(const connection_manager&) = delete;
			connection_manager& operator=(const connection_manager&) = delete;

			connection_manager();

			void start(connection_ptr c);
			
			void stop(connection_ptr c);

			void stop_all();

			void add_route(const std::string& url, std::function<void(const request&, reply&)> func);

			std::function<void(const request&, reply&)> get_route(const std::string& url) const;

		private:
			std::set<connection_ptr> connections_;
			RouteMap route_map_;
		};
	}
}

#endif