#ifndef _MAIN_H_
#define _MAIN_H_

#include <math.h>
#include <io.h>
#include <altera_avalon_pio_regs.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "sys/alt_irq.h"
#include "system.h"
#include "Config.h"
#include "Robot.h"
#include "Timer.h"
#include "Misc.h"
#include "nRF2401.h"
#include "Pca9557.h"
#include "Packet.h"
#include "Action.h"

#define IOWR_AVALONCONTROL_A(base, data)       IOWR(base, 0, data)
#define IOWR_AVALONCONTROL_B(base, data)       IOWR(base, 1, data)
#define IOWR_AVALONCONTROL_SET(base, data)       IOWR(base, 2, data)
#define IOWR_AVALONCONTROL_Z_OPENLOOP(base, data)       IOWR(base, 3, data)
#define PI 3.14159

#endif

