bin=HttpdServer
cc=g++

$(bin):HttpdServer.cc
	$(cc) -o $@ $^ -std=c++11
