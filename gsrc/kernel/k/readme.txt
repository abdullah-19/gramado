
Sobre o kernel base:
====================
    O kernel base � o n�cleo do sistema operacional, roda no ambiente
ring0 e � construido em camadas. O execut�vel chama-se KERNEL.BIN.


k\sm.c:
    respons�vel por gerenciar os servidores que compoem o sistema.
    Obs: esses servidores podem ser persistentes e ficarem dentro
do kernel base o tempo todo ou podem ser caregados pelo sistema de arquivos.
   
   

*******************************************************************************************
Lista de diret�rios do kernel base:
===================================

    /gramado      - (4) GUI.
    /executive    - (3) Drivers VIPs, que ficam em kernel mode.
    /microkernel  - (2) Kernel m�nimo e fundamental. (processos e threads).	
    /hal          - (1) Hardware abstraction layer

	
    Os diret�rios representam as camadas do kernel base. 
	
    A camada mais alta � a camada /gramado, que � a interface gr�fica.
    A camada mais baixa � a camada /hal que lida liretamente com o hardware. Mas
essa camada poder� inda chamar um modulo externo em kernel mode que possivelmente ser� criado
e chamar� HAL.BIN, para efeito de portabilidade do kernel base, o m�dulo externo HAL.BIN
se encarregar� das diferen�as de arquitetura deixando para o m�dulo interno /hal o trabalho
de interface entre o kernel base e o m�dulo externo HAL.BIN.	
*********************************************************************************************	


Sobre o executive:
    S�o modulos incluidos ao 'kernel base' na hora da compila��o
    pois s�o modulos e drivers sistemicamente importantes.
    os m�dulos do kernel que ser�o ligados dinamicamente ser�o
 m�dulos de menor importancia para o funcionamento do sistema, como o kd.
   por exermplo: n�o se deve fazer um modulo para o timer.

...
 

Obs:
    A outra parte do controle do hardware fica em user mode. 
    (servers, drivers)




Design Notes 1.


Notas de Design que descreve as principais partes do Kernel Base.
Estamos no diret�rio /k e cada um dos diret�rios contidos aqui representam uma
das partes fundamentais do Kernel Base, S�o elas /hal, /microkernel, /executive e /gramado.


Pode-se imaginas o kernel dividido em 3 classes principais. As classes do kernel s�o: hal, microkernel, executive.


    k.hal
    k.microkernel
    k.executive
    k.gramado

    Kernel classes (k.____).
	
/gramado � a camada mais externa.	


Vers�o do documento: 1.0, 2016.
   


  
;
; fim.
;