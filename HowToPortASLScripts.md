## How to port Auto Splitter Language scripts to LinuxAutoSplitter
### Addresses
For some games the addresses on Linux are different from Windows so they need to be converted. Luckily, this is pretty straightforward to do.

Open up a terminal and whilst the game is running, type `cat /proc/<PID>/maps | grep <executable name>`.

For Jet Set Radio, this returns:

`00400000-00401000 r-xp 00000000 08:11 4605307                            /mnt/4eabe652-9c9b-470b-8228-0b51558e76c5/SteamLibrary/steamapps/common/Jet Set Radio/jetsetradio.exe`

You want to copy the first address shown (which in this case is 0x0400000) and then add it on to the Windows address.

If the game is 64-bit, it may return an address like `140000000`. In this case, add the 0x before the whole address. So for this address, it'd be `0x140000000`.

(Also make sure that the 64-bit address is stored as a `uint64_t`!)

### Current and Old
The current and old flags work pretty similarly but it's not too hard to get confused. I'll give an example of a command that's been ported:

`return old.bossGraffiti != 7 && current.bossGraffiti == 7 || !old.rankingScreen && current.rankingScreen;` turns into:

```
if(bossGraffiti == 7 && prevBossGraffiti != 7) 
{
    lsClient.SendLSCommand("split\r\n")
}
else if(rankingScreen == 1 && prevRankingScreen != 1)
{
    lsClient.SendLSCommand("split\r\n")
}
```
