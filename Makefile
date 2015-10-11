CC = g++
CFLAGS = -Wall
PROG = Carroms_By_201301040

SRCS = Carroms_201301040.cpp
LIBS = -lglut -lGL -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
