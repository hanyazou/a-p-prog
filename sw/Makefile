DESTDIR=
PREFIX=/usr/local

CC=gcc
CFLAGS=-g

all: pp3 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

CF_OBJS+=cf_p16f_a.o
CF_OBJS+=cf_p16f_b.o
CF_OBJS+=cf_p18f_qxx.o

pp3: pp3.o pp_ops.o pp_utils.o $(CF_OBJS)
	$(CC) $^ -o pp3

install: pp3 
	mkdir -p $(DESTDIR)/etc/pp3
	install -m0644 pp3_devices.dat $(DESTDIR)/etc/pp3/
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m0755 pp3 $(DESTDIR)$(PREFIX)/bin

clean:
	rm -f pp3 pp3.o pp_ops.o pp_utils.o  $(CF_OBJS)
