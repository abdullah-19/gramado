sobre o diret�rio /include:
============================


   /include stuff.


   O diret�rio /include cont�m headers da biblioteca padr�o C e os principais
   header do kernel, principalmente os que tem atua��o global e n�o se
   incluem nos m�dulos /executive /hal /microkernel e /gramado.


*****************************************************
 Os diret�rios s�o:
 ==================

/gramado          - Headers para a GUI.
/executive        - Headers para o executive do kernel.
/microkernel      - Headers para o microkernel do kernel.
/hal              - Headers para o hal do kernel.
 

Obs: Cada diret�rio � uma camada do kernel, sendo /gramado a
camada mais alta, compreendendo os recursos de interface gr�fica.

*******************************************************


   Criado o arquivo de defini��es globais 'gdef.h'
   ele deve ficar no in�cio das defini��es, pois
   servir� � todos os m�dulos.
  
   set 2017.