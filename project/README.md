# Course Project: Understand Linux OS via Modern Tool (Systemtap)

### Abstract—Operating system (OS) is a piece of software that helps users to operate the hardware. To understand the operating system, it is very important to know it’s internal logic and performance characteristics. The purpose of this project is to understand and observe the internal behaviors and performance characteristics of Linux operating system. In exploring the relations among some of these quantities, I have used a dynamic tracing tool namely Systemtap.

### I. Introduction
As I mentioned earlier, operating system is a system software that manages computer hardware, software resources, and provides common services for computer program [1]. It has several components to make sure the various parts of a computer system works together. All userlevel software has to undergo the operating system to get the utilization of the hardware as well as the other system resources. Among the core components, in this course. we have been tought about the kernel, process management, task scheduler, memory management, filesystem management, etc.

Kernel is the heart of the operating system. It is responsible for the core operating system tasks i.e. protection of resources, isolation of applications and users etc. Kernel has the full access to all the machine hardware. Kernel is considered as the only trusted component and all other softwares are obliged to perform previliged instructions through it. Among the other components, scheduler is responsible in scheduling the proesses to the processor. Memory management is responsible to make sure any program does not conflicts with the memory that is own by some other program. File sytem is another abstruction of operating system to provide persistent, named data.

To find software bugs due to incorrect system setup, system administrator perform instrumentation in finding those bugs, which includes, performance statistics collection and their analysis, debug or system audit. One of the common approaches to instrumentation is “sampling” by installing probes at specified places of software to collect state of the system: values of some variables, stacks of threads, etc. Sampling is very helpful when you do not know where issue happens.

For example, some function, say foo() that processes lists of elements, consumes 80% of the time, but doesn't say why. It could be because the list is too long or list is an inappropriate data structure for foo(). With tracing we can install a probe to that function, gather information on lists (i.e. their length) and collect cumulative execution of function foo(), and then cross-reference them, searching for a pattern in lists whose processing costs too much CPU time.

To gain deep understanding about the various parts of the Linux Operating System, I have conducted the following experiments using SystemTap:

1. Understand OS kernel: Addresses and contents of user frame and kernel frame of a user process.
2. Understand system-calls:
    * Within specific time duration, observe all the system-calls issued by a process, and, in the whole system.
    * Within specific time duration, trace time spent in all system calls in a per process way
3. Understand Processes
    * Trace parent-child tree of a process
    * Detailed current process information (including executable files, command line arguments, environment variables, uid, gid, cpuid, etc.)
    * Trace the time duration a process takes before the process actually starts to execute.
4. Understand Virtual File System
    * Trace major and minor page faults
5. Understand Virtual Memory System
    * Trace file open/close/read/write/mmap operations done in the system.
    * Trace the above mentioned operations in a per-process way.

### II. Systemtap
I have spent quality time for the background study which includes learning to write systemtap script to trace, study, and monitor the activities of the Linux operating system [5], [6], [7], [8].

SystemTap is both scripting language and tool which can be used for profiling program on Linux kernel-based operating systems in runtime. SystemTap scripts are generally focuses on tracking events. It is designed to balance between several key requirements including easy of use, performance, tranparency, flexibility and sefty to use in production system etc. The structures and components of systemtap is presented in Figure 1.

One of the systemtap’s wellknown keyword is prob point, which indicate the a particular point in kernel/userspace code, or a specific event. The systemtap input consists of a script which makes the assiciation of hendeler routines to a specific prob point. When a prob point hits, the associated handler routine will be executed.

When we place a systemtap script, it will be passed through a translator where it will be parsed first, and then been elaborated. Elaboration is a script processing phase where the necessary symbolic references to the kernel or user program is been resolved. After that, the script goes through the translation phase and been translated to a quantity of C code. From the figure, we can see the output file of the translator as prob.c

To run the probs, the systemtap driver program loads the kernel module by using insmod. Insmod is a simple program to insert a module into the Linux Kernel. The module will initialize and register itself and let the probs so their tasks.

### References
    [1] Wiki Page: https://en.wikipedia.org/wiki/Operating_system
    [2] Architecture of systemtap: a Linux trace/probe tool: https://sourceware.org/systemtap/archpaper.pdf
    [3] Dynamic Tracing with DTrace & SystemTap: https://myaut.github.io/dtrace-stap-book/
    [4] Linux Source Code, mm.h file: https://github.com/torvalds/linux/blob/master/include/linux/mm.h
    [5] SystemTap Tapset Reference Manual: https://sourceware.org/systemtap/tapsets/
    [6] Brendan's blog on "Using SystemTap": dtrace.org/blogs/brendan/2011/10/15/using-systemtap/
    [7] Systemtap Examples: https://sourceware.org/systemtap/examples/index.html#process/strace.stp
    [8] SystemTap Beginners Guide: https://lrita.github.io/images/posts/systemtap/SystemTap_Beginners_Guide.pdf
    [9] Red Hat Enterprise SystemTap Language Reference: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/5/pdf/systemtap_language_reference/Red_Hat_Enterprise_Linux-5-SystemTap_Language_Reference-en-US.pdf
    [10] Lightweight Provenance Service for High-Performance Computing: https://webpages.uncc.edu/ddai/papers/dong-pact-lps-2017.pdf
    [11] sourceware's systemtap example scripts: https://sourceware.org/systemtap/examples/keyword-index.html
