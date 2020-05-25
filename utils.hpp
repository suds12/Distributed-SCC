#ifndef __UTILS_HPP
#define __UTILS_HPP

#ifdef HAVE_PETSC
#include <petscsys.h>
PetscLogEvent event_read_input, event_local_scc, event_create_meta, event_init_coo, event_make_meta_par, event_make_meta_seq;
PetscLogStage stage_init, stage_update;
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
