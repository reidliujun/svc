# Project: SVC
# Created by M�d�ric Blestel <mblestel@insa-rennes.fr>


INCS = -I"../../AVC/h264_baseline_decoder/lib_baseline" -I"../../AVC/h264_main_decoder/lib_main"

SRC = $(wildcard *.c)

all: $(SRC:%.c=./obj/%.d) $(SRC:%.c=%.o) libsvc.a($(SRC:%.c=%.o))

./obj/%.d: %.c
	@mkdir -p obj
	@set -e; rm -f $@; \
	$(CC) $(INCS) $(CFLAGS) -MM $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	

-include $(SRC:.c=.d)

clean:
	rm -f *.o *.d *.a
	rm -rf obj
