# Check if there are any arguments passed
if {$argc == 0} {
    puts "No command-line arguments provided."
} else {
    set dir [file dirname [info script]]

    connect
    
    targets -set -nocase -filter {name=~"PSU"}
    rst -system

    targets -set -nocase -filter {name=~"*A53*0"}
    rst -cores
    dow $dir/fsbl/bin/fsbl_a53.elf

    bpadd XFsbl_Loop
    con

    while {1} {
        set status [state]
        if {$status == "Stopped: (Breakpoint)"} break
    }
    puts "FSBL finished"

    dow -keepsym $dir/LOADER/bin/LOADER.elf
    puts "LOADER & MMU loaded"

    # Loop through each argument in argv
    foreach arg $argv {
        if {$arg == "Core4"} {
            targets -set -nocase -filter {name=~"*A53*3"}
            dow -keepsym $dir/Core4/bin/Core4.elf
            con
            puts "Core4 Launched"
        }
        if {$arg == "Core3"} {
            targets -set -nocase -filter {name=~"*A53*2"}
            dow -keepsym $dir/Core3/bin/Core3.elf
            con
            puts "Core3 Launched"
        } 
        if {$arg == "Core2"} {
            targets -set -nocase -filter {name=~"*A53*1"}
            dow -keepsym $dir/Core2/bin/Core2.elf
            con
            puts "Core2 Launched"
        } 
        if {$arg == "Core1"} {
            targets -set -nocase -filter {name=~"*A53*0"}
            dow -keepsym $dir/Core1/bin/Core1.elf
            con
            puts "Core1 Launched"
        } 
    }
    disconnect
    exit
}
