/* Programming Assignment 3: Exercise D
 *
 * Now that you have a working implementation of semaphores, you can
 * implement a more sophisticated synchronization scheme for the car
 * simulation. 
 *
 * Study the program below.  Car 1 begins driving over the road,  entering
 * from the East at 40 mph. After 900 seconds, both Car 3 and Car 4 try to
 * enter the road.  Car 1 may or may not have exited by this time (it should
 * exit after 900 seconds,  but recall that the times in the simulation are
 * approximate).  If Car 1 has not exited and Car 4 enters,  they will crash
 * (why?).  If Car 1 has exited,  Car 3 and Car 4 will be able to enter the
 * road, but since they enter from opposite directions,  they will eventually
 * crash. Finally, after 1200 seconds,  Car 2 enters the road from the West
 * and traveling twice as fast as Car 4.  If Car 3 was not coming from the
 * opposite direction,  Car 2 would eventually reach Car 4 from the back and
 * crash. You may wish to experiment with reducing the initial delay of
 * Car 2,  or increase the initial delay of Car 3, to cause Car 2 to crash
 * with Car 4 before Car 3 crashes with Car 4.  
 *
 *
 * Exercises
 *
 * 1) Modify the procedure driveRoad such that the following rules are obeyed:
 *
 *   	A) Avoid all collisions.  
 *
 * 	B) Multiple cars should be allowed on the road,  as long as they are
 *  	traveling in the same direction.  
 *
 * 	C) If a car arrives and there are already other cars traveling in the
 *  	SAME DIRECTION, the arriving car should be allowed enter as soon as it
 *  	can. Two situations might prevent this car from entering immediately:
 *  	(1) there is a car immediately in front of it (going in the same
 *   	direction),  and if it enters it will crash (which would break rule A);
 * 	(2) one or more cars have arrived at the other end to travel in the
 *  	opposite direction and are waiting for the current cars on the road
 * 	to exit,  which is covered by the next rule.  
 *
 *  	D) If a car arrives and there are already other cars traveling in the
 *  	OPPOSITE DIRECTION,  the arriving car must wait until all these other
 *  	cars complete their course over the road and exit. It should only wait
 *   	for the cars already on the road to exit; no new cars traveling in the
 * 	same direction as the existing ones should be allowed to enter.  
 *
 *  	E) The previous rule implies that if there are multiple cars at each
 * 	end waiting to enter the road, each side will take turns in allowing
 *  	one car to enter and exit.  However,  if there are no cars waiting at
 *  	one end, then as cars arrive at the other end,  they should be allowed
 *  	to enter the road immediately.  
 *
 *   	F) If the road is free (no cars),  then any car attempting to enter
 * 	should not be prevented from doing so. 
 *
 *  	G) All starvation must be avoided.  For example,  any car that is
 * 	waiting must eventually be allowed to proceed. 
 *
 * This must be achieved ONLY by adding synchronization and making use of
 * shared memory (as described in Exercise C).  You should NOT modify the
 * delays or speeds to solve the problem.  In other words, the delays and
 * speeds are givens,  and your goal is to enforce the above rules by making
 * use of only semaphores and shared memory.  
 *
 * 2) Devise different tests (using different numbers of cars, speeds
 * directions) to see whether your improved implementation of driveRoad
 * obeys the rules above. 
 *
 * IMPLEMENTATION GUIDELINES
 * 
 * 1) Avoid busy waiting.  In class one of the reasons given for using
 * semaphores was to avoid busy waiting in user code and limit it to
 * minimal use in certain parts of the kernel. This is because busy
 * waiting uses up CPU time,  but a blocked process does not.  You have
 * semaphores available to implement the driveRoad function,  so you
 * should not use busy waiting anywhere.  
 *
 * 2) Prevent race conditions.  One reason for using semaphores is to
 * enforce mutual exclusion on critical sections to avoid race conditions. 
 * You will be using shared memory in your driveRoad implementation.  
 * Identify the places in your code where there may be race conditions
 * (the result of a computation on shared memory depends on the order
 * that processes execute). Prevent these race conditions from occurring
 * by using semaphores.  
 *
 * 3) Implement semaphores fully and robustly.  It is possible for your
 * driveRoad function to work with an incorrect implementation of
 * semaphores,  because controlling cars does not exercise every use of
 * semaphores.  You will be penalized if your semaphores are not correctly
 * implemented, even if your driveRoad works. 
 *
 * 4) Control cars with semaphores: Semaphores should be the basic
 * mechanism for enforcing the rules on driving cars.  You should not
 * force cars to delay in other ways inside driveRoad such as by calling
 * the Delay function or changing the speed of a car. (You can leave in
 * the delay that is already there that represents the car's speed,  just
 * don't add any additional delaying).  Also, you should not be making
 * decisions on what cars do using calculations based on car speed (since
 * there are a number of unpredictable factors that can affect the
 * actual cars' progress).  
 *
 * GRADING INFORMATION
 *
 * 1) Semaphores: We will run a number of programs that test your
 * semaphores directly (without using cars at all).  For example:
 * enforcing mututal exclusion,  testing robustness of your list of
 * waiting processes,  calling signal and wait many times to make sure
 * the semaphore state is consistent,  etc. 
 *
 * 2) Cars: We will run some tests of cars arriving in different ways
 * to make sure that you correctly enforce all the rules for cars given
 * in the assignment.  We will use a correct semaphore implementation for
 * these tests so that even if your semaphores are not correct you could
 * still get full credit on the driving part of the grade (you may assume
 * that in our implementation, semaphore unblocking is FIFO,  i.e., for
 * each semaphore,  the order in which processes are unblocked is the
 * same order in which those same processes blocked). Think about
 * how your driveRoad might handle different situations and write your
 * own tests with cars arriving in different ways to make sure you handle
 * all cases correctly.  
 *
 *
 * WHAT TO TURN IN
 *
 * You must turn in two files: mycode3.c and pa3d.c.  mycode3.c should
 * contain you implementation of semaphores,  and pa3d.c should contain
 * your modified version of InitRoad and driveRoad (Main will be ignored).  
 * Note that you may set up your static shared memory struct and other
 * functions as you wish. They should be accessed via InitRoad and driveRoad
 * as those are the functions that we will call to test your code.  
 *
 * Your programs will be tested with various Main programs that will exercise
 * your semaphore implementation,  AND different numbers of cars, directions
 * and speeds,  to exercise your driveRoad function. Our Main programs will
 * first call InitRoad before calling driveRoad.  Make sure you do as much
 * rigorous testing yourself to be sure your implementations are robust.  
 * Good luck.  
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "aux.h"
#include "sys.h"
#include "umix.h"

struct {
  int pos[MAXPROCS];
  int num[MAXPROCS];
  int number;
  int enm;
  int wnm;
  int road;
  int run;
  int turn[MAXPROCS];
  //sem
  int one;
  int two;
  int three;
  int four;
  int five;
  int six;
  int seven;
  int eight;
  int nine;
  int ten;
  int east;
  int west;
  int edir;
  int wdir;
  int eque;
  int wque;
} cars;

void InitRoad(void);
void driveRoad(int from, int mph);

void Main()
{
	InitRoad();

	/* The following code is specific to this simulation, e.g.,  number
	 * of cars,  directions and speeds. You should experiment with
	 * different numbers of cars,  directions and speeds to test your
	 * modification of driveRoad.  When your solution is tested, we
	 * will use different Main procedures,  which will first call
	 * InitRoad before any calls to driveRoad. So, you should do
	 * any initializations in InitRoad.  
	 */
  int from, speed;
  //int i;
  //for(i=0; i<=10; i++){
  //srand(time(NULL));
  while(1){
	if (Fork() == 0) {
	 	//Delay(100);			// car 2
    from = (rand()%2) == 0?WEST:EAST;
    speed = rand()%91 + 30;
		driveRoad(from,speed);
		Exit();
	}
 }
 /*
	if (Fork() == 0) {
		//Delay(100);			// car 3
		driveRoad(WEST, 100);
		Exit();
	}
  //}
  
	if (Fork() == 0) {
		//Delay(150);		// car 4
		driveRoad(EAST, 100);
		Exit();
  }
	if (Fork() == 0) {
		Delay(3000);		// car 5
		driveRoad(WEST, 50);
		Exit();
	}*/
	/*
	if (Fork() == 0) {
		//Delay(400);		// car 6
		driveRoad(WEST, 50);
		Exit();
	}

	if (Fork() == 0) {
		Delay(150);		// car 7
		driveRoad(EAST, 50);
		Exit();
	}*/
	//driveRoad(WEST, 50);			// car 1

	Exit();
}

/* Our tests will call your versions of InitRoad and driveRoad,  so your
 * solution to the car simulation should be limited to modifying the code
 * below.  This is in addition to your implementation of semaphores
 * contained in mycode3.c.  
 */

void InitRoad()
{
  Regshm ((char* )&cars, sizeof(cars));
  
  //init sem
  cars.one = Seminit(1);
  cars.two = Seminit(1);
  cars.three = Seminit(1);
  cars.four = Seminit(1);
  cars.five = Seminit(1);
  cars.six = Seminit(1);
  cars.seven = Seminit(1);
  cars.eight = Seminit(1);
  cars.nine = Seminit(1);
  cars.ten = Seminit(1);

  cars.east = Seminit(1);
  cars.west = Seminit(1);

  cars.edir = Seminit(1);
  cars.wdir = Seminit(1);
  cars.eque = Seminit(0);
  cars.wque = Seminit(0);
 
  //init cars
  for(int i=0; i<MAXPROCS; i++){
    cars.num[i] = -1;
    cars.pos[i] = -1;
    cars.turn[i] = 1;
  }
  cars.number = 1;
  cars.enm = 0;
  cars.wnm = 0;
  cars.road = 0;
  cars.run = -1;
}

#define IPOS(FROM)	(((FROM) == WEST) ? 1 : NUMPOS)

void driveRoad(int from, int mph)
	// from: coming from which direction
	// mph: speed of car
{
  int i;

  //position earned, register car
  for(i=0; i<MAXPROCS; i++){
    if(cars.num[i] == -1){
      cars.num[i] = cars.number;
      cars.number++;
      //DPrintf("car%d with dir%d has speed%d at %d\n", cars.num[i], cars.dir[i], cars.speed[i], i);
      break;
    }
  }

  //if no car on road, wait oppo and enter
  if(from == EAST){
    cars.enm++;
    //DPrintf("%d\n", cars.enm);  
    Wait(cars.east);

    //car goes in east end
    if(!cars.wnm){
      Signal(cars.eque);
      if(i == 0){
        cars.turn[9] = 0;
      }
      else{
        cars.turn[i-1] = 0;
      }
    }
    Wait(cars.eque);

    Wait(cars.wdir);
    Signal(cars.wdir);

    Wait(cars.ten);
  }
  else{
    //car goes in west end
    //DPrintf("about to block car %d on west\n", GetCurProc(), from);
    cars.wnm++;

    Wait(cars.west);
    //DPrintf("before sig enm%d %d\n", cars.enm, cars.run);
    if(!cars.enm){
      Signal(cars.wque);
      if(i==0){
        cars.turn[9] = 0;
      }
      else{
        cars.turn[i-1] = 0;
      }
    }
    Wait(cars.wque);

    Wait(cars.edir);
    Signal(cars.edir);

    Wait(cars.one);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////
	int p;
	EnterRoad(from);

  //tells sem.run dir
  if(from == EAST){
    cars.enm--;
    cars.run = 1;
    cars.pos[i] = 9;
    if(!cars.road){
    //DPrintf("edir - 1 after enter road\n");
      Wait(cars.edir);
    }
    cars.road++;
  
    Wait(cars.nine);
  }
  else{
    cars.wnm--;
    cars.run = 0;
    cars.pos[i] = 0;
    if(!cars.road){
      Wait(cars.wdir);
    }
    cars.road++;
    
    Wait(cars.two);
  }
  
  //DPrintf("%d cars on the road\n", cars.road);
  int crash = 0;

  //-----------------------------------------------------------------------------------------------------------
	for (p = 1; p <= NUMPOS; p++) {
		Delay(3600/mph);		// MUST NOT MODIFY!

		ProceedRoad();
    
    //relocate the car
    if(p < 9){
      if(from == EAST){
        cars.pos[i]--;
        //block sem
        Signal(cars.pos[i]+1);
        Wait(cars.pos[i]-1);
        if(p == 1){
          Signal(cars.east);
        }
      }
      else{
        cars.pos[i]++;
        //block sem
        Signal(cars.pos[i]-1);
        Wait(cars.pos[i]+1);
        if(p == 1){
          Signal(cars.west);
        }
      }
    }
    else if(p == 9){
      if(from == EAST){
        cars.pos[i]--;
        Signal(cars.pos[i]+1);
      }
      else{
        cars.pos[i]++;
        Signal(cars.pos[i]-1);
      }
    }
    else if(p == 10){
      if(from == EAST){
        cars.pos[i]--;
        Signal(cars.pos[i]+1);
      }
      else{
        cars.pos[i]++;
        Signal(cars.pos[i]-1);
      }   
    }
	}
  //------------------------------------------------------------------------------------------------------------


  cars.road--;
  //DPrintf("%d\n",cars.enm); 
  //move the car outside the road
  if(from == EAST){
    //Signal(cars.west);
    if(!cars.road){
      cars.run = -1;
      Signal(cars.edir);
      if(cars.turn[i] == 1){
        Signal(cars.wque);
      }
    }
  }
  else{
    //Signal(cars.east);
    if(!cars.road){
      cars.run = -1;
      Signal(cars.wdir);
      if(cars.turn[i] == 1){
        Signal(cars.eque);
      }
    }
  }

  //clear exited car
  cars.pos[i] = -1;
  cars.num[i] = -1;
  cars.turn[i] = 1;
}
