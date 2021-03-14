/* mycode2.c: your portion of the kernel
 *
 *   	Below are functions that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these functions.  You may
 *  	modify the bodies of these functions (and add code outside of them)
 * 	in any way you wish (however, you must not change their interfaces). 
 */

#include "aux.h"
#include "sys.h"
#include "mycode2.h"

#define TIMERINTERVAL 1	// in ticks (tick = 10 msec)

/*  	A sample process table.  You may change this any way you wish.  
 */

static struct {
	int valid;		// is this entry valid: 0 = no, 1 = yes 
	int pid;		// process ID (as provided by kernel)
} proctab[MAXPROCS];

//FIFO variables
int fif[MAXPROCS];
int fifi;
int fifcur;

//LIFO variables
int lif[MAXPROCS];
int lifi;
int lifcur;
int lifend;

//ROUNDROBIN variables
int rr[MAXPROCS];
int rrcur;

//PROPORTIONAL
static struct{
  int pass;
  int rate;
  int pid;
  int stride;
}pp[MAXPROCS];

/*  	InitSched() is called when the kernel starts up.  First, set the
 *  	scheduling policy (see sys.h). Make sure you follow the rules below
 *   	on where and how to set it.  Next, initialize all your data structures
 * 	(such as the process table). Finally, set the timer to interrupt
 *  	after a specified number of ticks.  
 */

void InitSched()
{
	int i;

	/* First, set the scheduling policy.  You should only set it from
	 * within this conditional statement.  While you are working on
	 * this assignment, GetSchedPolicy() will return NOSCHEDPOLICY. 
	 * Thus, the condition will be true and you may set the scheduling
	 * policy to whatever you choose (i.e., you may replace ARBITRARY).  
	 * After the assignment is over, during the testing phase, we will
	 * have GetSchedPolicy() return the policy we wish to test (and
	 * the policy WILL NOT CHANGE during the entirety of a test). Thus
	 * the condition will be false and SetSchedPolicy(p) will not be
	 * called, thus leaving the policy to whatever we chose to test
	 * (and so it is important that you NOT put any critical code in
	 * the body of the conditional statement, as it will not execute
	 * when we test your program).  
	 */
	if (GetSchedPolicy() == NOSCHEDPOLICY) {	// leave as is
							// no other code here
		SetSchedPolicy(PROPORTIONAL);		// set the policy here
							// no other code here
	}
		

	for (i = 0; i < MAXPROCS; i++) {
    //FIFO
    fif[i] = 0;
    //LIFO
    lif[i] = 0;
    //ARBITRARY
		proctab[i].valid = 0;
    //ROUNDROBIN
    rr[i] = 0;
    //PROPORTIONAL
    pp[i].pass = 0;
    pp[i].pid = 0;
    pp[i].rate = 0;
    pp[i].stride = 0;
	}
  //FIFO
  fifi = 0;
  fifcur = -1;

  //LIFO
  lifi = 0;
  lifcur = -1;
  lifend = 0;

  //ROUNDROBIN
  rrcur = -1;

  //PROPORTIONAL

	/* Set the timer last */
	SetTimer(TIMERINTERVAL);
}


/* 	StartingProc(p) is called by the kernel when the process
 *  	identified by PID p is starting.  This allows you to record the
 *  	arrival of a new process in the process table, and allocate any
 *  	resources (if necessary).  Returns 1 if successful, 0 otherwise. 
 */

int StartingProc(int p) 		
	// p: process that is starting
{
  int i;
  switch(GetSchedPolicy()) {

  //ARBITRARY
  case ARBITRARY:
    for (i = 0; i < MAXPROCS; i++) {
	  	if (! proctab[i].valid) {
		  	proctab[i].valid = 1;
		  	proctab[i].pid = p;
		  	return (1);
	    }
    }
  break;

  //FIFO
  case FIFO:
    //search for a position to insert
    for (i = 0; i<MAXPROCS; i++){
      //position found
      if(fif[fifi] == 0){
        fif[fifi] = p;
        fifi++;
        if(fifi == MAXPROCS){
          fifi = 0;
        }
        return(1);
      }
      //position occupied, look next
      fifi++;
      if(fifi == MAXPROCS){
        fifi = 0;
      }
    }
    //list full
  break;

  //LIFO
  case LIFO:
    //search for a position to insert
    if(lif[lifi] == 0){
      lif[lifi] = p;
      lifi++;
      lifend = 0;
      DoSched();
      return(1);
    }
  break;

  //ROUNDROBIN
  case ROUNDROBIN:
    for (i = 0; i<MAXPROCS; i++){
      if(rr[i] == 0){
        rr[i] = p;
        return(1);
      }
    }
  break;

  //PROPORTIONAL
  case PROPORTIONAL:
    for (i = 0; i<MAXPROCS; i++){
      if(pp[i].pid == 0){
        DPrintf("starting process %d\n", p);
        pp[i].pid = p;
        return(1);
      }
    }
  break;
  }

	DPrintf("Error in StartingProc: no free table entries\n");
	return(0);
}
			

/*   	EndingProc(p) is called by the kernel when the process
 * 	identified by PID p is ending.  This allows you to update the
 *  	process table accordingly, and deallocate any resources (if
 * 	necessary). Returns 1 if successful, 0 otherwise.  
 */


int EndingProc(int p)
	// p: process that is ending
{
  int i;
  switch(GetSchedPolicy()) {
  //ARBITRARY
  case ARBITRARY:

	  for (i = 0; i < MAXPROCS; i++) {
		  if (proctab[i].valid && proctab[i].pid == p) {
			  proctab[i].valid = 0;
			  return(1);
		  }
	  }
  break;
  
  //FIFO
  case FIFO:
    for(i=0; i<MAXPROCS; i++){
      if(fif[i] == p){
        fif[i] = 0;
        fifcur = i;
        return(1);
      }
    }
  break;

  //LIFO
  case LIFO:
    for(i=0; i<MAXPROCS; i++){
      if(lif[i] == p){
        lif[i] = 0;
        lifcur = i;
        lifi = lifcur;
        lifend = 1;
        return(1);
      }
    }

  //ROUNDROBIN
  case ROUNDROBIN:
    for(i=0; i<MAXPROCS; i++){
      if(rr[i] == p){
        rr[i] = 0;
        return(1);
      }
    }
  break;

  //PROPORTIONAL
  case PROPORTIONAL:
    for(i=0; i<MAXPROCS; i++){
      if(pp[i].pid == p){
        MyRequestCPUrate(p, -1);
        return(1);
      }
    }
  break;
  }

	DPrintf("Error in EndingProc: can't find process %d\n", p);
	return(0);
}


/*  	SchedProc() is called by the kernel when it needs a decision for
 *  	which process to run next.  It will call the kernel function
 *  	GetSchedPolicy() which will return the current scheduling policy
 *   	which was previously set via SetSchedPolicy(policy).  SchedProc()
 * 	should return a process PID, or 0 if there are no processes to run. 
 */

int SchedProc()
{
	int i;
  int least;

	switch(GetSchedPolicy()) {

  //ARBITRARY
	case ARBITRARY:
		for (i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid) {
				return(proctab[i].pid);
			}
		}
	break;

  //FIFO
	case FIFO:
    if(fifcur == -1){
      return fif[0];
    }
    if(fifcur == 9){
      return(fif[0]);
    }
    return(fif[fifcur+1]);
	break;

  //LIFO
	case LIFO:
    if(lifcur == -1){
      lifcur = 0;
      return lif[0];
    }
    if(lifend == 1){
      lifcur--;
      return(lif[lifcur]);
    }
    else if(lifend == 0){
      lifcur ++;
      return(lif[lifcur]);
    }
	break;

  //ROUNDROBIN
	case ROUNDROBIN:
    for(i = 0; i<MAXPROCS; i++){
      rrcur++;
      if(rrcur == 10){
        rrcur = 0;
      }
      if(rr[rrcur] != 0){
        SetTimer(TIMERINTERVAL);
        return(rr[rrcur]);
      }
    }
	break;

  //PROPORTIONA
  case PROPORTIONAL:
    least = -1;
    for(i = 0; i<MAXPROCS; i++){
      if(pp[i].pid){
        if(least == -1){
          least = pp[i].pass;
        }
        if((pp[i].pass < least)&&(pp[i].stride)){
          least = pp[i].pass;
        }
      }
    }
    /*for(int j=0; j<MAXPROCS; j++){
      if(pp[j].pid){
        DPrintf("least is %d from process %d\n", pp[j].pass, pp[j].pid);
      }
    }*/
    for(i = 0; i<MAXPROCS; i++){
      if((pp[i].pass == least)&&(pp[i].pid)){
        pp[i].pass += pp[i].stride; 
        SetTimer(TIMERINTERVAL);
       // DPrintf("schedule to process %d\n", pp[i].pid);
        return(pp[i].pid);
      }
    }
	break;

	}
	
	return(0);
}


/*  	HandleTimerIntr() is called by the kernel whenever a timer
 * 	interrupt occurs.  Timer interrupts should occur on a fixed
 *  	periodic basis.
 */

void HandleTimerIntr()
{
	SetTimer(TIMERINTERVAL);

	switch(GetSchedPolicy()) {	// is the policy preemptive?

	case ROUNDROBIN:		// ROUNDROBIN is preemptive
	case PROPORTIONAL:		// PROPORTIONAL is preemptive

		DoSched();		// make a scheduling decision
		break;

	default:			// if non-preemptive, do nothing
		break;
	}
}

/*  	MyRequestCPUrate(p, n) is called by the kernel whenever a process
 *  	identified by PID p calls RequestCPUrate(n). This is a request for
 *   	n% of CPU time if n > 0, i.e., roughly n out of every 100 quantums
 * 	should be allocated to the calling process, or to run at no fixed
 *  	allocation (the default) if n = 0.  MyRequestCPUrate(p, n) should
 * 	return 0 if successful, i.e., if such a request can be satisfied,
 *  	otherwise it should return -1, i.e., error (including if n < 0
 *  	or n > 100).  If MyRequestCPUrate(p, n) fails, it should have no
 *  	effect on the scheduling of this or any other process, i.e., AS
 *   	IF IT WERE NEVER CALLED.  
 */

int MyRequestCPUrate(int p, int n)
	// p: process whose rate to change
	// n: percent of CPU time
{
  //invalid rate n<-1
  if(n < -1){
    return(-1);
  }
  
  //invalid rate n>100
  if(n>100){
    return(-1);
  }
	int i;
  for(i=0; i<MAXPROCS; i++){
    if(n != -1){
      if(pp[i].pid == p){
        int sum=0;
        for(int j=0; j<MAXPROCS; j++){
          if(pp[j].pid){
            sum+=pp[j].rate;
          }
        }
        sum += n;
        
        //over-allocate
        if(sum > 100){
          return(-1);
        }

        //assign rate and stride
        pp[i].rate = n;
        if(n){
          pp[i].stride = 1000/n;
        }
        else{
          pp[i].stride = 1000;
        }

        //count no request processes
        int count=0;
        for(int j=0; j<MAXPROCS; j++){
          if(pp[j].pid){
            //count # of processes whose rate is 0 (stride is 1000)
            if(pp[j].rate == 0){
              count++;
            } 
          }
        }
        
        //if all processes have request, evenly allocate the remained cpu
        if(count == 0){
          for(int j=0; j<MAXPROCS; j++){
            if(pp[j].pid){
              count++;
            }
          }
          int remain = 100-sum;
          remain = remain/count;
          for(int j=0; j<MAXPROCS; j++){
            if(pp[j].pid){
              pp[j].stride = 1000/(pp[j].rate+remain);
            }
          }
        }
        
        //if no request processes, evenly allocate the remained cpu
        else{
          int remain = 100-sum;
          remain = remain/count;
          for(int j=0; j<MAXPROCS; j++){
            if(pp[j].pid && (pp[j].rate == 0)){
              if(remain == 0){
                pp[j].stride = 0;
                if(sum < 100){
                  pp[j].stride = 1000;
                }
              }
              else{
                pp[j].stride = 1000/remain;
              }
            }
          }
        }
        for(int j=0; j<MAXPROCS; j++){
          if( pp[j].pid){
            DPrintf("%d has stride rate %d and stride %d\n", pp[j].pid, pp[j].rate, pp[j].stride);
          }
        }
        return(0);
      }
    }
    else{
      if(pp[i].pid == p){
        DPrintf("process %d quit\n", pp[i].pid);
        //reset quitting process
        pp[i].rate = 0;
        pp[i].stride = 0;
        pp[i].pid = 0;
        pp[i].pass = 0;

        //calculate the sum of remained processes
        int sum=0;
        for(int j=0; j<MAXPROCS; j++){
          if(pp[j].pid){
            sum+=pp[j].rate;
          }
        }
        
        //count no request processes
        int count=0;
        for(int j=0; j<MAXPROCS; j++){
          if(pp[j].pid){
            //count # of processes whose rate is 0 (stride is 1000)
            if(pp[j].rate == 0){
              count++;
            } 
          }
        }
        DPrintf("%d processes are no request\n", count);        
        //if all processes have request, evenly allocate the remained cpu
        if(count == 0){
          for(int j=0; j<MAXPROCS; j++){
            if(pp[j].pid){
              count++;
            }
          }
          if(count == 0){
            return(0);
          }
          int remain = 100-sum;
          remain = remain/count;
          for(int j=0; j<MAXPROCS; j++){
            if(pp[j].pid){
              pp[j].stride = 1000/(pp[j].rate+remain);
            }
          }
        }
        
        //if no request processes, evenly allocate the remained cpu
        else{
          int remain = 100-sum;
          remain = remain/count;
          for(int j=0; j<MAXPROCS; j++){
            if(pp[j].pid && (pp[j].rate == 0)){
              if(remain == 0){
                pp[j].stride = 0;
                if(sum < 100){
                  pp[j].stride = 1000;
                }
              }
              else{
                pp[j].stride = 1000/remain;
              }
            }
          }
        }
        
        for(int j=0; j<MAXPROCS; j++){
          if( pp[j].pid){
            DPrintf("%d has stride rate %d and stride %d\n", pp[j].pid, pp[j].rate, pp[j].stride);
          }
        }
        return(0);

      }
    }
  }
	return(-1);
}
