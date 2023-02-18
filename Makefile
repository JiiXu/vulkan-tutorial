CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan
DIRS = build build/shaders

a.out: shaders src/*.cpp src/*.hpp
	g++ $(CFLAGS) src/*.cpp $(LDFLAGS) -o $@

shaders:
	glslc src/shaders/simple_shader.vert -o build/shaders/simple_shader.vert.spv
	glslc src/shaders/simple_shader.frag -o build/shaders/simple_shader.frag.spv

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out build/*.o build/shaders/*.spv

$(shell mkdir -p $(DIRS))
