
 Gramado:
 =======

     'Gramado OS'
     Esse ser� o nome do sistema operacional.
     Servir� de guia para todos os componentes criados, como:
     bm, bl, kernel, idle, shell, taskman ...


 Diret�rios:
 ==========

     A organiza��o dos diret�rios no desenvolvimento do sistema operacional Gramado utilizando 
a plataforma Windows:

************************************************
 Principais:
 ===========
 
         /gbuild    - Scripts e documenta��o para compila��o.
	 /gdocs     - Documenta��o.
	 /gramado   - Destino de bin�rios e arquivos de configura��o.
	 /gsdk      - Ferramentas para desenvolvimento.
	 /gsrc      - *C�digos.
	 
	 
***********************************************
 Organiza��o do diret�rio /gsrc:
 ==============================
 
         /boot   - Programas utilizados na inicializa��o.
	 /config - Arquivos de configura��o.
	 /gapps  - Aplica��es em user mode.
	 /glibs  - Bibliotecas e frameworks para user mode. (backend support??)
         /gshell - Conjunto de programas para intera��o com o usu�rio.
	 /gutil  - Utilit�rios do sistema.
	 /kernel - O programa kernel.
	 

 As camadas principais em user mode devem ser:
 =============================================
    +Shells           /gshell
    +Utilities        /gutil
    +Applications     /gapp
    +Libraries        /glib




