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

WrapperFolder="${DataType}_${Op}_n${N}_w${W}_m${M}"
DstDir="$PWD/src/syn/$WrapperFolder"
WrapperFile="$DstDir/wrapper_module.v"

# Check if the output directory exists, Create it if does not exist
if [ ! -d "$DstDir" ]; then
    mkdir -p $DstDir
fi

cmd="$PWD/script/wrapper/gen_wrapper_${DataType}_${Op}.sh $WrapperFile $N $W $M"

$cmd

echo "The wrapper file was generated."
