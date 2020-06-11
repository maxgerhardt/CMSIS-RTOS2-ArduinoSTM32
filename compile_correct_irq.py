Import('env')

from os.path import join, realpath
import sys

# ignore the wrong irq_cm* files, build the correct one.
# we can deduce the Cortex-M type from the board information.

board_cpu = env.BoardConfig().get("build.cpu")

# list of all supported IRQ files. exclude the ones we do not want. 
irq_files = [
	"irq_cm0.S",
	"irq_cm3.S",
	"irq_cm4f.S"
]

irq_file = None 
if board_cpu == "cortex-m0": 
    irq_file = "src/irq_cm0.S"
elif board_cpu == "cortex-m3":
    irq_file = "src/irq_cm3.S"
# cortex m7 is closer to cortex m4f.
# validation pending: https://github.com/ARM-software/CMSIS_5/issues/943
elif board_cpu == "cortex-m4" or board_cpu == "cortex-m7":
    irq_file = "src/irq_cm4f.S"
else: 
    sys.stderr.write("Cannot deduce correct IRQ source file for given build.cpu \"%s\". Build will fail.\n" % board_cpu)
    # default to Cortex M3..
    irq_file = "src/irq_cm3.S"

# build list of excluded IRQ files 
excluded_irqs = ["-<%s>" % irq for irq in irq_files if irq != irq_file]
# append it to the blacklist
env.Append(SRC_FILTER=excluded_irqs)
