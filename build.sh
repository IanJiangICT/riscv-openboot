#!/bin/bash

plat_name="null"
plat_name="apple"
plat_name="banana"
plat_name="grape"
build_dir="./build"
src_dir="./src"
bc_buildin="bootconf0.c"

if [ $# -gt 0 ]
then
	plat_name=$1
fi

plat_dir="./plat-$plat_name"
if [ -d $plat_dir ]
then
	echo "Build for platform $plat_name under $plat_dir"
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
ret=$?
if [ ! $ret -eq 0 ]
then
	echo "Error: Failed to build bootconf. Exit with $ret"
	exit $ret
fi

echo "Build ZSBL"
make -f Makefile.zsbl-mc clean
make -f Makefile.zsbl-mc
ret=$?
if [ ! $ret -eq 0 ]
then
	echo "Error: Failed to build ZSBL for MC. Exit with $ret"
	exit $ret
fi
make -f Makefile.zsbl-pc clean
make -f Makefile.zsbl-pc
ret=$?
if [ ! $ret -eq 0 ]
then
	echo "Error: Failed to build ZSBL for PC. Exit with $ret"
	exit $ret
fi

echo "Build FSBL"
make -f Makefile.fsbl clean
make -f Makefile.fsbl
ret=$?
if [ ! $ret -eq 0 ]
then
	echo "Error: Failed to build FSBL. Exit with $ret"
	exit $ret
fi

rom_elfs="zsbl_mc.elf"
rom_elfs="$rom_elfs zsbl_pc.elf"
echo "Build ROM binary and hex files for $rom_elfs"
for f in $rom_elfs
do
	if [ -f $build_dir/$f ]
	then
		./script/elf2hex-rom.sh $build_dir/$f
	else
		echo "Error: ELF file not found $build_dir/$f"
		exit 2
	fi
done
ls -l $build_dir/*rom*hex*

ram_elfs="fsbl.elf"
ram_elfs="$ram_elfs bc1-vcs.elf"
ram_elfs="$ram_elfs bc1-zebu.elf"
echo "Build RAM binary and hex files for $ram_elfs"
for f in $ram_elfs
do
	if [ -f $build_dir/$f ]
	then
		./script/elf2hex-ram.sh $build_dir/$f
	else
		echo "Error: ELF file not found $build_dir/$f"
		exit 2
	fi
done
ls -l $build_dir/*ram*hex*

exit 0
