#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Nurse.h"
using namespace std;

//helps outside functions check constraints
int Nurse::checkAvailability(int day, int shift){
  return constraints[day*numShifts+shift];
}

//returns percent satisfied 0-1
//if hard constraint is ignored returns -inf (-10000)
float Nurse::getScore(){
  //variables to keep track of score
  float pos = 0.0;
  float req = 0.0;
  //Variable to reduce indexing
  int at;
  //check all days for score
  //printf("getScore %d %d %d %d\n", return_nurseID(), numShifts, numDays, numShifts*numDays);
  for (int i = 0; i < numShifts*numDays; i++){
    //if there is a constraint
    at = constraints[i];

    if (at!=0){

      //add to the number of requests for later division
      if (at==2 | at==3 | at==4){
        req+=1.0;
      }
      //if working
      if (shifts[i]!=0){
        //care about hard constraints if defied
        //printf("true\n");
        //if wanted to work
        if (at==4) pos+=1.0;
      }
      //else not working
      else {
        //if didn't want to work and not
        if (at==2){
          pos+=1.0;
        }
        else if (at==3){
          pos+=1.0;
        }
      }
    }
  }
  float score = 0.0;

  //if flag is on then account for it
  bool flag = false;

  // if (nurseHard) {
  //   score = defyhard*-10000.0; //set to -inf
  //   if (flag) printf("Nurse %d defied %f\n",nurseID,score);
  // }
  score = -10000.0 * scoreInvalid();
  //avoids divide by 0
  if (req==0.0) {
    if (flag) printf("Nurse %d req=0.0\n", nurseID);
    return score; //avoids divide by 0
  }
  else if (nurseSoft) {
    if (flag) printf("Nurse %d nurseSoft %f %f %f\n", nurseID, pos,req, pos/req);
    score += pos/req; //between 0 and 1
  }
  return score;
}

float Nurse::scoreInvalid(){
  bool invdebug  = false;
  float a = 0.0;
  if (minmax_staffing){
    if (workInRange()!=-1){
      //hard constraint violated, -inf from score
      a+=1.0;
      if (invdebug) printf("minmax %f\n", a);
    }
  }
  //counts the last time have not worked
  int rest = 0;
  int at;
  for (int j = 0; j<numShifts; j++){
    if (balanced_schedules){
      if (balanced_schedule_shift_check(j)<0){
        //hard constraint violated, -inf from score
        //printf("not in range %d\n",allNurses[i]->workInRange());
        a+=1.0;
        if (invdebug) printf("balanced %f\n", a);

      }
    }
    for (int i=0; i<numDays; i++){
      at = i*numShifts + j;
      if(nurseHard){
        if (constraints[at] == 1 && shifts[at] == 1){
          a+=1.0;
          if (invdebug) printf("nurhard %f\n", a);

        }
      }
      if(daysInRow && (j == 0)){
        if (days[i] == inRow + 1){
          a+=1;
          if (invdebug) printf("inrow %f\n", a);

        }
      }
      if (restTime>0){
        if (shifts[at] == 0){
          //if not working add to rest
          rest++;

        }
        //if working
        else if (shifts[at] == 1){
          //check rested enough
          if (rest < restTime){
            a+=1.0;
            if (invdebug) printf("rest %f\n", a);

          }
          rest = 0;
        }
      }
    }
  }
  return a;
}


//returns true if a shift is valid
bool Nurse::validShift(int date, int shift){
  int at = date*numShifts+shift;

  if (balanced_schedules){
    if (BS[shift] >= min_max_BS[shift*2 + 1])
    return false;
  }
  //if set to absolutely cannot work
  if (nurseHard){
      if (constraints[at] == 1) {

        //printf("%d %d. Nurse %d requested to not work\n", date,shift,nurseID);
        return false;
      }
  }

  if (minmax_staffing){
    //if max days is already reached
    if (assigned>=nurMaxDay){

      //printf("Nurse has already worked a full schedule\n");

      return false;
    }
  }

  if (daysInRow){
    if (date>0){
      if (days[date-1] >= inRow) {//if previous day was max days in a row allowed to work
        //printf ("worked previous 7 days\n");
        return false;
      }
      int nextDay = date+1;
      while (nextDay<numDays){
        //if in sequence of days and previous sequence of days and date day >inrow
        if (days[nextDay]+days[date-1]+1>inRow){
          //printf("shift here would create %d days in a row\n",inRow);

          return false;
        }
        //break if reached day off
        else if (days[nextDay]==0) nextDay=numDays;
        nextDay++;
      }
    }
    else if (date==0 && numDays>inRow) {
      if (days[inRow]==inRow) {
        //printf("shift here would create %d days in a row\n",inRow);
        return false;
      }
    }
  }
  //sufficient rest time
  if (shifts[at]!=0){
    return false;
  }

  if (restTime>0){

    int count=0;

    for (int i=0;i<=restTime;i++){
      int a = i;

      if ((at-a)>=0){
        count+=shifts[at-a];
        //if (count > 0) printf("%d %d %d %d %d prev\n",nurseID,count,shifts[at-a],date+1,shift);
      }
      if ((at+a) < (numDays*numShifts)){
        count+=shifts[at+a];
        //if (count > 0) printf("%d %d %d %d %d next\n",nurseID,count,shifts[at+a],date+1,shift);
      }
    }
    if (count!=0) {
        //printf("%d restTime 3 ?\n", nurseID);

        return false;
    }

  }
  //if no constraints have been opposed
  return true;
}

//attempts to assign nurse to a shift while ensuring its validity
void Nurse::assign(int date, int shift){
  int at;
  //can assign if not working
  if (date<numDays && shift<numShifts){
    at = date*numShifts+shift;
    //if not already working
    if (shifts[at]==0){
      //set shift as working
      shifts[at] = 1;
      BS[shift] = BS[shift] + 1;
      assigned++;



      //days keeps an array of each day and shift with the numbers stored as the number of previous days worked
      if (date==0) days[date]=1;
      else days[date]=days[date-1] + 1;

      int nextDay=date+1;
      while (nextDay<numDays){
        //if working the next day
        if (days[nextDay]!=0) days[nextDay] += days[date];
        else {
          nextDay = numDays;
        }
        nextDay++;
      }
    }
    //else printf("failed assign 2\n");
  }
  //else printf("failed assign 1\n");

}

void Nurse::unassignAll(){
  assigned=0;
  //defyhard=0;
  for (int i=0;i<numDays;i++){
    for (int j=0;j<numShifts;j++){
      BS[j] = 0;
      for (int i=0; i<numDays*numShifts; i++) {
        shifts[i] = 0;
        //constraints[i]= 0;
        days[(i % numDays)] = 0;
      }
    }
  }
}



//unassigns nurses from a shift and adjusts all variables accordingly
bool Nurse::unassign(int date, int shift){
  int at;
  if (date<numDays && shift<numShifts){
    at = date*numShifts+shift;
    if (shifts[at]!=0){
      shifts[at]=0;

      BS[shift] = BS[shift] - 1;

      assigned--;

      days[date]=0;
      int a=-1;
      int i=1;

      while(a!=0){ //iterates until day off
        if (date + i < numDays){
          a=days[(date+i)];
          if (a>i) days[date+i]=i;
          i++;
        }
        else a=0;
      }
      return true;
    }
    else {
      //printf("failed unassign 2\n");
      return false;
    }
  }
  else {
    //printf("failed unassign 1\n");
    return false;
  }

}

//prints shifts assigned for any individual nurse. Mostly useful for debugging
void Nurse::printSchedule(){
  printf("\nNurse %d schedule:\n",nurseID);
  //size is the number of days shown across
  //calculated so same or less than width as 3 shift print schedule from Optimize_Schedule class
  //for the sake of keeping formatting the same
  for (int i=0; i<numDays; i++){
    printf("day %d - ",i+1);
    if (i+1<10) printf(" " );
    for (int j=0; j<numShifts; j++) {
      printf("%d ",shifts[i*numShifts+j]);
    }
    printf("| ");
    if (i % 4==3) printf("\n");
  }
  printf("\n \n");
}

//prints constraints for any nurse. Mostly useful for debugging
void Nurse::printTimeConstrained(){
  printf("\nNurse %d constraints schedule:\n",nurseID);
  //size is the number of days shown across
  //calculated so same or less than width as 3 shift print schedule from Optimize_Schedule class
  //for the sake of keeping formatting the same
  for (int i=0; i<numDays; i++){
    printf("day %d - ",i+1);
    if (i+1<10) printf(" " );
    for (int j=0; j<numShifts; j++) {
      printf("%d ",constraints[i*numShifts+j]);
    }
    printf("| ");
    if (i % 4==3) printf("\n");
  }
  printf("\n \n");
}

//inputs constraints at date and shift with a certain importance
void Nurse::inputConstraint(int date, int shift, int importance){
  nurseHard=nurseSoft=true;
  //make for loop
  //for easy indexing
  int at = date*numShifts+shift;
  //printf("%d %d %d %d\n", date,shift,importance, at);
  if (shift == numShifts){
    //for easy indexing
    int allDay=date*numShifts;
    for (int i=0;i<numShifts;i++){
      constraints[allDay+i]=importance;
    }
  }
  else {
    constraints[at]=importance;
  }
}

//randomly fills the constraints
void Nurse::randomFill(){
  //srand(time(NULL));
  int rand_fill, shift, importance;
  for (int i=0; i<numDays; i++) { //the following is a random density of dates requested off
    rand_fill = rand() % 4; //randomly 0-2 resulting in one third of the days having some constraint
    if (rand_fill==1){
      importance = (rand() % 10) +1; //chooses inability to go 1-10
      if (importance >7) {
        importance=4; //Mostly nurses will have days they want to work
      }
      else if (importance>1){
        importance=2;
      }
      shift = (rand() % numShifts+1); //choose which shifts the nurse is busy for 0-numshifts (numshifts being all day)
      inputConstraint(i, shift,importance);
    }
  }
}

//input as many constraints for a nurse as possible
void Nurse::inputFill(){
  int loop=1;
  int date= -1, shift = -1, importance =-1;
  while (true){
    printf("Enter 0 to exit or enter day of the month (out of %d days):\n",numDays);
    scanf("%d", &date);
    if (date == 0) break;
    else if (date<=numDays && date >0){ //loops while date is an invalid int
      printf("\nEnter importance:\n1) Cannot work \n2) Don't want to work\n3) Prefer not to work\n4) Want to work\n");
      scanf("%d", &importance);
      if (importance>0 && importance<=4){//loops while importance is an invalid int
        printf("\nEnter shift numberL \n 1-%d or %d for all day\n Which shift(s) would you prefer not to work: \n",numShifts,numShifts+1);
        scanf("%d", &shift);
        if (shift>0 && shift<=numShifts+1){//loops while shift is an invalid int
          inputConstraint(date-1, shift-1, importance);
        }
      }
    }
  }
}


void Nurse::toggleConstraintCheck(int n, bool state){
  //Acknowledge nurse soft requests
  if (n == 1) {
    nurseSoft=state;
  }
  //Acknowledge nurse hard requests
  else if (n==2){
    nurseHard=state;
  }
  //Acknowledge maximum number of days allowed to work in a row
  else if (n==3){
    daysInRow=state;
  }
  else if (n==4){
    minmax_staffing=state;
  }
  // else if (n==5){
  //   allow_doubles = state;
  // }
}

int Nurse::workInRange(){
  //if assigned is unacceptable return assigned
  if (minmax_staffing){
    if (assigned > nurMaxDay | assigned < nurMinDay){
      return assigned;
    }
  }
  return -1;
}


int Nurse::balanced_schedule_return(int shift, int minmax){
  if (minmax == 1) return min_max_BS[shift*2 +1];
  else return min_max_BS[shift*2];

}

int Nurse::balanced_schedule_shift_check(int shift){
  //if assigned is unacceptable return assigned
  if (balanced_schedules){
    //if less than minimum
    if (BS[shift] < min_max_BS[shift*2]){
      return -2;
    }
    //if greater than maximum
    else if(BS[shift] > min_max_BS[(shift*2) + 1]){
      return -1;
    }
  }
  return BS[shift];
}

bool Nurse::BS_bool(){
  return balanced_schedules;
}

bool Nurse::minimum_bool(){
  return minmax_staffing;
}

void Nurse::BSinput(int shift, int min, int max){
  //maximum input
  min_max_BS[shift*2 +1] = max;
  //minimum input
  min_max_BS[shift*2] = min;
}

void Nurse::BStoggle(){
  balanced_schedules = !balanced_schedules;
}

int Nurse::checkWorking(int date, int shift){
  if (nurseID==3) shifts[17*numShifts+2]=2;
  return shifts[(date*numShifts+shift)];
}

int Nurse::return_nurseID(){
  return nurseID;
}

void Nurse::setRestTime(int timeTo){
  restTime = timeTo;
}

//constructor
Nurse::Nurse(int fill, int shiftsPerDay, int daysPerMonth,int inaRow, int maxDay, int minDay, int rest,int i){
  //defyhard = 0;
  nurseID = i;
  inRow=inaRow;
  nurMaxDay=maxDay;
  nurMinDay=minDay;
  restTime=rest;
  assigned=0;
  numDays = daysPerMonth;
  numShifts = shiftsPerDay;
  //frees space for shifts and constraints
  constraints = new int[numDays*numShifts];
  shifts = new int[numDays*numShifts];
  days = new int[numDays];
  //fills shifts and constraints with default values
  for (int i=0; i<numDays*numShifts; i++) {
    shifts[i] = 0;
    constraints[i]= 0;
    days[(i % numDays)] = 0;
  }
  //default toggled on
  nurseSoft=true;
  nurseHard=true;
  daysInRow=true;
  minmax_staffing=true;
  balanced_schedules=false;


  min_max_BS.reserve(2*shiftsPerDay);
  BS.reserve(shiftsPerDay);


  if (fill==2) randomFill();
  else if (fill==1) inputFill();
  else nurseSoft=nurseHard=false;
}

//destructor
Nurse::~Nurse(){
  delete [] constraints;
  delete [] shifts;
  delete [] days;
  constraints = NULL;
  shifts = NULL;
}
