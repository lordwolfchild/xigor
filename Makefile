# gcc `pkg-config gtk+-3.0 --cflags --libs` xigor.c -o xigor

GTK3-DEPS :=  $(shell pkg-config gtk+-3.0 --cflags --libs)
GCC-OPTS := "-Wno-deprecated-declarations"

default: all

all: xigor

xigor: xigor.c
	gcc $(GTK3-DEPS) $(GCC-OPTS) -o $@ $<

clean:
	rm -rf xigor
