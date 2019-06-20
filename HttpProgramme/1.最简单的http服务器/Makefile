.PHONY:all
all:http

http:http.cpp tcp_thread_server.hpp tcp_socket.hpp
	g++ -o $@ http.cpp -std=c++11 -lpthread

.PHONY:clean
clean: 
	rm http