#!/bin/sh

# Define variables
WrapperFile=$1
N=$2
W=$3
M=$4

DataType="cufp"
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
EOL

    # Define x and a interfaces
    for ((i=0; i<N; i++)); do
        printf "    x_%d,\n" $i
    done
	for ((i=0; i<N; i++)); do
        printf "    a_%d,\n" $i
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
	
	# Define x and a signals
    for ((i=0; i<N; i++)); do
        printf "    input wire [W-1:0] x_%d;\n" $i
    done
	for ((i=0; i<N; i++)); do
        printf "    input wire [W-1:0] a_%d;\n" $i
    done

    cat <<EOL
    output reg done;
    output reg idle;
    output reg ready;
    output reg [W-1:0] z;

    wire ex_x_mnts, ex_x_exp, ex_a_mnts, ex_a_exp;
    wire ex_z_mnts, ex_z_exp;
    wire ap_z_mnts, ap_z_exp, ap_z_sign;

    reg s_start;
    reg [W-1:0] s_x [N-1:0];
    reg [W-1:0] s_a [N-1:0];

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

    # Generate .x_mnts_V, .x_exp_V, and .x_sign signals
    for ((i=0; i<N; i++)); do
        printf "        .x_%d_mnts_V({ex_x_mnts, s_x[%d][M-1:0]}), .x_%d_exp_V({ex_x_exp, s_x[%d][W-2:M]}), .x_%d_sign(s_x[%d][W-1]),\n" $i $i $i $i $i $i
    done
	
	# Generate .a_mnts_V, .a_exp_V, and .a_sign signals
    for ((i=0; i<N; i++)); do
        printf "        .a_%d_mnts_V({ex_a_mnts, s_a[%d][M-1:0]}), .a_%d_exp_V({ex_a_exp, s_a[%d][W-2:M]}), .a_%d_sign(s_a[%d][W-1]),\n" $i $i $i $i $i $i
    done

    cat <<EOL
	    .agg_result_mnts_V({ex_z_mnts, w_z[M-1:0]}),
        .agg_result_mnts_V_ap_vld(ap_z_mnts),
        .agg_result_exp_V({ex_z_exp, w_z[W-2:M]}),
        .agg_result_exp_V_ap_vld(ap_z_exp),
        .agg_result_sign(w_z[W-1]),
        .agg_result_sign_ap_vld(ap_z_sign)
    );

    assign ex_x_mnts = 1'b0;
    assign ex_x_exp = 1'b0;
    assign ex_a_mnts = 1'b0; 
    assign ex_a_exp = 1'b0;

    integer i, j;
    always @(posedge ap_clk) begin 
        s_start <= ap_start;
EOL

    for ((i=0; i<N; i++)); do
        printf "        s_x[%d] <= x_%d;\n" $i $i
    done

    for ((i=0; i<N; i++)); do
        printf "        s_a[%d] <= a_%d;\n" $i $i
    done

    cat <<EOL
        idle <= w_idle;
        ready <= w_ready;
        done <= w_done;
		z <= w_z;
    end

endmodule
EOL
} > $WrapperFile

