#reboot command for gde

#history:
#2019 - Created by Fred Nora.

#todo:
#fazer um makefile pra compilar o shell
#obs: #bugbug tem que arrumas as coisas aqui.



VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 0
EXTRAVERSION =
#NAME = reboot

#todo:
#We need to simplify these flags

CFLAGS = -m32 \
	--std=gnu89 \
	-nodefaultlibs \
	-nostdinc \
	-nostdlib \
	-static \
	-fgnu89-inline \
	-ffreestanding \
	-fno-builtin \
	-fno-pie \
	-no-pie \
	-fno-stack-protector \
	-s




LIBC    = ../../../lib/libc03/include/
LIBCOBJ = ../../../lib/libc03/obj


	##
	## Objects
	##

myObjects = crt0.o main.o \
ctype.o \
stdio.o \
stdlib.o \
string.o \
unistd.o \
termios.o \
ioctl.o \
fcntl.o \
stubs.o    



.PHONY: all all-c jackpot-link finalize clean

all: main.o all-c jackpot-link finalize clean
	@echo "Ok?"

main.o:
	gcc  -c  main.c  $(CFLAGS) -I $(LIBC) -o main.o

all-c:

	cp $(LIBCOBJ)/crt0.o   .
	cp $(LIBCOBJ)/stubs.o  .


	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/unistd.o  .
	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .
	cp $(LIBCOBJ)/fcntl.o  .



jackpot-link:
	ld -m elf_i386 -T ../link.ld -o REBOOT.BIN $(myObjects)    




finalize:
	cp REBOOT.BIN  ../bin

clean:
	-rm *.o
	-rm REBOOT.BIN 
	
	
	
