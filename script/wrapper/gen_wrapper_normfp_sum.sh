#!/bin/sh

# Define variables
WrapperFile=$1
N=$2
W=$3
M=$4

DataType="normfp"
Op="dp"

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
  x,
  y,
  // outputs
  done,
  idle,
  ready,
  z
);
    input ap_clk;
    input ap_rst;
    input ap_start;
    input wire [W-1:0] x;
    input wire [W-1:0] y;

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
		.ap_return(w_z),
		.x(x),
		.y(y)
    );

    integer i, j;
    always @(posedge ap_clk) begin 
        idle <= w_idle;
        ready <= w_ready;
        done <= w_done;
		z <= w_z;
    end

endmodule
EOL
} > $WrapperFile

