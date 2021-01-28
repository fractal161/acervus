CC=gcc
CFLAGS=-I . -lstdc++ -lm -std=c++14 -O3 -lgmp -lgmpxx -march=native
PFLAGS=-pg -fsanitize=undefined
DEPS=gameState.hpp gameParts.hpp search.hpp controller.hpp ../rank/surfaces.hpp

ODIR=obj
PODIR=pobj
_OBJ=gameTest.o gameParts.o gameState.o search.o controller.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
POBJ=$(patsubst %,$(PODIR)/%,$(_OBJ))


gameTest: $(OBJ) obj/surfaces.o
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/surfaces.o: ../rank/surfaces.hpp
	$(CC) -c -o $@ ../rank/surfaces.cpp $(CFLAGS)

profGameTest: $(POBJ) obj/surfaces.o
	$(CC) -o $@ $^ $(CFLAGS) $(PFLAGS)

$(PODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(PFLAGS)

$(PODIR)/surfaces.o: ../rank/surfaces.hpp
	$(CC) -c -o $@ ../rank/surfaces.cpp $(CFLAGS) $(PFLAGS)



clean:
	rm gameTest $(ODIR)/*.o profGameTest $(PODIR)/*.o
