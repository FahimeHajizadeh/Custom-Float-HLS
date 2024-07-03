# Welcome to CuFP: An HLS Library for Customized Floating-Point Operators

This project provides High-Level Synthesis source codes implementing customized floating point numbers with their primary and dedicated operations for developing HLS-based projects in which there is a need for non-standard floating point data types. It has also prepared some scripts to facilitate the process of compiling and exporting IP in the Vivado HLS tool, as well as synthesizing and implementing the exported IP in the Vivado Design Suite and reporting the resource and timing results.

## Prequisites

You need to install a bash-compatible program like `git-bash` on your system. You must also install one of AMD Xilinx Vivado and HLS tool versions (2019.1 and later) on your system.

# How to Run 

First of all, you must update the Vivado HLS (or Vitis HLS) and Vivado executable paths in `run_single.sh` script. Then, follow the next steps for executing the script.

```sh
# Define tool paths
VivadoHLS="C:/Xilinx/Vivado/2019.1/bin/vivado_hls.bat"      # Vivado HLS or Vitis HLS binary path
Vivado="C:/Xilinx/Vivado/2019.1/bin/vivado.bat"             # Vivado binary path
```

## Run a single process

Here you can run the whole process for only one configuration using `run_single.sh` script. Open a shell and enter `cd <path-to-project-folder>` and enter the following command with appropriate options to start the process:

```sh
$ ./run_single.sh [options]
```
| Option | Description |
| ------ | ----------- |
| -i | Enable Exporting the IP |
| -s | Enable Synthesizing     |
| -p | Enable Implementation   |
| -d | cufp, normfp |
| -o | sum, mul, vsum, dp, mvm |
| -n | \<vector size\> |
| -w | \<total width\> |
| -m | \<mantissa (fraction) width\> |
| -f | \<clock frequency\> |
| -u | \<clock uncertainty (%)\> |
| -h | virtex7, ultrascale |

Here are some command examples:
```sh
$ ./run_single.sh -isp -h ultrascale -d cufp   -o sum        -w 32 -m 23 -f 400 -u 5
$ ./run_single.sh -isp -h virtex7    -d normfp -o mul        -w 32 -m 23 -f 400 -u 5
$ ./run_single.sh -is  -h ultrascale -d cufp   -o vsum -n 4  -w 32 -m 23 -f 200 -u 5
$ ./run_single.sh -is  -h ultrascale -d cufp   -o dp   -n 32 -w 32 -m 23 -f 200 -u 5
$ ./run_single.sh -i   -h ultrascale -d cufp   -o mvm  -n 8  -w 32 -m 23 -f 200 -u 5
```

## Run a batch process

In order to run the process for a list of configurations, you can put their corresponding commands in `run_batch.sh` and easily run this script:

```sh
$ ./run_batch.sh
```
# How to Use 

This library project can be utilized at both a low and high level, depending on the requirements. The library can be imported into an HLS project for high-level use, or it can be exported as an RTL IP and instantiated into an RTL design. We created a script that exports an RTL IP from the source code, and synthesizes and implements it automatically. This approach can serve as inspiration for developing an FPGA-based project using the CuFP library.

# How to cite this work
This work has been submitted to Electronics journal. The preprint of the paper is available online at http://www.preprints.org with the following information:

Hajizadeh, F.; Ould-Bachir, T.; David, J.-P. CuFP: An HLS Library for Customized Floating-Point Operators. Preprints 2024, 2024061239. https://doi.org/10.20944/preprints202406.1239.v1   

Authors: Fahimeh Hajizadeh, Tarek Ould-Bachir, Jean-Pierre David  
Emails: fahimeh.hajizadeh@polymtl.ca, tarek.ould-bachir@polymtl.ca, jean-pierre.david@polymtl.ca  
