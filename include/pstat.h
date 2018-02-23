#ifndef _PSTAT_H
#define _PSTAT_H

#include "param.h"

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Copied directly form the assignment sheet
// Everything in this structure is DESCRIPTIVE
// but NOT PRESCRIPTIVE
//
// Meaning that the values of the fields here
// should have absolutely no effect on the kernel
// (but maybe the userspace if the programmer is being an idiot)
struct pstat {
	// Whether the process is running
	// Each entry will be 1 or 0 
	//
	// There should only be at most one '1'
	// (Per CPU)
	_Bool inuse[NPROC];

	// PID of each process
	int pid[NPROC];

	// Number of ticks each process has accumulated
	int ticks[NPROC];

	// Number of tickets
	int tickets[NPROC];

	//state
	enum procstate state[NPROC];

	int total_tickets;

};

#endif
