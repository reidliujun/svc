# Project: main
# Created by Médéric Blestel <mblestel@insa-rennes.fr>


SRC= $(wildcard *.c)

all: $(SRC:%.c=./obj/%.d) $(SRC:%.c=%.o) libmain.a($(SRC:%.c=%.o))

./obj/%.d: %.c
	@mkdir -p obj
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


-include $(SRC:.c=.d)

clean:
	rm -f *.o *.d *.a *.bak *.~
	rm -rf obj
