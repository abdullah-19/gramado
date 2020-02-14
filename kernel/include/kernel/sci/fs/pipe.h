

int sys_dup ( int oldfd );
int sys_dup2 (int oldfd, int newfd);
int sys_dup3 (int oldfd, int newfd, int flags);


//POSIX.1-2001, POSIX.1-2008.
int sys_pipe ( int *pipefd );



//#todo: rotina provisória
//o serviço de ler num pipe deve ser o mesmo de ler num stream dado o fd.
int sys_read_pipe ( int fd, int count );

//#todo: rotina provisória
//o serviço de escrever num pipe deve ser o mesmo de escrever num stream dado o fd.
int sys_write_pipe ( int fd, int count );



