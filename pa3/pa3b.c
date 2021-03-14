/* Programming Assignment 3: Exercise B
 *
 * One way to solve the "race condition" causing the cars to crash is to add
 * synchronization directives that cause cars to wait for others.  In this
 * assignment, we will use semaphores.  The kernel supports a large number
 * of semaphores (defined by MAXSEMS in sys.h, currently set to 100), and
 * each semaphore is identified by an integer 0 - 99 (MAXSEMS-1).  
 *
 * Semaphore operations are:
 *
 *	int s = Seminit(int v)
 *
 *		Allocates a semaphore and initializes its value to v.
 *		Returns a unique identifier s of the semaphore, which is
 *		then used to refer to the semaphore in Wait and Signal
 *		operations.  Returns -1 if unsuccessful (e.g., if there
 *		are no available semaphores).
 *
 *	int Wait(int s)
 *
 *		Semaphore wait operation.
 *		Returns -1 if unsuccessful, else 0 (success).
 *
 * 	int Signal(int s)
 *
 *		Semaphore signal operation.
 *		Returns -1 if unsuccessful, else 0 (success).
 *
 * The above are system calls that can be called by user processes.  These
 * procedures cause a trap into the kernel, and each calls a corresponding
 * procedure located in mycode3.c:
 *
 * 	int s = MySeminit(int v)
 *
 *	int MyWait(int s)
 *
 *	int MySignal(int s)
 *
 * Given these interfaces, you are to implement these routines.
 *
 * One additional note about semaphores in UMIX:  Once a semaphore is created by
 * a process, that semaphore is available for use by all processes.  So, even a
 * process that did not create the semaphore may use it by calling Wait(s) and
 * Signal(s), where s is the semaphore identifier.  This is because semaphores
 * are implemented in the kernel, and thus are available to (shared by) all
 * processes.  A separate question is: How do all the processes that are to use
 * a semaphore learn what its integer identifer is (after all, only one process
 * created the semaphore, and so the identifier is initially known only to that
 * process).  The solution is to place the variable that stores the identifier
 * into shared memory (to be discussed in Part C).  
 *
 * Exercises
 *
 * 1. Study the program below.  Process 1 (Car 1) allocates a semaphore,
 * storing its ID in sem, and initializes its value to 0.  It then creates
 * process 2 (Car 2) which immediately executes Wait(sem).  What should
 * happen to process 2 given that sem is initialized to 0?  After driving
 * over the road, process 1 executes Signal(sem).  What should happen to
 * process 2?
 *
 * 2. Study the file mycode3.c.  It contains a skeletal data structure and
 * code for the semaphore operations.  Notice how MySeminit finds a free 
 * entry in the semaphore table, allocates it, initializes it, and uses
 * the index as the semaphore ID that is returned.  The other routines,
 * MyWait and MySignal have minimal bodies that decrement and increment
 * the semaphore value, but have no effect on synchronization.  To
 * implement synchronization, you need two utility kernel functions
 * that are provided to you:
 *
 * 	Block() causes the current process to block.  It basically removes
 *	the current process from being eligible for scheduling, and causes
 * 	a context switch to another process that is eligible.
 *
 *	Unblock(int p) causes process p to be eligible for scheduling.
 *	This does not mean p will execute immediately, but only that
 *	when a scheduling decision is made, p may be selected.
 *
 * Given these utility routines, implement the semaphore routines.
 *
 * NOTE: The kernel already enforces atomicity of MySignal and MyWait, 
 * so you do NOT need implement any additional mechansims for atomicity.
 *
 * 3. Run the program below.  It should now cause Car 2 to wait for Car 1
 * before driving over the road, thus avoiding a crash.
 *
 */

#include <stdio.h>
#include "aux.h"
#include "sys.h"
#include "umix.h"
struct{
  int m;
}se;

void driveRoad(int from, int mph);

void Main()
{
	int sem;
  Regshm((char *)&se, sizeof(se)); 
	sem = Seminit(0);
  se.m = Seminit(1);
  DPrintf("check sem = %d, se.m = %d\n", sem, se.m);
	if (Fork() == 0) {
		Wait(sem);			// car 2
		Delay(0);
		driveRoad(EAST, 60);
		Exit();
	}
  if (Fork() == 0) {		
    Delay(0);
		driveRoad(EAST, 60);
		Exit();
	}
	driveRoad(WEST, 40);			// car 1
	Signal(sem);

	Exit();
}

#define IPOS(FROM)	(((FROM) == WEST) ? 1 : NUMPOS)

void driveRoad(int from, int mph)
	// from: coming from which direction
	// mph: speed of car
{
	int p;
  Wait(se.m);
	EnterRoad(from);
  
	for (p = 1; p <= NUMPOS; p++) {

		Delay(3600/mph);		// MUST NOT MODIFY!

		ProceedRoad();
	}
  Signal(se.m);
}
