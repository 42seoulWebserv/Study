#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

sockaddr_in make_ip_address(int domain, int port, uint32_t ipaddr) {
	sockaddr_in addr;
	addr.sin_family = domain;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(ipaddr);
	memset(&addr.sin_zero, 0, sizeof(addr.sin_zero));
	return addr;
}

int main() {
	/**socket(int domain, int type, int protocol)
     * domain: AF_INET, AF_INET6, AF_UNIX
     * type: SOCK_STREAM, SOCK_DGRAM, SOCK_RAW
     * protocol: 0 (default)
    */
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		std::cerr << "Failed to create socket!" << std::endl;
		return 1;
	}
	sockaddr_in server_addr = make_ip_address(AF_INET, 8080, INADDR_ANY);

	/**int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
     * sockfd: socket file descriptor
     * addr: socket address 	// bind() 함수 호출 시, sockaddr_in을 sockaddr로 형 변환하여 전달
     * addrlen: socket address length
    */
	if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Failed to bind socket!" << std::endl;
		exit(EXIT_FAILURE);
	}

	/**int listen(int sockfd, int backlog);
     * sockfd: socket file descriptor
     * backlog: maximum length to which the queue of pending connections for sockfd may grow
    */
	if (listen(socket_fd, 1) < 0) {
		std::cerr << "Failed to listen socket!" << std::endl;
		exit(EXIT_FAILURE);
	}

	int new_socket;									  // 새로운 소켓 파일 디스크립터를 위한 변수 선언
	sockaddr_in client_addr;						  // 클라이언트 주소 정보를 위한 구조체
	socklen_t client_addr_len = sizeof(client_addr);  // 클라이언트 주소의 크기

	char buffer[1024] = {0};
	while (true) {
		new_socket = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
		if (new_socket < 0) {
			std::cerr << "Failed to accept socket!" << std::endl;
			exit(EXIT_FAILURE);
		}
		int valread = recv(new_socket, buffer, 1024, 0);
		std::cout << "Connection accepted! but I'm thinking now." << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));  // 5초 동안 대기
		std::cout << "Received: " << buffer << std::endl;
		send(new_socket, buffer, valread, 0);
		close(new_socket);
		memset(buffer, 0, sizeof(buffer));
	}
	close(socket_fd);
	return 0;
}