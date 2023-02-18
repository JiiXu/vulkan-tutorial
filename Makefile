CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

a.out: src/*.cpp src/*.hpp
	g++ $(CFLAGS) src/*.cpp $(LDFLAGS) -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out

