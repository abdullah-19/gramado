

//   sys.h 




//SVr4,  4.3BSD,  POSIX.1-2001. 
int sys_open ( const char *pathname, int flags, mode_t mode );
int sys_close ( int fd );





// 289
int sys_serial_debug_printk ( char *s );


// 18
int sys_read (unsigned int fd,char *buf,int count);


// 19
int sys_write (unsigned int fd,char *buf,int count);







//
// PROCESSES
//


// 73
void *sys_create_process ( struct room_d *room,
                           struct desktop_d  *desktop,
                           struct window_d *window,
                           unsigned long init_eip, 
                           unsigned long priority, 
                           int ppid, 
                           char *name, 
                           unsigned long iopl,
                           unsigned long directory_address );



int sys_getpid (void);
int sys_getppid (void);

int sys_fork (void);

void sys_exit_process (int pid, int code);




//
// THREADS
//


// 72
void *sys_create_thread ( struct room_d *room,
                          struct desktop_d  *desktop,
                          struct window_d *window,
                          unsigned long init_eip, 
                          unsigned long priority, 
                          int ppid, 
                          char *name );

void sys_exit_thread (int tid);

void sys_dead_thread_collector (void);


//Sincronização do retraço vertical.
void sys_vsync (void);


//Show kernel info.
void sys_showkernelinfo (void);

//Show pci info.
int sys_showpciinfo (void);


//
// reboot and shutdown
//

void sys_reboot (void);
void sys_shutdown (void);


// 178
unsigned long sys_get_file_size ( char *path );


// Usada por vários serviços de debug.
// Usada para debug.
void sys_show_system_info ( int n );


//
// End.
//

