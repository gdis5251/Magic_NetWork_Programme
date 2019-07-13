.PHONY:all
all:client server

client:cal_client.cpp tcp_client.hpp tcp_socket.hpp
	g++ -o client cal_client.cpp -std=c++11 -ljsoncpp

server:cal_server.cpp select_tcp_server.hpp tcp_socket.hpp
	g++ -o server cal_server.cpp -std=c++11 -ljsoncpp

.PHONY:clean
clean:
	rm server client
