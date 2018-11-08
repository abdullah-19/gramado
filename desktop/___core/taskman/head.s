;
; Gramado Task Manager - This is a 32bit, user mode, system application. 
; It's a task manager to manage the system while we are in user mode.
; (c) Copyright 2015-2016 Fred Nora.
;
; File: head.s 
;
; Descri��o:
;     Parte principal Task Manager, TASKMAN.BIN. 
;     � o entry point do programa na arquitetura x86.
;     Faz a vez do crt0.o por enquanto.
;
; Alguns par�metros: (@todo: rever)
; ================== 
;  4MB � partir de 400000.
;  f�sico = virtual      
;  400000 = 400000
;
;  +O kernel � carrega o programa em 0x00400000 
;   com o entry point em 0x0041000.
;  +Quem configurou a pagina��o foi o Boot Loader.
;
;   Stack: (rever)
;   ss  = 0x23
;   esp = 0x00709fff0  ???
;
;
; Vers�o 1.0, 2015, 2016.
;


;;
;; codename db 'new york'
;;

segment .head_x86

 
[bits 32]
 
;
; Fun��es importadas.
;

extern _crt0
;extern _appMain
;extern _exit


;==============================
; _taskmanager_entry_point:
;     Entry point
;
; IN: ??
;
global _taskmanager_entry_point              
_taskmanager_entry_point:
    
	;NOP	
	
	call _crt0
	
	cmp eax, dword 0
	je retOk

HANG:	
	;NOP
	PAUSE
	JMP HANG

retOk:	
	;
	; tornar zombie
	;
    ;pushd eax
	;call _exit
	;mov byte [0x800000], byte "z"	
    ;mov byte [0x800001], byte 0x09	
	
	JMP HANG

;
; End.
;
