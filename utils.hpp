#ifndef __UTILS_HPP
#define __UTILS_HPP

#ifdef HAVE_PETSC
#include <petscsys.h>
#define log_begin(event) PetscLogEventBegin(event,0,0,0,0)
#define log_end(event) PetscLogEventEnd(event,0,0,0,0)
#define log_stage_begin(stage_var) PetscLogStagePush(stage_var)
#define log_stage_end(stage_var) PetscLogStagePop()

#else
#define log_begin(event)
#define log_end(event) 
#define log_stage_begin(stage_var)
#define log_stage_end(stage_var)
#endif


#endif
