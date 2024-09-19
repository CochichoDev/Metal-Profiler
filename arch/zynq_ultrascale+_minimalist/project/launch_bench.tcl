# Check if there are any arguments passed
if {$argc == 0} {
    puts "No command-line arguments provided."
} else {
    set dir [file dirname [info script]]

    connect
    
    targets -set -nocase -filter {name=~"PSU"}
    rst -system

    targets -set -nocase -filter {name=~"*A53*0"}
    rst -cores -clear-registers
    dow $dir/fsbl/bin/fsbl_a53.elf
    bpadd XFsbl_Loop
    con

    #after 500

    while {1} {
        #stop
        #set pc_value [rrd pc]
        set status [state]
        if {[string match "*Stopped*" $status]} {
            break
        }
        #if {[string match "*fffd282c*" $pc_value]} {
        #    break
        #}
        #con
    }
    bpremove 0
    puts "FSBL finished"
    rst -cores -clear-registers

    dow -keepsym $dir/LOADER/bin/LOADER.elf
    puts "LOADER & MMU loaded"


    # Loop through each argument in argv
    foreach arg $argv {
        if {$arg == "Core4"} {
            targets -set -nocase -filter {name=~"*A53*3"}

            while {1} {
                set status [state]
                if {[string match "*Stopped*" $status]} break
            }
            dow -keepsym $dir/Core4/bin/Core4.elf
            puts "Core4 Launched"
            con
        }
        if {$arg == "Core3"} {
            targets -set -nocase -filter {name=~"*A53*2"}

            while {1} {
                set status [state]
                if {[string match "*Stopped*" $status]} break
            }

            dow -keepsym $dir/Core3/bin/Core3.elf
            puts "Core3 Launched"
            con
        } 
        if {$arg == "Core2"} {
            targets -set -nocase -filter {name=~"*A53*1"}

            while {1} {
                set status [state]
                if {[string match "*Stopped*" $status]} break
            }

            dow -keepsym $dir/Core2/bin/Core2.elf
            puts "Core2 Launched"
            con
        } 
        if {$arg == "Core1"} {
            targets -set -nocase -filter {name=~"*A53*0"}

            while {1} {
                set status [state]
                if {[string match "*Stopped*" $status]} break
            }

            dow -keepsym $dir/Core1/bin/Core1.elf
            puts "Core1 Launched"
            con
        } 
    }
    disconnect
    exit
}
