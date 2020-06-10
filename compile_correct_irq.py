Import('env')

from os.path import join, realpath
import sys

# ignore the wrong irq_cm* files, build the correct one.
# we can deduce the Cortex-M type from the board information.

board_cpu = env.BoardConfig().get("build.cpu")

irq_file = None 
if board_cpu == "cortex-m0": 
    irq_file = "RTX/Source/GCC/irq_cm0.S"
elif board_cpu == "cortex-m3":
    irq_file = "RTX/Source/GCC/irq_cm3.S"
elif board_cpu == "cortex-m4":
    irq_file = "RTX/Source/GCC/irq_cm4f.S"
else: 
    sys.stderr.write("Cannot deduce correct IRQ source file for given build.cpu \"%s\". Build will fail.\n" % board_cpu)
    # default to Cortex M3..
    irq_file = "RTX/Source/GCC/irq_cm3.S"

# just modify the source filter. must still manually excluse examples though -- otherwise it will compile it.
env.Replace(SRC_FILTER=["+<*>", "-<.git/>", "-<examples/>", "-<RTX/Source/GCC/*>", "+<%s>" % irq_file])
