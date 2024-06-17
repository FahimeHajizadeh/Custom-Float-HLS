#!/bin/sh

# Define variables
WrapperFile=$1
N=$2
W=$3
M=$4

DataType="normfp"
Op="mvm"

{
    cat <<EOL
\`timescale 1ns / 1ps
// Automatically generated Verilog wrapper module for ${DataType}_${Op}_N${N}_w${W}_m${M}
module wrapper_module 
#(parameter W = $W, parameter M = $M, parameter N = $N) 
(
  // inputs
  ap_clk,
  ap_rst,
  ap_start,
EOL

    # Define x, y, and z interfaces
    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "    x_%d_%d,\n" $i $j
        done
    done
	for ((i=0; i<N; i++)); do
	    printf "    y_%d,\n" $i
	done
	
	for ((i=0; i<N; i++)); do
	    printf "    z_%d,\n" $i
	done

    cat <<EOL
  done,
  idle,
  ready
);
    input ap_clk;
    input ap_rst;
    input ap_start;
EOL

	# Define x and y signals
    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "    input wire [W-1:0] x_%d_%d;\n" $i $j
        done
    done
	for ((i=0; i<N; i++)); do
	    printf "    input wire [W-1:0] y_%d;\n" $i
	done

    cat <<EOL
    output reg done;
    output reg idle;
    output reg ready;
EOL
	
	# Define z signals
	for ((i=0; i<N; i++)); do
	    printf "    output reg [W-1:0] z_%d;\n" $i
	done
	
    cat <<EOL
    wire w_idle;
    wire w_ready;
    wire w_done;
    wire [W-1:0] w_z [N-1:0];

    run_experiment_0 dut (
        .ap_clk(ap_clk),
        .ap_rst(ap_rst),
        .ap_start(s_start),
EOL

    # Generate .z_mnts_V, .z_exp_V, and .z_sign signals
    for ((i=0; i<N; i++)); do
        printf "        .z_%d(w_z[%d]),\n" $i $i
    done

    # Generate .y_mnts_V, .y_exp_V, and .y_sign signals
    for ((i=0; i<N; i++)); do
        printf "        .y_%d(y_%d),\n" $i $i
    done

    # Generate .x_mnts_V, .x_exp_V, and .x_sign signals
    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "        .x_%d_%d(x_%d_%d),\n" $i $j $i $j
        done
    done

    cat <<EOL
	    .ap_done(w_done),
        .ap_idle(w_idle),
        .ap_ready(w_ready)
    );

    integer i;
    always @(posedge ap_clk) begin 
        idle <= w_idle;
        ready <= w_ready;
        done <= w_done;
EOL

    for ((i=0; i<N; i++)); do
        printf "        z_%d <= w_z[%d];\n" $i $i
    done

    cat <<EOL
    end

endmodule
EOL
} > $WrapperFile

