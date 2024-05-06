#ifndef HTTP_SERVER_SERVER_HPP
#define HTTP_SERVER_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request.hpp"
#include "reply.hpp"


namespace http {
	namespace server {

		class Server {
		public:
			Server(const Server&) = delete;
			Server& operator=(const Server&) = delete;

			explicit Server(const std::string& address, const std::string& port);

			void run();

			void add_route(const std::string& url, std::function<void(const request&, reply&)> func);
		
		private:
			void do_accept();

			void do_await_stop();

			boost::asio::io_context io_context_;
			boost::asio::signal_set signals_;
			boost::asio::ip::tcp::acceptor acceptor_;
			connection_manager connection_manager_;
		};
	}
}

#endif