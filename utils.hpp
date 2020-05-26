#ifndef DISTRIBUTED_SCC_UTILS_HPP
#define DISTRIBUTED_SCC_UTILS_HPP

#include <boost/program_options.hpp>

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

namespace po = boost::program_options;
void process_options(int, char**, po::variables_map&);

#endif
