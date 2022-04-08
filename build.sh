#!/bin/bash

plat_name="null"
plat_name="apple"
plat_name="banana"
plat_dir="./plat-$plat_name"
build_dir="./build"
src_dir="./src"
bc_buildin="bootconf0.c"

if [ $# -gt 0 ]
then
	plat_name=$1
fi
if [ -d $plat_dir ]
then
	echo "Build for platform $plat_name"
else
	echo "Error: Platform directory not found $plat_dir"
	exit 2
fi

echo "Prepare"
echo "    platform   : " $plat_name
echo "    bc-buildin : " $bc_buildin
if [ -d $build_dir ]
then
	rm -rf $build_dir
fi
mkdir $build_dir
for f in `ls $src_dir`
do
	if [ -L $src_dir/$f ]
	then
		rm -f $src_dir/$f
	fi
done

for f in `ls $plat_dir`
do
	ln -s ../$plat_dir/$f $src_dir/$f
done
ln -s ./$bc_buildin $src_dir/bc_buildin.c

echo "Build bootconf"
make -f Makefile.bc clean
make -f Makefile.bc

echo "Build ZSBL"
make -f Makefile.zsbl-mc clean
make -f Makefile.zsbl-mc
make -f Makefile.zsbl-pc clean
make -f Makefile.zsbl-pc

echo "Build FSBL"
make -f Makefile.fsbl clean
make -f Makefile.fsbl

echo "Build binary and hex files"
./script/elf2hex-rom.sh $build_dir/zsbl_mc.elf
./script/elf2hex-rom.sh $build_dir/zsbl_pc.elf
./script/elf2hex-ram.sh $build_dir/fsbl.elf

exit 0
