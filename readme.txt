===================================================
for developers:
    You're gonna need the folders /include and /lib.
===================================================


Isso ser� um conjunto de aplicativos em user mode.

+N�o haver� c�digo em ring0 em /gramado.
O Norax kernel poder� ser incluido no projeto em sua forma bin�ria,
compilada em outro projeto.

+N�o h� bootloader pra esse projeto, ele rodar� sobre o 
 kernel Norax e poder� ser instalado nele.
 os bin�rios do Gramado podem ser instalados em Norax..
 

=========================================


    Main folders for Gramado Operating System.



//(relations of production)


//==============
/arch     
    Archtechture stuff.
    Here you can make th VHD file to run Gramado on a VM.


//==============
/bin      
    .bin compiled files.
    All the executable files are here.

//===============
/cmd 
    Console applications.

//===============
/docs    
    Documentation.
 

//================
/fasm
/fasm-win
    Trying to port fasm to Gramado.
    S�o duas tentativas diferentes de portar o fasm para o gramado.
    Uma delas � usando a interface do fasm com o windows.
    A outra � a interface com a linguagem C unix-like.
    A alternativa usando a libc parece ser a melhor op��o.


//================
/gfe
    Gramado File Explorer

//================
/gramcc
    Gramado Compiler Collection.

//================
/gramcode
    Source code editor.

//================
/gramtext
     Text editor.

//================
/gwm
     Gramado Window Manager
     GUI in user mode.

//================
/include  
    Headers.
    libc headers.
    Use these header to create the applications.

//================
/init
     Gramado initialization stuff.

//================    
/lib      
    Libs.
    Compiled libs.
    Source codes for libs.

//================
/licenses
    Licenses

//================
/scripts  
    Scripts.





//(material productive forces)
*/tools    - Different tools. 



//(User ideas, human action)
*/user     - User ideas.
