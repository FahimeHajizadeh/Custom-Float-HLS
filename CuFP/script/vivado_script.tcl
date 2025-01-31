set ProjectName vivado
set ProjectDir <set automatically>
set OutputDir <set automatically>
set IPName <set automatically>
set IPDir <set automatically>
set IPModule run_experiment_0
set IPInstance run_experiment
set ConstFile <set automatically>
set WrapperFile <set automatically>
set TbFile <set automatically>
set FpgaPart xcvu9p-fsgd2104-3-e
set Synth off
set Impl off

create_project $ProjectName $ProjectDir -part $FpgaPart

close_project

open_project $ProjectDir/$ProjectName.xpr 

import_files -fileset constrs_1 $ConstFile
set_property  ip_repo_paths  $IPDir [current_project]
update_ip_catalog

create_ip -name $IPInstance -vendor xilinx.com -library hls -version 1.0 -module_name $IPModule -dir $ProjectDir/$ProjectName.srcs/sources_1/ip
generate_target {instantiation_template} [get_files $ProjectDir/$ProjectName.srcs/sources_1/ip/$IPModule/$IPModule.xci]

update_compile_order -fileset sources_1
set_property generate_synth_checkpoint false [get_files  $ProjectDir/$ProjectName.srcs/sources_1/ip/$IPModule/$IPModule.xci]
generate_target all [get_files  $ProjectDir/$ProjectName.srcs/sources_1/ip/$IPModule/$IPModule.xci]

export_ip_user_files -of_objects [get_files $ProjectDir/$ProjectName.srcs/sources_1/ip/$IPModule/$IPModule.xci] -no_script -sync -force -quiet
export_simulation -of_objects [get_files $ProjectDir/$ProjectName.srcs/sources_1/ip/$IPModule/$IPModule.xci] -directory $ProjectDir/$ProjectName.ip_user_files/sim_scripts -ip_user_files_dir $ProjectDir/$ProjectName.ip_user_files -ipstatic_source_dir $ProjectDir/$ProjectName.ip_user_files/ipstatic -lib_map_path [list {modelsim=$ProjectDir/$ProjectName.cache/compile_simlib/modelsim} {questa=$ProjectDir/$ProjectName.cache/compile_simlib/questa} {riviera=$ProjectDir/$ProjectName.cache/compile_simlib/riviera} {activehdl=$ProjectDir/$ProjectName.cache/compile_simlib/activehdl}] -use_ip_compiled_libs -force -quiet

import_files -fileset sources_1 $WrapperFile
set_property top wrapper_module [current_fileset]
update_compile_order -fileset sources_1

import_files -fileset sim_1 $TbFile
set_property top full_tb [current_fileset]
update_compile_order -fileset sim_1

if {$Synth==on || $Impl==on} {
    puts "Start Synthesizing..."
	launch_runs synth_1 -jobs 8
	wait_on_run synth_1

	open_run synth_1

	# Write report utilization
    report_utilization -file $OutputDir/synth_utilization.txt
}

if {$Impl==on} {
    puts "Start Implementing..."
    launch_runs impl_1 -jobs 8
    wait_on_run impl_1
    
    open_run impl_1
    
    # Write report utilization and timing estimates
    report_utilization -file $OutputDir/impl_utilization.txt
    report_timing > $OutputDir/impl_timing.txt
    report_timing_summary > $OutputDir/impl_timing_summary.txt
    report_clocks > $OutputDir/impl_clock.txt
}

close_project
