#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include "reply.hpp"
#include "request.hpp"
#include <fstream>
#include "mime_types.hpp"

int main(int argc, char* argv[]) {
	try {
		if (argc != 4) {
			std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
			std::cerr << "  For IPv4, try:\n";
			std::cerr << "    receiver 0.0.0.0 80 . \n";
			std::cerr << "  For IPv6, try:\n";
			std::cerr << "    receiver 0::0 80 . \n";
			return 1;
		}

		http::server::Server server(argv[1], argv[2]);

		server.add_route("/", [argv](const http::server::request& req, http::server::reply& rep) {
			std::string request_path = req.decoded_uri;

			// Request path must be absolute and not contain "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos)
			{
				rep = http::server::reply::stock_reply(http::server::reply::bad_request);
				return;
			}

			// If path ends in slash (i.e. is a directory) then add "index.html".
			if (request_path[request_path.size() - 1] == '/')
			{
				request_path += "index.html";
			}

			// Determine the file extension.
			std::size_t last_slash_pos = request_path.find_last_of("/");
			std::size_t last_dot_pos = request_path.find_last_of(".");
			std::string extension;
			if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
			{
				extension = request_path.substr(last_dot_pos + 1);
			}

			// Open the file to send back.
			std::string doc_root_(argv[3]);
			std::string full_path = doc_root_ + request_path;
			std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
			if (!is)
			{
				rep = http::server::reply::stock_reply(http::server::reply::not_found);
				return;
			}

			// Fill out the reply to be sent to the client.
			rep.status = http::server::reply::ok;
			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				rep.content.append(buf, is.gcount());
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = std::to_string(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = http::server::mime_types::extension_to_type(extension);
		});

		server.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}
