#include "connection.hpp"
#include <utility>
#include "connection_manager.hpp"
#include <string>

namespace http {
	namespace server {

		connection::connection(boost::asio::ip::tcp::socket socket,
			connection_manager& manager)
			: socket_(std::move(socket)),
			connection_manager_(manager) {
		}

		void connection::start(){
			do_read();
		}
		
		void connection::stop() {
			socket_.close();
		}

		void connection::do_read() {
			auto self(shared_from_this());
			socket_.async_read_some(boost::asio::buffer(buffer_),
				[this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
					if (!ec) {
						request_parser::result_type result;
						std::tie(result, std::ignore) = request_parser_.parse(
							request_, buffer_.data(), buffer_.data() + bytes_transferred
						);

						if (result == request_parser::good) {
							handle_request(request_, reply_);
							do_write();
						}
						else if (result == request_parser::bad) {
							reply_ = reply::stock_reply(reply::bad_request);
							do_write();
						}
						else {
							do_read();
						}
					}
					else if (ec != boost::asio::error::operation_aborted) {
						connection_manager_.stop(shared_from_this());
					}
				}
			);
		}

		void connection::handle_request(const request& req, reply& rep) {
			std::function<void(const request&, reply&)> route_fn = connection_manager_.get_route(req.decoded_uri);
			if (route_fn == nullptr) {
				rep = reply::stock_reply(reply::not_found);
				return;
			}
			route_fn(req, rep);
		}

		void connection::do_write() {
			auto self(shared_from_this());
			boost::asio::async_write(socket_, reply_.to_buffers(),
				[this, self](boost::system::error_code ec, std::size_t) {
					if (!ec) {
						boost::system::error_code ignored_ec;
						socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
					}

					if (ec != boost::asio::error::operation_aborted) {
						connection_manager_.stop(shared_from_this());
					}
				}
			);
		}
	}
}