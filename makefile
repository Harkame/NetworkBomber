MKDIR = mkdir -p

mkdir: directory main

directory:
	${MKDIR} ./bin/;

main: network_bomber

network_bomber:
	gcc ./src/network_bomber -pthread -o ./bin/network_bomber.o
