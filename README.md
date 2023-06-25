DISKDRM.efi
===
A UEFI shell utility that:
1. finds all disk storage devices
2. generate hash from their descriptions (like serial number, manufacturer, ...)
3. add disk hashes into the local database (local database is a simple binary file).


## Prerequisites:
1. Python 3.7+,
2. git 2.25.0+
3. [UDK/EDK2 code tree](https://github.com/tianocore/edk2) in following tags: edk2-stable20{1911, 2002, 2005, 2008}

## Usage of DISKDRM
0. Boot into the EFI Shell.<br>
1. To get the disk info including the serial number<br>
    `DISKDRM info`
1. To add a hash of disk to the local database (#DISK_NUMBER is the index of disk showed in `info` command)<br>
    `DISKDRM add #DISK_NUMBER`
1. To remove a hash of disk from local database<br>
    `DISKDRM remove #DISK_NUMBER`

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

	  	# NASM version 2.13.02
	        # By default 'nasm' version 2.13 will be installed  in ubuntu 18.04 but
			# we need 'nasm' version 2.15. So, we need to install 'nasm' version 2.15 manually
			# https://www.linuxfromscratch.org/blfs/view/cvs/general/nasm.html
	
	`wget https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/nasm-2.15.05.tar.xz` <br>
        `tar -xf nasm-2.15.05.tar.xz --strip-components=1` <br>
        `./configure --prefix=/usr && make` <br>
        `sudo make install` <br>
        `nasm --version` <br>

		# NASM version 2.15.05 compiled on DATE

	`gcc --version` <br>
 
		# gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
			# By default the GCC version is 7.5 in Ubuntu-18.04

	`sudo apt-get install g++-5 -y` <br>
        `sudo apt-get install gcc-5 -y` <br>
        `gcc --version` <br>

		# gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
		#	The gcc is still in version 7.5, so we need to change the default gcc
		#	https://askubuntu.com/questions/26498/how-to-choose-the-default-gcc-and-g-version

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
 
   		# Python 3.6.9

2. `pip3 install ipug --user --upgrade`
3. `git-clone https://github.com/mohammadrezaabdi/DISKDRM.efi`
4. Change directory to folder **DISKDRM.efi**.
5. (Optional) Edit `CODETREE` in `project.py` to specify where to place the downloaded source files of the UDK git repo or any other additional repos.
6. To setup the EDK2 code base and build the BaseTools executables, run `ipug setup`.
7. To build the code, run `ipug build` (iPug will then handle all the rest of remaining tedious works with the UDK code tree setup and the build process.)
8. Browse to folder **Build/DISKDRM** for the build results.
9. Browse to folder **Build/Conf** for CONF_PATH setting files.
10. Run `ipug {clean, cleanall}` to clean (all) the intermediate files.

## Run inside UEFI Shell

After you have built the project successfully, you should link the `efi` file in `./Build/DISKDRM/RELEASE_GCC5/X64/DISKDRM.efi` to BOOTX64.EFI file which will be placed in QEMU virtual VirtIO-BLK boot hard drive partition. for example, consider `start.sh` bash file like below:

```bash
#!/bin/sh
mkdir -p /tmp/qemu-hda/EFI/BOOT/
cp ./Build/DISKDRM/RELEASE_GCC5/X64/DISKDRM.efi /tmp/qemu-hda/EFI/BOOT/
exec qemu-system-x86_64
	-net none \        
	-bios ./Binaries/OVMF.fd \
    -drive file=fat:rw:/tmp/qemu-hda
```

As you have entered UEFI shell, first you have to load `Hash2DxeCrypto.efi` which placed in *Binaries* folder. then enter utility commands as described above.

if you want to add a virtual hard drive, you can create the image using ‍`dd` (like `dd if=/dev/zero of=/path/to/nvme.img bs=1M count=4096`) and start QEMU like this:

```bash
exec qemu-system-x86_64
	-net none \        
	-bios ./Binaries/OVMF.fd \
    -drive file=fat:rw:/tmp/qemu-hda \
    -drive file=/path/to/nvme.img,if=none,id=D22 \
    -device nvme,drive=D22,serial=1234
```
