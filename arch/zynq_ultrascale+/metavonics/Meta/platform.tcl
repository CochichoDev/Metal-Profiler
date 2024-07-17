# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/local/MetaVonics_1/Meta/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/local/MetaVonics_1/Meta/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {Meta}\
-hw {/tools/Xilinx/Vitis/2021.2/data/embeddedsw/lib/fixed_hwplatforms/zcu102.xsa}\
-proc {psu_cortexa53_0} -os {standalone} -arch {64-bit} -fsbl-target {psu_cortexa53_0} -out {/home/local/MetaVonics_1}

platform write
platform generate -domains 
platform active {Meta}
platform active {Meta}
domain create -name {standalone_psu_cortexa53_1} -display-name {standalone_psu_cortexa53_1} -os {standalone} -proc {psu_cortexa53_1} -runtime {cpp} -arch {64-bit} -support-app {hello_world}
platform generate -domains 
platform write
domain active {zynqmp_fsbl}
domain active {zynqmp_pmufw}
domain active {standalone_domain}
domain active {standalone_psu_cortexa53_1}
platform generate -quick
platform active {Meta}
domain create -name {standalone_psu_cortexa53_2} -display-name {standalone_psu_cortexa53_2} -os {standalone} -proc {psu_cortexa53_2} -runtime {cpp} -arch {64-bit} -support-app {hello_world}
platform generate -domains 
platform write
domain active {zynqmp_fsbl}
domain active {zynqmp_pmufw}
domain active {standalone_domain}
domain active {standalone_psu_cortexa53_1}
domain active {standalone_psu_cortexa53_2}
platform generate -quick
platform active {Meta}
domain create -name {standalone_psu_cortexa53_3} -display-name {standalone_psu_cortexa53_3} -os {standalone} -proc {psu_cortexa53_3} -runtime {cpp} -arch {64-bit} -support-app {hello_world}
platform generate -domains 
platform write
domain active {zynqmp_fsbl}
domain active {zynqmp_pmufw}
domain active {standalone_domain}
domain active {standalone_psu_cortexa53_1}
domain active {standalone_psu_cortexa53_2}
domain active {standalone_psu_cortexa53_3}
platform generate -quick
platform active {Meta}
domain create -name {standalone_psu_cortexr5_0} -display-name {standalone_psu_cortexr5_0} -os {standalone} -proc {psu_cortexr5_0} -runtime {cpp} -arch {32-bit} -support-app {hello_world}
platform generate -domains 
platform write
domain active {zynqmp_fsbl}
domain active {zynqmp_pmufw}
domain active {standalone_domain}
domain active {standalone_psu_cortexa53_1}
domain active {standalone_psu_cortexa53_2}
domain active {standalone_psu_cortexa53_3}
domain active {standalone_psu_cortexr5_0}
platform generate -quick
platform active {Meta}
domain create -name {standalone_psu_cortexr5_1} -display-name {standalone_psu_cortexr5_1} -os {standalone} -proc {psu_cortexr5_1} -runtime {cpp} -arch {32-bit} -support-app {hello_world}
platform generate -domains 
platform write
domain active {zynqmp_fsbl}
domain active {zynqmp_pmufw}
domain active {standalone_domain}
domain active {standalone_psu_cortexa53_1}
domain active {standalone_psu_cortexa53_2}
domain active {standalone_psu_cortexa53_3}
domain active {standalone_psu_cortexr5_0}
domain active {standalone_psu_cortexr5_1}
platform generate -quick
platform generate
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate -domains standalone_domain 
platform generate
platform clean
platform generate
platform generate -domains standalone_domain,standalone_psu_cortexa53_1,standalone_psu_cortexa53_2,standalone_psu_cortexa53_3,standalone_psu_cortexr5_0,standalone_psu_cortexr5_1,zynqmp_fsbl,zynqmp_pmufw 
platform generate -domains standalone_domain,standalone_psu_cortexa53_1,standalone_psu_cortexa53_2,standalone_psu_cortexa53_3,standalone_psu_cortexr5_0,standalone_psu_cortexr5_1,zynqmp_fsbl,zynqmp_pmufw 
platform generate
platform clean
platform generate
platform clean
platform generate
platform generate
platform generate -domains standalone_psu_cortexa53_2 
platform clean
platform generate
platform generate -domains standalone_psu_cortexa53_3 
platform active {Meta}
domain active {standalone_psu_cortexa53_3}
bsp reload
domain active {standalone_psu_cortexa53_2}
bsp reload
domain active {standalone_psu_cortexa53_1}
bsp reload
domain active {zynqmp_fsbl}
bsp reload
bsp reload
domain active {standalone_domain}
bsp reload
platform generate
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform generate
platform generate
platform clean
platform generate
platform generate -domains standalone_domain,zynqmp_fsbl 
platform clean
platform generate
platform clean
platform generate
platform generate -domains standalone_domain 
