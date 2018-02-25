#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"
#include "sysfunc.h"
extern const int total_tickets;

int sys_settickets(void)
{
	int number_of_tickets;
	// Error
	if(argint(0, &number_of_tickets) < 0)
		return -1;

	acquire(&ptable.lock);
	setproctickets(proc, number_of_tickets);
	release(&ptable.lock);

	return 0;
}

int sys_getpinfo(void)
{
	acquire(&ptable.lock);
	struct pstat* target;
	if(argint(0, (int*)(&target)) < 0)
		return -1;

	for(struct proc* p=ptable.proc;p != &(ptable.proc[NPROC]); p++)
	{
		const int index = p - ptable.proc;
		if(p->state != UNUSED)
		{
			target->pid[index] = p->pid;
			target->ticks[index] = p->ticks;
			target->tickets[index] = p->tickets;
			target->inuse[index] = p->inuse;
			target->state[index] = p->state;
		}
	}
	target->total_tickets = total_tickets;
	release(&ptable.lock);
	return 0;

}
int sys_fork(void)
{
	return fork();
}

int sys_exit(void)
{
	exit();
	return 0;  // not reached
}

int sys_wait(void)
{
	return wait();
}

int sys_kill(void)
{
	int pid;

	if(argint(0, &pid) < 0)
		return -1;
	return kill(pid);
}

int sys_getpid(void)
{
	return proc->pid;
}

int sys_sbrk(void)
{
	int addr;
	int n;

	if(argint(0, &n) < 0)
		return -1;
	addr = proc->sz;
	if(growproc(n) < 0)
		return -1;
	return addr;
}

int sys_sleep(void)
{
	int n;
	uint ticks0;

	if(argint(0, &n) < 0)
		return -1;
	acquire(&tickslock);
	ticks0 = ticks;
	while(ticks - ticks0 < n){
		if(proc->killed){
			release(&tickslock);
			return -1;
		}
		sleep(&ticks, &tickslock);
	}
	release(&tickslock);
	return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int sys_uptime(void)
{
	uint xticks;

	acquire(&tickslock);
	xticks = ticks;
	release(&tickslock);
	return xticks;
}
