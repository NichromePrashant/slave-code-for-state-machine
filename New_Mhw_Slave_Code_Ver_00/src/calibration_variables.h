/*
 * calibration_variables.h
 *
 *  Created on: Feb 25, 2022
 *      Author: Dnyaneshwar
 */

#ifndef CALIBRATION_VARIABLES_H_
#define CALIBRATION_VARIABLES_H_

#ifndef CALIBRATION_VAR
# define _CALIBRATION extern
# define _CALIBRATION_INIT(x)
#else
# define _CALIBRATION
# define _CALIBRATION_INIT(x)  = x
#endif

_CALIBRATION _Bool Flag_CalibOperation _CALIBRATION_INIT(0);
_CALIBRATION _Bool Flag_Tare_Done _CALIBRATION_INIT(0);
_CALIBRATION _Bool Flag_Calib_Done _CALIBRATION_INIT(0);
_CALIBRATION _Bool Flag_CalOpr_Stage1 _CALIBRATION_INIT(0);
_CALIBRATION _Bool Flag_CalOpr_Stage2 _CALIBRATION_INIT(0);
_CALIBRATION _Bool Flag_CalOpr_Stage3 _CALIBRATION_INIT(0);

_CALIBRATION _Bool Tare_Flag _CALIBRATION_INIT(0);
_CALIBRATION _Bool Dump_Flag _CALIBRATION_INIT(0);
_CALIBRATION _Bool Calibeate_Flag _CALIBRATION_INIT(0);
_CALIBRATION _Bool flag_home_wb _CALIBRATION_INIT(0),flag_home_fb _CALIBRATION_INIT(0);

_CALIBRATION signed int 	TareCnt _CALIBRATION_INIT(0);
_CALIBRATION unsigned int Tare_Send_Time _CALIBRATION_INIT(0),Tare_Send_Flag _CALIBRATION_INIT(0);
_CALIBRATION signed int	CalibSlope _CALIBRATION_INIT(0);
_CALIBRATION signed int	CalibCnt _CALIBRATION_INIT(0);

#endif /* CALIBRATION_VARIABLES_H_ */
