bin=HttpdServer
cc=g++

$(bin):HttpdServer.cc
	$(cc) -o $@ $^ -lpthread -std=c++11
