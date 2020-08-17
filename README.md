                  Ralink/MediaTek U-Boot for MIPS SoC
        RT3052/RT3352/RT3883/RT5350/MT7620/MT7621/MT7628/MT7688
                     Based on MediaTek SDK 5.0.1.0


#### Preparing Toolchain

For MT7621 U-Boot:
- extract 'toolchain/mips-2012.03.tar.bz2' to /opt

For RT3XXX/MT7620/MT7628/MT7688 U-Boot:
- extract 'toolchain/buildroot-gcc342.tar.bz2' to /opt

Both toolchains require x86 (32-bit) Linux environment. If you are on x64 (64-bit)
environment you need to do the following:
```
dpkg --add-architecture i386

apt install libc6:i386 libncurses5:i386 libstdc++6:i386
```

#### Build Instructions

- Copy appropriate '.config' file (e.g. profiles/ASUS/RT-N11P/.config)
  to 'uboot-5.x.x.x' dir.
- Goto 'uboot-5.x.x.x' dir.
- Run 'make menuconfig', choose [Exit] and confirm [Save]. This is important step!
- Run 'make'.
- Use image file uboot.bin (ROM mode) for NOR and SPI-flash boards.
- Use image file uboot.img (RAM mode) for NAND-flash boards.

To clean U-Boot tree:
- Run 'make clean'.
- Run 'make unconfig'.

#### Build for new Boards

- Goto 'uboot-5.x.x.x' dir.
- Run 'make menuconfig'.
- Select SoC, Ram, Flash type for your board. See some .config from profiles, you will get the general idea.
- Choose [Exit] and confirm [Save] then run 'make'.

NOTE:
1. Before building for a new board, know the GPIO number for reset, leds and configure them
   properly during 'make menuconfig'. Extra careful with the reset button (GPIO_BTN_RESET).
2. It's better to configure UART baud rate same as your stock firmware 115200 or 57600.
3. All profiles has disabled option "Enable all Ethernet PHY" to prevent LAN-WAN
   spoofing (EPHY will be enabled later in FW logic). To force enable EPHY (e.g. for
   use OpenWRT/PandoraBox), select option "Enable all Ethernet PHY".
4. See if your board's spi or nand flash chip is listed on here 'uboot-5.x.x.x/drivers/spi_flash.c' (281 line)
   or 'uboot-5.x.x.x/drivers/nand/nand_device_list.h'. Don't risk it if your flash chip is not on the list.

#### Flash Instructions

- Upload appropriate U-Boot image file to router's /tmp dir (e.g. via WinSCP or just wget it by creating a local http server).
- Check U-Boot image checksum and compare with uboot.md5:
```
md5sum /tmp/uboot.bin
```
- Flash checked U-Boot via SSH or Telnet console (flash duration ~3 sec):
```
mtd write /tmp/uboot.bin Bootloader
```
or in some boards
```
mtd_write write /tmp/uboot.bin Bootloader
```
Double check the boot partition name 'Bootloader' by 'cat /proc/mtd', usually it's on /dev/mtd0 or sometimes mtd1.
- Reboot router.

                             WARNING

- Do not remove power supply during flash U-Boot!!!
- Device may be bricked due to your incorrect actions!!!

#### Main Features

1. Press and hold the RESET button on Power-On for 2~3 sec, this will switch to Recovery mode. Set your Ethernet
   ipv4 to 192.168.1.2, subnet mask 255.255.255.0 and gateway 192.168.1.1.
2. Go to 192.168.1.1 from any browser and upload or upgrade your firmware. You can also upgrade your factory and u-boot
   partition from 192.168.1.1/factory.html and 192.168.1.1/uboot.html respectively.

<img src="https://i.imgur.com/qadrqTK.png" width="300" height="177"><img src="https://i.imgur.com/xLrROI2.png" width="300" height="177"><img src="https://i.imgur.com/eoB2H5P.png" width="300" height="177">

3. Also you can use TFTP client or ASUS Firmware Restoration (device IP-address is 192.168.1.1). Some devices with usb
   port can also support Recovery from USB storage.
4. If your device has a dedicated WPS button you can press and hold the WPS button on Power-On to perform erase 'Config'
   partition (U-Boot Env & NVRAM) and self-reboot. Of cource you have to set the WPS gpio properly during .config state.

NOTE:
- U-Boot will perform switch to Recovery mode on flash content integrity fail.
- Alert LED(s) is blinking in Recovery mode and on erasing/flashing.
- To Recovery from USB storage, place FW image with a filename 'root_uImage' to first
  FAT16/FAT32 partition, plug-in USB2 pen and switch to Recovery mode (see 3).
- Recovery from USB storage is not supported for ASUS RT-N65U (external USB chip).
- This U-Boot on default load firmware from 0x50000. So your partition layout will be
  something like [this](https://gist.github.com/shibajee/6812bab70dde591506e58ea215041544).

                             GOOD LUCK!
