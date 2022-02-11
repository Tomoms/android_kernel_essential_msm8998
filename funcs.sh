#!/bin/bash

mymake() {
	make CC=clang CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_ARM32=arm-linux-gnueabi- LD=ld.lld ${1}
}

setenv () {
	export ARCH=arm64
	export SUBARCH=arm64
	export PATH=/mnt/data/android/proton-clang/bin:$PATH
}

checkenv () {
	if [[ $ARCH != "arm64" ]] || [[ $SUBARCH != "arm64" ]]; then
		echo "Environment variables are unset!"
		return 1
	fi
	echo "All good!"
}

fullclean () {
	setenv
	if ! checkenv; then
		echo "Aborting!"
		return 1
	fi
	echo "Performing a full clean..."
	mymake mrproper
}

clean () {
	setenv
	if ! checkenv; then
		echo "Aborting!"
		return 1
	fi
	echo "Cleaning..."
	mymake clean
}

mkcfg () {
	setenv
	if ! checkenv; then
		echo "Aborting!"
		return 1
	fi
	if [ -f ".config" ]; then
		echo ".config exists, running make oldconfig"
		mymake oldconfig
	else
		echo ".config not found"
		mymake lineageos_mata_defconfig
	fi
}

editcfg () {
	setenv
	if ! checkenv; then
		echo "Aborting!"
		return 1
	fi
	if [ -f ".config" ]; then
		echo ".config exists"
		mymake nconfig
	else
		echo ".config not found, run mkcfg first!"
		return 1
	fi
}

savecfg () {
	setenv
	if ! checkenv; then
		echo "Aborting!"
		return 1
	fi
	mymake savedefconfig
	mv defconfig arch/arm64/configs/lineageos_mata_defconfig
}

build () {
	setenv
	if ! checkenv; then
		echo "Aborting!"
		return 1
	fi
	if [ -z "$1" ]; then
		echo "No number of jobs has been passed"
		return 1
	fi
	echo "Running make..."
	mymake -j${1}
}

mkzip () {
	cp arch/arm64/boot/Image.gz-dtb ../AnyKernel3/
	(cd ../AnyKernel3 && zip -r ../kernel_Tom_`date +%Y%m%d`.zip *)
	printf "Sideload zip? [Y/n]"
	read answer
	if [[ $answer != "n" ]]; then
		adb sideload ../kernel_Tom_`date +%Y%m%d`.zip
	fi
}
