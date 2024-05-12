#ifndef HTTP_SERVER_CONNECTION_HPP
#define HTTP_SERVER_CONNECTION_HPP

#include <boost/asio.hpp>
#include <array>
#include <memory>
#include "response.hpp"
#include "request.hpp"
#include "request_parser.hpp"


namespace http {
	namespace server {
		class connection_manager;

		class connection : public std::enable_shared_from_this<connection> {
		public:
			connection(const connection&) = delete;
			connection& operator=(const connection&) = delete;
			
			explicit connection(boost::asio::ip::tcp::socket socket,
				connection_manager& manager);

			void start();

			void stop();

		private:
			void do_read();

			void handle_request(Request& req, Response& rep);
			
			void do_write();

			boost::asio::ip::tcp::socket socket_;

			connection_manager& connection_manager_;

			std::array<char, 8192> buffer_;

			Request request_;

			request_parser request_parser_;

			Response reply_;
		};

		typedef std::shared_ptr<connection> connection_ptr;
	}
}

#endif