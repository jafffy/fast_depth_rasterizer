CC=g++
EXE=fast_depth_rasterizer
SRC=main.cc

all:
	$(CC) -o $(EXE) $(SRC) -O3

debug:
	$(CC) -o $(EXE) $(SRC) -g

profile:
	$(CC) -o $(EXE) -pg
