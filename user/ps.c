/* This utility runs unit tests*/
#include "types.h"
#include "pstat.h"
#include "user.h"

int ps(){
	// Get process info first
	struct pstat pinfo = {0};
	if(-1 == getpinfo(&pinfo)){
		return 0;
		fprintf(1, "\n\t FAILURE\n");
	}
	// Our process id
	const int current_pid = getpid();
	// Total ticks of not sleeping processes(not including this one)
	int total_ticks = 0;
	// Where we are in pinfo
	int current_entry = 0;
	for(int i=0;i<NPROC;i++)
	{
		if(pinfo.pid[i] == current_pid)
		{
			current_entry = i;
		}else if(pinfo.state[i] != SLEEPING)
		{
			total_ticks += pinfo.ticks[i];
		}
	}


	// Header
	fprintf(stdout,"This process: %d\n",
			getpid());
	fprintf(stdout,"Total tickets: %d\n\n",
			pinfo.total_tickets);
	fprintf(stdout,
			"PID\tTicks\tTickets\tState\tE%\tA%\n");

	// Body
	for(int i=0;i<NPROC;i++){
		if(pinfo.pid[i]==0)continue;
		_Bool skip_yield = 0;
		// What we expect the percentage of ticks to be
		float expected_yield =
			100*
			(float)pinfo.tickets[i]/
			((float)pinfo.total_tickets - pinfo.tickets[current_entry]);
		// What the actual percentage of ticks is
		float actual_yield =
			100*
			(float)pinfo.ticks[i]/
			(float)total_ticks;

		if(pinfo.state[i] == 2 || pinfo.pid[i] == current_pid)
		{
			skip_yield = 1;
		}
		int ey_left =
			(int)expected_yield;
		int ey_right =
			(int)((expected_yield-ey_left)*10);
		int ay_left =
			(int)actual_yield;
		int ay_right =
			(int)((actual_yield-ay_left)*10);


		// Indicate which process is in use
		if(pinfo.inuse[i])
			putchar('>');
		else
			putchar('|');

		// Write the row
		fprintf(stdout, 
				skip_yield?
				"%d\t%d\t%d\t%d\t-\t-\n":
				"%d\t%d\t%d\t%d\t%d.%d\t%d.%d\n", 
				pinfo.pid[i],
				pinfo.ticks[i],
				pinfo.tickets[i],
				pinfo.state[i],
				ey_left,
				ey_right,
				ay_left,
				ay_right
				);
	}
	return 1;
}

int main(int argc, char *argv[])
{
	ps();

	exit();
}
