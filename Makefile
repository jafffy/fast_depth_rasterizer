CC=g++
EXE=fast_depth_rasterizer
SRC=main.cc

all:
	$(CC) -o $(EXE) $(SRC)

debug:
	$(CC) -o $(EXE) $(SRC)
