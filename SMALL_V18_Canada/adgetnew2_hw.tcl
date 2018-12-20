# TCL File Generated by Component Editor 9.1
# Fri Oct 08 11:01:12 CST 2010
# DO NOT MODIFY


# +-----------------------------------
# | 
# | adgetnew2 "adgetnew2" v1.0
# | mayue 2010.10.08.11:01:12
# | 
# | 
# | E:/project/svn_workspace/motorcontrol/adgetnew2.v
# | 
# |    ./adgetnew2.v syn, sim
# | 
# +-----------------------------------

# +-----------------------------------
# | request TCL package from ACDS 9.1
# | 
package require -exact sopc 9.1
# | 
# +-----------------------------------

# +-----------------------------------
# | module adgetnew2
# | 
set_module_property NAME adgetnew2
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property GROUP Communication
set_module_property AUTHOR mayue
set_module_property DISPLAY_NAME adgetnew2
set_module_property TOP_LEVEL_HDL_FILE adgetnew2.v
set_module_property TOP_LEVEL_HDL_MODULE adgetnew2
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ANALYZE_HDL TRUE
# | 
# +-----------------------------------

# +-----------------------------------
# | files
# | 
add_file adgetnew2.v {SYNTHESIS SIMULATION}
# | 
# +-----------------------------------

# +-----------------------------------
# | parameters
# | 
# | 
# +-----------------------------------

# +-----------------------------------
# | display items
# | 
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point clock_reset
# | 
add_interface clock_reset clock end

set_interface_property clock_reset ENABLED true

add_interface_port clock_reset clk clk Input 1
add_interface_port clock_reset reset_n reset_n Input 1
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point avalon_slave_0
# | 
add_interface avalon_slave_0 avalon end
set_interface_property avalon_slave_0 addressAlignment DYNAMIC
set_interface_property avalon_slave_0 associatedClock clock_reset
set_interface_property avalon_slave_0 burstOnBurstBoundariesOnly false
set_interface_property avalon_slave_0 explicitAddressSpan 0
set_interface_property avalon_slave_0 holdTime 0
set_interface_property avalon_slave_0 isMemoryDevice false
set_interface_property avalon_slave_0 isNonVolatileStorage false
set_interface_property avalon_slave_0 linewrapBursts false
set_interface_property avalon_slave_0 maximumPendingReadTransactions 0
set_interface_property avalon_slave_0 printableDevice false
set_interface_property avalon_slave_0 readLatency 0
set_interface_property avalon_slave_0 readWaitTime 1
set_interface_property avalon_slave_0 setupTime 0
set_interface_property avalon_slave_0 timingUnits Cycles
set_interface_property avalon_slave_0 writeWaitTime 0

set_interface_property avalon_slave_0 ASSOCIATED_CLOCK clock_reset
set_interface_property avalon_slave_0 ENABLED true

add_interface_port avalon_slave_0 addr address Input 2
add_interface_port avalon_slave_0 read_n read_n Input 1
add_interface_port avalon_slave_0 cs_n chipselect_n Input 1
add_interface_port avalon_slave_0 readdata readdata Output 16
# | 
# +-----------------------------------

# +-----------------------------------
# | connection point conduit_end
# | 
add_interface conduit_end conduit end

set_interface_property conduit_end ENABLED true

add_interface_port conduit_end clk_out export Output 1
add_interface_port conduit_end din export Output 1
add_interface_port conduit_end dout export Input 1
add_interface_port conduit_end cs export Output 1
add_interface_port conduit_end sars export Input 1
# | 
# +-----------------------------------
