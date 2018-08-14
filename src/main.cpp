/*main






*/


#include <iostream>
#include <stdlib.h>
#include <string>
#include "Nurse.h"
#include "Optimize_Schedule.h"
#include "Schedule.h"

using namespace std;

Nurse *inst1;
Optimize_Schedule *inst2;

// array of nursesPerDay, nurseSoft, nurseHard, daysInRow, minmax_staffing
int constraintStatus[] = {1,1,1,1,0};
int debug = 0;
string strings[] = {"false.","true."};
bool emptySchedule = true;
int restTime = 0;

void toggleConstraints(){
  int constraint=-1, state=-1;
  while (constraint < 0 | constraint >6){
    printf("Choose a constraint to change/toggle\n");
    cout << "6) Minimum rest time between shifts (Hard constraint). Currently " << restTime << endl;
    cout << "5) Nurses must work each specific shifts a minimum and maximum number of times (Hard constraint). Currently " << strings[constraintStatus[4]] << endl;
    cout << "4) Minimum/maximum days a nurse can work (Hard constraint). Currently "<< strings[constraintStatus[3]] << endl;
    cout << "3) Maximum days working in a row (Hard constraint). Currently "<< strings[constraintStatus[2]] << endl;
    cout << "2) Nurse requests (Hard constraints). Currently "<< strings[constraintStatus[1]] << endl;
    cout << "1) Nurse requests (Soft constraints). Currently "<< strings[constraintStatus[0]] << endl;
    printf( "0) Back\n");
    scanf("%d", &constraint);
  }
  if (constraint == 6){
    while (state < 0 ){
      cout << "Enter minimum number of unworked shifts between working shifts (rest time)" << endl;
      scanf("%d", &state);
    }
    restTime = state;
    inst2->restTimeSet(state);
  }
  else if (constraint == 5){
    int storeToFlip = constraintStatus[constraint-1];
    if (storeToFlip == 1) storeToFlip = 0;
    else storeToFlip = 1;
    constraintStatus[constraint-1] = storeToFlip;
    inst2->toggleBR();
  }
  else if (constraint != 0){

      int storeToFlip = constraintStatus[constraint-1];
      if (storeToFlip == 1) storeToFlip = 0;
      else storeToFlip = 1;
      constraintStatus[constraint-1] = storeToFlip;

      inst2->setConstraint(constraint, storeToFlip);
  }
}

void newSchedule(){
  int constraint=-1;
  while (constraint < 1 | constraint >3){
    printf("Choose how to input the days that nurses want to work, do not want to work, and cannot work\n");
    printf("3) Fill empty\n");
    printf("2) Fill randomly\n");
    printf("1) Input for individual nurses\n");

    scanf("%d", &constraint);
    if (constraint >=1 && constraint<=3){
      inst2 = new Optimize_Schedule(constraint);
    }
  }
}



void Constraint_menu(){
  int var=-1;
  while (var < 0 | var >3){
    printf("Choose a constraint to manage.\n");
    printf("2) Change or view a specific nurse's constraints.\n");
    printf("1) Toggle constraints\n");
    printf("0) Quit\n");
    scanf("%d", &var);
  }
  if (var == 2) {
    //allow quit at any time
    //input number or 'scroll' through nurses
    //print nurses menu on scroll
    //modify selected nurs
    //nurse names?
  }
  if (var == 1) toggleConstraints();
}


void GA_menu(bool newSched){
  int sizeOfGeneration = -1;
  int numberOfGenerations = -1;
  float mutatePercent = -1;
  float crossoverPercent = -1;
  float mutateAmount = -1.0;
  float crossoverAmount = -1.0;

  printf("Beginning genetic algorithm setup.\n");
  int var = -1;
  while (var<0 | var > 2){
    printf("How would you like to choose you genetic algorithm's values? \n");
    printf("2) Default (suggested)\n");
    printf("1) Custom\n");
    printf("0) Quit\n");
    scanf("%d", &var);
  }
  if (var == 1){
    while (sizeOfGeneration < 0){
      printf("Choose the population size of each generation. (Suggested 50-200)\n");
      scanf("%d", &sizeOfGeneration);
      while (numberOfGenerations < 0){
        printf("Choose the number of generations. (Suggested 50-200)\n");
        scanf("%d", &numberOfGenerations);
        while (mutatePercent < 0 | mutatePercent > 100){
          printf("Choose the percent of mutation introduced into the population. (0-100)\n");
          scanf("%f", &mutatePercent);
          mutateAmount = mutatePercent/100.0;
          while (crossoverPercent < 0 | crossoverPercent > 100){
            printf("Choose the percent of crossover introduced into the population. (0-100)\n");
            scanf("%f", &crossoverPercent);
            crossoverAmount = crossoverPercent/100.0;
          }
        }
      }
    }
  }
  if (var == 2){
    sizeOfGeneration = 75;
    numberOfGenerations = 150;
    mutateAmount = .20;
    crossoverAmount = .5;
  }
  if (var != 0){
    inst2->geneticAlgorithm(newSched, numberOfGenerations, sizeOfGeneration, mutateAmount, crossoverAmount);
  }
}

void heuristic_menu(){
  int var = -1;
  if (emptySchedule) printf("No schedule to improve. The schedule is currently empty.\n");
  else {
    while (var<0 | var>4){
      printf("Choose a heuristic\n");
      printf("4) Genetic Algorithm\n");
      printf("3) 3-Swap\n");
      printf("2) 2-Swap\n");
      printf("1) Attempt to swap to feasible\n");
      printf("0) Back\n");
      scanf("%d", &var);
    }
    if (var>0 && var < 4) {
      inst2->swapForMinimum();
      inst2->swapForMinimumBR();
      inst2->swapToFeasible();
    }
    if (var==2) inst2->swapToImprove();
    else if (var==3) inst2->threeSwapToImprove();
    // populationSize, mutate, crossover
    else if (var==4) GA_menu(true);

    if (var!= 0){
      float a;
      a = inst2->objectiveFunction();
      if (a < 0) printf("\n \n The current schedule is infeasible with a negative score: %f\n \n \n ", a);
      else printf("\n \n The current schedule has a score of %f/100\n \n \n ", 100*a/inst2->retNumNurses());
    }
  }
}


void initialAssign_menu(){
  int a=-1;
  while (a<0 | a>5){
    printf("Choose how to begin assignment\n");
    printf("5) Empty Schedule\n");
    printf("4) Genetic Algorithm\n");
    printf("3) Assign Greedily\n");
    printf("2) Randomly assign nurses\n");
    printf("1) Evenly assign nurses\n");
    printf("0) Back\n");
    scanf("%d", &a);
  }
  if (a != 0) {
    emptySchedule = false;
    inst2->resetSchedule();
  }

  if (a == 1) {
    inst2->evenAssign();
  }
  else if (a == 2) {
    inst2->trueRandomAssign();
  }
  else if (a == 3) {
    inst2->randomAssign();
  }
  else if (a==4) {
    GA_menu(false);
  }
  else if (a==5){
    inst2->resetSchedule();
    emptySchedule = true;
  }
}

void loadSchedule_menu(){
  int input = -1;
  int no_sched = inst2->retNumSchedule();
  inst2 -> scheduleLoaderHelp();
  if (no_sched != 0){
    while (input<1 | input>no_sched){
      scanf("%d", &input);
      if (input > 0 && input <= no_sched){
        inst2->resetSchedule();
        inst2->loadSchedule(input);
      }
    }
  }
}


void display_menu(){
  int a=-1;
  while (a<0 | a>3){
    printf( "Choose display option\n");
    printf( "3) Display score.\n");
    printf( "2) Display schedule. \n");
    cout << "1) Toggle debug text. Currently " << strings[debug] << endl;
    printf( "0) Back\n");
    scanf("%d", &a);
  }
  if (a == 1) {
    if (debug == 1) debug = 0;
    else debug = 1;
    inst2->toggleDebug();
  }
  else if (a == 2) {
    inst2->printSchedule();
  }
  else if (a == 3) {
    inst2->formatObjectiveFunction(-1);
  }
}





int main(int argc, char **argv) {
  int input=-1;

  newSchedule();

  //menu
  while (input != 0){
    printf("\n What function(s) would you like to perform on this schedule \n");
    printf("6) Load Schedule. \n");
    printf("5) Save schedule. \n");
    printf("4) Manage constraints. \n");
    printf("3) Display options. \n");
    printf("2) Improve using an algorithm. \n");
    printf("1) Generate an initial schedule. \n");
    printf("0) None. Exit. \n");
    scanf("%d", &input);
    switch (input) {
      case 0: inst2->~Optimize_Schedule();
        break;
      case 1: initialAssign_menu();
        break;
      case 2: heuristic_menu();
        break;
      case 3: display_menu();
        break;
      case 4: toggleConstraints();
        break;
      case 5: inst2->saveSchedule();
        break;
      case 6: loadSchedule_menu();
        break;
      default:
        break;
    }
  }
  return 0;
}
