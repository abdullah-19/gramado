

/fs

 nesse diret�rio ficar� o suporte aos diversos sistemas de arquivos suportados
pelo sistema.
Mesmo que o driver seja um servidor em user mode ele poder� ter algum c�digo aqui
no kernel base que ajude a inicializar o servidor em user mode.
algum suporte a inicializa��o dos diversos sistemas de arquivos dever� ficar aqui mesmo.



    O sistema de arquivos ser� o controlador do sistema.
    Tudo ser� considerado um arquivo,
    At� mesmo o gerente de objetos estar� sujeito ao sistema de aruqivos
    para isso o diret�rio /objects ser� criado mas nunca ser� salvo no disco.
Esse diret�rio conter� o arquivo raiz da ger�ncia de objetos. Um descritor de
objeto significar o indice da entrada em /objects.


