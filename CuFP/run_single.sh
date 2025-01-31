#!/bin/sh

# Define tool paths
VivadoHLS="C:/Xilinx/Vivado/2019.1/bin/vivado_hls.bat"
Vivado="C:/Xilinx/Vivado/2019.1/bin/vivado.bat"

# Scripts
HLSScript="$PWD/script/hls_script.tcl"
VivadoScript="$PWD/script/vivado_script.tcl"

# Define general paths
LogDir="log"
LogHLS="vivado_hls.log"

SrcDir="src"
ConfigFile="config.h"

IPDir="CustomFloat/solution1/impl/ip"
IPSrcName="xilinx_com_hls_run_experiment_1_0.zip"
SynReport="CustomFloat/solution1/syn/report/run_experiment_csynth.rpt"
SynReportDst="hls_synthesis_report.txt"

# Define variables
DataType="cufp"
Op="sum"
NVec=4
W=32
M=23
Freq=200
ClkUncertainty=5
Hw=""
StepIP=false
StepSynth=false
StepImpl=false

# Process input arguments
while getopts 'ispd:o:n:v:w:m:f:u:h:' flag
do
    case "${flag}" in
        i) StepIP=true;;
		s) StepSynth=true;;
		p) StepImpl=true;;
		d) DataType=${OPTARG};;
        o) Op=${OPTARG};;
		n) NVec=${OPTARG};;
        w) W=${OPTARG};;
		m) M=${OPTARG};;
		f) Freq=${OPTARG};;
		u) ClkUncertainty=${OPTARG};;
		h) Hw=${OPTARG};;
		*) exit 1;;
    esac
done

IPDstName="hls_ip_${Hw}_${DataType}_${Op}_N${NVec}_${Freq}_w${W}_m${M}.zip"
OutDir="out/hls_${Hw}_${DataType}_${Op}_N${NVec}_${Freq}_${W-M-1}_${M}"
SynReportDst="hls_report_${Hw}_${DataType}_${Op}_N${NVec}_${Freq}_w${W}_m${M}.rpt"

if [ "$StepIP" = false ] && [ "$StepSynth" = false ] && [ "$StepImpl" = false ]; then
    echo "At least one step [-i:IP -s:Synthesis -p:Implementation] must be enabled!"
	exit 1
fi

# FPGA part (Hardware)
FpgaPart="xcvu9p-fsgd2104-3-e"
if [ "$Hw" == "virtex7" ]; then
    FpgaPart="xc7vx330tffg1157-3"
elif [ "$Hw" == "ultrascale" ]; then
    FpgaPart="xcvu9p-fsgd2104-3-e"
else
    echo "Error: Unknown hardware: ${Hw}"
fi

# Check if the log directory exists, Create it if does not exist
if [ ! -d "$LogDir" ]; then
    mkdir $LogDir
	touch $LogDir/$LogHLS
fi

# Check if the output directory exists, Create it if does not exist
if [ ! -d "$OutDir" ]; then
    mkdir -p $OutDir
fi

function echo_line { printf -- '-%.0s' $(seq 100); echo ""; }

# -------------------------------------------------------------
# Vivado HLS Synthesis and Exporting IP
# -------------------------------------------------------------
if [ "$StepIP" = true ]; then

    echo "#ifndef CONFIG_H" > $ConfigFile
    echo "#define CONFIG_H" >> $ConfigFile
    
    # Data type
    if [ "$DataType" == "cufp" ]; then
    	echo "#define CUSTOM_FLOAT" >> $ConfigFile
    elif [ "$DataType" == "normfp" ]; then
        echo "#define NORMAL_FLOAT" >> $ConfigFile
	elif [ "$DataType" == "normdb" ]; then
        echo "#define NORMAL_DOUBLE" >> $ConfigFile
    else
        echo "Error: Unknown data-type: ${DataType}"
    fi
    
    # Operation
    if [ "$Op" == "sum" ]; then
    	echo "#define SINGLE_OP_SUM" >> $ConfigFile
    elif [ "$Op" == "mul" ]; then
        echo "#define SINGLE_OP_MUL" >> $ConfigFile
    elif [ "$Op" == "matmul" ]; then
        echo "#define MATMUL" >> $ConfigFile
    elif [ "$Op" == "mvm" ]; then
        echo "#define MVM" >> $ConfigFile
    elif [ "$Op" == "dp" ]; then
        echo "#define DOT_PRODUCT" >> $ConfigFile		
    elif [ "$Op" == "vsum" ]; then
	    echo "#define VECTOR_OP_VSUM" >> $ConfigFile
    else
        echo "Error: Unknown operation: ${Op}"
    fi
	
	if [ "$Op" == "vsum" ] || [ "$Op" == "dp" ] || [ "$Op" == "mvm" ] || [ "$Op" == "matmul" ]; then
	    echo "#if defined(VECTOR_OP_VSUM) or defined(DOT_PRODUCT) or defined(MVM) or defined(MATMUL)" >> $ConfigFile
        echo "#define NN $NVec" >> $ConfigFile
        echo "#endif" >> $ConfigFile
	fi

    # Data width
    echo "#if defined(CUSTOM_FLOAT)
const int W1=$W;
const int M1=$M;
const int W2=$W;
const int M2=$M;
const int WR=$W;
const int MR=$M;
#endif" >> $ConfigFile
	
	# Standard floating point data type definition
	echo "#if defined(NORMAL_DOUBLE)
typedef double f_type;
#else
typedef float f_type;
#endif" >> $ConfigFile
    
    echo "#endif // CONFIG_H" >> $ConfigFile
    
    # Print the generated config.h
    echo -e "\n\nconfig.h:"
    echo_line
    cat $ConfigFile
    echo_line
    
    # Copy the config file to the source directory
    mv $ConfigFile $SrcDir
    
    # Update the clock configs
    sed -i "/set clkFreq/c\set clkFreq ${Freq}" $HLSScript
    sed -i "/set clkUncertainty/c\set clkUncertainty \"${ClkUncertainty}\%\"" $HLSScript
    sed -i "/set FpgaPart/c\set FpgaPart {${FpgaPart}}" $HLSScript
    
    # Run Vivado HLS synthesis and export RTL
    echo_line >> $LogDir/$LogHLS
    echo "Vivado HLS: Run for Data-Type:${DataType} Operation:${Op} Clk Freq.:${Freq} W:${W} M:${M}" >> $LogDir/$LogHLS
    echo_line >> $LogDir/$LogHLS
    $VivadoHLS $HLSScript >> $LogDir/$LogHLS
    echo_line >> $LogDir/$LogHLS
    echo -e "\n\n" >> $LogDir/$LogHLS
    
    # Move the generated files to the output directory
    if [ -e $IPDir/$IPSrcName ]; then
        mv $IPDir/$IPSrcName $OutDir/$IPDstName
    	echo "Exported IP: $PWD/$OutDir/$IPDstName"
    fi
    
    if [ -e $SynReport ]; then
        mv $SynReport $OutDir/$SynReportDst
    	echo "Synthesis Report: $PWD/$OutDir/$SynReportDst"
    fi
	
	if [ -d "CustomFloat" ]; then
        rm -rf "CustomFloat"
    fi
    
    echo "Vivado HLS Log: $PWD/$LogDir/$LogHLS"
    echo_line

fi

# -------------------------------------------------------------
# Vivado Synthesis and Implementation
# -------------------------------------------------------------
IPUnzipFolder="hls_ip_${Hw}_${DataType}_${Op}_N${NVec}_${Freq}_w${W}_m${M}"

if [ ! -d "$OutDir/$IPUnzipFolder" ]; then
    unzip -q -d $OutDir/$IPUnzipFolder $OutDir/$IPDstName
fi

cmd_gen_wrapper="$PWD/script/generate_wrapper.sh -d ${DataType} -o ${Op} -n ${NVec} -w ${W} -m ${M}"
$cmd_gen_wrapper

cmd_gen_tb="$PWD/script/generate_tb.sh -d ${DataType} -o ${Op} -n ${NVec} -w ${W} -m ${M}"
$cmd_gen_tb

ProjectName="vivado"
ProjectDir=$PWD/$OutDir/$ProjectName
OutputDir=$PWD/$OutDir
IPDir=$PWD/$OutDir/$IPUnzipFolder
ConstFile=$PWD/xdc/constraints.xdc
WrapperFile=$PWD/src/syn/${DataType}_${Op}_n${NVec}_w${W}_m${M}/wrapper_module.v
TbFile=$PWD/src/sim/${DataType}_${Op}_n${NVec}_w${W}_m${M}/full_tb.sv

if [ -d "$ProjectDir" ]; then
    rm -rf $ProjectDir
fi

Synth="off"
Impl="off"
if [ "$StepSynth" = true ] || [ "$StepImpl" = true ]; then
    Synth="on"
	echo "Synthesizing is enabled"
	if [ "$StepImpl" = true ]; then
        Impl="on"
		echo "Implemention is enabled"
    fi
fi

sed -i "/set ProjectName/c\set ProjectName ${ProjectName}" $VivadoScript
sed -i "/set ProjectDir/c\set ProjectDir ${ProjectDir}" $VivadoScript
sed -i "/set OutputDir/c\set OutputDir ${OutputDir}" $VivadoScript
sed -i "/set IPName/c\set IPName ${IPUnzipFolder}" $VivadoScript
sed -i "/set IPDir/c\set IPDir ${IPDir}" $VivadoScript
sed -i "/set IPModule/c\set IPModule run_experiment_0" $VivadoScript
sed -i "/set IPInstance/c\set IPInstance run_experiment" $VivadoScript
sed -i "/set ConstFile/c\set ConstFile ${ConstFile}" $VivadoScript
sed -i "/set WrapperFile/c\set WrapperFile ${WrapperFile}" $VivadoScript
sed -i "/set TbFile/c\set TbFile ${TbFile}" $VivadoScript
sed -i "/set FpgaPart/c\set FpgaPart ${FpgaPart}" $VivadoScript
sed -i 's+/c/+c:/+g' $VivadoScript
sed -i 's+/d/+d:/+g' $VivadoScript
sed -i 's+/e/+e:/+g' $VivadoScript
sed -i "/set Synth/c\set Synth ${Synth}" $VivadoScript
sed -i "/set Impl/c\set Impl ${Impl}" $VivadoScript

cmd="print(\"create_clock -period {:.3f} -name clock -waveform {{0.000 {:.3f} }} [get_ports ap_clk]\".format(1000/${Freq}, 500/${Freq}))"
echo $cmd > tmp.py
python tmp.py > $ConstFile
rm tmp.py

$Vivado -mode batch -source $VivadoScript >> $LogDir/$LogHLS

# Remove unnecessary log files
rm vivado*.jou
rm vivado*.log

echo_line
echo "Finished."
