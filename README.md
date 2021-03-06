# Gramado: unix-like operating system

<p align=center>
  <a href="https://github.com/frednora/gramado">
    <img alt="Gramado" src="https://github.com/gramado/img/blob/master/gramado2.png">
  </a>
</p>

---

<p align=center>
  <a href="https://github.com/frednora/gramado/archive/master.zip">
  <img alt="Download " src="https://img.shields.io/badge/Download-latest-green.svg">
  </a>
  <a href="https://gramado.github.io/projects">
    <img alt="Donate" src="https://img.shields.io/badge/%24-Donate-orange.svg">
  </a>
</p>

## Links:

- [Gramado on Github](https://github.com/frednora/gramado)


## What is Gramado?

    Gramado (c) is a 32 bit multithreaded operating system.


## Kernel features.

* Paging.
* Threads.
* IDE PATA, e1000 NIC, Serial Port, ps2 mouse and keyboard.
* FAT16 file system.

## Userland features.

* Working on a Window Server. (gws)
* Working on a virtual terminal. 
* unix-like commands running in the virtual console.

## Libraries.

* unix-like libc for commands. (libc03)
* API for GUI apps. (libcore)
* Lib for the window server's clients. (libgws)


## Compiling Gramado on Linux.

    Linux 5.3.0-53-generic x86_64
    gcc (Ubuntu) 7.5.3
    GNU ld (GNU Binutils for Ubuntu) 2.30
    NASM version 2.13.02


## VHD

You can run the VHD on Oracle Virtual Box and qemu.

$ make qemu-test


## Testing

Please, give us some feedback.


## More information

Gramado is a Free and Open Source operating system created by Fred Nora.

The source code uses the BSD license.

# Quote:

    WWW - Isolation makes me hungry.
