CXX = clang++
CXXFLAGS = -O2 -Wall -Wextra -pedantic -std=c++17 -I.
LIBS = -lGL -lglfw -lGLEW -lpng
GLM = /usr/include/glm
DEPS = shader.hpp png_writer.hpp $(GLM)
ODIR = obj
_OBJ = penrose.o shader.o png_writer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

penrose: $(OBJ) $(LIBS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f $(ODIR)/*.o

.PHONY: clean
