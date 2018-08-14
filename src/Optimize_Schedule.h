/*



*/

#ifndef Optimize_Schedule_H
#define Optimize_Schedule_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Nurse.h"
#include "Hospital.h"
#include "Schedule.h"

class Optimize_Schedule{
private:
  Hospital *inst;
  /*
  holds all shifts and nurse IDs in those shifts
  */
  //workSchedule holds the current nurse IDs assigned to work at certain times (shifts+days)
  //the schedule in the workSchedule array is the schedule that may be modified by the Optimize_Schedule class
  int * workSchedule;
  //invalid times is an array that stores the indexes into workschedule where invalid assignments were forced with the intent of being fixed in the future
  int * invalidTimes;
  //invalid is the total number of invalid assignments
  int invalid;
  /*
  vector of all nurses
  IDs stores in workSchedule are 1 greater than the index stored in the vector
  */
  std::vector<Nurse*> allNurses;
  /*
  vector of schedule objects that have been saved and may be loaded
  */
  std::vector<Schedule*> savedSchedules;
  //Number of nurses to schedule
  int numNurses;
  //number of current schedules in vector
  int numSchedules;
  //the most and least number of nurses that can work any given shift
  int maxShift;
  int minShift;
  //number of shifts per day
  int numShifts;
  //number of days in the period that's being scheduled (typically a month: 28-31 days)
  int numDays;
  //maximum/minimum number of days any nurse is allowed to work
  int nurMaxDay;
  int nurMinDay;

  //a flag that toggles debug text/schedule printing
  bool debug;

  bool firstRunBR;
  //variables for genetic algorithms
  //population size of each generation
  int popSize;
  //Two generations must be stored. Always one being a child and the other a parent
  std::vector<Schedule*> GAgenerationA;
  std::vector<Schedule*> GAgenerationB;
  //an array of indices for a GAgeneration ordered by the scores
  int * orderedScores;

  //currentGen marks whetehr generation A or B is the most recent generation
  int currentGen;
public:
  /*
  takes ways to fill each nurses constraints and number of nurses to generate
  */
  Optimize_Schedule(int fill);
  /*
  frees vector allNurses and deletes workSchedule
  */
  ~Optimize_Schedule();
  /*
  takes nurse ID, date and shift to assign nurse to
  assigns in workSchedule and calls nurse assign function
  */
  int assign(int ID, int date, int shift);
  //removes nurse ID from schedule
  //calls nurse unassign function
  void unassign(int ID, int date, int shift);
  /*
  Prints out the schedule
  */
  void printSchedule();
  /*
  Different types of initial assignments (greedy and fully random assignments)
  */
  void smartAssign();
  void randomAssign();
  void evenAssign();
  void trueRandomAssign();
  //returns the scores of all the nurses
  float objectiveFunction();
  void formatObjectiveFunction(int);
  //swap functions made with only the intent of making an invalid schedule valid
  void swapToFeasible();
  void swapForMinimum();
  void swapForMinimumBR();
  //swap function used for improvement
  //2-swap (swaps 2 nurses and tests to see if score is improved)
  void swapToImprove();
  //3-swap (swaps 3 nurses and tests to see if score is improved)
  void threeSwapToImprove();
  //updates invalidTimes array and invalid counter
  void updateInvalid();
  //sets constraints to be ignore or included in scoring (tells nurses and hospital)
  void setConstraint(int constraint, bool state);
  //toggle debug variable
  void toggleDebug();
  //empties workSchedule and tells all nurses to unassign all days
  void resetSchedule();

  void restTimeSet(int timeTo);

  //used to load in and out schedules from savedSchedules vector
  void scheduleLoaderHelp();
  void loadSchedule(int);
  void saveSchedule();
  //returns the number of schedules that are saved currently
  int retNumSchedule();
  //return the number of nurses working
  int retNumNurses(){return numNurses;}

  void toggleBR();

  int calcDay(int at);
  int calcShift(int at);
  int calcShiftPos(int at);
  //returns index into workSchedule 3d array as single integer
  int schedulePosition(int day, int shift, int shift_position);

  /*********************************************************
  Functions used in the genetic algorithm
  *********************************************************/

  void geneticAlgorithm(bool curSchedule, int numGen, int popSize, float mutate, float crossover);
  void GAinitializeGenerations(bool curSchedule, int populationSize);
  void GAputSchedule(int schedulePosition);
  void GAgetSchedule(int schedulePosition);
  int GAquicksortPartition(int arr[], int low, int high);
  void GAquicksort(int arr[], int low, int high);
  void GAcrossover(float crossoverAmount, int parent_1_index, int parent_2_index, int child_location);
  void GAmutate(float mutateAmount, int mutateType, bool validMutatesOnly);
  int weightedRandomParent(int worstIndex, bool firstRun);
  void GAcombineGens(int arr1[], int arr2[]);


  /*********************************************************
  Functions not for users -> used for debugging
  **********************************************************/
  //prints the ocnstraints of a specific nurse
  void cantWork();



  /**********************************************************
  Functions currently incomplete
  **********************************************************/

  void hospitalSaveSchedule();


};

#endif
