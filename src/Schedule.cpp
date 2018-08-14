#include <stdio.h>
#include <stdlib.h>
#include "Schedule.h"


using namespace std;



Schedule::Schedule(){
  score = 0.0;
  days=1;
  shifts=1;
  maxShifts=1;
  sched.reserve(days*shifts*maxShifts);

}

void Schedule::loadDim(int size){
  if (days==1) {days=size;}
  else if (shifts==1) {shifts=size;}
  else if (maxShifts==1) {maxShifts=size;}
  sched.resize(days*shifts*maxShifts,0);
}

void Schedule::put(int ID, int x,int y,int z){
  sched[x*shifts*maxShifts+y*maxShifts+z]=ID;
}

void Schedule::putAt(int ID, int at){
  sched[at]=ID;

}

int Schedule::get(int x,int y,int z){
  return sched[x*shifts*maxShifts + y*maxShifts + z];
}

int Schedule::getAt(int at){
  return sched[at];
}


int Schedule::getDate(int at){
  return at/days*shifts*maxShifts;
}
int Schedule::getShift(int at){
  int day = at/(shifts*maxShifts);
  return (at-day*shifts*maxShifts)/(maxShifts);
}
int Schedule::getShiftPos(int at){
  return at % maxShifts;
}

void Schedule::setScore(float scoreGiven) {score=scoreGiven;}
float Schedule::getScore(){return score;}
