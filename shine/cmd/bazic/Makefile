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
API02   = ../../../lib/libcore/include/
APIOBJ  = ../../../lib/libcore/obj


	##
	## Objects
	##

myObjects = crt0.o main.o \
ctype.o \
stdio.o \
stdlib.o \
string.o \
time.o \
api.o \
unistd.o \
math.o \
strtol.o \
strtoul.o \
fscanf.o \
fcntl.o \
termios.o \
ioctl.o \
wait.o \
stubs.o    



.PHONY: all all-c jackpot-link finalize clean

all: main.o all-c jackpot-link finalize clean
	@echo "Ok?"

main.o:
	gcc  -c  main.c  -Wall  $(CFLAGS) -I $(LIBC) -I $(API02) -o main.o


all-c:

	cp $(LIBCOBJ)/crt0.o   .
	
	cp $(LIBCOBJ)/stubs.o  .
	
	cp $(LIBCOBJ)/ctype.o   .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/time.o    .
	cp $(LIBCOBJ)/unistd.o  .

	cp $(LIBCOBJ)/math.o  .
	cp $(LIBCOBJ)/fscanf.o  .

	cp $(LIBCOBJ)/strtol.o  .
	cp $(LIBCOBJ)/strtoul.o  .
	
	cp $(LIBCOBJ)/fcntl.o  .
	cp $(LIBCOBJ)/termios.o  .
	cp $(LIBCOBJ)/ioctl.o  .
	
	cp $(LIBCOBJ)/wait.o .

	cp $(APIOBJ)/api.o      .



jackpot-link:
	ld -m elf_i386 -T ../link.ld -o BAZIC.BIN $(myObjects)    
	
	

finalize:
	cp BAZIC.BIN ../bin 

clean:
	-rm *.o
	-rm BAZIC.BIN 
	
	
