#include <stdio.h>
#include <stdlib.h>
#include "Hospital.h"
#include <string>
#include <fstream>

using namespace std;

//set check for contraints
//1. consec
//2. minmax_staffing
//3. minmax_staffing_soft
//4. consecShift
//5. work_enough
//to state
void Hospital::setFlag(int flag, bool state){
  if (flag==3) consec=state;
  else if (flag==1) minmax_staffing=state;
  else if (flag==4) work_enough=state;
  else if (flag==5) consecShift=state;
  else if (flag==2) minmax_staffing_soft=state;


}


//constraints class checks the wants of the hospital
//-meet mininum staffing requirement
//


//returns the value of a given score
int Hospital::hospital_score(){
  forScore=true;
  checkSchedule();
  forScore=false;
  return numNurses/3-hosScore;
}


//currently broken
//loads in a schedule for check schedule and
// void Hospital::replaceSchedule (int * copy){
//   int at;
//   for (int i; i<numDays; i++){ // days per month
//     for (int j; j<numShifts; j++){ //shifts per day
//       for (int k; k<maxShift; k++){ // nurses per shift
//         at = i*numShifts*maxShift+j*maxShift+k;
//         workSchedule[at]=copy[at];
// }}}}

void Hospital::putAt(int ID, int at){
  workSchedule[at]=ID;
}


//checks current schedule held in workSchedule for validity
//generates hosScore
//checks all flags if turned on
int Hospital::checkSchedule(){
  int consecutiveDay[numNurses*2]; //holds number of days worked consecutively and previous day worked
  int daysWorked[numNurses];  //holds number of days worked
  int shiftMembers=0;
  //int consecutiveShift[maxShift];
  for (int i; i<numNurses; i++){//days per month
    consecutiveDay[i]=0;
    consecutiveDay[i*2]=0;
    daysWorked[i]=0;
  }
  int at, nurID;
  for (int i; i<numDays; i++){//days per month
    for (int j; j<numShifts; j++){//shifts per day
      if (minmax_staffing){
        //if there aren't minShift nurses per shift
        if (workSchedule[i*numShifts*maxShift+j*maxShift+minShift]==0){
        if (!forScore) return (i*numShifts*maxShift+j*maxShift+minShift);
        hosScore+=10000;
      }


      }
      for (int k; k<maxShift; k++){//nurses per shift
        //ints for easy access
        at = i*numShifts*maxShift+j*maxShift+k;
        nurID = workSchedule[at];
        //0 is reserved for empty positions
        if (nurID>0){
          nurID--;

          if (minmax_staffing_soft){
            if (k>minShift) hosScore++;
          }

          if (consecShift){ //Disallows doubles
            if (consecutiveDay[nurID*2+1]==j){ //if last day worked was today
              if (!forScore) return at;
              hosScore+=10000;
            }
          }

          if (consec){
            //if last day worked was today or yesterday
            if (consecutiveDay[nurID*2+1]==j | consecutiveDay[nurID*2+1]==j-1)
              {consecutiveDay[nurID*2]+=1; consecutiveDay[nurID*2+1]=i;}
            else consecutiveDay[nurID*2]=0;
            //if worked more than max consec days
            if (consecutiveDay[nurID*2]>inRow) {
              if (!forScore) return at;
              hosScore+=10000;
            }
          }
        }
      }
    }
  }
  if (work_enough){
    for (int i; i<numNurses; i++){
      //returns negative value on nurse that caused problem
      if (daysWorked[i] < minDay | daysWorked[i] > maxDay) {
        if (!forScore) return -i;
        hosScore+=10000;
      }
    }
  }
  //if successful
  return -numNurses;
}

Hospital::~Hospital(){
  delete [] workSchedule;
  workSchedule = NULL;
}

Hospital::Hospital(){
    //default for flags
    minmax_staffing = true;
    minmax_staffing_soft = true;
    int r;
    //set to false so
    consec = false;
    work_enough = false;
    consecShift = false;
    forScore = false;


    ifstream inFile;
    inFile.open("./config.txt",ifstream::in);

    if (!inFile) {
      printf("Unable to open file config.txt");
      exit(1);   // call system to stop
    }

    //inputs variables from the config file

    string temp;
    inFile >> temp;
    inFile >> temp;
    inRow = atoi(temp.c_str());
    inFile >> temp;
    inFile >> temp;
    maxDay = atoi(temp.c_str());

    inFile >> temp;
    inFile >> temp;
    minDay = atoi(temp.c_str());
    inFile >> temp;
    inFile >> temp;
    maxShift = atoi(temp.c_str());
    inFile >> temp;
    inFile >> temp;
    minShift = atoi(temp.c_str());
    inFile >> temp;
    inFile >> temp;
    numDays = atoi(temp.c_str());
    inFile >> temp;
    inFile >> temp;
    numShifts = atoi(temp.c_str());
    inFile >> temp;
    inFile >> temp;
    numNurses = atoi(temp.c_str());

    inFile.close();


    //calculate nurse range
    int shiftsToFill = numDays * numShifts;
    //1 + deals with truncations of integers
    if (maxDay>0) int minNurses = 1 + shiftsToFill * minShift/maxDay;
    if (minDay>0) int maxNurses = 1 + shiftsToFill * maxShift/minDay;
    // if (numNurses > maxNurses | numNurses < minNurses){
    //   printf("%d nurses not in feasible range %d--%d nurses\n", numNurses, minNurses,maxNurses);
    //   numNurses = (maxNurses+minNurses)/2;
    //   printf("Number of nurses changed to  %d\n", numNurses);
    // }

    workSchedule = new int[numDays*numShifts*maxShift]; //numDays days, numShifts shifts per day, maxShift nurses per shift


}
