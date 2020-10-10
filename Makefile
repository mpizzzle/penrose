CXX = clang++
CXXFLAGS = -O2 -Wall -Wextra -pedantic -std=c++17 -I.
LIBS= -lSDL2
ODIR = obj
_OBJ = penrose.o

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(LIBS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

penrose: $(OBJ) $(LIBS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f $(ODIR)/*.o

.PHONY: clean
