CC := gcc

# Detect the operating system
OS := $(shell uname -s)

# Set CCFLAGS based on the detected OS
ifeq ($(OS),Darwin)
CCFLAGS := -Xclang -fopenmp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp
else
ifeq ($(OS),Linux)
CCFLAGS := -fopenmp
else
$(error Unsupported operating system: $(OS))
endif
endif

# Define the target executable
TARGET := bellmanford

# Define the source files
SRCS := bellmanford.c

# Build rules
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.o
