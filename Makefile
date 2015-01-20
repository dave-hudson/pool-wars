CC := gcc
CFLAGS := -std=gnu99 -O2 -Wall
LDFLAGS := -std=gnu99
RM := rm

#
# Define our target app.
#
APP := pool_wars

#
# Define the source files for our build.
#
POOL_WARS_SRCS := pool_wars.c

#
# Create a list of object files from source files.
#
POOL_WARS_OBJS := $(patsubst %.c,%.o,$(filter %.c,$(POOL_WARS_SRCS)))


%.o : %.c
	$(CC) $(CFLAGS) -MD -c $< -o $@

.PHONY: all

all: $(APP)

pool_wars: $(POOL_WARS_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(POOL_WARS_OBJS)

-include $(POOL_WARS_OBJS:.o=.d) 

.PHONY: clean

clean:
	$(RM) -f $(APP) *.o $(patsubst %,%/*.o,$(SUBDIRS))
	$(RM) -f $(APP) *.d $(patsubst %,%/*.d,$(SUBDIRS))

.PHONY: realclean

realclean: clean
	$(RM) -f *~ $(patsubst %,%/*~,$(SUBDIRS))
	$(RM) -f *.txt $(patsubst %,%/*~,$(SUBDIRS))

