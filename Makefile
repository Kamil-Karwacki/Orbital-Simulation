main:
	g++ -pipe ./src/*.cpp ./include/imgui/*.cpp -o Export -w -Iinclude -Isrc -Llibs -lglfw3dll include/glad/glad.c

	