Executive: (Internal Servers) 

   Sobre o diret�rio /executive:
   ============================

   O diret�rio /executive � dividido em partes:
   
   /dd    device drivers
   /fs    file system
   /sci   sytem call interface

 
   Ambiente= ring0

   S�o os m�dulos servidores incluidos no Kernel Base.
   
   Os processos em user mode podem chamar esses servi�os via interrup��o, 
atrav�s do kernel


   Client --> Kernel --> Server


   Pode haver algum tipo de encapsulamento de vari�veis e estruturas
   desses m�dulos, o que permite alguma portabilidade desses m�dulos.



   *** Boa parte do c�digo desses porcessos ser�o aproveitados 
   para drivers e servidores em user mode. por exemplo a GUI.



   //
   //    ****    O SHELL E O EXECUTIVE    ****
   //


 O utilit�rio 	SHELL.BIN em user mode poder� chamar, atrav�z de comandos, v�rios programas,
ou funcionalidades internas. Quando o shell ou outro programa chamar rotinas de gerenciamento de sistema ele deve interacir principalmente com o m�dulo /sm, que � o console do sistema.

   //
   //    ****    OS UTILIT�RIOS E O EXECUTIVE    ****
   //



    Como os utilit�rios tem por objetivo principal algum tipo de an�lise ou gerenciamento do sistema,
� natural que eles interajam com o m�dulo /executive. sendo o executive a principal porta de entrada
para os utilit�rios ... em especial o m�dulo /executive/sm, que � o console do sistema.

  Na verdade topo programa em user mode devera utilizar o executive como porta de entrada, mas apenas os utilit�rios utilizar�o mais as funcionalidades de console /sm.





























