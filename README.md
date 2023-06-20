# WARNING: This repo is deprecated! Go [here](https://github.com/wins1ey/LinuxAutoSplitter)!


# LinuxAutosplitter
## A configurable autosplitter program for GNU/Linux systems that uses the LiveSplit Server component

**Important: Your kernel must support the process_vm_readv syscall! Most kernels will by default but if you're using a custom kernel or you compiled your kernel yourself then you may want to check.**

This is an autosplitter program that is designed to function similar to Auto Splitter Language scripts as in it reads the value of virtual memory addresses in programs. It then uses the virtual memory values to send commands to LiveSplit via the LiveSplit Server.

**If you would like to see an example of this being used for a real game then look [here](https://github.com/Loomeh/JSRLinuxAutosplitter)**

## How to set the library up
First, find out if your game runs in x86 or x64. To do this, open a terminal and type `file <executable>`.

If it returns `ELF 32-bit LSB executable`, it's 32 bit.
If it returns `ELF 64-bit LSB executable`, it's 64 bit.

Then copy the .hpp files from the appropriate x86 or x64 folder into your project files.

## Clearing some things up
When I posted this on Reddit a lot of people were confused on what this is and how to use this. Hopefully this section will answer those questions.

This repo isn't meant to be used by runners. This is meant to be used by autosplitter developers as a base. If you are a runner, **DON'T USE THIS!** Look for another repo that has been made for your specific game.

## FAQ
### How are you supposed to run this?
The program runs through a terminal and interacts with LiveSplit (running in Wine)

### Does this require elevated permissions (sudo)?
No. If it does then there's an error.

### Does this work with LiveSplit One?
No.


## TODO
1. Make the code look decent
2. ~~Format the code into a library that can be referenced.~~
