#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Nurse.h"
#include "Optimize_Schedule.h"
#include "Hospital.h"
#include "Schedule.h"


using namespace std;


Optimize_Schedule::Optimize_Schedule(int input){
  firstRunBR = true;

  srand(time(NULL));
  debug = false;

  inst = new Hospital();
  numSchedules=0;
  //inputs variables from the config file
  //except inRow, maxDay, minDay, maxShift;

  minShift = inst->ret_minShift();
  maxShift = inst->ret_maxShift();
  numDays = inst->ret_numDays();
  numShifts = inst->ret_numShifts();

  numNurses = inst->ret_numNurses();
  nurMaxDay = inst->ret_maxDay();
  nurMinDay = inst->ret_minDay();
  int inRow = inst->ret_inRow();



  workSchedule = new int[numDays*numShifts*maxShift]; //numDays days, numShifts shifts per day, maxShift nurses per shift
  invalidTimes = new int[numDays*numShifts*maxShift]; //numDays days, numShifts shifts per day, maxShift nurses per shift

  //vector<Nurse> obj_vec(numNurses, Nurse(input));
  allNurses.reserve(numNurses);
  Nurse * temp;
  int in = input;
  for (int i=0; i<numNurses; i++) {
    if (input==1){
      in=-1;
      while (in<1 | in> 5){
        printf("Currently at nurse %d of %d\n", i+1, numNurses);
        printf("1) Input constraints\n");
        printf("2) Random constraints\n");
        printf("3) No constraints\n");
        printf("4) Fill remaining randomly\n");
        printf("5) Fill remaining with no constraints\n");
        scanf("%d",&in);
        if (in==4 | in==5){
          input = 2;
          in -= 2;
        }
      }
    }
    temp = new Nurse(in,numShifts,numDays,inRow,nurMaxDay,nurMinDay,0,i);
    allNurses.push_back(temp);
  }




  resetSchedule();
}

void Optimize_Schedule::toggleDebug(){
  debug=!debug;
  printf("Debug set to ");
  if (debug) printf("true.\n");
  else printf("false.\n");
}

Optimize_Schedule::~Optimize_Schedule(){

  for (int i=0; i<numNurses;i++){
     allNurses[i]->~Nurse();
  }
   // for (int i=0; i<numSchedules;i++){
   //   allSchedules[i]->~Schedule();
   // }
  //allNurses.~vector();
  //allSchedules.~vector();
  delete [] workSchedule;
  workSchedule = NULL;
}


//assigns a schedule in an attempt to make it feasible
//if infeasible it stores the invalid times in invalidTimes
void Optimize_Schedule::randomAssign(){
  //srand(time(NULL));
  int a, at, k;// acc=0;
  bool run;
  for (int i=0; i<numDays; i++){ //iterates days to numDays
    for (int j=0; j<numShifts; j++){ //iterates shifts to numShifts
      at=0;
      k=0;
      for (int l=0; l<minShift; l++){ //iterates through minshift requirement. Not max
        //loops until a valid nurse is found
        //if not found then assigns a random nurse
        run=true;
        while (run){
          //printf("Try to assign %d to day %d shift %d \n",k,i,j);

          if (k<numNurses) at=k;
          //if looped through all nurses assign invalid nurses to fill schedule

          if (allNurses[at]->validShift(i,j)){
            a = assign(at, i, j);
            if (a != -1) {run = false;} //break if successful assignment
          }
          else if (k>=numNurses){
            //choose random nurse to assign
            at = rand() % numNurses;

            a = assign(at, i, j);
            if (a != -1) {
              //stores invalid assignment to fix later
              //printf("Invalid = %d. %d/%d/%d Forced random assignment of nurse %d. Since k = %d\n",invalid,i,j,l,at,k);
              invalidTimes[invalid]=schedulePosition(i,j,l);
              invalid++;
              run=false;
            }
          }
          k++;

        }
      }
    }
  }
}

//assigns a schedule in an attempt to make it feasible
//if infeasible it stores the invalid times in invalidTimes
void Optimize_Schedule::evenAssign(){
  //srand(time(NULL));
  bool run;
  int where;
  int prev = numNurses-1;
  int a, at, k;// acc=0;
  for (int i=0; i<numDays; i++){ //iterates days to numDays
    for (int j=0; j<numShifts; j++){ //iterates shifts to numShifts
      for (int l=0; l<minShift; l++){ //iterates through minshift requirement. Not max
        run=true;
        //stores previous nurse put in schedule
        where=prev;
        //loops until a valid nurse is found
        //if not found then assigns a random nurse
        //printf("next\n");
        while (run){
          where++;
          //turns into a loop;
          if (where==numNurses) where=0;
          //printf("Try to assign %d to day %d shift %d \n",k,i,j);

          if (allNurses[where]->validShift(i,j)){
            //printf("did %d %d %d\n", where, i, j);
            a = assign(where, i, j);
            if (a != -1) {prev=where; run = false;} //break if successful assignment
          }
          else if (prev==where){

            int a = 0;
            while (a != -1){
              //choose random nurse to assign
              at = rand() % numNurses;
              //printf("didn't %d %d %d\n", at, i, j);
              a = assign(at, i, j);
            }

            //stores invalid assignment to fix later
            invalidTimes[invalid]=schedulePosition(i,j,l);
            invalid++;
            prev=where;
            run=false;
          }
        }
      }
    }
  }

}

//assigns a schedule in an attempt to make it feasible
//if infeasible it stores the invalid times in invalidTimes
void Optimize_Schedule::trueRandomAssign(){
  //srand(time(NULL));
  int ID;// acc=0;
  int a;
  bool run;
  for (int i=0; i<numDays; i++){ //iterates days to numDays
    for (int j=0; j<numShifts; j++){ //iterates shifts to numShifts
      for (int l=0; l<minShift; l++){ //iterates through minshift requirement. Not max
        run=true;
        int count=0;
        while (run){
          count++;
          ID = rand() % numNurses;

          if (allNurses[ID]->validShift(i,j)){
            a = assign(ID, i, j);

            if (a != -1) {run = false;} //break if successful assignment
          }
          //tries numNurses times for valid schedule, then assigns no matter what
          else if (count>numNurses){

            //choose random nurse to assign
            a = assign(ID, i, j);

            if (a != -1) {
              //stores invalid assignment to fix later
              //printf("Invalid = %d. %d/%d/%d Forced random assignment of nurse %d. Since k = %d\n",invalid,i,j,l,at,k);
              invalidTimes[invalid]=schedulePosition(i,j,l);
              invalid++;
              run=false;
            }
          }
        }
      }
    }
  }
}


//determines the busiest day
//sorts
//assigns available nurses to busiests days first
void Optimize_Schedule::smartAssign(){
  // int dayScore = 0;
  // int worstBestNurse=-1;
  // bestNurse = new int[numNurses*maxShift];
  // for (int i = 0; i<numDays; i++){
  //   for (int j = 0; j<numShifts; j++){
  //     for (int k = 0; k<numNurses; k++){
  //       allNurses[k];
  //     }
  //   }
  // }
}


int Optimize_Schedule::assign(int ID, int date, int shift){
  int a;
  //checks to see if the nurse is already working that shift specifically
  for(int i=0; i<maxShift;i++){

    a = workSchedule[schedulePosition(date,shift,i)];

    if (a == ID+1) {
      return - 1; //unsuccessful
      i=maxShift;//break; never happens
    } //if nurse is not on schedule for today
    else if (a == 0){ //if shift is open

      //printf("tried to assign"); //for debugging
      workSchedule[schedulePosition(date,shift,i)] = ID+1;

      allNurses[ID]->assign(date, shift);

      //i=maxShift;//break;
      return a;
    }
  }
  //if (a == -1) {
    //printf("Could not assign nurse %d to %d/%d\n", ID+1, date+1, shift+1);
  //}
  return -1;
}


void Optimize_Schedule::unassign(int ID, int date, int shift){
  int a, at;
  //printf("try %d %d %d\n",ID, date,shift);
  for (int i=0; i<maxShift; i++){
    at = schedulePosition(date,shift,i);
    a=workSchedule[at];
    //printf("%d %d %d %d %d\n",a, at,date,shift, date*numShifts+shift);
    if (ID==a-1){
      //printf("did %d %d %d\n",ID, date,shift);
      workSchedule[at] = 0;
      allNurses[ID]->unassign(date,shift);

      i=maxShift; //break loop
    }
  }
}

//Similar to swap to improve but only attempts to swap all invalid times
//Useful when avoiding getting stuck at local maximum found with swapToimprove
void Optimize_Schedule::swapToFeasible(){
  updateInvalid();
  int time_out,at,loops,day,shift,ID1,ID2,t;
  bool run;
  time_out=0;
  int maxsize = maxShift*numDays*numShifts;
  if(debug) printf("%d invalid assignments\n", invalid);
  for (int i=0; i<maxsize; i++){
    //store = -1 when valid
    if (invalidTimes[i]==1){
      //calculates day, shift and position in shift (t) for ID1
      day = calcDay(i);
      shift = calcShift(i);
      t = calcShiftPos(i);
      ID1 = workSchedule[i];
      if (ID1>0){
        ID1--;
        unassign(ID1, day,shift);
        //printf("exit check %d %d %d\n",ID1,day,shift);

        run=true; //becomes false when assigned correctly
        for (int j=0;j<numDays && run;j++){
          for (int k=0;k<numShifts && run;k++){
            //if valid shift is here then try and swap
            if (allNurses[ID1]->validShift(j,k)){
              for (int l=0;l<maxShift && run;l++){
                //choose a nurse
                ID2=workSchedule[schedulePosition(j,k,l)];
                if (ID2>0){
                  ID2--;
                  unassign(ID2, j,k);
                  //ask nurse to take shift
                  if (allNurses[ID2]->validShift(day,shift)){
                    //printf("swapped %d with %d on %d %d and %d %d\n",ID1+1,ID2+1,day+1,shift+1,j+1,k+1);
                    assign(ID2, day, shift);
                    assign(ID1, j,k);
                    invalidTimes[i]=-1;
                    invalid--;
                    run=false;
                  }
                  //if the nurse cannot take shift
                  else assign(ID2, j,k);
                }
              }
            }
          }
        }
        //if looped without a fix reassign
        if (run) assign(ID1,day,shift);
      }
    }
  }
}


void Optimize_Schedule::updateInvalid(){
  //stores iterated to
  int at, nurID;
  //stores invalid times
  int day, shift;
  //will be recounted
  invalid=0;
  for (int i=0; i<numDays;i++){
    for (int j=0; j<numShifts;j++){
      for (int k=0; k<maxShift;k++){
        at = schedulePosition(i,j,k);
        //get what's at the invalid time
        nurID=workSchedule[at];
        if (nurID>0){
          nurID--;
          //unassign to ensure validShift is checking correctly
          allNurses[nurID]->unassign(i,j);
          //if it was actually a valid shift
          if (allNurses[nurID]->validShift(i,j)){
            //mark invalidTimes index as okay
            invalidTimes[at]=-1;
          }
          //if not a valid shift
          else{
            invalid++;
            invalidTimes[at]=1;
          }
          //reassigns
          allNurses[nurID]->assign(i,j);
        }
      }
    }
  }
}


void Optimize_Schedule::swapForMinimum(){
  int time_out=2, store, workNo;//nurMaxDay nurMinDay
  //debug=true;
  if(debug) formatObjectiveFunction(-1);
  if(debug) {
    printf("swapping to meet minimum requirement\n");
    for (int i=0;i<numNurses;i++){
      store = allNurses[i]->workInRange();
      if (-1 != store){
        printf("Nurse %d works %d days\n", i, store );
      }
    }
  }
  if (allNurses[0]->minimum_bool()){
    for (int i=0;i<numNurses;i++){
      //if nurse i worked the wrong amount
      store = allNurses[i]->workInRange();
      time_out=2;
      while(store != -1&& time_out!=0){
        time_out--;
        //printf("%d %d\n",i,store);
        //if not working enough
        if (store < nurMinDay){
          for (int b=0; b < numShifts; b++){
            for (int a=0; a < numDays; a++){
              if (assign(i, a, b) != -1){
                //breaks
                b=numShifts;
                a=numDays;

                time_out=2;
              }
            }
          }
          //printf("try to take from\n");
          //try to take from some j
          for (int j=0; j<numNurses; j++){
            //find j that worked above min
            workNo = allNurses[j]->ret_assigned();
            if (workNo>nurMinDay){

              for (int a=0; a < numDays; a++){
                for (int b=0; b < numShifts; b++){
                  if (allNurses[j]->checkWorking(a,b) != 0 && allNurses[i]->validShift(a,b)){
                    unassign(j,a,b);
                    assign(i,a,b);
                    //breaks
                    b=numShifts;
                    a=numDays;
                    j=numNurses;

                    time_out=2;
                  }
                }
              }
            }
          }
        }
        else if (store>nurMaxDay){

          //find a day when nurse i is working
          for (int a=0; a<numDays; a++){

            for (int b=0; b<numShifts; b++){
              if (allNurses[i]->checkWorking(a,b) != 0)
              //try to replace with some j
              for (int j=0; j<numNurses; j++){
                if (allNurses[j]->ret_assigned() < nurMaxDay && allNurses[j]->validShift(a,b)){

                  //printf("try swap %d and %d store > nurMinDay\n",i,j);
                  unassign(i,a,b);
                  assign(j,a,b);
                  //breaks
                  j=numNurses;
                  a=numDays;
                  b=numShifts;
                  time_out=2;
                }
              }
            }
          }
        }
        store = allNurses[i]->workInRange();
      }
    }
    if (debug){
      for (int i=0;i<numNurses;i++){
        store = allNurses[i]->workInRange();
        if (-1 != store){
          printf("Nurse %d works %d days\n", i, store );
        }
      }
    }
    if (debug) formatObjectiveFunction(-1);
  }
}


void Optimize_Schedule::toggleBR(){
  for (int k = 0; k < numNurses; k++){
    allNurses[k]->BStoggle();
  }
  int min_entry, max_entry;
  if(firstRunBR){
    firstRunBR = false;
    for (int i =0; i<numShifts; i++){
      min_entry = -1;
      max_entry = -1;
      while (min_entry < 0){
        printf("Enter the minimum number of shifts to be filled for shift %d\n", i + 1);
        scanf("%d", &min_entry);
        if (min_entry < 0) printf("Cannot be a negative entry\n");
      }
      while (max_entry < min_entry){
        printf("Enter the maximum number of shifts to be filled for shift %d\n", i + 1);
        scanf("%d", &max_entry);
        if (max_entry < 0) printf("Cannot be a negative entry\n");
        else if (max_entry < min_entry) printf("Maximum times shift is filled cannot be less than minimum\n");
      }
      for (int j = 0; j < numNurses; j++){
        allNurses[j]->BSinput(i, min_entry, max_entry);
      }
    }
  }
}



void Optimize_Schedule::swapForMinimumBR(){
  int time_out=2,store, workNo;   //nurMaxDay nurMinDay
  //if balanced rosters constraint is turned on
  if(allNurses[0]->BS_bool()){
    if(debug) {
      formatObjectiveFunction(-1);
      printf("swapping to meet minimum requirement for balanced schedules\n");
    }
    //check all nurse's rosters
    for (int i=0;i<numNurses;i++){
      //for l shifts
      for (int l=0; l < numShifts; l++){
        //if nurse i worked the wrong amount of shift l then fix it
        store = allNurses[i]->balanced_schedule_shift_check(l);
        time_out=2;
        //while unfixed or not timed out
        while(store < 0 && time_out!=0){
          time_out--;
          //printf("%d %d\n",i,store);
          //printf("try to take from\n");
          //try to take from some j
          for (int j=0; j<numNurses; j++){
            if(allNurses[0]->ret_assigned() > nurMinDay){
              //find j that worked above minimum requirement for both BS or minimum work
              workNo = allNurses[j]->balanced_schedule_shift_check(l);
              //if not working enough of l
                if (store == -2){
                if (workNo > 0 && workNo > allNurses[j]->balanced_schedule_return(l,0)){

                  for (int a=0; a < numDays; a++){
                    if (allNurses[j]->checkWorking(a,l) != 0 && allNurses[i]->validShift(a,l)){
                      unassign(j,a,l);
                      assign(i,a,l);
                      //breaks
                      // b=numShifts;
                      // a=numDays;
                      // j=numNurses;

                      time_out=2;
                    }
                  }
                }
              }
              //if working too much of l
              else if (store == -1){
                if (workNo > 0 && workNo < allNurses[j]->balanced_schedule_return(l,1)){

                  for (int a=0; a < numDays; a++){
                    if (allNurses[j]->checkWorking(a,l) != 0 && allNurses[i]->validShift(a,l)){
                      unassign(j,a,l);
                      assign(i,a,l);
                      //breaks
                      // b=numShifts;
                      // a=numDays;
                      // j=numNurses;

                      time_out=2;
                    }
                  }
                }
              }
            }
          }
        }
      }
    if (debug) formatObjectiveFunction(-1);
    }
  }
}


//2swap has already been preformed
void Optimize_Schedule::threeSwapToImprove(){
  swapToImprove();
  int nur1,nur2,nur3,nur1Day,nur2Day,nur3Day,nur1Shift,nur2Shift,nur3Shift;
  int time_out=1, loop;
  float prevScore=0.0, curScore=0.0;
  bool swapSuccess;
  if (debug) formatObjectiveFunction(-1);
  while(time_out!=0 && loop<15){
    loop++;
    time_out--;
    for(int i=0; i<(numDays*numShifts*maxShift)-(maxShift * 2); i++){
      //choose first nurse and confirm its validity
      nur1 = workSchedule[i];

      if (nur1>0){
        nur1--;
        swapSuccess=false;
        for(int j=i+1; j<(numDays*numShifts*maxShift)-maxShift && !swapSuccess; j++){
          //choose second nurse and confirm its validity

          nur2 = workSchedule[j];
          if (nur2>0 && nur2-1!=nur1){
            nur2--;
            for(int k=j+1; k<(numDays*numShifts*maxShift) && !swapSuccess; k++){
              //choose third nurse and confirm its validity

              nur3 = workSchedule[k];
              if (nur3>0 && nur3-1!=nur2 && nur3-1!=nur1){
                nur3--;


                //ID is 1 greater than array index

                //decipher days
                nur1Day = calcDay(i);
                nur2Day = calcDay(j);
                nur3Day = calcDay(k);
                nur1Shift = calcShift(i);
                nur2Shift = calcShift(j);
                nur3Shift = calcShift(k);
                //save current scores
                prevScore = allNurses[nur1]->getScore() + allNurses[nur2]->getScore() + allNurses[nur3]->getScore();

                allNurses[nur1]->unassign(nur1Day, nur1Shift);
                allNurses[nur2]->unassign(nur2Day, nur2Shift);
                allNurses[nur3]->unassign(nur3Day, nur3Shift);

                //swap order to 231,312 (other orders are covered by 2swap)
                //first swap is 231 (2->1, 3->2, 1->3)
                if (allNurses[nur2]->validShift(nur1Day, nur1Shift)
                 && allNurses[nur3]->validShift(nur2Day, nur2Shift)
                 && allNurses[nur1]->validShift(nur3Day, nur3Shift)) {


                 allNurses[nur2]->assign(nur1Day, nur1Shift);
                 allNurses[nur3]->assign(nur2Day, nur2Shift);
                 allNurses[nur1]->assign(nur3Day, nur3Shift);
                 curScore = allNurses[nur1]->getScore() + allNurses[nur2]->getScore() + allNurses[nur3]->getScore();
                 if (curScore>prevScore){
                   assign(nur2, nur1Day, nur1Shift);
                   assign(nur3, nur2Day, nur2Shift);
                   assign(nur1, nur3Day, nur3Shift);
                   if (debug) {
                     printf("Swapped. ");
                     formatObjectiveFunction(-1);
                   }
                   time_out=1;
                   swapSuccess=true;
                 }
                 else{
                   allNurses[nur2]->unassign(nur1Day, nur1Shift);
                   allNurses[nur3]->unassign(nur2Day, nur2Shift);
                   allNurses[nur1]->unassign(nur3Day, nur3Shift);
                 }
                }
                //second swap is 312 (3->1, 1->2, 2->3)
                if (!swapSuccess
                  && allNurses[nur3]->validShift(nur1Day, nur1Shift)
                  && allNurses[nur1]->validShift(nur2Day, nur2Shift)
                  && allNurses[nur2]->validShift(nur3Day, nur3Shift)) {
                  allNurses[nur3]->assign(nur1Day, nur1Shift);
                  allNurses[nur1]->assign(nur2Day, nur2Shift);
                  allNurses[nur2]->assign(nur3Day, nur3Shift);
                  curScore = allNurses[nur1]->getScore() + allNurses[nur2]->getScore() + allNurses[nur3]->getScore();
                  if (curScore>prevScore){
                    assign(nur3, nur1Day, nur1Shift);
                    assign(nur1, nur2Day, nur2Shift);
                    assign(nur2, nur3Day, nur3Shift);
                    //full unassign reassign
                    if (debug) {
                      printf("Successful swap. "),
                      formatObjectiveFunction(-1);
                    }
                    time_out=1;
                    swapSuccess=true;
                  }
                  else {
                    allNurses[nur3]->unassign(nur1Day, nur1Shift);
                    allNurses[nur1]->unassign(nur2Day, nur2Shift);
                    allNurses[nur2]->unassign(nur3Day, nur3Shift);
                  }
                }
                //if no successful swap return to initial order
                if (!swapSuccess){
                  allNurses[nur1]->assign(nur1Day,nur1Shift);
                  allNurses[nur2]->assign(nur2Day, nur2Shift);
                  allNurses[nur3]->assign(nur3Day, nur3Shift);
                }
              }
            }
          }
        }
      }
    }
  }
  if (debug) formatObjectiveFunction(-1);

}


//attempts to swap any 2 nurses with eachother in an attempt to improve the schedule
void Optimize_Schedule::swapToImprove(){
  int nur1,nur2,dayi,dayj,shifti,shiftj;
  int time_out=1;
  int loop = 0;
  bool work;
  float ob1;
  float nur1Score=0.0, nur2Score=0.0, curScore=0.0;
  bool swapSuccess;
  if (debug) formatObjectiveFunction(-1);
  if (debug) printf("Testing swaps for improvement\n");
  while(time_out!=0 && loop<15){
    loop++;
    time_out--;
    for(int i=0; i<(numDays*numShifts*maxShift)-maxShift; i++){
      nur1=workSchedule[i];
      if (nur1!=0){
        nur1--;
        dayi = calcDay(i);
        shifti = calcShift(i);
        ob1 = objectiveFunction();
        //prevobj = objectiveFunction();
        nur1Score = allNurses[nur1]->getScore();
        work = allNurses[nur1]->unassign(dayi, shifti);
        swapSuccess=false;
        if (work){
          for(int j=i+1; j<(numDays*numShifts*maxShift) && !swapSuccess; j++){
            nur2=workSchedule[j];

            if (nur2!=0){
              nur2--;
              //if both swaps are valid
              dayj = calcDay(j);
              shiftj = calcShift(j);
              //printf("nurse %d date %d %d, want %d %d %d\n", nur2, dayj, shiftj,nur1,dayi,shifti);
              nur2Score = allNurses[nur2]->getScore();


              work = allNurses[nur2]->unassign(dayj, shiftj);


              //if they can be assigned to opposing shifts validly
              if (allNurses[nur1]->validShift(dayj, shiftj)
                 && allNurses[nur2]->validShift(dayi, shifti)){

                allNurses[nur1]->assign(dayj, shiftj);
                allNurses[nur2]->assign(dayi, shifti);

                curScore = allNurses[nur1]->getScore() + allNurses[nur2]->getScore();
                if (work && ((curScore - nur2Score - nur1Score) > 0) && (nur1 != nur2)) {
                  //printf("%f > %f = %f + %f\n",curScore , nur1Score + nur2Score, nur1Score,nur2Score );
                  //printf("nurse %d nurse %d, cur %d, prev %d\n", nur1,nur2,curScore, nur1Score+nur2Score);

                  //printf("swapped %d %d\n",nur1,nur2);
                  //full unassign/assign

                  unassign(nur1, dayi, shifti);
                  unassign(nur2, dayj, shiftj);
                  assign(nur1, dayj, shiftj);
                  assign(nur2, dayi, shifti);


                  if (debug) {
                    printf("Successful swap. ");
                    formatObjectiveFunction(-1);
                  }
                  time_out=1;
                  swapSuccess=true;
                }
                else {

                  allNurses[nur2]->unassign(dayi, shifti);
                  allNurses[nur1]->unassign(dayj, shiftj);
                  allNurses[nur2]->assign(dayj, shiftj);


                }
              }
              //if not a valid shift
              else {

                allNurses[nur2]->assign(dayj, shiftj);

              }
            }
          }
          //if no swap was made then revert
          if (!swapSuccess) {
            allNurses[nur1]->assign(dayi, shifti);

          }
        }
      }
    }
  }
  if (debug) formatObjectiveFunction(-1);
  //printSchedule();
}


/*
void Optimize_Schedule::swapToImprove(){
  resetSchedule();
  formatObjectiveFunction(-1);
  float f1, n1;
  for (int i=0; i<numNurses; i++){
    for (int j=0; j<numDays; j++){
      for (int k=0; k<numShifts; k++){
        n1 = allNurses[i]->getScore();

        allNurses[i]->assign(j,k);
        allNurses[i]->unassign(j,k);
        if (n1 != allNurses[i]->getScore()){
          printf("nurse %d\n", i);
          printf("nurse score %f %f\n", n1, allNurses[i]->getScore());
        }
      }
    }
  }
}
*/

void Optimize_Schedule::cantWork(){
  int ID=0, a=0;
  while (ID!=-1){
    printf("enter nurse ID\n");
    scanf("%d",&ID);
    ID--;
    if (ID>=0 && ID<numNurses){
      allNurses[ID]->printTimeConstrained();
    }
  }
}

float Optimize_Schedule::objectiveFunction(){
  float a=0.0;
  for (int i=0;i<numNurses; i++){
    a += allNurses[i]->getScore();
    //allNurses[i]->getScore();
  }
  //hospitalSaveSchedule();
  //a+=inst -> hospital_score();
  //a+=inst->hospital_score();
  return a;
}

void Optimize_Schedule::formatObjectiveFunction(int input){
  float a;
  if (input == -1){
    a = objectiveFunction();
  }
  else {
    a = input;
  }
  if (a < 0) printf(" This schedule is infeasible with a negative score: %f\n ", a);
  else printf(" This schedule is feasible with a score of %f/100\n ", 100*a/numNurses);

}

//returns index into workSchedule 3d array as single integer
int Optimize_Schedule::calcDay(int at){
  int day = at / (numShifts * maxShift);
  return day;
}

//returns index into workSchedule 3d array as single integer
int Optimize_Schedule::calcShift(int at){
  int shift = (at / maxShift) % numShifts;
  return shift;
}

//returns index into workSchedule 3d array as single integer
int Optimize_Schedule::calcShiftPos(int at){
  int pos = at % maxShift;
  return pos;
}


//returns index into workSchedule 3d array as single integer
int Optimize_Schedule::schedulePosition(int day, int shift, int shift_position){
  return day * numShifts * maxShift + shift * maxShift + shift_position;
}


void Optimize_Schedule::printSchedule(){
  int nurseID;

  printf("\n/Day--");
  for (int i=0; i<numShifts;i++){
    for (int j=0;j<maxShift;j++){
      printf("---");
    }
    printf("-");
    if (i!=numShifts-1) printf("----");
  }
  printf("\\ \n");
  for (int i=0; i<numDays; i++){
    printf(/*"Day*/ "|%d  ", (i+1));
    if (i<9) printf(" ");
    for (int j=0; j<numShifts; j++){
      printf("| ");
      for (int k=0; k<maxShift; k++){
        if (workSchedule[schedulePosition(i,j,k)] < 10) printf(" ");

        nurseID = workSchedule[schedulePosition(i,j,k)];
        if (nurseID == 0){
          printf("- ");

        }
        else printf("%d ", nurseID);
      }
      printf("|  ");
    }
    printf("\n");
  }
  printf("\\-----");
  for (int i=0; i<numShifts;i++){
    for (int j=0;j<maxShift;j++){
      printf("---");
    }
    printf("-");
    if (i!=numShifts-1) printf("----");
  }
  printf("/ \n\n");
}

int Optimize_Schedule::retNumSchedule(){
  return numSchedules;
}

void Optimize_Schedule::saveSchedule(){
  numSchedules++;
  Schedule * input;
  input = new Schedule();
  input->loadDim(numDays);
  input->loadDim(numShifts);
  input->loadDim(maxShift);
  for (int i=0; i<numDays*numShifts*maxShift;i++){
    input->putAt(workSchedule[i],i);
  }
  input->setScore(objectiveFunction());
  savedSchedules.push_back(input);
  printf("Added to schedule position %d\n",numSchedules);
}


void Optimize_Schedule::scheduleLoaderHelp(){
  if (numSchedules == 0){
    printf("There are no schedules to load\n");
  }
  else{
    for (int i = 0; i < numSchedules; i++){
      printf(" Schedule %d\n", i+1);
      formatObjectiveFunction(savedSchedules[i]->getScore());
    }
  }
}


void Optimize_Schedule::loadSchedule(int num){
  resetSchedule();
  if (num > 0 && num<=numSchedules){
    for (int i=0; i<numDays;i++){
      for (int j=0; j<numShifts;j++){
        for (int k=0; k<maxShift;k++){
          assign((savedSchedules[num-1]->get(i,j,k))-1,i,j);
        }
      }
    }
  }
}

void Optimize_Schedule::hospitalSaveSchedule(){
  for (int i=0; i<numDays*numShifts*maxShift;i++){
    inst->putAt(workSchedule[i],i);
  }
}





void Optimize_Schedule::restTimeSet(int timeTo){
  for (int i = 0; i<numNurses; i++){
    allNurses[i]->setRestTime(timeTo);
  }
}

void Optimize_Schedule::setConstraint(int constraint, bool state){
  //to change hospital constraints
  if (constraint==-1){
    for (int i=0; i<numNurses; i++){
      printf("%d\n", allNurses[i]->workInRange());
    }
  }
  if (constraint==5){
      inst->setFlag(1,state);
      inst->setFlag(2,state);
  }
  else if (constraint>0 && constraint < 5){
    //To change all nurse constraints
    for (int i = 0; i<numNurses; i++){
      allNurses[i]->toggleConstraintCheck(constraint, state);
    }
    if (constraint>2){
      //inst->setFlag(constraint, state);
    }
  }
}


void Optimize_Schedule::resetSchedule(){
  invalid=0;
  for (int i=0; i<numNurses; i++) {
    allNurses[i]->unassignAll();
  }

  for (int i=0; i<numDays*numShifts*maxShift; i++) {
    workSchedule[i]=0; //fills array with not working value (0)
    invalidTimes[i]=-1;
  }
}





/**************************************************
Genetic Algorithms functions

void geneticAlgorithm(int popSize, float mutate, float crossover);
void GAinitializeGenerations();
void GAputSchedule(int schedulePosition);
void GAgetSchedule(int schedulePosition);
int GAquicksortPartition(int arr[], int low, int high);
void GAquicksort(int low, int high);
void GAcrossover(float crossoverAmount, int parent_1_index, int parent_2_index, int child_location);
void GAmutate(float mutateAmount, int mutateType, bool validMutatesOnly);
void GAcombineGens(int orderedScoreIndicesA[], int orderedScoreIndicesB[]);
int weightedRandomParent(int worstIndex, bool firstRun);


***************************************************/

//Formats the vectors of schedules GAgenerationA and GAgenerationB
//Gives initial schedules the GA seed (initial schedule assignment)
void Optimize_Schedule::GAinitializeGenerations(bool curSchedule, int populationSize){


  GAgenerationA.reserve(populationSize);
  GAgenerationB.reserve(populationSize);


  Schedule * temp;



  for (int i = 0; i < populationSize; i++){
    temp = new Schedule();
    GAgenerationA.push_back(temp);
    temp = new Schedule();
    GAgenerationB.push_back(temp);

  }

  //fills the schedules with an initial "seed"
  for (int i = 0; i<populationSize; i++){
    //load schedule size
    GAgenerationA[i]->loadDim(numDays);
    GAgenerationA[i]->loadDim(numShifts);
    GAgenerationA[i]->loadDim(maxShift);
    GAgenerationB[i]->loadDim(numDays);
    GAgenerationB[i]->loadDim(numShifts);
    GAgenerationB[i]->loadDim(maxShift);


    //use the current generation as a seed if asked to
    if (!curSchedule) {
      resetSchedule();
      trueRandomAssign();
    }

    // loads schedule in
    for (int j = 0; j<numDays*numShifts*maxShift; j++){
      GAgenerationB[i]->putAt(workSchedule[j], j);
    }
    GAgenerationB[i]->setScore(objectiveFunction());

  }
}

//puts the current active schedule in Optimize_Schedule into a GAgeneration
void Optimize_Schedule::GAputSchedule(int schedulePosition){
  //input schedule
  int ID;
  for (int i=0; i<numDays*numShifts*maxShift;i++){
    ID = workSchedule[i];
    if (currentGen==1) GAgenerationA[schedulePosition]->putAt(ID,i);
    else if (currentGen==2) GAgenerationB[schedulePosition]->putAt(ID,i);
  }
  //input score
  if (currentGen==1) GAgenerationA[schedulePosition]->setScore(objectiveFunction());
  else if (currentGen==2) GAgenerationB[schedulePosition]->setScore(objectiveFunction());

}



void Optimize_Schedule::GAgetSchedule(int schedulePosition){
  resetSchedule();
  int ID;
  for (int i = 0; i < numDays; i++){
    for (int j = 0; j < numShifts; j++){
      for (int k = 0; k < maxShift; k++){
        if (currentGen==1) {
          ID = GAgenerationA[schedulePosition]->get( i, j, k)-1;
        }
        if (currentGen==2) {
          ID = GAgenerationB[schedulePosition]->get( i, j, k)-1;
        }
        assign(ID,i,j);

      }
    }
  }
}


//classic quicksort algorithm
//results in arr being an array of indices to the current parent generation sorted by the objective function
int Optimize_Schedule::GAquicksortPartition(int arr[], int low, int high){
  // pivot (Element to be placed at right position)
  float pivot;
  if (currentGen==1) pivot = GAgenerationA[arr[high]]->getScore();
  else pivot = GAgenerationB[arr[high]]->getScore();

  int i = (low - 1);  // Index of smaller element

  //temp for swapping
  float temp;
  for (int j = low; j <= high-1; j++){
    if (currentGen==1 && GAgenerationA[arr[j]]->getScore() <= pivot){
      i++;    // increment index of smaller element
      temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
    else if (currentGen==2 && GAgenerationB[arr[j]]->getScore() <= pivot){
      i++;    // increment index of smaller element
      temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
  temp = arr[i+1];
  arr[i+1] = arr[high];
  arr[high] = temp;
  return (i + 1);
}

//takes a generation and sorts it by each schedule's objective function in
//low is starting index, high is ending index
void Optimize_Schedule::GAquicksort(int arr[], int low, int high){
  //end condition
  if (low<high){
    int partitionIndex = GAquicksortPartition(arr, low, high);

    GAquicksort(arr, low, partitionIndex - 1);  // Before partitionIndex
    GAquicksort(arr, partitionIndex + 1, high); // After partitionIndex
  }
}


//choose 2 schedules
//crosses over crossoverAmount percent (at most 50%)
void Optimize_Schedule::GAcrossover(float crossoverAmount, int parent_1_index, int parent_2_index, int child_location){
  //rounds to an integer and decides randomly the number of full shifts parent_2 gives to child
  int shifts_crossed_over = crossoverAmount * (numDays * numShifts);

  int shifts_to_exchange[shifts_crossed_over];
  bool run;
  int temp;
  //select shifts
  for (int i = 0; i < shifts_crossed_over; i++){
    run = true;
    while (run){
      run = false;
      //choose a random shift
      temp = (rand() % numDays) * numShifts + (rand() % numShifts);
      for (int j = 0; j < i; j++){
        //check if random shift is already chosen for crossover
        if (shifts_to_exchange[j] == temp) run = true;
      }
    }
    shifts_to_exchange[i] = temp;
  }

  //Copy all of parent_1 to the child
  for (int i = 0; i < numDays*numShifts*maxShift; i++){
    if (currentGen==1){
      GAgenerationA[child_location]->putAt(GAgenerationB[parent_1_index]->getAt(i),i);
    }
    else if (currentGen==2){
      GAgenerationB[child_location]->putAt(GAgenerationA[parent_1_index]->getAt(i),i);
    }
  }
  int shiftLocation;
  //puts parent_2 crossover into child's "DNA"
  for (int i = 0; i < shifts_crossed_over; i++){
    for (int j = 0; j < maxShift; j++){
      shiftLocation = shifts_to_exchange[i];
      if (currentGen==1){
        GAgenerationA[child_location]->putAt(GAgenerationB[parent_2_index]->getAt(shiftLocation*maxShift+j),shiftLocation*maxShift+j);
      }
      if (currentGen==2){
        GAgenerationB[child_location]->putAt(GAgenerationA[parent_2_index]->getAt(shiftLocation*maxShift+j),shiftLocation*maxShift+j);
      }
    }
  }
}



//takes place after GAcrossover
//takes mutateType where:
//1 = mutate with random swaps
//2 = mutate by randomizing a nurse in a position
//3 = combination of swap 1 and 2
void Optimize_Schedule::GAmutate(float mutateAmount, int mutateType, bool validMutatesOnly){
  //if validMutatesOnly = true then mutates are only allowed if they don't defy hard constraints
  bool GAmutateDebug = false;

  //counts the number of successful mutations
  int successfulMutations = 0;
  //numberOfMutations is equal to the number of mutations
  int numberOfMutations = mutateAmount * (maxShift * numDays * numShifts);
  //if mutate type 3 then half mutations are random swaps and half are random changes
  if (mutateType == 3 && !validMutatesOnly) numberOfMutations = numberOfMutations/2;

  //time_out in case of impossibility
  int time_out;
  int nur1ID, nur1Day, nur1Shift, nur1ShiftPos;
  int nur2ID, nur2Day, nur2Shift, nur2ShiftPos;
  if (GAmutateDebug) printf("mutateType %d\n", mutateType);
  //mutate with swaps
  if (mutateType == 1 | mutateType == 3){
    //numberOfMutations is divided by 2 since swaps change 2 positions
    for (int i = 0; i < numberOfMutations/2; i++){
      time_out=100;
      nur1ID = nur2ID = 0;
      //choose a nurse at a random shift twice and swap
      while (nur1ID < 1 && time_out!=0){
        time_out--;
        nur1Day = rand() % numDays;
        nur1Shift = rand() % numShifts;
        nur1ShiftPos = rand() % maxShift;

        nur1ID = workSchedule[nur1Day * maxShift * numShifts + nur1Shift * numShifts + nur1ShiftPos];
      }
      nur1ID--;
      while (nur2ID < 1 && time_out!=0){
        time_out--;
        nur2Day = rand() % numDays;
        nur2Shift = rand() % numShifts;
        nur2ShiftPos = rand() % maxShift;
        nur2ID = workSchedule[nur2Day * maxShift * numShifts + nur2Shift * numShifts + nur2ShiftPos];
      }
      nur2ID--;
      if (time_out!=0){
        unassign(nur1ID, nur1Day, nur1Shift);
        unassign(nur2ID, nur2Day, nur2Shift);
        //if only valid mutates should be used then only assign if valid.
        if (validMutatesOnly){
          if (allNurses[nur1ID]->validShift(nur1Day, nur1Shift)
            && allNurses[nur2ID]->validShift(nur2Day, nur2Shift)){
            assign(nur1ID, nur2Day, nur2Shift);
            assign(nur2ID, nur1Day, nur1Shift);
            if (GAmutateDebug) printf("successful Swap of %d at %d %d %d and %d at %d %d %d\n", nur1ID+1, nur1Day+1, nur1Shift+1, nur1ShiftPos+1, nur2ID+1, nur2Day+1, nur2Shift+1, nur2ShiftPos+1);

            successfulMutations+=2;
          }
          else{
            //if (GAmutateDebug)printf("unsuccessful\n");
            assign(nur1ID, nur1Day, nur1Shift);
            assign(nur2ID, nur2Day, nur2Shift);
          }
        }
        else {
          assign(nur1ID, nur2Day, nur2Shift);
          assign(nur2ID, nur1Day, nur1Shift);
        }
      }
    }
  }

  if (GAmutateDebug && mutateType == 3 && validMutatesOnly) printf("%d swap mutations %d remaining mutations\n", successfulMutations, numberOfMutations-successfulMutations);
  if (GAmutateDebug && mutateType == 3 && !validMutatesOnly) printf("%d swap mutations %d random mutations\n", numberOfMutations, numberOfMutations);

  //mutate with randomness
  if (mutateType == 2 | mutateType == 3){
    //if mutate type 3 then half mutations are random swaps and half are random changes
    int stopAt;
    if (mutateType == 3 && !validMutatesOnly) stopAt = numberOfMutations;
    else stopAt = numberOfMutations-successfulMutations;
    for (int i = 0; i < stopAt; i++){
      //numberOfMutations is divided by 2 since swaps change 2 positions
      time_out=100;
      nur1ID = 0;
      //choose a time a nurse is working
      while (nur1ID < 1 && time_out!=0){
        time_out--;
        nur1Day = rand() % numDays;
        nur1Shift = rand() % numShifts;
        nur1ShiftPos = rand() % maxShift;

        nur1ID = workSchedule[nur1Day * numDays * numShifts + nur1Shift * numShifts + nur1ShiftPos];
      }
      nur1ID--;
      //change the nurse that's working at this time
      unassign(nur1ID, nur1Day, nur1Shift);
      bool run = true;
      while(run && time_out != 0){
        time_out--;
        nur2ID = rand() % numNurses;
        //if validMutatesOnly then only put in a new nurse if it's valid to do so
        if (validMutatesOnly){
          if (allNurses[nur2ID]->validShift(nur1Day, nur1Shift)){
            if (GAmutateDebug) printf("Randomly putting %d at %d %d %d\n", nur2ID, nur1Day, nur1Shift, nur1ShiftPos);
            assign(nur2ID, nur1Day, nur1Shift);
            successfulMutations++;

            run = false;
          }
        }
        else{
          assign(nur2ID, nur1Day, nur1Shift);
          run = false;
        }
      }
      //if loop ended with no valid nurse assignment
      if (run){
        //replace with previous nurse
        assign(nur1ID, nur1Day, nur1Shift);
      }
    }
  }
  if (GAmutateDebug) printf("Mutated %d times out of attempted %d times\n",successfulMutations, numberOfMutations);

}

//returns the index to a random weighted parent
int Optimize_Schedule::weightedRandomParent(int worstIndex, bool firstRun){
  //checks to see if the worst score is positive
  bool worstPos = false;
  //for debug text
  bool weightedRandomParentDebug = false;
  float sum = 0;

  //this only needs to run once each generation
  if (firstRun){
    float temp;
    float worstScore;
    //chooses the worst score to add to all other scores
    if (currentGen==1) {
      worstScore = GAgenerationB[worstIndex]->getScore();

    }
    else if (currentGen==2) {
      worstScore = GAgenerationA[worstIndex]->getScore();
    }
    if (worstScore < 0) {
      worstScore /= -2000;
      worstScore += 5;
    }
    else {
      worstPos = true;
      worstScore *= -1;
      worstScore += .4;
    }
    if (weightedRandomParentDebug) printf("worstScore %f\n", worstScore);

    //weights the parents
    for (int i = 0; i < popSize; i++){
      if (currentGen==1) temp = GAgenerationB[i]->getScore();
      else if (currentGen==2) temp = GAgenerationA[i]->getScore();
      if (temp < 0) temp /= 2000;
      if (weightedRandomParentDebug) printf("temp %f\n", temp);
      temp += worstScore;
      sum += temp;
      orderedScores[i] = int(sum);
    }
  }
  //if !firstRun then finds sum
  //if firstRun then sum = orderedScores[popSize-1] anyway
  sum = orderedScores[popSize-1];
  //chooses a score in the weigted range
  int choose = (rand() % int(sum)) - 1;
  if (weightedRandomParentDebug) printf("choose %d\n", choose );
  //chooses the parent
  for (int i = 0; i < popSize; i++){
    if (choose < orderedScores[i]) return i;
  }

  return popSize;
  //printf("ERROR\n");
  //return -1;
}

//combines the best members of both generations into the current generation
void Optimize_Schedule::GAcombineGens(int orderedScoreIndicesA[], int orderedScoreIndicesB[]){
  int generationIndex = popSize-1;
  int count = 0;
  for (int i = 0; i < popSize; i++){

    if (currentGen==1){
      //swaps worst from A with best from B
      if (GAgenerationA[orderedScoreIndicesA[i]]->getScore()
        < GAgenerationB[orderedScoreIndicesB[generationIndex]]->getScore()){

        count++;
        currentGen = 2;
        GAgetSchedule(orderedScoreIndicesB[generationIndex]);
        currentGen = 1;
        GAputSchedule(orderedScoreIndicesA[i]);
        generationIndex--;
      }
    }
    else{
      //swaps worst from B with best from A
      if (GAgenerationB[orderedScoreIndicesB[i]]->getScore()
        < GAgenerationA[orderedScoreIndicesA[generationIndex]]->getScore()){
        count++;
        currentGen = 1;
        GAgetSchedule(orderedScoreIndicesA[generationIndex]);
        currentGen = 2;
        GAputSchedule(orderedScoreIndicesB[i]);
        generationIndex--;
      }
    }
  }
  if (debug) printf("%d out of %d members of generation replaced with more fit members from parent generation\n", count, popSize);
}



void Optimize_Schedule::geneticAlgorithm(bool curSchedule, int numGen, int populationSize, float mutate, float crossover){
  if (debug) printf("Entering the genetic algorithm\n");


  //Initialize variables (global)
  srand(time(NULL));
  //popSize=populationSize
  popSize = populationSize;
  currentGen=2; //keeps track of which generation is overwritten (the other is the parent)
  int orderedScoreIndicesA[popSize]; //numDays days, numShifts shifts per day, maxShift nurses per shift
  int orderedScoreIndicesB[popSize]; //numDays days, numShifts shifts per day, maxShift nurses per shift

  orderedScores = new int[popSize]; //numDays days, numShifts shifts per day, maxShift nurses per shift
  int mutateType = 2;
  bool smartMutate = false;


  for (int i=0; i<popSize;i++) {
    orderedScoreIndicesA[i]=i;
    orderedScoreIndicesB[i]=i;

  }
  int worst;   //best score of previous generation
  float random;


  //prepares function and initiailizes starting generation
  if (debug) printf("Creating initial generation...\n");
  GAinitializeGenerations(curSchedule, populationSize);
  if (debug) printf("Sorting initial generation...\n");

  //sort members of the active generation in orderedScoreIndices array by objectiveFunction()
  if (currentGen == 1) GAquicksort(orderedScoreIndicesA, 0, popSize-1);
  else GAquicksort(orderedScoreIndicesB, 0, popSize-1);




  int numberOfGenerations=1;

  //TODO: decide on a stop conditions
  while (numberOfGenerations < numGen){
    numberOfGenerations++; //counts generations (for debugging but also as a potential stopping condition)
    //remember worst member of the generation
    if (currentGen==1) worst = orderedScoreIndicesA[0];
    else worst = orderedScoreIndicesB[0];

    //alternates child/parent generation
    if (currentGen!=1) currentGen=1;
    else currentGen=2;

    if (debug) printf("Beginning crossover...\n");
    //crossover members of a generation
    if (popSize > 1){
      int parent_1_index, parent_2_index;
      bool firstRun = true;
      for (int i=0; i<popSize; i++){
        parent_1_index =  weightedRandomParent(worst,firstRun);
        firstRun = false;
        parent_2_index =  weightedRandomParent(worst,firstRun);
        while (parent_1_index == parent_2_index){
          parent_2_index = weightedRandomParent(worst,firstRun);
        }
        random = rand()/ float(RAND_MAX); //produces a number between 0 and 1
        //crossover * random creates a number from 0 to crossover
        //breed new child
        GAcrossover(random * crossover, parent_1_index, parent_2_index, i);
      }
    }

    if (debug) printf("Beginning mutation...\n");
    //if (currentGen==1) if(GAgenerationA[worst]->getScore() > 0) {smartMutate = true;}
    //else if (GAgenerationB[worst]->getScore() > 0) {smartMutate = true;}
    //mutate members of a generation
    for (int i=0; i<popSize; i++){

      //load in schedule i
      GAgetSchedule(i);
      //mutate that schedule
      random = rand()/ float(RAND_MAX); //produces a number between 0 and 1

      GAmutate(random * mutate, mutateType, smartMutate);
      GAputSchedule(i);
    }



    if (currentGen == 1) GAquicksort(orderedScoreIndicesA, 0, popSize-1);
    else GAquicksort(orderedScoreIndicesB, 0, popSize-1);

    GAcombineGens(orderedScoreIndicesA, orderedScoreIndicesB);
    if (debug) printf("Sorting generation %d...\n", numberOfGenerations);
    //sort members of the active generation in orderedScoreIndices array by objectiveFunction()
    if (currentGen == 1) GAquicksort(orderedScoreIndicesA, 0, popSize-1);
    else GAquicksort(orderedScoreIndicesB, 0, popSize-1);

    if (debug) {
      if (currentGen == 1) {
        GAgetSchedule(orderedScoreIndicesA[popSize-1]);
        printf("Best score of generation :");
        formatObjectiveFunction(-1);
      }
      else {
        GAgetSchedule(orderedScoreIndicesB[popSize-1]);
        printf("Best score of generation :");
        formatObjectiveFunction(-1);
      }
    }

    //TODO: check stopping condition
  }

  if (debug) printf("Finished. Persisted for %d generations.\n", numberOfGenerations );


  if (debug) printf("Loading in best schedule\n");
  //load in best schedule created by the genetic algorithm
  if (currentGen == 1) GAquicksort(orderedScoreIndicesA, 0, popSize-1);
  else GAquicksort(orderedScoreIndicesB, 0, popSize-1);
  if (currentGen == 1) GAgetSchedule(orderedScoreIndicesA[popSize-1]);
  else GAgetSchedule(orderedScoreIndicesB[popSize-1]);

}
