List of team members and their contact info (do not use private emails)
What are assigned roles: architect, tester, coordinator, etc with brief roles description.
General design of the software. How it is divided into pieces.
Road map (as a table). Each row of the table is


Team 15 

- Abid Hossain ( hossaiabid ) hossaiabid@ecs.vuw.ac.nz          -Tester
    -Tests the code for the robot and provides help to other roles while no testing is needed

- Nick Yannan Luo ( luoyann ) luoyann@ecs.vuw.ac.nz             -Absent

- Jack Scrivener ( scrivejack ) scrivejack@ecs.vuw.ac.nz        -Programmer
    -Designs and writes program for robot

- Vincent Wong ( wongwai2 ) wongwai2@ecs.vuw.ac.nz              -Robot builder
    -Designs and builds the robot




--------------- Program design ---------------
The Program will consist of 5 main functions one for each of the 4 major sections of the course and 1 for detecting if
the section has changed. These 4 main functions will be supported by some static functions which will do basic things
like reading a row or column and returning the average black pixel or checking if a pixel is black.

4 main functions 
- Open gate (section 1)
    -This will communicate with the server and open the gate for the robot

- line following (section 2)
    -This will make the robot cruise at a speed and try to keep the line in the middle of the camera. It will
     do this using a PID system to 

- Intersections  (section 3)
    -This will make the robot navigate through the intersections. It will detect what turns are
     available to it and then always make a left turn if possible, then go straight if possible, then
     right if possible and if no turns are available turn around.

- Red ball pusher (section 4)
    -This will turn the camera to face forward and then search for the red ball by spinning ~360 deg. If it is
     not detected then it will move and spin again. If the ball is detected then it will attempt to keep it 
     directly in front of the robot and cruise at a constant speed similar to the line following

- section change
    -This will be called during sections 2 and 3 to check if the red section change markers are detected.
     It will do this by checking if the number of red pixels in the screen is above a threshold value.

Static functions
- read 
    -This will take a row or column number, a reference to a bool for if at least 1 black pixel is detected and a bool
     for if a column or row is to be read. 
    - This will return the distance from the centre of the screen to the average (mean) black pixel in the screen.

- is black 
    -This will take an x and y coordinate of the pixel to be checked
    -This will return a bool of if the pixel is black. it will do this by checking if the intensity is below a threshold
     value and the RGB values are within a threshold value of each other. 

- draw box 
    -This will take a left, right, top and bottom value and draw a box to the screen with these values

- test motors
    -This will check the polarity of the motors to make sure the motors are being driven correctly
     This will be done by driving the left motor backwards and the right motor forward and then waiting for 5 seconds
     and repeating this process except reversing which motor is driven backwards.

other static functiosns will probably be added as required. 

The main loop will call each function of the main 4 functions and the section change function to decide when
to change between the 4 main functions. Except for the open gate function as there is no marker.





| Start date | Task description | Responsible team member | End date | Outcome |
| ------ | ------ | ------ | ------ | ------ |
| Week 1: 2/5/2024| Build the minimum viable product (MVP) of the robot. No fancy bells or whistles| Vincent Wong & Abid Hossain| 9/5/2024| Ahead of due date by three days|
| 2/5/2024|Program the gate opening and basic line following functions.|Jack Scrivener|9/5/2024|Signifagant changes to aproach where required but finished on time|
| Week 2: 9/5/2024|Tune the line following using the MVP robot|Abid Hossain| 16/5/2024|Rigorously adjusted KP and KD values to tune the PID with the goal of the robot taking the smoothest turns possible|
| 9/5/2024|Make upgrades to the robot as neeed during testing| Vincent Wong|16/5/2024 |Upgrades have been completed it included: Smaller wheels, better camera position, and made the base out of aluminimium|
| 9/5/2024|Program the intersection and pole pushing functions |Jack Scrivener |16/5/2024 |...|
| Week 3: 16/5/2024|Tune the intersection and pole pushing functions| Abid Hossain| 23/5/2024|...|
| 16/5/2024|Make any final upgrades to the robot as needed during testing|Vincent Wong| 23/5/2024|...|
| 16/5/2024|Fix bugs in the intersection and pole pushing functions|Jack Scrivener| 23/5/2024|...|
| Week 4 (PANIC): 23/5/2024|Fix any programming bugs but do not add any extra functionality|Jack Scrivener| 30/5/2024|...|
| 23/5/2024|Fix any critical design descions but do not anything unnessecary|Vincent Wong| 30/5/2024|...|
| 23/5/2024|Provide suppoort for programming and building|Abid Hossain| 30/5/2024|...|