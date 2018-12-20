#!/bin/sh
#
# generated.sh - shell script fragment - not very useful on its own
#
# Machine generated for a CPU named "cpu_0" as defined in:
# c:\SMALL_V18_Canada_6_10_Final_Test_Stage\SMALL_V18_Canada_6_10\SMALL_V18_Canada\software\SmallSizeV14_sim_syslib\..\..\SMALL14_CPU.ptf
#
# Generated: 2018-07-14 16:33:56.185

# DO NOT MODIFY THIS FILE
#
#   Changing this file will have subtle consequences
#   which will almost certainly lead to a nonfunctioning
#   system. If you do modify this file, be aware that your
#   changes will be overwritten and lost when this file
#   is generated again.
#
# DO NOT MODIFY THIS FILE

# This variable indicates where the PTF file for this design is located
ptf=c:\SMALL_V18_Canada_6_10_Final_Test_Stage\SMALL_V18_Canada_6_10\SMALL_V18_Canada\software\SmallSizeV14_sim_syslib\..\..\SMALL14_CPU.ptf

# This variable indicates whether there is a CPU debug core
nios2_debug_core=yes

# This variable indicates how to connect to the CPU debug core
nios2_instance=0

# This variable indicates the CPU module name
nios2_cpu_name=cpu_0

# These variables indicate what the System ID peripheral should hold
sidp=0x01002288
id=151178878u
timestamp=1526974626u

# Include operating system specific parameters, if they are supplied.

if test -f /cygdrive/c/altera/91/nios2eds/components/altera_hal/build/os.sh ; then
   . /cygdrive/c/altera/91/nios2eds/components/altera_hal/build/os.sh
fi