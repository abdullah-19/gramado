:Setup
@echo off  

:Start
echo -{ ...
echo -{ Creating application SHELL.BIN ...
echo -{ Creating head.o ...
	nasm  head.s  -felf -o head.o
    rem essa deveria ser crt0.s ? (C RunTime)
	
echo -{ Creating main.o ...
    rem crt0.o n�o deve ser compilado aqui ... isso � s� um teste.
    gcc  -c  crt0.c -o crt0.o 	
	gcc  -c  main.c  -o main.o 
    gcc  -c  shellui.c  -o shellui.o
    gcc  -c  builtins.c  -o builtins.o	
	
:SetingObjects		
set myObjects=head.o ^
crt0.o ^
main.o ^
shellui.o ^
api.o ^
stdio.o ^
stdlib.o ^
string.o ^
conio.o ^
builtins.o  




:Linking
echo -{ ...
echo -{ Linking ... 
   ld -T link.ld -o SHELL.BIN %myObjects% -Map shell_map.s
   
   rem
   rem N�O pode deletar os objetos que foram inclu�dos.
   rem @todo: Deve haver uma pasta para biblioteca dentro do /sdk.
   rem ex: /sdk/libc   /sdk/gl.
   rem
   
:Moving
echo -{ Moving ...    
   move SHELL.BIN bin\SHELL.BIN 

:End   
echo End!
	pause