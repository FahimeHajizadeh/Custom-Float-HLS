############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2019 Xilinx, Inc. All Rights Reserved.
############################################################
set clkFreq 200
set clkPeriod [expr 1000.0 / $clkFreq]
set clkUncertainty "2%"
set FpgaPart {xcvu9p-fsgd2104-3-e}

open_project CustomFloat
set_top run_experiment
add_files src/config.h
add_files src/run_operations.h
add_files src/run_operations.cpp
add_files src/array_utils.h
add_files src/arith_utils.h
add_files src/normal_float.h -cflags "-std=c++11"
add_files src/custom_float.h -cflags "-std=c++11"
add_files -tb src/run_custom_float_tb.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1"
set_part $FpgaPart
create_clock -period $clkPeriod -name default
config_sdx -target none
config_export -display_name CuFP -format ip_catalog -rtl verilog -version 1.0 -vivado_optimization_level 2 -vivado_phys_opt place -vivado_report_level 0
set_clock_uncertainty $clkUncertainty
#source "./CustomFloat/solution1/directives.tcl"
#csim_design
csynth_design
#cosim_design
export_design -rtl verilog -format ip_catalog -display_name "CuFP"
