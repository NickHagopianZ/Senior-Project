/*
  The nurses class exists for each nurse and it stores the constraints of the nurse in addition to when the nurse is working

*/

#ifndef Nurse_H
#define Nurse_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class Nurse{
private:
  int defyhard;
  //stores what is considered sufficient rest time from construction
  int restTime;
  /*
  0 if no constraint
  1 if cannot work (hard constraint)
  2 would like not to work (soft constraint)
  3 not ideal time (soft constraint)
  4 would like to work
  */
  int * constraints;

  //0 if not working or 1 if working
  int * shifts;

  // 0 if not working
  // n equal to number of previous days working.
  // n intent is to ensure nurse has time off
  int * days;

  //checks the vectors
  vector<int> min_max_BS;
  vector<int> BS;

  //stores the number of days in the month and number of shifts per day
  int numDays, numShifts;

  //stores total number of shifts working per month
  int assigned;

  int inRow,nurMaxDay,nurMinDay,nurseID;
  //constraint flags
  bool nurseSoft;
  bool nurseHard;
  bool daysInRow;
  bool minmax_staffing;
  bool allow_doubles;
  bool balanced_schedules;

public:
  /*
  Constructor takes an int to decide how to fill nurse schedule with constraints
  0 is randomFill
  1 is inputFill
  */
  Nurse(int fill,int shift,int day,int inaRow, int maxDay, int minDay, int sufRest, int i);
  /*
  Destructor deletes private arrays
  */
  ~Nurse();
  /*
  Checks to ensure nurse hard constraints are met
  */
  bool validShift(int date, int shift);
  /*
  Partially a helper function, partially for use by external classes
  */
  int checkAvailability(int date, int shift);
  /*
  Partially a helper function, partially for use by external classes
  */
  void inputConstraint(int date, int shift, int importance);
  /*
  Prints constraints, mostly used for debugging
  */
  void printTimeConstrained();
  /*
  Prints shifts, mostly used for debugging
  */
  void printSchedule();
  /*
  Fills nurses constraint schedule by means of user input
  */
  void inputFill();
  /*
  Fills a nurses schedule randomly with constraints
  */
  void randomFill();
  /*
  Assigns a nurses shifts if validity of shift is true
  */
  void assign(int date, int shift);
  /*
  unassigns nurse from a given shift and fixes the days array
  */
  bool unassign(int date, int shift);
  /*
  calls unassign on every day and shift of the month in order to return a schedule to empty
  */
  void unassignAll();
  /*
  checks if the nurse is working at a specific times
  */
  int checkWorking(int date, int shift);
  /*
  returns nurses ID
  */
  int return_nurseID();
  /*
  Calculates and returns nurse score
  */
  float getScore();
  /*

  */
  float scoreInvalid();
  /*
  sets constraints given by int to state
  1 = nurseSoft
  2 = nurseHard
  3 = daysInRow
  4 = allow doubles
  5 = minmax staffing
  */
  void toggleConstraintCheck(int, bool state);
  //checks if days workings is in range
  int workInRange();
  //returns assigned value for comparision
  int ret_assigned(){return assigned;}

  int balanced_schedule_shift_check(int shift);
  bool BS_bool();
  bool minimum_bool();
  int balanced_schedule_return(int shift, int minmax);
  void BSinput(int shift, int min, int max);
  void BStoggle();
  void setRestTime(int timeTo);
};

#endif
