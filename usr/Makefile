INCLUDES += -I../include -I.

CFLAGS += -D_GNU_SOURCE
CFLAGS += $(INCLUDES)
ifneq ($(DEBUG),)
CFLAGS += -g -O0 -ggdb -rdynamic
else
CFLAGS += -g -O2 -fno-strict-aliasing
endif
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -DSS0D=\"ss0d\"
CFLAGS += -DSS0CLI=\"ss0cli\"
CFLAGS += -DSS0D_VERSION=\"$(VERSION)$(EXTRAVERSION)\"

PROGRAMS += ss0d ss0cli

LDFLAGS = -Wl,-E

.PHONY:all
all: $(PROGRAMS)

########
# SS0D #
########
SS0D_OBJS += ss0d.o util.o log.o event.o control.o
SS0D_DEP = $(SS0D_OBJS:.o=.d)

ss0d: $(SS0D_OBJS)
	echo $(CC) $^ -o $@ $(LIBS)
	$(CC) $^ -o $@ $(LDFLAGS) $(LIBS)

-include $(SS0D_DEP)

##########
# SS0CLI #
##########
SS0CLI_OBJS = ss0cli.o
SS0CLI_DEP = $(SS0CLI_OBJS:.o=.d)

ss0cli: $(SS0CLI_OBJS)
	echo $(CC) $^ -o $@
	$(CC) $^ -o $@

-include $(SS0CLI_DEP)

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	@$(CC) -MM $(CFLAGS) -MF $*.d -MT $*.o $*.c

clean:
	rm -f *.[od] *.so $(PROGRAMS)
