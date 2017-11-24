main:
	-rm -f client server
	g++ client.cpp -std=c++11  -pthread  -o client
	g++ -std=c++11 -pthread -o server server.cpp
