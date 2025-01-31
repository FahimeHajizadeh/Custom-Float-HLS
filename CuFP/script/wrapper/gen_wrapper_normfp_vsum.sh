#!/bin/sh

# Define variables
WrapperFile=$1
N=$2
W=$3
M=$4

DataType="normfp"
Op="vsum"

{
    cat <<EOL
\`timescale 1ns / 1ps
// Automatically generated Verilog wrapper module for ${DataType}_${Op}_n${N}_w${W}_m${M}
module wrapper_module 
#(parameter W = $W, parameter M = $M, parameter N = $N) 
(
  // inputs
  ap_clk,
  ap_rst,
  ap_start,
EOL

    # Define x interface
    for ((i=0; i<N; i++)); do
        printf "    x_%d,\n" $i
    done

    cat <<EOL
  // outputs
  done,
  idle,
  ready,
  z
);
    input ap_clk;
    input ap_rst;
    input ap_start;
EOL
	
	# Define x signals
    for ((i=0; i<N; i++)); do
        printf "    input wire [W-1:0] x_%d;\n" $i
    done

    cat <<EOL
    output reg done;
    output reg idle;
    output reg ready;
    output reg [W-1:0] z;

    wire w_idle;
    wire w_ready;
    wire w_done;
    wire [W-1:0] w_z;

    run_experiment_0 dut (
        .ap_clk(ap_clk),
        .ap_rst(ap_rst),
        .ap_start(s_start),
        .ap_done(w_done),
        .ap_idle(w_idle),
        .ap_ready(w_ready),
EOL

    # Generate x signals
    for ((i=0; i<N; i++)); do
        printf "        .x_%d(x[%d]),\n" $i $i
    done

    cat <<EOL
	    .ap_return(w_z)
    );

    always @(posedge ap_clk) begin 
        idle <= w_idle;
        ready <= w_ready;
        done <= w_done;
		z <= w_z;
    end

endmodule
EOL
} > $WrapperFile

