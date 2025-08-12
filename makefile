# The compiler to use
CXX=  g++

# The name of the final executable
TARGET = flip

# All the source (.cpp) files
SRCS = main.cpp Scheme.cpp Rank1Tensor.cpp

# The default rule. Typing 'make' will run this rule.
all: $(TARGET)

# Rule to build the executable.
$(TARGET): $(SRCS)
	$(CXX) -o $(TARGET) $(SRCS)

# A clean rule to remove the executable.
# You can run this by typing 'make clean'.
clean:
	rm -f $(TARGET)
