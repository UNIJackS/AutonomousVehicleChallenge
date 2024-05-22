| Start date | Task description | Responsible team member | End date | Outcome | Actual progress as per date |
| ------ | ------ | ------ | ------ | ------ | ------ |
| Week 1: 2/5/2024| Build the minimum viable product (MVP) of the robot. No fancy bells or whistles| Vincent Wong & Abid Hossain| 9/5/2024| Ahead of due date by three days|
| 2/5/2024|Program the gate opening and basic line following functions.|Jack Scrivener|9/5/2024|Signifagant changes to aproach where required but finished on time|
| Week 2: 9/5/2024|Tune the line following using the MVP robot|Abid Hossain| 16/5/2024|Rigorously adjusted KP and KD values to tune the PID with the goal of the robot taking the smoothest turns possible|
| 9/5/2024|Make upgrades to the robot as neeed during testing| Vincent Wong|16/5/2024 |Upgrades have been completed it included: Smaller wheels, better camera position, and made the base out of aluminimium|
| 9/5/2024|Program the intersection and pole pushing functions |Jack Scrivener |16/5/2024 |The Intersection code has been written and is working however the pole pushing is not due to lack of time|
| Week 3: 16/5/2024|Tune the intersection and pole pushing functions| Abid Hossain| 23/5/2024|...|
| 16/5/2024|Make any final upgrades to the robot as needed during testing|Vincent Wong| 23/5/2024|...|
| 16/5/2024|Fix bugs in the intersection and pole pushing functions|Jack Scrivener| 23/5/2024|...|
| Week 4 (PANIC): 23/5/2024|Fix any programming bugs but do not add any extra functionality|Jack Scrivener| 30/5/2024|...|
| 23/5/2024|Fix any critical design descions but do not anything unnessecary|Vincent Wong| 30/5/2024|...|
| 23/5/2024|Provide suppoort for programming and building|Abid Hossain| 30/5/2024|...|

Analysis of Project progress
Write short analysis of how Project is going:
- What went according to plan. What did not.
- What are bottlenecks, what is slowing the development
- Team recommendation on how to advance the Project
Make sure that all three bullet points are covered.

**Programming**:
The parts of the program for opening the gate, following the line and going through the intersections were all delivered on time and were working by the end of there respecive testing period. The programing has fallen behind with the intial draft of the pole pushing code not being delivered on time. This was due to the long devalopment time of the intersection code. To speed up devalopment i started using WinSCP as it allows for code to be written on my laptop and then sent to the pi. This speeds up devalopment as adjustments can be be made to the program without leaving the testing table. It also speeds up the time to write and debug the program as i can use visual stduio code which is familar to me. We are still on track to have the code written as we allowed for the final week for final fixes and testing however it is not ideal.

**Building**:
The very first week of starting to construct the robot went pretty smoothly, with being able to complete the MVP in two lab sessions. On the Monday of week 2 Jack had brought some very fancy tools and materials(Carbon fiber pole, and aluminimium) that we could make our near final product with. So I went to rebuild our robot with the newly acquired materials. During the middle of the second week however, there were some minor issues where when we were testing out the robot with people already around the table we decided to put it out on track without being close to our robot and unfortunatly it fell off the table and broke the camera mounting. Although with that being said nothing else on the robot broke except the ribbon cable disconnecting on impact but it didn't break luckly. This fall had impacted our testing time with the robot. With now repairing the robot I thought of having smaller wheels instead of the medium wheels because of the way our camera was mounted above the wheels it made the position of it high and messed up during testing, with smaller wheels it allowed us to have the camera position closer to the ground for more accurate tracking potential. Rebuilding the robot didn't take us too long as we already had the MVP so it made it easier to build.

**Testing**:
During the inital construction of the robot, I provided assistance and offered a few suggestions which helped get an MVP vehicle up and ready to test the following week. While Jack was busy developing his MVP for the software, Vincent was busy improving the robot design and I provided some assistance and helped the group reach testing stage. The following week Jack had the software for the first few quadrants ready for tuning/testing. After a while of changing KP/KD, recompiling, and executing, I noticed that we spent quite a lot of time retyping in the command to compile and run so I created a Makefile which saved time during testing. Unfortunately during testing there was an incident of the robot incurring damage via a fall off the edge of the table. This was a set back and deducted a major portion of the amount of time we had available for testing. Although with some clever design changes, our vehicle could follow the line more optimally and did not require much further tuning of the PID, we were still behind on the initial goal of tuning both intersection and the pole pushing functions. However, those features have now mostly been dealt with, requiring a minor amount of tuning. We are therefore still on track to complete the project.