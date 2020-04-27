#include <boost/asio.hpp>
#include <iostream>
#include <string>

void read_data(boost::asio::ip::tcp::socket& socket, std::mutex& mutex, std::string name) {
	while (true) {
		boost::asio::streambuf buf;
		boost::asio::read_until(socket, buf, '\n');
		std::string msg;
		std::istream input_stream(&buf);
		std::getline(input_stream, msg, '\n');
		{
			std::lock_guard lock(mutex);
			std::cout << name << ": " << msg << std::endl;
		}
	}
}

std::string read_name(boost::asio::ip::tcp::socket& socket) {
	boost::asio::streambuf buf;
	boost::asio::read_until(socket, buf, '\n');
	std::string name;
	std::istream input_stream(&buf);
	std::getline(input_stream, name, '\n');

	return name;
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
	std::mutex mutex;
	std::cout << "Please, enter your name: ";
	std::getline(std::cin, name);
	boost::asio::write(socket, boost::asio::buffer(name + '\n'));

	/* working */
	std::thread thread{ read_data, std::ref(socket), std::ref(mutex), read_name(std::ref(socket)) };
	std::string msg;
	while (std::getline(std::cin, msg)) {
		boost::asio::write(socket, boost::asio::buffer(msg + '\n'));
	}

	boost::asio::write(socket, boost::asio::buffer("EOF\n"));
	thread.join();

	return 0;
}