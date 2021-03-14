/* mycode4.c: UMIX thread package
 *
 *   	Below are functions that comprise the UMIX user level thread package. 
 * 	These functions are called by user programs that use threads.  You may
 *  	modify the bodies of these functions (and add code outside of them)
 * 	in any way you wish (however, you must not change their interfaces). 
 */

#include <setjmp.h>
#include <string.h>

#include "aux.h"
#include "umix.h"
#include "mycode4.h"

static int MyInitThreadsCalled = 0;	// 1 if MyInitThreads called else 0

static int curr=0;

static int id=0;

static int create = MAXTHREADS;

static struct thread {			// thread table
	int valid;			// 1 if entry is valid else 0
	jmp_buf env;			// current context
  int seq;
  int exit;
  jmp_buf temp;
  void (*func)();
  int param;
} thread[MAXTHREADS];

static struct jump{
  jmp_buf env;
} jump;


#define STACKSIZE	65536		// maximum size of thread stack

/*  	MyInitThreads() initializes the thread package.  Must be the first
 *  	function called by any user program that uses the thread package.  
 */

void MyInitThreads()
{
	int i;
  //MAXTHREADS = 3;
	if (MyInitThreadsCalled) {		// run just one time
		Printf("MyInitThreads: should be called only once.\n");
		Exit();
	}

	for (i = 0; i < MAXTHREADS; i++) {	// initialize thread table
		thread[i].valid = 0;
    thread[i].seq = MAXTHREADS;
    thread[i].exit = 0;

    if(i > 0){
        char stack[STACKSIZE*i/i];
        if (((int) &stack[STACKSIZE-1]) - ((int) &stack[0]) + 1 != STACKSIZE) {
			    Printf("Stack space reservation failed\n");
			    Exit();
		    }
        //DPrintf("%d : %p - %p\n", i, &stack[0], &stack[STACKSIZE-1]);
    }

    if(setjmp(thread[i].env)){
      
      //DPrintf("run function of %d, %p (%p)\n", i, &func, &param);
      thread[curr].func(thread[curr].param);
      MyExitThread();
    }
    memcpy(thread[i].temp, thread[i].env, sizeof(jmp_buf));
    
	}
  
	thread[0].valid = 1;			// initialize thread 0

	MyInitThreadsCalled = 1;
}

/*  	MyCreateThread(f, p) creates a new thread to execute f(p),
 *   	where f is a function with no return value and p is an
 * 	integer parameter.  The new thread does not begin executing
 *  	until another thread yields to it. 
 */

int MyCreateThread(void (*f)(), int p)
	// f: function to be executed
	// p: integer parameter
{
	if (! MyInitThreadsCalled) {
		Printf("MyCreateThread: must call MyInitThreads first.\n");
		Exit();
	}
  
  int j;
  int idx = 0;
  
  for(j=1; j<=MAXTHREADS; j++){
    if((id+j) >= MAXTHREADS){
      if(!thread[id-MAXTHREADS+j].valid){
        idx = id-MAXTHREADS+j;
        break;
      }
    }
    else{
      if(!thread[id+j].valid){
        idx = id+j;
        break;
      }
    }
  }

  if(j == MAXTHREADS+1){
    return -1;
  }
  j = idx;
  id = idx;
  //DPrintf("id is %d\n", id);
    
  thread[j].func = f;
  thread[j].param = p;
  memcpy(thread[j].env, thread[j].temp, sizeof(jmp_buf));
    
	thread[j].valid = 1;	// mark the entry for the new thread valid
  int num=-1,i;
  for(i = 0; i<MAXTHREADS; i++){
    if(thread[i].seq > num && thread[i].seq != MAXTHREADS){
      num = thread[i].seq;
    }
  }

  if(num == -1){
    thread[j].seq = 0;
  }
  else{
    thread[j].seq = num + 1;
  }

	return(j);		// done, return new thread ID
}

/* 	MyYieldThread(t) causes the running thread, call it T, to yield to
 *  	thread t.  Returns the ID of the thread that yielded to the calling
 *  	thread T or -1 if t is an invalid ID. Example: given two threads
 *  	with IDs 1 and 2, if thread 1 calls MyYieldThread(2), then thread 2
 *   	will resume, and if thread 2 then calls MyYieldThread(1), thread 1
 * 	will resume by returning from its call to MyYieldThread(2), which
 *  	will return the value 2.
 */

int MyYieldThread(int t)
	// t: thread being yielded to
{
	if (! MyInitThreadsCalled) {
		Printf("MyYieldThread: must call MyInitThreads first.\n");
		Exit();
  }

	if (t < 0 || t >= MAXTHREADS) {
		Printf("MyYieldThread: %d is not a valid thread ID\n", t);
		return(-1);
	}

	if (!thread[t].valid) {
		Printf("MyYieldThread: Thread %d does not exist\n", t);
		return(-1);
	}

  //put the thread to the end of the queue
  int num=-1,i;
  for(i = 0; i<MAXTHREADS; i++){
    if(thread[i].seq > num && thread[i].seq != MAXTHREADS){
      num = thread[i].seq;
    }
  }

  if(num == -1){
    thread[curr].seq = 0;
  }
  else{
    thread[curr].seq = num + 1;
  }

  for(int k=0; k<MAXTHREADS; k++){
    if(thread[k].seq != MAXTHREADS && thread[k].seq > thread[t].seq){
      thread[k].seq --;
    }
  }

  //remove the yielded thread from the queue
  thread[t].seq = MAXTHREADS;  

  //jump
  int left = curr;
  int bc; 
  if ((bc = setjmp(thread[curr].env)) == 0) {
    curr = t;
    longjmp(thread[t].env, left+1);
  }
  
  curr = left;

  return bc-1;
}

/* 	MyGetThread() returns ID of currently running thread.  
 */

int MyGetThread()
{
	if (! MyInitThreadsCalled) {
		Printf("MyGetThread: must call MyInitThreads first.\n");
		Exit();
	}
  return curr;
}

/*  	MySchedThread() causes the running thread to simply give up the
 *  	CPU and allow another thread to be scheduled.  Selecting which
 *  	thread to run is determined here.  Note that the same thread may
 *   	be chosen (as will be the case if there are no other threads). 
 */

void MySchedThread()
{
	if (! MyInitThreadsCalled) {
		Printf("MySchedThread: Must call MyInitThreads first.\n");
		Exit();
	}
  int exit = 0;

  //put the thread to the end of the queue
  if(!thread[curr].exit){
    int num= -1,i;
    for(i = 0; i<MAXTHREADS; i++){
      if(thread[i].seq > num && thread[i].seq != MAXTHREADS){
        num = thread[i].seq;
      }
    }

    if(num == -1){
      thread[curr].seq = 1;
    }
    else{
      thread[curr].seq = num + 1;
    }
  }
  else{
    thread[curr].exit = 0;
    exit = 1;
  }
  
  //remove the yielded thread from the queue
  int sched;
  int i;
  for(i = 0; i<MAXTHREADS; i++){
    if(thread[i].seq != MAXTHREADS){
      thread[i].seq --;
      if(thread[i].seq == -1){
        sched = i;
      }
    }
  }
  
  //jump
  int left = curr;
  
  thread[i].seq = MAXTHREADS;
  /*for(int k=0; k<MAXTHREADS; k++){
    DPrintf("%d  ", k);
  }
  DPrintf("\n");
  for(int k=0; k<MAXTHREADS; k++){
    DPrintf("%d ", thread[k].seq);
  }
  DPrintf("\n");*/
  if(setjmp(thread[curr].env) == 0){
    curr = sched;
    longjmp(thread[sched].env, left+1);
  }

  curr = left;

  return;
}

/* 	MyExitThread() causes the currently running thread to exit.  
 */

void MyExitThread()
{
	if (! MyInitThreadsCalled) {
		Printf("MyExitThread: must call MyInitThreads first.\n");
		Exit();
	}
  
  thread[curr].valid = 0;
  thread[curr].seq = MAXTHREADS;

  int i;
  for(i=0; i<MAXTHREADS; i++){
    if(thread[i].valid){
      break;
    }
  }
  if(i == MAXTHREADS){
    //DPrintf("thread %d left, all done\n", curr);
    Exit();
  }
  else{
    thread[curr].exit = 1;
    MySchedThread();
  }
}
