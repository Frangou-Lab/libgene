SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(SOURCES:%.cpp=%.o)
TARGET := libgene.a
EXTERNAL_LIBRARIES := -lz

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)

$(OBJECTS): $(SOURCES)
	$(CC) $(LDFLAGS) -c $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f libgene.a $(OBJECTS)

