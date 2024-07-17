# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/local/MetaVonics_1/Core0_system/_ide/scripts/systemdebugger_core0_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/local/MetaVonics_1/Core0_system/_ide/scripts/systemdebugger_core0_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
source /tools/Xilinx/Vitis/2021.2/scripts/vitis/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*"}
reset_apu
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/local/MetaVonics_1/Meta/export/Meta/hw/zcu102.xsa -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
set mode [expr [mrd -value 0xFF5E0200] & 0xf]
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow /home/local/MetaVonics_1/Meta/export/Meta/sw/Meta/boot/fsbl.elf
set bp_43_33_fsbl_bp [bpadd -addr &XFsbl_Exit]
con -block -timeout 60
bpremove $bp_43_33_fsbl_bp
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow /home/local/MetaVonics_1/Core0/Debug/Core0.elf
targets -set -nocase -filter {name =~ "*A53*#1"}
rst -processor
dow /home/local/MetaVonics_1/Core1/Debug/Core1.elf
targets -set -nocase -filter {name =~ "*A53*#2"}
rst -processor
dow /home/local/MetaVonics_1/Core2/Debug/Core2.elf
targets -set -nocase -filter {name =~ "*A53*#3"}
rst -processor
dow /home/local/MetaVonics_1/Core3/Debug/Core3.elf
targets -set -nocase -filter {name =~ "*R5*#0"}
rst -processor
dow /home/local/MetaVonics_1/R5_0/Debug/R5_0.elf
targets -set -nocase -filter {name =~ "*R5*#1"}
rst -processor
dow /home/local/MetaVonics_1/R5_1/Debug/R5_1.elf
configparams force-mem-access 0
bpadd -addr &main
