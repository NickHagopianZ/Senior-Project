/*
  Add global variables number of nurses per shift min and max



*/

#ifndef Schedule_H
#define Schedule_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>

class Schedule{
private:
  std::vector <int> sched;
  //inputted score
  float score;

  //stores max days, shifts, maxshift size
  int days;
  int shifts;
  int maxShifts;
public:
  Schedule();

  ~Schedule();
  void loadDim(int);



  void put(int ID, int x,int y, int z);
  void putAt(int ID, int at);


  //get ID from
  int get(int x,int y, int z);
  int getAt(int at);

  //convert at to
  int getDate(int at);
  int getShift(int at);
  int getShiftPos(int at);

  float getScore();
  void setScore(float scoreGiven);



};

#endif
