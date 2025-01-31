#!/bin/sh

# Define variables
TbFile=$1
N=$2
W=$3
M=$4

DataType="cufp"
Op="mvm"

gen_rand_int() {
    # Generate a random number between 0 and 2000
    local random_number=$(( RANDOM % 201 ))

    # Shift the range to [-1000, 1000]
    local shifted_number=$(( random_number - 100 ))

    # Return the shifted number
    echo $shifted_number
}

gen_rand_int_pos() {
    # Generate a random number between 0 and 2000
    local random_number=$(( RANDOM % 1000 ))

    # Return the shifted number
    echo $random_number
}

gen_rand_float() {
    echo $(gen_rand_int).$(gen_rand_int_pos)
}

{
    cat <<EOL
\`timescale 1ns / 1ps
// Automatically generated Verilog Tb module for ${DataType}_${Op}_N${N}_w${W}_m${M}
module full_tb 
#(parameter W = $W, parameter M = $M, parameter N = $N) ();
    localparam period = 5;
	// Seed the random number generator for reproducibility
    int seed = 12345;
    
    reg clk;
    reg rst;
    reg start;
	wire done;
    wire idle;
    wire ready;
EOL

    # Define x, y and zinterfaces
    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "    reg [W-1:0] x_%d_%d;\n" $i $j
        done
    done
	for ((i=0; i<N; i++)); do
	    printf "    reg [W-1:0] y_%d;\n" $i
	done
	
	for ((i=0; i<N; i++)); do
	    printf "    wire [W-1:0] z_%d;\n" $i
	done

    cat <<EOL
// ------------------------------ 
// IP instantiation 
// ------------------------------ 
    wrapper_module dut (.ap_clk(clk),
                        .ap_rst(rst),
                        .ap_start(start),
EOL

	# Define x and y signals
    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "                        .x_%d_%d(x_%d_%d),\n" $i $j $i $j
        done
    done
	for ((i=0; i<N; i++)); do
	    printf "                        .y_%d(y_%d),\n" $i $i
	done
	for ((i=0; i<N; i++)); do
	    printf "                        .z_%d(z_%d),\n" $i $i
	done

    cat <<EOL
                        .idle(idle),
                        .ready(ready),
                        .done(done));

// ---------------------- 
// Clock Generation block
// ----------------------                         
    always 
    begin
    clk = 1'b0; 
    #(period/2.0);

    clk = 1'b1;
    #(period/2.0); 
    end
// ----------------------  

// -----------------------------
// Set inputs and run simulation
// -----------------------------
EOL
	
	for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "    real a_%d_%d;\n" $i $j
        done
    done
	for ((i=0; i<N; i++)); do
	    printf "    real b_%d;\n" $i
	done
	for ((i=0; i<N; i++)); do
	    printf "    real z_ref_%d;\n" $i
	done
	for ((i=0; i<N; i++)); do
	    printf "    real z_dut_%d;\n" $i
	done


    for ((i=0; i<N; i++)); do
        for ((j=0; j<N; j++)); do
            printf "    assign x_%d_%d = \$shortrealtobits(a_%d_%d);\n" $i $j $i $j
        done
    done
	for ((i=0; i<N; i++)); do
	    printf "    assign y_%d = \$shortrealtobits(b_%d);\n" $i $i
	done
	
	for ((i=0; i<N; i++)); do
	    printf "    assign z_ref_%d = " $i
        for ((j=0; j<N; j++)); do
            printf "a_%d_%d * b_%d" $i $j $j
			if [ $j -lt $((N-1)) ]; then
			    printf " + "
			else
			    printf ";\n"
			fi
        done
    done
	
	for ((i=0; i<N; i++)); do
	    printf "    assign z_dut_%d = \$bitstoshortreal(z_%d);\n" $i $i
	done
   
    cat <<EOL
    initial
    begin
        #(100);
        
        rst = 1'b1;
        start = 1'b0;
        #(period);
EOL

    for ((i=0; i<N; i++)); do
	    for ((j=0; j<N; j++)); do
		    printf "        a_%d_%d = $(gen_rand_float);\n" $i $j
		done
	done
	for ((i=0; i<N; i++)); do
	    printf "        b_%d = $(gen_rand_float);\n" $i
	done
		
    cat <<EOL
	    #(1);
        rst = 1'b0;
        start = 1'b1;
        #(period);
		
EOL

    for ((i=0; i<N; i++)); do
	    for ((j=0; j<N; j++)); do
		    printf "        a_%d_%d = $(gen_rand_float);\n" $i $j
		done
	done
	for ((i=0; i<N; i++)); do
	    printf "        b_%d = $(gen_rand_float);\n" $i
	done
	
	cat <<EOL
	#(period);
	
EOL
	
    for ((i=0; i<N; i++)); do
	    for ((j=0; j<N; j++)); do
		    printf "        a_%d_%d = $(gen_rand_float);\n" $i $j
		done
	done
	for ((i=0; i<N; i++)); do
	    printf "        b_%d = $(gen_rand_float);\n" $i
	done
	
	cat <<EOL
	#(period);
	
EOL

    for ((i=0; i<N; i++)); do
	    for ((j=0; j<N; j++)); do
		    printf "        a_%d_%d = $(gen_rand_float);\n" $i $j
		done
	done
	for ((i=0; i<N; i++)); do
	    printf "        b_%d = $(gen_rand_float);\n" $i
	done
	
	cat <<EOL
	#(period);
	
    end

endmodule
EOL
} > $TbFile

