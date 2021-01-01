CC=gcc
CFLAGS=-I . -lstdc++ -lm -pg
DEPS=gameState.hpp gameParts.hpp search.hpp controller.hpp ../rank/surfaces.hpp

ODIR=obj
_OBJ=gameTest.o gameParts.o gameState.o search.o controller.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

gameTest: $(OBJ) obj/surfaces.o
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/surfaces.o: ../rank/surfaces.hpp
	$(CC) -c -o $@ ../rank/surfaces.cpp $(CFLAGS)

clean:
	rm gameTest $(ODIR)/*.o
