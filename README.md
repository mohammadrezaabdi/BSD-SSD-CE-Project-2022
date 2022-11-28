SSDDRM.efi
===
A UEFI shell utility that:
1. finds all SSD storage devices
2. generate hash from their descriptions (like serial number, manufacturer, ...)
3. disconnect any SSD storage from OS if its hash not found in local database.


## Prerequisites:
1. Python 3.7+,
2. git 2.25.0+
3. [UDK/EDK2 code tree](https://github.com/tianocore/edk2) in following tags: edk2-stable20{1911, 2002, 2005, 2008}

## Usage of SSDDRM
0. Boot into the EFI Shell.<br>
1. To get the NVME info including the serial number<br>
    `SSDDRM info`
<!-- 
2. To wipe out the GPT. The serial number can be the first 6 characters.<br>
    `SSDDRM zap Nvme_SSD_serial_number`<br>
    **WARNING: The specified NVME SSD's GUID Partition Tables would be wiped out !** -->


## Build using iPug:
0. Install dependencies

	`sudo apt-get update` <br>
	`sudo apt-get install build-essential` <br>
	`sudo apt-get install git -y` <br>
	`sudo apt-get install autopoint -y` <br>
	`sudo apt-get install uuid-dev -y` <br>
	`sudo apt-get install cmake -y` <br>
	`sudo apt-get install libfreetype6-dev -y` <br>
	`sudo apt-get install libfontconfig1-dev -y` <br>
	`sudo apt-get install xclip -y` <br>
	`sudo apt-get install unifont -y` <br>
	`sudo apt-get install autotools-dev -y` <br>
	`sudo apt-get install autoconf -y` <br>
	`sudo apt-get install automake -y` <br>
	`sudo apt-get install iasl -y` <br>
	`sudo apt-get install qemu-system -y` <br>
    `sudo apt-get install ovmf -y` <br>
	`sudo apt-get install nasm -y` <br>
	`nasm --version` <br>
	> \# NASM version 2.13.02 <br>
        // By default 'nasm' version 2.13 will be installed  in ubuntu 18.04 but <br>
		// we need 'nasm' version 2.15. So, we need to install 'nasm' version 2.15 manually <br>
		// [Ref] https://www.linuxfromscratch.org/blfs/view/cvs/general/nasm.html <br>
        `wget https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/nasm-2.15.05.tar.xz` <br>
        `tar -xf nasm-2.15.05.tar.xz --strip-components=1` <br>
        `./configure --prefix=/usr && make` <br>
        `sudo make install` <br>
        `nasm --version` <br>
            # NASM version 2.15.05 compiled on DATE

    `gcc --version` <br>
	> \# gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0 <br>
	    // By default the GCC version is 7.5 in Ubuntu-18.04 <br>
        `sudo apt-get install g++-5 -y` <br>
        `sudo apt-get install gcc-5 -y` <br>
        `gcc --version` <br>
            # gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0 <br>
            // The gcc is still in version 7.5, so we need to change the default gcc <br>
            // [Ref] https://askubuntu.com/questions/26498/how-to-choose-the-default-gcc-and-g-version <br>
        `sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 10` <br>
        `sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 20` <br>
        `sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 10` <br>
        `sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 20` <br>
        `sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30` <br>
        `sudo update-alternatives --set cc /usr/bin/gcc` <br>
        `sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30` <br>
        `sudo update-alternatives --set c++ /usr/bin/g++` <br>
        `gcc --version` <br>
            # gcc (Ubuntu 5.5.0-12ubuntu1) 5.5.0 20171010

    `sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.6 10` <br>
	`python --version` <br>
	> \# Python 3.6.9

1. `pip3 install ipug --user --upgrade`
2. `git-clone https://github.com/mohammadrezaabdi/SSDDRM.efi`
3. Change directory to folder **SSDDRM.efi**.
4. (Optional) Edit `CODETREE` in `project.py` to specify where to place the downloaded source files of the UDK git repo or any other additional repos.
5. To setup the EDK2 code base and build the BaseTools executables, run `ipug setup`.
6. To build the code, run `ipug build` (iPug will then handle all the rest of remaining tedious works with the UDK code tree setup and the build process.)
7. Browse to folder **Build/SSDDRM** for the build results.
8. Browse to folder **Build/Conf** for CONF_PATH setting files.
9. Run `ipug {clean, cleanall}` to clean (all) the intermediate files.

## Run inside UEFI Shell

After you have built the project successfully, you should link the `efi` file in `./Build/SSDDRM/RELEASE_GCC5/X64/SSDDRM.efi` to BOOTX64.EFI file which will be placed in QEMU virtual VirtIO-BLK boot hard drive partition. for example, consider `start.sh` bash file like below:

```bash
#!/bin/sh
mkdir -p /tmp/qemu-hda/EFI/BOOT/
cp ./Build/SSDDRM/RELEASE_GCC5/X64/SSDDRM.efi /tmp/qemu-hda/EFI/BOOT/
exec qemu-system-x86_64
	-net none \        
	-bios /usr/share/ovmf/OVMF.fd \
    -drive file=fat:rw:/tmp/qemu-hda
```

As you have entered UEFI shell, you can run project commands as we have mentioned before.

if you want to add a virtual hard drive, you can create the image using ‍`dd` (like `dd if=/dev/zero of=/path/to/nvme.img bs=1M count=4096`) and start QEMU like this:

```bash
exec qemu-system-x86_64
	-net none \        
	-bios /usr/share/ovmf/OVMF.fd \
    -drive file=fat:rw:/tmp/qemu-hda \
    -drive file=/path/to/nvme.img,if=none,id=D22 \
    -device nvme,drive=D22,serial=1234
```
