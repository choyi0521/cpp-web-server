#include "server.hpp"
#include "mime_types.hpp"
#include "route_manager.hpp"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <fstream>

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

		server.add_route(
			"GET",
			http::server::RoutePath("/blog/([^/]*)$", http::server::MatchType::REGEX),
			[argv](const http::server::Request& req, http::server::Response& resp) {
				std::cout << "Request: " << req.decoded_uri << "\n";
				for (auto& it : req.path_match) {
					std::cout << "match: " << it << std::endl;
				}

				// Open the file to send back.
				std::string doc_root_(argv[3]);
				std::string full_path = doc_root_ + "/index.html";
				std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
				if (!is)
				{
					resp = http::server::Response::stock_reply(http::server::Response::not_found);
					return;
				}
				std::cout << "full_path: " << full_path << std::endl;

				// Fill out the reply to be sent to the client.
				resp.status = http::server::Response::ok;
				char buf[512];
				while (is.read(buf, sizeof(buf)).gcount() > 0)
					resp.content.append(buf, is.gcount());
				resp.headers.resize(2);
				resp.headers[0].name = "Content-Length";
				resp.headers[0].value = std::to_string(resp.content.size());
				resp.headers[1].name = "Content-Type";
				resp.headers[1].value = http::server::mime_types::extension_to_type("html");
			});

		server.add_route("GET", "/", [argv](const http::server::Request& req, http::server::Response& rep) {
			std::string request_path = req.decoded_uri;

			// Request path must be absolute and not contain "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos)
			{
				rep = http::server::Response::stock_reply(http::server::Response::bad_request);
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
				rep = http::server::Response::stock_reply(http::server::Response::not_found);
				return;
			}

			// Fill out the reply to be sent to the client.
			rep.status = http::server::Response::ok;
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
