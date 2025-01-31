#!/bin/sh

# Define variables
DataType="cufp"
Op="sum"
N=4
W=32
M=23

# Process input arguments
while getopts 'o:d:n:w:m:' flag; do
    case "${flag}" in
        d) DataType=${OPTARG};;
        o) Op=${OPTARG};;
        n) N=${OPTARG};;
        w) W=${OPTARG};;
        m) M=${OPTARG};;
        *) exit 1;;
    esac
done

TbFolder="${DataType}_${Op}_n${N}_w${W}_m${M}"
DstDir="$PWD/src/sim/$TbFolder"
TbFile="$DstDir/full_tb.sv"

# Check if the output directory exists, Create it if does not exist
if [ ! -d "$DstDir" ]; then
    mkdir -p $DstDir
fi

cmd="$PWD/script/tb/gen_tb_${DataType}_${Op}.sh $TbFile $N $W $M"

$cmd

echo "The tb file was generated."
