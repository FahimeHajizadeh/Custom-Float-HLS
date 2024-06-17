#!/bin/sh

# Define variables
WrapperFile=$1
N=$2
W=$3
M=$4

DataType="cufp"
Op="sum"

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

    wire ex_x_mnts, ex_x_exp, ex_y_mnts, ex_y_exp;
    wire ex_z_mnts, ex_z_exp;
    wire ap_z_mnts, ap_z_exp, ap_z_sign;

    reg s_start;
    reg [W-1:0] s_x;
    reg [W-1:0] s_y;

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
		.x_mnts_V({ex_x_mnts, s_x[M-1:0]}), .x_exp_V({ex_x_exp, s_x[W-2:M]}), .x_sign(s_x[W-1]),
		.y_mnts_V({ex_y_mnts, s_y[M-1:0]}), .y_exp_V({ex_y_exp, s_y[W-2:M]}), .y_sign(s_y[W-1]),
	    .agg_result_mnts_V({ex_z_mnts, w_z[M-1:0]}),
        .agg_result_mnts_V_ap_vld(ap_z_mnts),
        .agg_result_exp_V({ex_z_exp, w_z[W-2:M]}),
        .agg_result_exp_V_ap_vld(ap_z_exp),
        .agg_result_sign(w_z[W-1]),
        .agg_result_sign_ap_vld(ap_z_sign)
    );

    assign ex_x_mnts = 1'b0;
    assign ex_x_exp = 1'b0;
    assign ex_y_mnts = 1'b0; 
    assign ex_y_exp = 1'b0;

    integer i, j;
    always @(posedge ap_clk) begin 
        s_start <= ap_start;
		s_x <= x;
		s_y <= y;
        idle <= w_idle;
        ready <= w_ready;
        done <= w_done;
		z <= w_z;
    end

endmodule
EOL
} > $WrapperFile

