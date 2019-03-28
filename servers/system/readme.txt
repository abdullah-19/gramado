https://en.wikipedia.org/wiki/Runlevel

    ===================================
   /system

   Conjunto de programas que s�o usados durante a inicializa��o do sistema.
   Um desses programas podem ser o primeiro processo a rodar, normalmente 
   � o programa INIT.BIN.

   Ambiente: Apenas Ring 3.
   
   V�rias servidores de gerenciamento do sistema far�o parte
   desse ambiente chamado systemd.
   
   Em system/core/ ficar�o os programas principais do system.
   
   +init � um processo que serve para ser clonado para que o
   shell execute um programa no ambiente gramado core. Isso � provis�rio.
   
   +shell � um ambiente de usu�rio, tem um terminal de comandos
    e um servidor de recursos gr�ficos chamado gws.
   
   +taskman ser� o gerenciador principal.
   
   
   Outros programas far�o parte do ambiente system, todos voltados
   ao gerenciamento de recursos b�sicos do sistema.

   
   ===============================================
   /system/core

   Gramado Core Environment.

   Ambiente b�sico onde esses programas de habitam.
   Outros componentes do sistema podem atuar diretamente 
   no gramado core.

   Os programas do gramado core s�o:
   /system/core/init
   /system/core/shell
   /system/core/taskman   
   
   ==============================================

   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
  