.PHONY:ALL
ALL:Client Server

Client:echo_client.cpp
	g++ -o $@ $^ -std=c++11
Server:epoll.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm Server Client
