class GLOBALS
    x0
    mutex

    get_x0():
        x0' = 0
        lock(mutex)
        x0' = x0
        unlock(mutex)
        return x0'

    set_x0(arg):
        lock(mutex)
        this.x0 = arg
        unlock(mutex)

GLOBALS globals

def display():
    global.set_x0(3)
    print(global.get_x0())
    print("Audrey is dumb")
