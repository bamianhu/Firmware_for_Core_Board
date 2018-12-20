#!/bin/sh
#
# This file was automatically generated by the Nios II IDE Flash Programmer.
#
# It will be overwritten when the flash programmer options change.
#

cd C:/SMALL_V18_Canada_6_10_Final_Test_Stage/SMALL_V18_Canada_6_10/SMALL_V18_Canada/software/SmallSizeV14_sim/Debug

# Creating .flash file for the FPGA configuration
"$SOPC_KIT_NIOS2/bin/sof2flash" --epcs --input="C:/SMALL_V18_Canada_6_10_Final_Test_Stage/SMALL_V18_Canada_6_10/SMALL_V18_Canada/SMALL_V14.sof" --output="SMALL_V14.flash" 

# Programming flash with the FPGA configuration
"$SOPC_KIT_NIOS2/bin/nios2-flash-programmer" --epcs --base=0x01001800 --sidp=0x01002288 --id=151178878 --timestamp=1526974626 --accept-bad-sysid --instance=0 "SMALL_V14.flash"

# Creating .flash file for the project
"$SOPC_KIT_NIOS2/bin/elf2flash" --epcs --after="SMALL_V14.flash" --input="SmallSizeV14_sim.elf" --output="epcs_flash_controller_0.flash"

# Programming flash with the project
"$SOPC_KIT_NIOS2/bin/nios2-flash-programmer" --epcs --base=0x01001800 --sidp=0x01002288 --id=151178878 --timestamp=1526974626 --accept-bad-sysid --instance=0 "epcs_flash_controller_0.flash"
