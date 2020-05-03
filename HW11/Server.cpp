#include <boost/asio.hpp>
#include <iostream>
#include <string>

std::string read_data(boost::asio::ip::tcp::socket& socket) {
	boost::asio::streambuf buf;
	boost::asio::read_until(socket, buf, '\n');
	std::string msg;
	std::istream input_stream(&buf);
	std::getline(input_stream, msg, '\n');

	return msg;
}

void read_all(boost::asio::ip::tcp::socket& socket, std::string name) {
	while (true) {
		std::cout << name << ": " << read_data(socket) << std::endl;
	}
}

int main() {
	const auto port = 3333;
	std::string local_host = "127.0.0.1";
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(local_host), port);
	boost::asio::io_service ios;
	boost::asio::ip::tcp::acceptor acceptor(ios, endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen();
	boost::asio::ip::tcp::socket socket(ios);
	acceptor.accept(socket);

	/* exchange of names */
	std::string name;
	std::cout << "Please, enter your name: ";
	std::getline(std::cin, name);
	boost::asio::write(socket, boost::asio::buffer(name + '\n'));

	/* working */
	std::thread thread{ read_all, std::ref(socket), read_data(socket) };
	std::string msg;
	while (std::getline(std::cin, msg)) {
		boost::asio::write(socket, boost::asio::buffer(msg + '\n'));
	}

	boost::asio::write(socket, boost::asio::buffer("EOF\n"));
	thread.join();

	return 0;
}
