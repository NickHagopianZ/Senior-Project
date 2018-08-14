/*
reads from config.txt
also holds all non-nurse specific constraints (hospital constraints)

*/

#ifndef Hospital_h
#define Hospital_h

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class Hospital{
private:
  // stores a schedule for further analyzation
  int * workSchedule;
  //all ints to store
  int inRow, maxDay, minDay, maxShift,minShift,numDays,numShifts,numNurses;
  //the score for the hospital based on overstaffing
  int hosScore;
  //flags for whether a constraint is toggled on or off
  bool consec;
  bool minmax_staffing;
  bool minmax_staffing_soft;
  bool work_enough;
  bool consecShift;
  bool forScore;
public:
  //read from file
  //inRow = max days allowed to work in a row
  //maxDay = max days allowed to work per month
  //minDay = min days allowed to work per month
  //maxShift = max number of nurses per shift
  //minShift = min number of nurses per shift

  //returns ints
  int ret_inRow() {return inRow;}
  int ret_maxDay() {return maxDay;}
  int ret_minDay() {return minDay;}
  int ret_maxShift() {return maxShift;}
  int ret_minShift() {return minShift;}
  int ret_numDays() {return numDays;}
  int ret_numShifts() {return numShifts;}
  int ret_numNurses() {return numNurses;}
  //returns stored schedule
  //int* ret_schedule() {return workSchedule;}

  //Reads in the constraints from config.txt
  Hospital();
  ~Hospital();
  //checks the validity of an inputted schedule
  //returns the day and shift that something infeasible happens
  int checkSchedule();
  //returns the point at which a schedule is not valid
  int scheduleProblem(int *);

  //returns hospital score
  int hospital_score();

  //updates the schedule to be checked
  //void replaceSchedule (int * copy);
  void putAt(int ID, int at);
  //set check for contraints
  //1. consec
  //2. minmax_staffing
  //3. minmax_staffing_soft
  //4. consecShift
  //5. work_enough
  //to state
  void setFlag(int, bool state);
};

#endif
