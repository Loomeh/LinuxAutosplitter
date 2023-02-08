# LinuxAutosplitter
## A configurable autosplitter program for GNU/Linux systems that uses the LiveSplit Server component

**Important: Your kernel must support the process_vm_readv syscall! Most kernels will by default but if you're using a custom kernel or you compiled your kernel yourself then you may want to check.**

This is an autosplitter program that is designed to function similar to ASL as in it reads the value of virtual memory addresses in programs. It then uses the virtual memory values to send commands to LiveSplit via the LiveSplit Server.

**If you would like to see an example of this being used for a real game then look [here](https://github.com/Loomeh/JSRLinuxAutosplitter)**

## How to port ASL scripts to LinuxAutoSplitter
### Addresses
Addresses on Linux are different from Windows so they need to be converted. Luckily, this is pretty straightforward to do.

All you need to do is add 4 to the first bit of the address. For example, if you have the address `0x2B85A6`. It would be `0x6B85A6`.

### Current and Old
The current and old flags work pretty similarly but it's not too hard to get confused. I'll give an example of a command that's been ported:

`return old.bossGraffiti != 7 && current.bossGraffiti == 7 || !old.rankingScreen && current.rankingScreen;` turns into:

```
if(bossGraffiti == 7 && prevBossGraffiti != 7) 
{
    split_res = send(sock, split, strlen(split), 0);

    else if(rankingScreen == 1 && prevRankingScreen != 1)
    {
            split_res = send(sock, split, strlen(split), 0);
    }
}
```

## TODO
1. Make the code look decent
2. Format the code into a library that can be referenced.
