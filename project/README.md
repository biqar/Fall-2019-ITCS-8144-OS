[raqib@localhost systemcall]$ sudo stap syscall_count.stp 

Collecting system-call data for the whole system
Top 20 <processes, syscall> pair will be displayed for the interval of 10 seconds
Type Ctrl+c to exit the program


             Process Name              Syscall Name  #Syscalls
              gnome-shell                   recvmsg        606
              gnome-shell                      poll        235
                        X                   recvmsg        228
                        X                 setitimer        196
          gnome-terminal-                   recvmsg        160
                        X                    writev        146
                        X                epoll_wait        114
              gnome-shell                    writev        114
          gnome-terminal-                      poll         80
                   stapio                      read         70
                   stapio                     ppoll         49
                    gmain         inotify_add_watch         47
                   stapio                     fcntl         42
              gnome-shell                      stat         40
                    gdbus                      poll         32
              gnome-shell                     write         28
                    gdbus                     write         26
              gnome-shell                      read         21
                   stapio                  pselect6         20
                     pool                    keyctl         20
                            ***************<>***************
             Process Name              Syscall Name  #Syscalls
              gnome-shell                   recvmsg        630
                        X                   recvmsg        254
              gnome-shell                      poll        252
                        X                 setitimer        218
          gnome-terminal-                   recvmsg        190
                        X                    writev        163
                        X                epoll_wait        127
              gnome-shell                    writev        126
          gnome-terminal-                      poll         95
                   stapio                      read         71
                   stapio                     ppoll         50
                   stapio                     fcntl         40
                    gmain         inotify_add_watch         33
                   stapio                  pselect6         20
                     pool                    keyctl         20
          gnome-terminal-                    writev         19
              gnome-shell                      read         18
              gnome-shell                     write         18
                        X                     ioctl         18
             in:imjournal                      read         11
                            ***************<>***************
             Process Name              Syscall Name  #Syscalls
              gnome-shell                   recvmsg        140
                  firefox                      stat        121
                   stapio                      read         71
                        X                   recvmsg         58
              gnome-shell                      poll         56
                        X                 setitimer         50
                   stapio                     ppoll         50
                    gmain         inotify_add_watch         47
          gnome-terminal-                   recvmsg         43
                   stapio                     fcntl         40
                        X                    writev         37
                        X                epoll_wait         29
              gnome-shell                    writev         28
          gnome-terminal-                      poll         23
                   stapio                  pselect6         20
                     pool                    keyctl         20
                    gmain                      poll         14
             in:imjournal                      read         11
             in:imjournal                     ppoll         11
                    tuned                    select         10
                            ***************<>***************
^C
Total Result:
             Process Name              Syscall Name  #Syscalls
              gnome-shell                   recvmsg       1963
                        X                   recvmsg        772
              gnome-shell                      poll        770
                        X                 setitimer        660
          gnome-terminal-                   recvmsg        561
                        X                    writev        539
                        X                epoll_wait        397
              gnome-shell                    writev        381
          gnome-terminal-                      poll        287
                   stapio                      read        266
                   stapio                     ppoll        187
                    gmain         inotify_add_watch        159
                   stapio                     fcntl        152
                  firefox                      stat        121
              gnome-shell                     write         77
                   stapio                  pselect6         75
                     pool                    keyctl         70
              gnome-shell                      read         62
          gnome-terminal-                    writev         56
                        X                     ioctl         55
------------------------------------------------------------------------
[raqib@localhost systemcall]$ sudo stap syscall_count_arg.stp gnome-shell
[sudo] password for raqib: 
Collecting system-call data for the process: gnome-shell
Top 20 syscall by process [gnome-shell] will be displayed for the interval of 10 seconds
Type Ctrl+c to exit the program


             Process Name              Syscall Name  #Syscalls
              gnome-shell                   recvmsg        560
              gnome-shell                      poll        224
              gnome-shell                    writev        112
              gnome-shell                     write         16
              gnome-shell                      read         16
                            ***************<>***************
             Process Name              Syscall Name  #Syscalls
              gnome-shell                   recvmsg        693
              gnome-shell                      poll        274
              gnome-shell                    writev        137
              gnome-shell                     write         25
              gnome-shell                      read         21
                            ***************<>***************
^C
Total Syscall Result for Process [gnome-shell]:
             Process Name              Syscall Name  #Syscalls
              gnome-shell                   recvmsg       1526
              gnome-shell                      poll        600
              gnome-shell                    writev        299
              gnome-shell                     write         59
              gnome-shell                      read         47
[raqib@localhost systemcall]$ 

