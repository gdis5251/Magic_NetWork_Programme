.PHONY:all
all:server client

server:echo_server.cpp
	g++ -o server echo_server.cpp -std=c++11

client:echo_client.cpp
	g++ -o client echo_client.cpp -std=c++11

.PHONY:clean
clean:
	rm server client
