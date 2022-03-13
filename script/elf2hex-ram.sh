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
#   $ head build/fsbl.elf.ram.hex
#   00000313f12022f3
#   f14022f318628463
#   02029e630032f293
#   1782b28300004297
#   0062a02300100313
#   0000100f0ff0000f
#   00004297795020ef
#   0020031315c2b283
#   0ff0000f0062a023
#   0380006f0000100f
#   $ wc -l build/fsbl.elf.ram.hex-*
#     32768 build/fsbl.elf.ram.hex-0
#     32768 build/fsbl.elf.ram.hex-1
#     32768 build/fsbl.elf.ram.hex-2
#     32768 build/fsbl.elf.ram.hex-3
#    131072 total

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

echo "== Making RAM Hex for $elf =="
${CROSS_TOOL}objcopy -O binary ${elf} ${elf}.bin
let align_size=8
let origin_size=`wc --bytes ${elf}.bin | cut -d " " -f 1`
let final_size=(${origin_size}+${align_size}-1)/${align_size}*${align_size}
let append_size=${final_size}-${origin_size}
echo "align_size = ${align_size}"
echo "origin_size = ${origin_size}"
echo "final_size = ${final_size}"
echo "append_size = ${append_size}"
cp -f ${elf}.bin ${elf}.bin.aligned
dd if=/dev/zero of=${elf}.bin.aligned bs=1 count=${append_size} seek=${origin_size} conv=notrunc
xxd -c 8 -g 1 ${elf}.bin.aligned | awk '{print $9,$8,$7,$6,$5,$4,$3,$2}' | sed s/\ //g > ${elf}.ram.hex
ls -l ${elf}.ram.hex
wc -l ${elf}.ram.hex
head -n 4 ${elf}.ram.hex
echo "..."
tail -n 4 ${elf}.ram.hex

ram_hex=${elf}.ram.hex
let ram_part_cnt=4
let ram_hex_line_full=131072
let ram_hex_line_valid=`wc -l ${ram_hex} | cut -d " " -f 1`

echo "== Split $ram_hex into $ram_part_cnt partitions total $ram_hex_line_full lines =="
if [ ${ram_hex_line_valid} -gt ${ram_hex_line_full} ]
then
	echo "Error: Hex too long. line-count ${ram_hex_line_valid} > ${ram_hex_line_full}"
	exit 3
fi

let byte_per_line=17
let ram_hex_per_part=${ram_hex_line_full}/${ram_part_cnt}
let byte_per_part=${ram_hex_per_part}*${byte_per_line}
if [ ! -f all-zero-ram.hex ]
then
	echo "Create all-zero Hex firstly..."
	for ((i = 0; i < ${ram_hex_per_part}; i++))
	do
		echo "0000000000000000" >> all-zero-ram.hex
	done
fi
for ((i = 0; i < ${ram_part_cnt}; i++))
do
	cp -f all-zero-ram.hex ${ram_hex}-${i}
done

for ((i = 0 ; i < ${ram_part_cnt}; i++))
do
	let line_cnt_top=(${i}+1)*${ram_hex_per_part}
	let dd_skip=${i}*${byte_per_part}
	if [ ${ram_hex_line_valid} -gt ${line_cnt_top} ]
	then
		let dd_count=${byte_per_part}
		echo "Fill Hex partition ${i}: skip = ${dd_skip} count = ${dd_count}"
		dd if=${ram_hex} of=${ram_hex}-${i} bs=1 skip=${dd_skip} count=${dd_count}
		continue
	else
		let line_all_zero=${line_cnt_top}-${ram_hex_line_valid}
		let byte_all_zero=${line_all_zero}*${byte_per_line}
		let dd_count=${byte_per_part}-${byte_all_zero}
		echo "Fill Hex partition ${i}: skip = ${dd_skip} count = ${dd_count} notrunc"
		dd if=${ram_hex} of=${ram_hex}-${i} bs=1 skip=${dd_skip} count=${dd_count} conv=notrunc
		break
	fi
done

ls -l ${ram_hex}*

exit 0
