

/gramado

     Gramado � uma interface gr�fica, ela deve ficar dentro do
kernel base, para efeito de desempenho, mas fora dos outros componentes
do kernel base, para efeito de portabilidade. Desse modo poderemos
portar a interface gr�fica 'gramado' para outros kernel base com menos dificuldade.
     Muitas fun��es ser�o redefinidas e ter�o o nome traduzido para um nome
agrad�vel para a camada gramado. Essas fun��es ser�o canonizadas, permanecendo as
mesmas mesmo que a camada gramado seja usda em outro kernel.
    Ex: gramado_printf = kprintf


Diret�rios em /k/gramado:
========================	
	\gui     - Recursos gr�ficos padr�o. 
	\logoff  - A parte do kernelbase referente a ger�ncia de logoff.
	\logon   - A parte do kernelbase referente a ger�ncia de logon.
	\user    - A parte do kernelbase referente a gerencia de contas e usu�rios.
	
	
gramado\gui

    Recursos de suporte a interface gr�fica de usu�rio.
    O prop�sito � oferecer recursos gr�ficos de forma r�pida e direta.
Esse � o meio mais direto de um processo acessar recursos gr�ficos.
Muitas rotinas primitivas de recursos gr�ficos ser�o oferecidas dentro do
pr�prio kernel base para efeito de maior desempenho. Por�m haver�o bibliotecas em servidores de recursos gr�ficos 
em um user mode e em kernel mode. Cada servidor
de recurso gr�ficos ter� um desempenho e um privil�gio diferente. Os servidores de recursos gr�ficos poder�o utilizar 
das primitivas oferecidas pelo kernel base.

    Um servidor que ofere�a recursos gr�ficos de forma mais direta e com maior desempenho der� seu nome 
	relacionado com a palavra DIRECT ou FAST.
Ex: DIRECTMEDIA, DIRECTVIDEO, FASTVIDEO ... ETC...



Sobre o modulo gramado\gui:
===========================
    O prop�sito do m�dulo /gui � oferecer recursos para a interface gr�fica de usu�rio. 
	para que o usu�rio possa interagir com o sistema atrav�s de uma interface gr�fica, u
	sando janelas e menus...
    O m�dulo \gui da da interface gr�fica Gramado � o respons�vel pela parte da apresenta��o
da interface, S�O OS ELEMENTOS GR�FICOS PROPRIAMENTE DITOS. outros m�dulos dentro da pasta
\gramado poder�o ser criados para outras funcionalidades da interface gr�fica.

Outros m�dulos:
==============  
    Outro m�dulos de suporte a recursos gr�ficos poder�o ser criados aqui na camada \gramado.
modulos que gerenciem o suporte aos recurso necess�rios para se ter uma boa interface 
de usu�rio em modo gr�fico. Ex: Podem gerenciar os recursos de hardware como acelera��o 
2d e 3d entre outras funcionalidades. Como configra��es de propriedades f�sicas do video. 
Ex: Retra�o vertical...etc...Lembrando que quem acessar� o hardware ser� o m�dulo hal, 
aquia ficar�o alguns gerenciadores em camada mais alta.


 @todo:
 +Graphic Mode configuration.
 +Temas.
 +O diret�rio de imagens usadas pelo modo gr�fico
 +o arquivo que salva as configura��es de modo gr�fico.



  @todo:
  Sobre o processo de cria��o de janelas quando a rotina de cria��o de janelas � chamada por um processo em user mode:
  ====================================================================================================================
  
  *iMPORTANTE: Pode haver mais de um m�todo de cria��o de janela. Tanto eles podem coexistirem, quando podemos
               selecionar o melhor com o passar do tempo.

   + Uma op��o seria uma rotina em usermode de cria��o de janela realiza varias chamadas ao kernel soliciatando 
   primitivas de rotinas
     gr�ficas. Ex: Uma chamda estabelece as dimens�es da janela e outra as cores, epor fim uma rotina monta o 
	 window frame todo
     com os elementos previamente passados. 

     *importante:
     Obs: Uma rotina de inicializa��o do procedimento de pintura garante exclusividade para
     o processo, assim nenhum outro processo poder� realizar essa rotina de pintura enquanto o processo n�o 
	 tiver acabado.
	 
	 
	 
	 


    