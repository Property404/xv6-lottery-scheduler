/* This utility runs unit tests*/
#include "types.h"
#include "pstat.h"
#include "user.h"

int ps(){
	fprintf(1,"This process: %d\n", getpid());
	struct pstat target = {0};
	if(-1 == getpinfo(&target)){
		return 0;
		fprintf(1, "\n\t FAILURE\n");
	}
	fprintf(1, "PID\tTicks\tIn_Use\tTickets\tState\n");
	int skipped = 0;
	for(int i=0;i<NPROC;i++){
		if(target.pid[i] == 0)
		{
			skipped++;
			continue;
		}
		fprintf(1, "%d\t%d\t%d\t%d\t%d\n", 
				target.pid[i],
				target.ticks[i],
				target.inuse[i],
				target.tickets[i],
				target.state[i]
				);
	}
	return 1;
}

int
main(int argc, char *argv[])
{
	ps();

	exit();
}
