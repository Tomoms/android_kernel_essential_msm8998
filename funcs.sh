#!/bin/bash

mymake() {
	mkdir /tmp/out_mata_kernel
	make LLVM=1 LLVM_IAS=1 CROSS_COMPILE=aarch64-suse-linux- CROSS_COMPILE_ARM32=arm-suse-linux-gnueabi- LD=ld.lld O=/tmp/out_mata_kernel ${1}
}

setenv () {
	export ARCH=arm64
	export SUBARCH=arm64
	export PATH=/mnt/ssd/20/prebuilts/clang/host/linux-x86/adrian-clang/bin/:$PATH
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
	if [ -f "/tmp/out_mata_kernel/.config" ]; then
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
	mv /tmp/out_mata_kernel/defconfig arch/arm64/configs/lineageos_mata_defconfig
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
	cp /tmp/out_mata_kernel/arch/arm64/boot/Image.gz-dtb /mnt/data/android/mata/AnyKernel3/
	(cd /mnt/data/android/mata/AnyKernel3 && zip -r ../kernels/kernel_Tom_caf_`date +%Y%m%d`.zip *)
	printf "Sideload zip? [Y/n]"
	read answer
	if [[ $answer != "n" ]]; then
		adb sideload /mnt/data/android/mata/kernels/kernel_Tom_caf_`date +%Y%m%d`.zip
	fi
}
