#include "server.hpp"
#include <signal.h>
#include <utility>

namespace http {
	namespace server {
		Server::Server(const std::string& address, const std::string& port)
			: io_context_(1),
			signals_(io_context_),
			acceptor_(io_context_),
			connection_manager_() {
			signals_.add(SIGINT);
			signals_.add(SIGTERM);
#if defined(SIGQUIT)
			signals_.add(SIGQUIT);
#endif

			do_await_stop();

			boost::asio::ip::tcp::resolver resolver(io_context_);
			boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
			acceptor_.open(endpoint.protocol());
			acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor_.bind(endpoint);
			acceptor_.listen();

			do_accept();
		}

		void Server::run() {
			io_context_.run();
		}

		void Server::do_accept() {
			acceptor_.async_accept(
				[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
					if (!acceptor_.is_open()) {
						return;
					}

					if (!ec) {
						connection_manager_.start(std::make_shared<connection>(
							std::move(socket), connection_manager_));
					}

					do_accept();
				}
			);
		}

		void Server::do_await_stop() {
			signals_.async_wait(
				[this](boost::system::error_code, int) {
					acceptor_.close();
					connection_manager_.stop_all();
				}
			);
		}

		void Server::add_route(const std::string& url, std::function<void(const request&, reply&)> func) {
			connection_manager_.add_route(url, func);
		}
	}
}