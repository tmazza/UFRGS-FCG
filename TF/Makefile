PP=g++
CC=gcc
BIN_DIR=./bin
SRC_DIR=./src

all: src/main.cpp
	$(CC) -c $(SRC_DIR)/bitmap.c -o $(BIN_DIR)/bitmap.o
	$(PP) -c $(SRC_DIR)/glm.cpp -o $(BIN_DIR)/glm.o
	$(PP) -c $(SRC_DIR)/glmimg.cpp -o $(BIN_DIR)/glmimg.o
	$(PP) -c $(SRC_DIR)/Texture.cpp -o $(BIN_DIR)/Texture.o
	$(PP) -c $(SRC_DIR)/Camera.cpp -o $(BIN_DIR)/Camera.o
	$(PP) -c $(SRC_DIR)/Bloco.cpp -o $(BIN_DIR)/Bloco.o
	$(PP) -c $(SRC_DIR)/Partida.cpp -o $(BIN_DIR)/Partida.o
	$(PP) -o main $(SRC_DIR)/main.cpp -lglut -lGLU -lGL -lm $(BIN_DIR)/*.o
clean:
	rm -rf ./main $(BIN_DIR)/*.o

exec: src/main.cpp
	$(PP) -o main $(SRC_DIR)/main.cpp -lglut -lGLU -lGL -lm  $(BIN_DIR)/*.o
