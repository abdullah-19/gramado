
Allocating common symbols
Common symbol       size              file

g_cursor_x          0x4               crt0.o
taskmanagerBuffer   0x200             main.o
CurrentWindow       0x4               crt0.o
stdout              0x4               crt0.o
mm_prev_pointer     0x4               stdlib.o
mmblockList         0x400             stdlib.o
last_valid          0x4               stdlib.o
heapList            0x400             api.o
libcHeap            0x4               api.o
g_char_attrib       0x4               crt0.o
g_rows              0x4               crt0.o
dialogbox_button2   0x4               api.o
Streams             0x80              crt0.o
messagebox_button1  0x4               api.o
g_using_gui         0x4               crt0.o
ApplicationInfo     0x4               crt0.o
randseed            0x4               stdlib.o
heap_start          0x4               api.o
prompt_out          0x400             crt0.o
BufferInfo          0x4               crt0.o
g_available_heap    0x4               api.o
g_heap_pointer      0x4               api.o
taskmanagerStatus   0x4               main.o
g_columns           0x4               crt0.o
HEAP_SIZE           0x4               api.o
mmblockCount        0x4               stdlib.o
dialogbox_button1   0x4               api.o
prompt_pos          0x4               crt0.o
stdin               0x4               crt0.o
last_size           0x4               stdlib.o
prompt_status       0x4               crt0.o
prompt_err          0x400             crt0.o
CursorInfo          0x4               crt0.o
heap_end            0x4               api.o
stderr              0x4               crt0.o
prompt              0x400             crt0.o
HEAP_END            0x4               api.o
rect                0x4               crt0.o
g_cursor_y          0x4               crt0.o
taskmanagerBufferPos
                    0x4               main.o
ClientAreaInfo      0x4               crt0.o
messagebox_button2  0x4               api.o
Heap                0x4               api.o
current_semaphore   0x4               crt0.o
prompt_max          0x4               crt0.o
current_mmblock     0x4               stdlib.o
taskmanTest1        0x4               crt0.o
heapCount           0x4               api.o
HEAP_START          0x4               api.o
taskmanagerError    0x4               main.o

Memory Configuration

Name             Origin             Length             Attributes
*default*        0x0000000000000000 0xffffffffffffffff

Linker script and memory map


.text           0x00000000004a1000     0x7000
                0x00000000004a1000                code = .
                0x00000000004a1000                _code = .
                0x00000000004a1000                __code = .
 *(.head_x86)
 *(.text)
 .text          0x00000000004a1000       0x5a crt0.o
                0x00000000004a1000                crt0
 .text          0x00000000004a105a      0x454 main.o
                0x00000000004a106a                tmSleep
                0x00000000004a1087                tmProbeProcessList
                0x00000000004a10a5                tmCreateTaskBar
                0x00000000004a1110                tmProc
                0x00000000004a119c                tmUpdateStatus
                0x00000000004a11d9                tmDrawBar
                0x00000000004a122e                tmSetCursor
                0x00000000004a1244                tmstrlen
                0x00000000004a1272                DoProgress
                0x00000000004a1319                DoSome
                0x00000000004a1371                progress_bar_test
                0x00000000004a137f                tmInit
                0x00000000004a13a7                main
 .text          0x00000000004a14ae     0x2017 api.o
                0x00000000004a14ae                system_call
                0x00000000004a14d6                apiSystem
                0x00000000004a18de                system1
                0x00000000004a18ff                system2
                0x00000000004a1920                system3
                0x00000000004a1941                system4
                0x00000000004a1962                system5
                0x00000000004a1983                system6
                0x00000000004a19a4                system7
                0x00000000004a19c5                system8
                0x00000000004a19e6                system9
                0x00000000004a1a07                system10
                0x00000000004a1a28                system11
                0x00000000004a1a49                system12
                0x00000000004a1a6a                system13
                0x00000000004a1a8b                system14
                0x00000000004a1aac                system15
                0x00000000004a1acd                refresh_buffer
                0x00000000004a1ba9                print_string
                0x00000000004a1baf                vsync
                0x00000000004a1bc9                edit_box
                0x00000000004a1be5                chama_procedimento
                0x00000000004a1bef                SetNextWindowProcedure
                0x00000000004a1bf9                set_cursor
                0x00000000004a1c10                put_char
                0x00000000004a1c16                carrega_bitmap_16x16
                0x00000000004a1c2f                apiShutDown
                0x00000000004a1c46                apiInitBackground
                0x00000000004a1c4c                MessageBox
                0x00000000004a21d8                mbProcedure
                0x00000000004a2246                DialogBox
                0x00000000004a25f6                dbProcedure
                0x00000000004a2664                call_kernel
                0x00000000004a27df                call_gui
                0x00000000004a286b                APICreateWindow
                0x00000000004a28e4                APIRegisterWindow
                0x00000000004a290c                APICloseWindow
                0x00000000004a2934                APISetFocus
                0x00000000004a295c                APIGetFocus
                0x00000000004a2971                APIKillFocus
                0x00000000004a2999                APISetActiveWindow
                0x00000000004a29c1                APIGetActiveWindow
                0x00000000004a29d6                APIShowCurrentProcessInfo
                0x00000000004a29ec                APIresize_window
                0x00000000004a2a06                APIredraw_window
                0x00000000004a2a20                APIreplace_window
                0x00000000004a2a3a                APImaximize_window
                0x00000000004a2a56                APIminimize_window
                0x00000000004a2a72                APIupdate_window
                0x00000000004a2a8e                APIget_foregroung_window
                0x00000000004a2aa4                APIset_foregroung_window
                0x00000000004a2ac0                apiExit
                0x00000000004a2add                kill
                0x00000000004a2ae3                dead_thread_collector
                0x00000000004a2af9                api_strncmp
                0x00000000004a2b5c                refresh_screen
                0x00000000004a2b72                api_refresh_screen
                0x00000000004a2b7d                apiReboot
                0x00000000004a2b93                apiSetCursor
                0x00000000004a2bab                apiGetCursorX
                0x00000000004a2bc3                apiGetCursorY
                0x00000000004a2bdb                apiGetClientAreaRect
                0x00000000004a2bf3                apiSetClientAreaRect
                0x00000000004a2c12                apiCreateProcess
                0x00000000004a2c2b                apiCreateThread
                0x00000000004a2c44                apiStartThread
                0x00000000004a2c60                apiFOpen
                0x00000000004a2c8c                apiSaveFile
                0x00000000004a2cdf                apiDown
                0x00000000004a2d32                apiUp
                0x00000000004a2d85                enterCriticalSection
                0x00000000004a2dc0                exitCriticalSection
                0x00000000004a2dd9                initializeCriticalSection
                0x00000000004a2df2                apiBeginPaint
                0x00000000004a2dfd                apiEndPaint
                0x00000000004a2e08                apiPutChar
                0x00000000004a2e24                apiDefDialog
                0x00000000004a2e2e                apiGetSystemMetrics
                0x00000000004a2e4c                api_set_current_keyboard_responder
                0x00000000004a2e6b                api_get_current_keyboard_responder
                0x00000000004a2e83                api_set_current_mouse_responder
                0x00000000004a2ea2                api_get_current_mouse_responder
                0x00000000004a2eba                api_set_window_with_text_input
                0x00000000004a2efc                api_get_window_with_text_input
                0x00000000004a2f14                gramadocore_init_execve
                0x00000000004a2f1e                apiDialog
                0x00000000004a2faa                api_getchar
                0x00000000004a2fc2                apiDisplayBMP
                0x00000000004a33c9                apiSendMessage
                0x00000000004a33ff                apiDrawText
                0x00000000004a343e                apiGetWSScreenWindow
                0x00000000004a3456                apiGetWSMainWindow
                0x00000000004a346e                apiCreateTimer
                0x00000000004a348b                apiGetSysTimeInfo
                0x00000000004a34a9                apiShowWindow
 .text          0x00000000004a34c5        0x0 ctype.o
 .text          0x00000000004a34c5     0x2529 stdio.o
                0x00000000004a350d                stdio_atoi
                0x00000000004a35d4                stdio_fntos
                0x00000000004a36fe                fclose
                0x00000000004a371f                fopen
                0x00000000004a3740                scroll
                0x00000000004a380e                puts
                0x00000000004a3829                fread
                0x00000000004a3833                fwrite
                0x00000000004a3bfe                printf3
                0x00000000004a3c1b                printf_atoi
                0x00000000004a3d0b                printf_i2hex
                0x00000000004a3d6d                printf2
                0x00000000004a3ef2                sprintf
                0x00000000004a3f47                putchar
                0x00000000004a3f68                outbyte
                0x00000000004a4126                _outbyte
                0x00000000004a4155                input
                0x00000000004a42a9                getchar
                0x00000000004a42c4                stdioInitialize
                0x00000000004a44b1                fflush
                0x00000000004a44d2                fprintf
                0x00000000004a44f3                fputs
                0x00000000004a4514                gets
                0x00000000004a459f                ungetc
                0x00000000004a45a9                ftell
                0x00000000004a45b3                fileno
                0x00000000004a45bd                fgetc
                0x00000000004a45de                feof
                0x00000000004a45ff                ferror
                0x00000000004a4620                fseek
                0x00000000004a4641                fputc
                0x00000000004a4662                stdioSetCursor
                0x00000000004a467d                stdioGetCursorX
                0x00000000004a4698                stdioGetCursorY
                0x00000000004a46b3                scanf
                0x00000000004a484d                sscanf
                0x00000000004a4a08                kvprintf
                0x00000000004a5872                printf
                0x00000000004a589c                vfprintf
                0x00000000004a5911                vprintf
                0x00000000004a5930                stdout_printf
                0x00000000004a595c                stderr_printf
                0x00000000004a5988                perror
                0x00000000004a599f                rewind
                0x00000000004a59da                snprintf
 .text          0x00000000004a59ee     0x105e stdlib.o
                0x00000000004a5a0b                rtGetHeapStart
                0x00000000004a5a15                rtGetHeapEnd
                0x00000000004a5a1f                rtGetHeapPointer
                0x00000000004a5a29                rtGetAvailableHeap
                0x00000000004a5a33                heapSetLibcHeap
                0x00000000004a5ae6                heapAllocateMemory
                0x00000000004a5d18                FreeHeap
                0x00000000004a5d22                heapInit
                0x00000000004a5eb5                stdlibInitMM
                0x00000000004a5f18                libcInitRT
                0x00000000004a5f3a                rand
                0x00000000004a5f57                srand
                0x00000000004a5f65                xmalloc
                0x00000000004a5f97                stdlib_die
                0x00000000004a5fcd                malloc
                0x00000000004a6009                realloc
                0x00000000004a6046                free
                0x00000000004a604c                calloc
                0x00000000004a6092                zmalloc
                0x00000000004a60ce                system
                0x00000000004a6492                stdlib_strncmp
                0x00000000004a64f5                __findenv
                0x00000000004a65c0                getenv
                0x00000000004a65ed                atoi
                0x00000000004a66b4                reverse
                0x00000000004a671b                itoa
                0x00000000004a67c9                abs
                0x00000000004a67d9                strtod
                0x00000000004a6a0b                strtof
                0x00000000004a6a27                strtold
                0x00000000004a6a3a                atof
 .text          0x00000000004a6a4c      0x772 string.o
                0x00000000004a6a4c                memcmp
                0x00000000004a6ab1                strdup
                0x00000000004a6b03                strndup
                0x00000000004a6b64                strrchr
                0x00000000004a6b9f                strtoimax
                0x00000000004a6ba9                strtoumax
                0x00000000004a6bb3                strcasecmp
                0x00000000004a6c1b                strncpy
                0x00000000004a6c71                strcmp
                0x00000000004a6cd6                strncmp
                0x00000000004a6d39                memset
                0x00000000004a6d80                memoryZeroMemory
                0x00000000004a6da7                memcpy
                0x00000000004a6de4                strcpy
                0x00000000004a6e18                strcat
                0x00000000004a6e47                bcopy
                0x00000000004a6e73                bzero
                0x00000000004a6e93                strlen
                0x00000000004a6ec1                strnlen
                0x00000000004a6ef5                strcspn
                0x00000000004a6f94                strspn
                0x00000000004a7033                strtok_r
                0x00000000004a711a                strtok
                0x00000000004a7132                strchr
                0x00000000004a715e                strstr
 .text          0x00000000004a71be       0x89 conio.o
                0x00000000004a71be                putch
                0x00000000004a71e2                cputs
                0x00000000004a7217                getch
                0x00000000004a722f                getche
 .text          0x00000000004a7247      0x1b3 unistd.o
                0x00000000004a7247                execve
                0x00000000004a7251                exit
                0x00000000004a7271                fork
                0x00000000004a7289                setuid
                0x00000000004a7293                getpid
                0x00000000004a72ab                getppid
                0x00000000004a72c3                getgid
                0x00000000004a72cd                dup
                0x00000000004a72d7                dup2
                0x00000000004a72e1                dup3
                0x00000000004a72eb                fcntl
                0x00000000004a72f5                nice
                0x00000000004a72ff                pause
                0x00000000004a7309                mkdir
                0x00000000004a731d                rmdir
                0x00000000004a7327                link
                0x00000000004a7331                mlock
                0x00000000004a733b                munlock
                0x00000000004a7345                mlockall
                0x00000000004a734f                munlockall
                0x00000000004a7359                sysconf
                0x00000000004a7363                fsync
                0x00000000004a736d                fdatasync
                0x00000000004a7377                fpathconf
                0x00000000004a7381                pathconf
                0x00000000004a738b                ioctl
                0x00000000004a7395                open
                0x00000000004a73bb                close
                0x00000000004a73d9                pipe
 .text          0x00000000004a73fa       0x28 stubs.o
                0x00000000004a73fa                gramado_system_call
                0x00000000004a8000                . = ALIGN (0x1000)
 *fill*         0x00000000004a7422      0xbde 

.iplt           0x00000000004a8000        0x0
 .iplt          0x00000000004a8000        0x0 crt0.o

.rodata         0x00000000004a8000      0xe2e
 .rodata        0x00000000004a8000       0x38 crt0.o
 .rodata        0x00000000004a8038      0x149 main.o
 *fill*         0x00000000004a8181        0x3 
 .rodata        0x00000000004a8184      0x3ab api.o
 *fill*         0x00000000004a852f       0x11 
 .rodata        0x00000000004a8540      0x100 ctype.o
                0x00000000004a8540                _ctype
 .rodata        0x00000000004a8640      0x2d4 stdio.o
                0x00000000004a8780                hex2ascii_data
 *fill*         0x00000000004a8914        0x4 
 .rodata        0x00000000004a8918      0x510 stdlib.o
 .rodata        0x00000000004a8e28        0x6 conio.o

.eh_frame       0x00000000004a8e30     0x20c4
 .eh_frame      0x00000000004a8e30       0x34 crt0.o
 .eh_frame      0x00000000004a8e64      0x1f4 main.o
                                        0x20c (size before relaxing)
 .eh_frame      0x00000000004a9058      0xc2c api.o
                                        0xc44 (size before relaxing)
 .eh_frame      0x00000000004a9c84      0x6f0 stdio.o
                                        0x708 (size before relaxing)
 .eh_frame      0x00000000004aa374      0x400 stdlib.o
                                        0x418 (size before relaxing)
 .eh_frame      0x00000000004aa774      0x320 string.o
                                        0x338 (size before relaxing)
 .eh_frame      0x00000000004aaa94       0xa0 conio.o
                                         0xb8 (size before relaxing)
 .eh_frame      0x00000000004aab34      0x39c unistd.o
                                        0x3b4 (size before relaxing)
 .eh_frame      0x00000000004aaed0       0x24 stubs.o
                                         0x3c (size before relaxing)

.rel.dyn        0x00000000004aaef4        0x0
 .rel.got       0x00000000004aaef4        0x0 crt0.o
 .rel.iplt      0x00000000004aaef4        0x0 crt0.o
 .rel.text      0x00000000004aaef4        0x0 crt0.o

.data           0x00000000004aaf00     0x1100
                0x00000000004aaf00                data = .
                0x00000000004aaf00                _data = .
                0x00000000004aaf00                __data = .
 *(.data)
 .data          0x00000000004aaf00      0x448 crt0.o
 *fill*         0x00000000004ab348       0x18 
 .data          0x00000000004ab360      0x444 main.o
                0x00000000004ab7a0                listening
 *fill*         0x00000000004ab7a4       0x1c 
 .data          0x00000000004ab7c0      0x440 api.o
 .data          0x00000000004abc00        0x0 ctype.o
 .data          0x00000000004abc00        0x0 stdio.o
 .data          0x00000000004abc00        0x8 stdlib.o
                0x00000000004abc00                _infinity
 .data          0x00000000004abc08        0x0 string.o
 .data          0x00000000004abc08        0x0 conio.o
 .data          0x00000000004abc08        0x0 unistd.o
 .data          0x00000000004abc08        0x0 stubs.o
                0x00000000004ac000                . = ALIGN (0x1000)
 *fill*         0x00000000004abc08      0x3f8 

.got            0x00000000004ac000        0x0
 .got           0x00000000004ac000        0x0 crt0.o

.got.plt        0x00000000004ac000        0x0
 .got.plt       0x00000000004ac000        0x0 crt0.o

.igot.plt       0x00000000004ac000        0x0
 .igot.plt      0x00000000004ac000        0x0 crt0.o

.bss            0x00000000004ac000    0x127b4
                0x00000000004ac000                bss = .
                0x00000000004ac000                _bss = .
                0x00000000004ac000                __bss = .
 *(.bss)
 .bss           0x00000000004ac000        0x0 crt0.o
 .bss           0x00000000004ac000        0x0 main.o
 .bss           0x00000000004ac000     0x8004 api.o
 .bss           0x00000000004b4004        0x0 ctype.o
 .bss           0x00000000004b4004        0x1 stdio.o
 *fill*         0x00000000004b4005       0x1b 
 .bss           0x00000000004b4020     0x8020 stdlib.o
                0x00000000004b4020                environ
 .bss           0x00000000004bc040        0x4 string.o
 .bss           0x00000000004bc044        0x0 conio.o
 .bss           0x00000000004bc044        0x0 unistd.o
 .bss           0x00000000004bc044        0x0 stubs.o
                0x00000000004bd000                . = ALIGN (0x1000)
 *fill*         0x00000000004bc044      0xfbc 
 COMMON         0x00000000004bd000      0xd18 crt0.o
                0x00000000004bd000                g_cursor_x
                0x00000000004bd004                CurrentWindow
                0x00000000004bd008                stdout
                0x00000000004bd00c                g_char_attrib
                0x00000000004bd010                g_rows
                0x00000000004bd020                Streams
                0x00000000004bd0a0                g_using_gui
                0x00000000004bd0a4                ApplicationInfo
                0x00000000004bd0c0                prompt_out
                0x00000000004bd4c0                BufferInfo
                0x00000000004bd4c4                g_columns
                0x00000000004bd4c8                prompt_pos
                0x00000000004bd4cc                stdin
                0x00000000004bd4d0                prompt_status
                0x00000000004bd4e0                prompt_err
                0x00000000004bd8e0                CursorInfo
                0x00000000004bd8e4                stderr
                0x00000000004bd900                prompt
                0x00000000004bdd00                rect
                0x00000000004bdd04                g_cursor_y
                0x00000000004bdd08                ClientAreaInfo
                0x00000000004bdd0c                current_semaphore
                0x00000000004bdd10                prompt_max
                0x00000000004bdd14                taskmanTest1
 *fill*         0x00000000004bdd18        0x8 
 COMMON         0x00000000004bdd20      0x20c main.o
                0x00000000004bdd20                taskmanagerBuffer
                0x00000000004bdf20                taskmanagerStatus
                0x00000000004bdf24                taskmanagerBufferPos
                0x00000000004bdf28                taskmanagerError
 *fill*         0x00000000004bdf2c       0x14 
 COMMON         0x00000000004bdf40      0x438 api.o
                0x00000000004bdf40                heapList
                0x00000000004be340                libcHeap
                0x00000000004be344                dialogbox_button2
                0x00000000004be348                messagebox_button1
                0x00000000004be34c                heap_start
                0x00000000004be350                g_available_heap
                0x00000000004be354                g_heap_pointer
                0x00000000004be358                HEAP_SIZE
                0x00000000004be35c                dialogbox_button1
                0x00000000004be360                heap_end
                0x00000000004be364                HEAP_END
                0x00000000004be368                messagebox_button2
                0x00000000004be36c                Heap
                0x00000000004be370                heapCount
                0x00000000004be374                HEAP_START
 *fill*         0x00000000004be378        0x8 
 COMMON         0x00000000004be380      0x434 stdlib.o
                0x00000000004be380                mm_prev_pointer
                0x00000000004be3a0                mmblockList
                0x00000000004be7a0                last_valid
                0x00000000004be7a4                randseed
                0x00000000004be7a8                mmblockCount
                0x00000000004be7ac                last_size
                0x00000000004be7b0                current_mmblock
                0x00000000004be7b4                end = .
                0x00000000004be7b4                _end = .
                0x00000000004be7b4                __end = .
LOAD crt0.o
LOAD main.o
LOAD api.o
LOAD ctype.o
LOAD stdio.o
LOAD stdlib.o
LOAD string.o
LOAD conio.o
LOAD unistd.o
LOAD stubs.o
OUTPUT(TASKMAN.BIN elf32-i386)

.comment        0x0000000000000000       0x2b
 .comment       0x0000000000000000       0x11 crt0.o
                                         0x12 (size before relaxing)
 .comment       0x0000000000000011       0x12 main.o
 .comment       0x0000000000000011       0x1a api.o
                                         0x1b (size before relaxing)
 .comment       0x000000000000002b       0x1b ctype.o
 .comment       0x000000000000002b       0x1b stdio.o
 .comment       0x000000000000002b       0x1b stdlib.o
 .comment       0x000000000000002b       0x1b string.o
 .comment       0x000000000000002b       0x1b conio.o
 .comment       0x000000000000002b       0x1b unistd.o
 .comment       0x000000000000002b       0x1b stubs.o

.note.GNU-stack
                0x0000000000000000        0x0
 .note.GNU-stack
                0x0000000000000000        0x0 crt0.o
 .note.GNU-stack
                0x0000000000000000        0x0 main.o
 .note.GNU-stack
                0x0000000000000000        0x0 api.o
 .note.GNU-stack
                0x0000000000000000        0x0 ctype.o
 .note.GNU-stack
                0x0000000000000000        0x0 stdio.o
 .note.GNU-stack
                0x0000000000000000        0x0 stdlib.o
 .note.GNU-stack
                0x0000000000000000        0x0 string.o
 .note.GNU-stack
                0x0000000000000000        0x0 conio.o
 .note.GNU-stack
                0x0000000000000000        0x0 unistd.o
 .note.GNU-stack
                0x0000000000000000        0x0 stubs.o
