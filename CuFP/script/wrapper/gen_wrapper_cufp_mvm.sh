#!/bin/sh

# Define variables
WrapperFile=$1
N=$2
W=$3
M=$4

DataType="cufp"
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

    # Define x, y and zinterfaces
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
    wire ex_x_mnts, ex_x_exp, ex_y_mnts, ex_y_exp;
    wire ex_z_mnts [N-1:0], ex_z_exp [N-1:0];
    wire ap_z_mnts [N-1:0], ap_z_exp [N-1:0], ap_z_sign [N-1:0];

    reg s_start;
    reg [W-1:0] s_x [N-1:0][N-1:0];
    reg [W-1:0] s_y [N-1:0];

    wire w_idle;
    wire w_ready;
    wire w_done;
    wire [W-1:0] w_z [N-1:0];

    run_experiment_0 dut (
        .ap_clk(ap_clk),
        .ap_rst(ap_rst),
        .ap_start(s_start),
EOL

    # Generate .z_mnts_V_ap_vld, .z_exp_V_ap_vld, and .z_sign_ap_vld signals
    for ((i=0; i<N; i++)); do
        printf "        .z_%d_mnts_V_ap_vld(ap_z_mnts[%d]), .z_%d_exp_V_ap_vld(ap_z_exp[%d]), .z_%d_sign_ap_vld(ap_z_sign[%d]),\n" $i $i $i $i $i $i
    done

    # Generate .z_mnts_V, .z_exp_V, and .z_sign signals
    for ((i=0; i<N; i++)); do
        printf "        .z_%d_mnts_V({ex_z_mnts[%d], w_z[%d][M-1:0]}), .z_%d_exp_V({ex_z_exp[%d], w_z[%d][W-2:M]}), .z_%d_sign(w_z[%d][W-1]),\n" $i $i $i $i $i $i $i $i
    done

    # Generate .y_mnts_V, .y_exp_V, and .y_sign signals
    for ((i=0; i<N; i++)); do
        printf "        .y_%d_mnts_V({ex_y_mnts, s_y[%d][M-1:0]}), .y_%d_exp_V({ex_y_exp, s_y[%d][W-2:M]}), .y_%d_sign(s_y[%d][W-1]),\n" $i $i $i $i $i $i
    done

    # Generate .x_mnts_V, .x_exp_V, and .x_sign signals
    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "        .x_%d_%d_mnts_V({ex_x_mnts, s_x[%d][%d][M-1:0]}), .x_%d_%d_exp_V({ex_x_exp, s_x[%d][%d][W-2:M]}), .x_%d_%d_sign(s_x[%d][%d][W-1]),\n" $i $j $i $j $i $j $i $j $i $j $i $j
        done
    done

    cat <<EOL
	    .ap_done(w_done),
        .ap_idle(w_idle),
        .ap_ready(w_ready)
    );

    assign ex_x_mnts = 1'b0;
    assign ex_x_exp = 1'b0;
    assign ex_y_mnts = 1'b0; 
    assign ex_y_exp = 1'b0;

    integer i, j;
    always @(posedge ap_clk) begin 
        s_start <= ap_start;
EOL

    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "        s_x[%d][%d] <= x_%d_%d;\n" $i $j $i $j
        done
    done

    for ((i=0; i<N; i++)); do
        printf "        s_y[%d] <= y_%d;\n" $i $i
    done

    cat <<EOL
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

