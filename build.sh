mkdir build
gcc main.c -o build/main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./build/main
