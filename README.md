# Trinity

*Trinity* is the third public jailbreak for the *PS Vita™*, which supports the latest firmwares 3.69 and 3.70. The exploit chain consists of three stages: the MIPS Kernel Exploit, the PSP Emulator Escape and the ARM Kernel Exploit.

## Write-up

A technical explanation of the *Trinity* exploit chain is available [here](<https://theofficialflow.github.io/2019/06/18/trinity.html>).

## Requirements

- Your device must be on firmware 3.69 or 3.70. If you're on a lower firmware, please use [h-encore](<https://github.com/TheOfficialFloW/h-encore>) instead.
- If your device is a phat OLED model, you need a Memory Card in order to install. There's no need for a Memory Card on Slim/PS TV models, since they already provide an Internal Storage.
- Your device must be able to access the PlayStation Store.
- If your device region is mainland China (model PCH-2009) then this exploit may not be available for you, since this model is region locked to only accept mainland China PSN accounts to login and can only access mainland China PS Store, which doesn't contain any PSP games at all.

## Preparation

If you have already done the preparation, you can skip this part and go to the `Installation` section.

1. If you're on firmware 3.69, you have two options:

   - Either update to firmware 3.70 (go to Settings → System Update).
   - Or set DNS to `212.47.229.76` (go to Settings → Network → Wi-Fi Settings → Your access point → Advanced Settings and set DNS Settings to Manual and Primary DNS to `212.47.229.76`).

2. Register a PSN account if you don't have one yet (note that only 3 devices can be activated using the same account).

3. Download and install **any PSP/minis game (PS Vita or PS one Classics do not work)**. There are **demos** in most regions (if you know a title that is not listed here, please let me know):
   - EU/UK: Ape Quest
   - NA/SG: LocoRoco Midnight Carnival
   - JP: YS seven

   Unfortunately, if you can't find a demo in your region, you must either buy any PSP/minis game, or register a new PSN account in one of the regions listed above.

4. Please make sure that your demo **is a PSP/minis** game. To verify, please launch the game and hold the PS button for a while. Then a quick menu should come up with the `Settings` option, where you can set bilinear filter, etc. If this option is not there, you've likely downloaded the wrong game. For help, please consider watching some youtube tutorials and see how a PSP game should look like.


## Installation

1. Download and install [qcma](<https://codestation.github.io/qcma/>) and [psvimgtools](<https://github.com/yifanlu/psvimgtools/releases>).

2. Start qcma and within the qcma settings set the option `Use this version for updates` to `FW 0.00 (Always up-to-date)` to spoof the System Software check.

3. Launch Content Manager on your PS Vita and connect it to your computer, where you then need to select `PS Vita System -> PC`, and after that you select `Applications`. Finally select `PSP™/Other` and click on the game that you want to turn into the *Trinity* exploit. If you see an error message about System Software, you should simply reboot your device to solve it (if this doesn't solve, then put your device into airplane mode and reboot). If this does still not work, then alternatively set DNS to `212.47.229.76` to block updates.

4. Transfer the game over to your computer by clicking on `Copy` on your PS Vita. After copying, you go to the folder `/Documents/PS Vita/PGAME/xxxxxxxxxxxxxxxx/YYYYZZZZZ` on your computer, where `xxxxxxxxxxxxxxxx` is some string corresponding to your account ID and `YYYYZZZZZ` is the title id of the game that you've just copied over. You can look at the image at `YYYYZZZZZ/sce_sys/icon0.png` to verify that it is indeed your chosen game. Furthermore, the `YYYYZZZZZ` folder should contain these folders: `game`, `license` and `sce_sys`.

5. Before you attempt to modify the backup, you should make a copy of it. Just copy `YYYYZZZZZ` somewhere else, such that if you fail to follow the instructions, you can copy it back and retry.

6. Insert the `xxxxxxxxxxxxxxxx` string [here](<http://cma.henkaku.xyz/>). If the AID is valid, it will yield a key that you can now use to decrypt/re-encrypt your game.

7. Decrypt the game backup by executing the following command in your command line/terminal (make sure you're in the right working directory. On Windows you can open the terminal in the current working directory by typing in `cmd` in the path bar of the file explorer. Also, if you haven't installed psvimgtools yet, then just place them in the `YYYYZZZZZ` folder):

   ```
   psvimg-extract -K YOUR_KEY game/game.psvimg game_dec
   ```

   If done correctly, you should see an output like this:

   ```
   creating file ux0:pspemu/temp/game/PSP/GAME/YYYYZZZZZ/EBOOT.PBP (x bytes)...
   creating file ux0:pspemu/temp/game/PSP/GAME/YYYYZZZZZ/__sce_ebootpbp (x bytes)... 
   all done.
   ```

8. Download [Trinity](<https://github.com/TheOfficialFloW/Trinity/releases/download/v1.0/PBOOT.PBP>) and copy the `PBOOT.PBP` file to `game_dec/ux0_pspemu_temp_game_PSP_GAME_YYYYZZZZZ/PBOOT.PBP` (the files `EBOOT.PBP`, `__sce_ebootpbp` and `VITA_PATH.txt` should exist in this folder). If `PBOOT.PBP` does already exist there, just overwrite it.

9. Now re-encrypt the backup similar to above by typing this in your command line/terminal:

   ```
   psvimg-create -n game -K YOUR_KEY game_dec game
   ```

   If done correctly, you should see an output like this:

   ```
   adding files for ux0:pspemu/temp/game/PSP/GAME/YYYYZZZZZ
   packing file ux0:pspemu/temp/game/PSP/GAME/YYYYZZZZZ/EBOOT.PBP (x bytes)...
   packing file ux0:pspemu/temp/game/PSP/GAME/YYYYZZZZZ/PBOOT.PBP (x bytes)...
   packing file ux0:pspemu/temp/game/PSP/GAME/YYYYZZZZZ/__sce_ebootpbp (x bytes)...
   created game/game.psvimg (size: x, content size: x)
   created game/game.psvmd
   ```

10. Remove the `game_dec` folder (and PSVimg tools if copied here) and select `Refresh database` in qcma settings.

11. Now you need to copy back the modified backup to your PS Vita: Launch Content Manager on your PS Vita and connect it to your computer (if it's already open, just go back to the first menu), where you then need to select `PC -> PS Vita System`, and after that you select `Applications`. Finally select `PSP™/Other` and click on the modified game. Perform the copy operation and exit Content Manager.

12. In the livearea, the game should now have a different icon and should now be called *Trinity* (eventually you have to rebuild the database in recovery mode to make the bubble change its look). If not, please re-read the instructions more carefully and begin from fresh.

13. Turn on Wi-Fi, then reboot your device and straightly launch *Trinity*. Do not do anything else, otherwise the exploit will be less reliable. It is very important that you do not have any running downloads in background.

14. Enjoy the exploitation process and wait until it launches the *Construct*. If the exploit fails, simply rerun *Trinity*.

15. Within the *Construct*, select `Download VitaShell`, then `Install HENkaku` and finally `Exit`.

16. Congratulations, your device is now able to run homebrews. It is highly suggested that you downgrade your device to either firmware 3.60 or 3.65/3.67/3.68 using [modoru](<https://github.com/TheOfficialFloW/modoru>). On 3.60, you can use [HENkaku](<http://henkaku.xyz/>) and on 3.65/3.67/3.68 you can use [h-encore](<https://github.com/TheOfficialFloW/h-encore>). If you don't downgrade your device now, you may lose the ability to launch *Trinity* later and therefore not be able to hack your device anymore.

## FAQ

### Exploit

- "I get the error `[TURN ON WI-FI TO USE THIS EXPLOIT].`" - Just hold the PS button and turn on Wi-Fi in the quickmenu.
- "I get the error `[EXPLOIT FAILED: 0x800200CB].`" - This can sometimes happen. Just rerun the exploit.
- "My device freezes/panics." - Be sure you do not have any downloads running in background. Also make sure that you do not launch anything else before *Trinity*. A fresh reboot is always recommended.
- "Trinity crashes in the second run." - It is not recommended that you launch *Trinity* multiple times. Always reboot your device before launching *Trinity*.
- "Trinity bubble disappeared after downgrading." - Sony prevents you from playing games that have been downloaded from a higher firmware. Therefore, after downgrading, it is hidden. You can redownload the game on your lower firmware and reapply the *Trinity* patch to make it compatible with your current firmware and higher.

### HENkaku Settings

- "I don't see all folders in VitaShell." - Launch the Settings application and select `HENkaku Settings`, then select `Enable unsafe homebrews`. This will grant you full permission in VitaShell.
- "I can't find the HENkaku Settings." - Launch the exploit and reset taiHEN config.txt and reinstall HENkaku.

### enso/permanent hack

- "Can I install enso on 3.69 or 3.70?" - Not on these firmwares, but you can downgrade to firmware 3.60/3.65 using [modoru](https://github.com/TheOfficialFloW/modoru) and then install enso.

## Donation

If you like my work and want to support future projects, you can make a donation:

- via bitcoin `361jRJtjppd2iyaAhBGjf9GUCWnunxtZ49`
- via [paypal](https://www.paypal.me/flowsupport/20)
- via [patreon](https://www.patreon.com/TheOfficialFloW)

Thank you!

## Credits

- Thanks to qwikrazor87 for the PSP kernel exploit.
- Thanks to Freakler for the *Trinity* icon.
- Thanks to molecule for their initial work on the PS Vita.
- Thanks to Davee and Proxima for http://cma.henkaku.xyz/.
- Thanks to yifanlu for psvimgtools.
- Thanks to codestation for qcma.
- Thanks to the PS Vita hacking community.
- Thanks to Sony for this awesome device.
