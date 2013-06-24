# Sources:
SRCS:=main.c aux.c mem_init.c sem_init.c slave.c
OBJS:=$(SRCS:.c=.o)

# Config:
CC:=gcc
CFLAGS:= -c
LD:=gcc

all: p_calc.x

clean:
	@echo Cleaning.
	@rm -f *.o
	@rm -f p_calc.x

p_calc.x: $(OBJS)
	@echo $@
	@$(LD) -o $@ $^

aux.o: aux.c header.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

mem_init.o: mem_init.c header.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

sem_init.o: sem_init.c header.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

slave.o: slave.c header.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

main.o: main.c  main.h
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

main.h: header.h
	@echo $@
	@ touch $@

.PHONY: all clean