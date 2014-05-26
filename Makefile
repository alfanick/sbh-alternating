SOURCES=main.cpp reader.cpp oligo.cpp spectrum.cpp sequence.cpp node.cpp sequencer.cpp
CXX=clang++ -Wall -std=c++11 -g -stdlib=libc++ -O3 -fshort-enums
OBJECTS=$(patsubst %.cpp,obj/%.o,$(SOURCES))

.PHONY: all clean

all: bin/sbh

clean:
	rm bin/sbh
	rm obj/*.o

bin/sbh: $(OBJECTS)
	$(CXX) $(OBJECTS) -o bin/sbh

obj/%.o: src/%.cpp
	$(CXX) -c $< -o $@
