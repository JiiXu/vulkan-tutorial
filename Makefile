CC = clang++
CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan
DIRS = build assets/shaders
DEPS = build/first_app.o build/pipeline.o build/swap_chain.o build/window.o build/device.o build/model.o

first_app: shaders $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) src/main.cpp $(LDFLAGS) -o $@

build/first_app.o:
	$(CC) -c $(CFLAGS) src/first_app.cpp $(LDFLAGS) -o $@

build/model.o:
	$(CC) -c $(CFLAGS) src/model.cpp $(LDFLAGS) -o $@

build/pipeline.o:
	$(CC) -c $(CFLAGS) src/pipeline.cpp $(LDFLAGS) -o $@

build/swap_chain.o:
	$(CC) -c $(CFLAGS) src/swap_chain.cpp $(LDFLAGS) -o $@

build/window.o:
	$(CC) -c $(CFLAGS) src/window.cpp $(LDFLAGS) -o $@

build/device.o:
	$(CC) -c $(CFLAGS) src/device.cpp $(LDFLAGS) -o $@

shaders:
	glslc src/shaders/simple_shader.vert -o assets/shaders/simple_shader.vert.spv
	glslc src/shaders/simple_shader.frag -o assets/shaders/simple_shader.frag.spv

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out build/*.o build/shaders/*.spv

$(shell mkdir -p $(DIRS))
