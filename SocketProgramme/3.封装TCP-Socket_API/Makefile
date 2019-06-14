.PHONY:all
all:server client

server:dict_server.cpp
	g++ -o $@ $^ -std=c++11 -g

client:dict_client.cpp
	g++ -o $@ $^ -std=c++11 -g

.PHONY:clean
clean:
	rm server client
