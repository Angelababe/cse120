/* mycode3.c: your portion of the kernel
 *
 *   	Below are functions that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these functions.  You may
 *  	modify the bodies of these functions (and add code outside of them)
 * 	in any way you wish (however, you cannot change their interfaces). 
 */

#include "aux.h"
#include "sys.h"
#include "mycode3.h"

#define FALSE 0
#define TRUE 1

int procs[MAXPROCS*MAXSEMS];
/*  	A sample semaphore table.  You may change this any way you wish.  
 */

static struct {
	int valid;	// Is this a valid entry (was sem allocated)?
	int value;	// value of semaphore
} semtab[MAXSEMS];


/*  	InitSem() is called when kernel starts up.  Initialize data
 *  	structures (such as the semaphore table) and call any initialization
 *   	functions here. 
 */

void InitSem()
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {		// mark all sems free
		semtab[s].valid = FALSE;
	}

  for(int i=0; i<MAXPROCS*MAXSEMS; i++){
    procs[i] = -1;
  }
}

/* 	MySeminit(v) is called by the kernel whenever the system call
 *  	Seminit(v) is called.  The kernel passes the initial value v. 
 * 	MySeminit should allocate a semaphore (find a free entry in
 *  	semtab and allocate), initialize that semaphore's value to v,
 *  	and then return the ID (i.e., index of the allocated entry).  
 *  	Should return -1 if it fails (e.g., no free semaphores).  
 */

int MySeminit(int v)
	// v: initial value of semaphore
{
	int s;

	for (s = 0; s < MAXSEMS; s++) {
		if (semtab[s].valid == FALSE) {
			break;
		}
	}
	if (s == MAXSEMS) {
		DPrintf("No free semaphores\n");
		return(-1);
	}
	semtab[s].valid = TRUE;
	semtab[s].value = v;

	return(s);
}

/*   	MyWait(s) is called by the kernel whenever the system call
 * 	Wait(s) is called.  Return 0 if successful, else -1 if failed. 
 */

int MyWait(int s)
	// s: semaphore ID
{
  if(s>(MAXSEMS-1)||(s<0)){
    return -1; 
  }
  int i;
	semtab[s].value--;
  //DPrintf("sem value is %d\n", semtab[s].value);
  if(semtab[s].value < 0){
    for(i=0; i<MAXPROCS; i++){
      if(procs[s*MAXPROCS+i] == -1){
        break;
      }
    }
    if(i==MAXPROCS){
      return -1;
    }
    else{
      procs[s*MAXPROCS+i] = GetCurProc();
      DPrintf("proc %d blocked at %d\n", procs[s*MAXPROCS+i],s*MAXPROCS+i);
    } 
    Block();
  }
	return (0);
}

/*  	MySignal(s) is called by the kernel whenever the system call
 * 	Signal(s) is called.  Return 0 if successful, else -1 if failed. 
 */

int MySignal(int s)
	// s: semaphore ID
{
  semtab[s].value++;
  if(procs[s*MAXPROCS] != -1){
    Unblock(procs[s*MAXPROCS]);
    //DPrintf("proc %d unblocked at %d\n", procs[s*MAXPROCS], s*MAXPROCS);
    for(int i=0; i<MAXPROCS-1; i++){
      procs[s*MAXPROCS+i] = procs[s*MAXPROCS+i+1];
    }
    procs[s*MAXPROCS+MAXPROCS-1] = -1;
  }
  
  //DPrintf("one proc released, procs list is now %d, %d, %d\n", procs[0], procs[1], procs[2]);
	return (0);
}
