## How to port Auto Splitter Language scripts to LinuxAutoSplitter
### Addresses
For some games the addresses on Linux are different from Windows so they need to be converted. Luckily, this is pretty straightforward to do.

All you need to do is add 4 to the first bit of the address. For example, if you have the address `0x2B85A6`. It would be `0x6B85A6`. If you wanted to make things easier then you could just create a variable with the value of 0x4 and add it on.

### Current and Old
The current and old flags work pretty similarly but it's not too hard to get confused. I'll give an example of a command that's been ported:

`return old.bossGraffiti != 7 && current.bossGraffiti == 7 || !old.rankingScreen && current.rankingScreen;` turns into:

```
if(bossGraffiti == 7 && prevBossGraffiti != 7) 
{
    split_res = send(sock, split, strlen(split), 0);
}
else if(rankingScreen == 1 && prevRankingScreen != 1)
{
    split_res = send(sock, split, strlen(split), 0);
}
```
