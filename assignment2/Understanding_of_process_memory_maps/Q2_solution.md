# (a) exlpain regions in cat /proc/self/maps
```
5618aafbf000-5618aafc1000 r--p 00000000 08:01 1566                       /usr/bin/cat
5618aafc1000-5618aafc6000 r-xp 00002000 08:01 1566                       /usr/bin/cat
5618aafc6000-5618aafc9000 r--p 00007000 08:01 1566                       /usr/bin/cat
5618aafc9000-5618aafca000 r--p 00009000 08:01 1566                       /usr/bin/cat
5618aafca000-5618aafcb000 rw-p 0000a000 08:01 1566                       /usr/bin/cat
```
These first 5 regions are the file /usr/bin/cat mapped into the memory in order to read form it, run it and maybe alter somep part of it
```
5618ac48b000-5618ac4ac000 rw-p 00000000 00:00 0                          [heap]
```
This is the heap region - dymanically allocated memory will be allocated in these region
```
7f838662a000-7f838664c000 rw-p 00000000 00:00 0
```
This region is a writing area that was just mapped with mmap without any file related to it or system of access like the heap has
```
7f838664c000-7f838667e000 r--p 00000000 08:01 5978                       /usr/lib/locale/C.UTF-8/LC_CTYPE
7f838667e000-7f838667f000 r--p 00000000 08:01 5983                       /usr/lib/locale/C.UTF-8/LC_NUMERIC
7f838667f000-7f8386680000 r--p 00000000 08:01 5986                       /usr/lib/locale/C.UTF-8/LC_TIME
7f8386680000-7f83867f3000 r--p 00000000 08:01 5977                       /usr/lib/locale/C.UTF-8/LC_COLLATE
7f83867f3000-7f83867f4000 r--p 00000000 08:01 5981                       /usr/lib/locale/C.UTF-8/LC_MONETARY
7f83867f4000-7f83867f5000 r--p 00000000 08:01 5975                       /usr/lib/locale/C.UTF-8/LC_MESSAGES/SYS_LC_MESSAGES
7f83867f5000-7f83867f6000 r--p 00000000 08:01 5984                       /usr/lib/locale/C.UTF-8/LC_PAPER
7f83867f6000-7f83867f7000 r--p 00000000 08:01 5982                       /usr/lib/locale/C.UTF-8/LC_NAME
7f83867f7000-7f83867f8000 r--p 00000000 08:01 5976                       /usr/lib/locale/C.UTF-8/LC_ADDRESS
7f83867f8000-7f83867f9000 r--p 00000000 08:01 5985                       /usr/lib/locale/C.UTF-8/LC_TELEPHONE
7f83867f9000-7f83867fa000 r--p 00000000 08:01 5980                       /usr/lib/locale/C.UTF-8/LC_MEASUREMENT
```
These are all language configurations in order to print the output of "cat" in a readable way the program have to understand the encoding
```
7f83867fa000-7f8386ae0000 r--p 00000000 08:01 5972                       /usr/lib/locale/locale-archive
```
Archive file for optimization on files in order to handle them more quickly by the C library
```
7f8386ae0000-7f8386b05000 r--p 00000000 08:01 3447                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f8386b05000-7f8386c7d000 r-xp 00025000 08:01 3447                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f8386c7d000-7f8386cc7000 r--p 0019d000 08:01 3447                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f8386cc7000-7f8386cc8000 ---p 001e7000 08:01 3447                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f8386cc8000-7f8386ccb000 r--p 001e7000 08:01 3447                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
7f8386ccb000-7f8386cce000 rw-p 001ea000 08:01 3447                       /usr/lib/x86_64-linux-gnu/libc-2.31.so
```
The libc of the cat program (if cat needs something from libc then it must be mapped in memory like so)
```
7f8386cce000-7f8386cd4000 rw-p 00000000 00:00 0
```
Empty mapped write region could probably be used by libc
```
7f8386cd4000-7f8386cdb000 r--s 00000000 08:01 3756                       /usr/lib/x86_64-linux-gnu/gconv/gconv-modules.cache
7f8386cdb000-7f8386cdc000 r--p 00000000 08:01 3443                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f8386cdc000-7f8386cff000 r-xp 00001000 08:01 3443                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f8386cff000-7f8386d07000 r--p 00024000 08:01 3443                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f8386d07000-7f8386d08000 r--p 00000000 08:01 5979                       /usr/lib/locale/C.UTF-8/LC_IDENTIFICATION
7f8386d08000-7f8386d09000 r--p 0002c000 08:01 3443                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
7f8386d09000-7f8386d0a000 rw-p 0002d000 08:01 3443                       /usr/lib/x86_64-linux-gnu/ld-2.31.so
```
Holds the ld (with encoding configuration) in order to dynammically load/ unload .so
```
7f8386d0a000-7f8386d0b000 rw-p 00000000 00:00 0
7fffbf4b7000-7fffbf4d8000 rw-p 00000000 00:00 0                          [stack]
```
the process's stack region
```
7fffbf52c000-7fffbf52f000 r--p 00000000 00:00 0                          [vvar]
```
vdso variables region (vvar) contains the variables used by the vdso
```
7fffbf52f000-7fffbf530000 r-xp 00000000 00:00 0                          [vdso]
```
small shared library that the kernel automatically maps into the address space of all user-space applications. This is done to implement functionalities that otherwise would have to be triggered from a syscall - which can degrade performance
```
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```
The vsyscall or virtual system call region is a region mapped to accelerate execution of certain system calls by mappping these functions directly to the user's address space - this is a static memory (same address space every time) in the process which makes it an obsolete concept which is replaced by vdso (which does the same but is dynamically mapped as an .so)

# (b) Explain new regions in cat /proc/$(pidof systemd-journald)/maps
```
7f6976aa7000-7f69772a7000 rw-s 00000000 08:01 258451                     /var/log/journal/daa5f54b52c44b9886fdaa70812de1c5/user-1000.journal
...
7f697f08e000-7f697f88e000 rw-s 00408000 08:01 258445                     /var/log/journal/daa5f54b52c44b9886fdaa70812de1c5/system@75c34dcd1da64b79a0e8b2b9ef74d16d-0000000000000d16-0005d126bea1c87e.journa
```
This region is responsible for the system log
```
/usr/lib/x86_64-linux-gnu/libcap-ng.so.0.0.0
/usr/lib/x86_64-linux-gnu/libm-2.31.so
/usr/lib/x86_64-linux-gnu/libudev.so.1.6.17
/usr/lib/x86_64-linux-gnu/libaudit.so.1.0.0

/usr/lib/x86_64-linux-gnu/libunistring.so.2.1.0
/usr/lib/x86_64-linux-gnu/libgpg-error.so.0.28.0
/usr/lib/x86_64-linux-gnu/libjson-c.so.4.0.0
/usr/lib/x86_64-linux-gnu/libargon2.so.1
/usr/lib/x86_64-linux-gnu/libcrypto.so.1.1

/usr/lib/x86_64-linux-gnu/libdevmapper.so.1.02.1

/usr/lib/x86_64-linux-gnu/libuuid.so.1.3.0
/usr/lib/x86_64-linux-gnu/libdl-2.31.so
/usr/lib/x86_64-linux-gnu/libpcre2-8.so.0.9.0
/usr/lib/x86_64-linux-gnu/libpthread-2.31.so

/usr/lib/x86_64-linux-gnu/liblzma.so.5.2.4
/usr/lib/x86_64-linux-gnu/libseccomp.so.2.5.1
/usr/lib/x86_64-linux-gnu/librt-2.31.so
/usr/lib/x86_64-linux-gnu/libpam.so.0.84.2
/usr/lib/x86_64-linux-gnu/libmount.so.1.1.0
/usr/lib/x86_64-linux-gnu/liblz4.so.1.9.2

/usr/lib/x86_64-linux-gnu/libkmod.so.2.3.5
/usr/lib/x86_64-linux-gnu/libip4tc.so.2.0.0
/usr/lib/x86_64-linux-gnu/libidn2.so.0.3.6
/usr/lib/x86_64-linux-gnu/libgcrypt.so.20.2.5
/usr/lib/x86_64-linux-gnu/libcryptsetup.so.12.5.0
/usr/lib/x86_64-linux-gnu/libcrypt.so.1.1.0

/usr/lib/x86_64-linux-gnu/libcap.so.2.32
/usr/lib/x86_64-linux-gnu/libblkid.so.1.1.0
/usr/lib/x86_64-linux-gnu/libacl.so.1.1.2253
/usr/lib/x86_64-linux-gnu/libselinux.so.1

```
shared libraries to help logging (for example libcrypt to help with encryption or libjson-c to help with json format) and libraries to support the stream log options (for example libaudit - to help with auditing actions)
```
7f6983d10000-7f6983d11000 rw-s 00000000 00:17 17141                      /run/systemd/journal/kernel-seqnum
```
# (c) cat /proc/self/maps multiple times
The mappings change - that is because after cat is finished it will be destroyed and exited, which means that when we run cat again it will start and mapped the needed pages again - call mmap again and getting slightly different free-to-map addresses.
This can be easily seen with strace on the command.

# (d) vdso
vdso (virtual dynamic shared object) is a shared library aim to improve system performance by trasfering some highly used syscalls into the user space in order to save the overhead of actual syscalls. There are syscalls that are called frequent enough that these alone will degrade the system's performance, those will be mapped in the vdso section inside the *user's address space* in order to save the syscall.
