ifeq ($(CC),icl)
  CFLAGS+=/D "RELEASE"
else
ifeq ($(CC),cl)
  CFLAGS+=/D "RELEASE"
else
  CFLAGS+=-DRELEASE
endif
endif

SRCS=baseclasses_all.cpp

include ../makefile.inc

.all: $(OBJS)

clean:
	$(RM) $(TEMPFILES) *.a

#
# include dependency files if they exist
#
-include $(SRCS:.cpp=.d) $(SRCS_C:.c=.d)
