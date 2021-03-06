# Nurse Scheduling Problem
This software is created alongside a written analysis with the intention of critically examining all aspects of the nurse scheduling problem (NSP). This analysis considers varying constraints, problem size and solution methods to determine the best methods for solving the problem. The NSP found in the software, with the help of the balanced rosters constraint, is proven to be NP-hard.

**Before Compiling**
- Make sure all files are in the same folder
- Edit the config.txt as desired (found in the build folder)
- Open the terminal

**Compiling**
- Ensure you have a compiler (for the following I used cygwin)
- Double click run.bat
or
- Change directory to the folder containing all the src files and compile by typing into the command line:
`g++ -o ../build/main main.cpp Nurse.cpp Hospital.cpp Optimize_Schedule.cpp Schedule.cpp`

**Running**
If the run.bat file was not used to compile choose one of the following
- Go to the build folder and double click on main.exe
or
- Change the terminal's current directory to the build folder and type ./main

**Initial Setup**
Once the program is running there are two selection screens to get acquainted with.


The first is the startup screen as seen below. This menu allows the user to input nurse constraints. Inputting nurse constraints can only be done from this initial screen. Nurse constraints are specific days or shifts that should note that a nurse cannot work, does not want to work, or does want to work.

![Alt text](https://github.com/NickHagopianZ/Senior-Project/blob/master/sample%20images/Start_Screen.PNG)


The next screen is the main menu which will frequently be returned to. For a more complex understanding of the menu options or even submenus read the documentation for the software in the corresponding paper.
![Alt text](https://github.com/NickHagopianZ/Senior-Project/blob/master/sample%20images/Main_menu.PNG)


**[GitHub Repository](https://github.com/NickHagopianZ/Senior-Project/)**


**Contributors**
Special thanks to my advisors Dr. Rob Kelvey and Dr. Nathan Fox.
