# LinuxAutosplitter
## A configurable autosplitter program for GNU/Linux systems that uses the LiveSplit Server component

**Important: Your kernel must support the process_vm_readv syscall! Most kernels will by default but if you're using a custom kernel or you compiled your kernel yourself then you may want to check.**

This is an autosplitter program that is designed to function similar to ASL as in it reads the value of virtual memory addresses in programs. It then uses the virtual memory values to send commands to LiveSplit via the LiveSplit Server (currently non-functional).

# TODO
1. Make the code work with LiveSplit Server
2. Make the code look decent
3. Format the code into a library that can be referenced.
