#!/bin/bash

# Example
# - input:
#   $ xxd build/fsbl.elf.bin | head 
#   00000000: f322 20f1 1303 0000 6384 6218 f322 40f1  ." .....c.b.."@.
#   00000010: 93f2 3200 639e 0202 9742 0000 83b2 8217  ..2.c....B......
#   00000020: 1303 1000 23a0 6200 0f00 f00f 0f10 0000  ....#.b.........
#   00000030: ef20 5079 9742 0000 83b2 c215 1303 2000  . Py.B........ .
#   00000040: 23a0 6200 0f00 f00f 0f10 0000 6f00 8003  #.b.........o...
#   00000050: 9742 0000 83b2 0214 9303 2000 1300 0000  .B........ .....
#   00000060: 1300 0000 1300 0000 1300 0000 1300 0000  ................
#   00000070: 1300 0000 1300 0000 1300 0000 03a3 0200  ................
#   00000080: e34e 73fc 1300 0000 f322 40f1 93f2 3200  .Ns......"@...2.
#   00000090: 639e 0202 9742 0000 83b2 c20f 1303 3000  c....B........0.
# - output:
#   $ head build/fsbl.elf.rom.hex 
#   f12022f3
#   00000313
#   18628463
#   f14022f3
#   0032f293
#   02029e63
#   00004297
#   1782b283
#   00100313
#   0062a023

CROSS_TOOL=riscv64-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Error: No ELF prvided"
	echo "Usage:"
	echo "    $0 elf-file"
	exit 1
fi

elf=$1
if [ ! -f $elf ]
then
	echo "Error: ELF $elf not found"
	exit 2
fi

echo "== Making ROM Hex for $elf =="
${CROSS_TOOL}objcopy -O binary ${elf} ${elf}.bin
let align_size=4
let origin_size=`wc --bytes ${elf}.bin | cut -d " " -f 1`
let final_size=(${origin_size}+${align_size}-1)/${align_size}*${align_size}
let append_size=${final_size}-${origin_size}
echo "align_size = ${align_size}"
echo "origin_size = ${origin_size}"
echo "final_size = ${final_size}"
echo "append_size = ${append_size}"
cp -f ${elf}.bin ${elf}.bin.aligned
dd if=/dev/zero of=${elf}.bin.aligned bs=1 count=${append_size} seek=${origin_size} conv=notrunc
xxd -c 4 -g 1 ${elf}.bin.aligned | awk '{print $5,$4,$3,$2}' | sed s/\ //g > ${elf}.rom.hex
ls -l ${elf}.rom.hex
wc -l ${elf}.rom.hex
head -n 4 ${elf}.rom.hex
echo "..."
tail -n 4 ${elf}.rom.hex

exit 0
