FLAG=tcp_client.hpp tcp_thread_server.hpp tcp_socket.hpp 

.PHONY:all
all:server client

server:cal_server.cpp $(FLAG)
	g++ -o $@ cal_server.cpp -std=c++11 -ljsoncpp -lpthread

client:cal_client.cpp $(FLAG)
	g++ -o $@ cal_client.cpp -std=c++11 -ljsoncpp

.PHONY:clean
clean: 
	rm client server