Portando o interpretador de basic do mikeos 4.5

Com isso poderemos usar os programas em Basic feitos para 
o mikeos 4.5


Nessa pasta ficar�o v�rios arquivos em assembly que d�o suporte
ao arquivo basic.asm.


Todos arquivos ser�o inclu�dos em basic.asm e esse ser� inclu�do 
ao shell como um comando interno.


As fun��es precisam mudar de nome para n�o darem conflito com as
fun��es do shell 

As fun��es poder�o come�ar com 'basic_', para
n�o serem chamadas pelo shell.

Poderemos importar as fun��es as mesmas fun��es usadas pelo shell.
como printf da libc.para Isso o basic ter� um stuc em C,
ent�o o basic importar� as
fun��es do stub.  'bstub.c'.




