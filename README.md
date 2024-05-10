List of team members and their contact info (do not use private emails)
What are assigned roles: architect, tester, coordinator, etc with brief roles description.
General design of the software. How it is divided into pieces.
Road map (as a table). Each row of the table is


Team 15 

- Abid Hossain ( hossaiabid ) hossaiabid@ecs.vuw.ac.nz          -Tester
    -Tests the code for the robot and provides help to other roles while no testing is needed

- Nick Yannan Luo ( luoyann ) luoyann@ecs.vuw.ac.nz             -

- Jack Scrivener ( scrivejack ) scrivejack@ecs.vuw.ac.nz        -Programmer
    -Designs and writes program for robot

- Vincent Wong ( wongwai2 ) wongwai2@ecs.vuw.ac.nz              -Robot builder
    -Designs and builds the robot



--------------- Program design ---------------
there will be 4 dynamic functions some static functions called by the dynamic functions
and a main loop that calls the dynamic functions

Dynamic functions
1 - Open the gate 
    -This will take nothing and open the gate for the robot

2 - line following 
    -This will take nothing and will make the robot follow the line

3 - Intersections 
    -This will control the robot through the intersections

4 - Red pole pusher
    -This will make the robot push the red pole

Static functions
- motor drive 
    - takes motor instructions struct
    - writes to the motor the mapped value

- whole image read
    - returns 2D array
    - takes void

- row read 
    - returns array
    - takes a row number 


main loop decides when to call the 4 dynamic functions.



| Start date | Task description | Responsible team member | End date | Outcome |
| ------ | ------ | ------ | ------ | ------ |
| Week 1: 2/5/2024| Build the minimum viable product (MVP) of the robot. No fancy bells or whistles| Vincent Wong & Abid Hossain| 9/5/2024| Ahead of due date by three days|
| 2/5/2024|Program the gate opening and basic line following functions.|Jack Scrivener|9/5/2024|Signifagant changes to aproach where required but finished on time|
| Week 2: 9/5/2024|Tune the line following using the MVP robot|Abid Hossain| 16/5/2024|...|
| 9/5/2024|Make upgrades to the robot as neeed during testing| Vincent Wong|16/5/2024 |...|
| 9/5/2024|Program the intersection and pole pushing functions |Jack Scrivener |16/5/2024 |...|
| Week 3: 16/5/2024|Tune the intersection and pole pushing functions| Abid Hossain| 23/5/2024|...|
| 16/5/2024|Make any final upgrades to the robot as needed during testing|Vincent Wong| 23/5/2024|...|
| 16/5/2024|Fix bugs in the intersection and pole pushing functions|Jack Scrivener| 23/5/2024|...|
| Week 4 (PANIC): 23/5/2024|Fix any programming bugs but do not add any extra functionality|Jack Scrivener| 30/5/2024|...|
| 23/5/2024|Fix any critical design descions but do not anything unnessecary|Vincent Wong| 30/5/2024|...|
| 23/5/2024|Provide suppoort for programming and building|Abid Hossain| 30/5/2024|...|