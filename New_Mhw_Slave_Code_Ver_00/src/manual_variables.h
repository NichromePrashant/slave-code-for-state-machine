/*
 * manual_variables.h
 *
 *  Created on: Feb 25, 2022
 *      Author: Dnyaneshwar
 */

#ifndef MANUAL_VARIABLES_H_
#define MANUAL_VARIABLES_H_

#ifndef MANUAL_VAR
# define _MANUAL extern
# define _MANUAL_INIT(x)
#else
# define _MANUAL
# define _MANUAL_INIT(x)  = x
#endif

_MANUAL _Bool Flag_ManualOperation _MANUAL_INIT(0);
_MANUAL _Bool Flag_ManOpr_Stage1 _MANUAL_INIT(0);
_MANUAL _Bool Flag_ManOpr_Stage2 _MANUAL_INIT(0);
_MANUAL _Bool Flag_ManOpr_Stage3 _MANUAL_INIT(0);
_MANUAL _Bool Flag_ManOpr_Stage4 _MANUAL_INIT(0);
_MANUAL _Bool Flag_SingleCycle _MANUAL_INIT(0);

_MANUAL _Bool ManOpr_Vib_Flag _MANUAL_INIT(0);
_MANUAL _Bool ManOpr_FB_Flag _MANUAL_INIT(0);
_MANUAL _Bool ManOpr_WB_Flag _MANUAL_INIT(0);
_MANUAL _Bool ManOpr_SS_Flag _MANUAL_INIT(0);
_MANUAL _Bool ManOpr_CS_Flag _MANUAL_INIT(0);

#endif /* MANUAL_VARIABLES_H_ */
