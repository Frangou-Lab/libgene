SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(SOURCES:%.cpp=%.o)
TARGET := libgene.a
CXXFLAGS := -std=c++17 -O3 -mavx

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	libtool -static -o $@ $+

$(OBJECTS): $(SOURCES)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f libgene.a $(OBJECTS)

