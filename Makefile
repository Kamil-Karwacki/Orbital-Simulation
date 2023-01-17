main:
	g++ -std=c++17 -pipe ./src/*.cpp ./include/imgui/*.cpp -o Export -static-libgcc -static-libstdc++ -w -Iinclude -Isrc -Llibs -lglfw3dll include/glad/glad.c

	