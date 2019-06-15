SERVER=tcp_server.hpp tcp_thread_server.hpp tcp_socket.hpp
CLIENT=tcp_client.hpp tcp_socket.hpp

.PHONY:all
all:server client

server:dict_server.cpp $(SERVER)
	g++ -o $@ dict_server.cpp -std=c++11 -g -lpthread

client:dict_client.cpp $(CLIENT)
	g++ -o $@ dict_client.cpp -std=c++11 -g

.PHONY:clean
clean:
	rm server client
