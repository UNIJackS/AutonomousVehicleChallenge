
//refrance page : https://ecs.wgtn.ac.nz/Courses/ENGR101_2024T1/AVC_manuals
//This doccument uses camelCase 


//------- Imports -------
#include <iostream> // input-output library 
#include <math.h> //used for rounding function
#include "E101.h" // VUW camera library

using namespace::std;


//------- Global Varables ---------
//used for camera reading
const int totalXPixels = 320;
const int totalYPixesl = 240;

//Motor Varables
const int leftMotorPort = 1;  //port number for the left motor
const bool leftMotorReversed = false; // is the left motor fliped

const int rightMotorPort = 2; //port number for the right motor
const bool rightMotorReversed = true; // is the right motor fliped

const int cameraMotorPort = 3;//port number for the camera motor
const bool cameraMotorReversed = false; // is the camera motor fliped

//Black detection Varables
const float blackTolarance = 10; // The value by which red green and blue can differ and still be black
const int maxIntensity  = 20; // The maximum value of red + green + blue values to still be black

const int leftOfBox = 50; //px
const int rightOfBox = 100; //px
const int topOfBox = 50; //px
const int bottomOfBox = 100; //px


char[] serverAddress = {'1','3','0','.','1','9','5','.','3','.','5','4'} // server address
int serverPort = 1024; //server port

//used to store instructions to drive each motor

struct rowInfo{
    int firstIndex;
    int lastIndex;
    int averageIndex;
    bool[totalXPixels] rowList;
};

//------------------------- Static Functions ---------------------------
bool isblack(int x,int y){
    int red = get_pixel(rowToRead,currentYPixel,0)
    int green = get_pixel(rowToRead,currentYPixel,1)
    int blue = get_pixel(rowToRead,currentYPixel,2)
    int intensity = get_pixel(rowToRead,currentYPixel,3)

    //checsk if the red green and blue values are within + or - blackTolarance of each other 
    //and intesity is less than the maximum intesity
    if((red - blackTolarance > green || red + blackTolarance < green) 
        && (red - blackTolarance > blue || red + blackTolarance < blue) 
        && (green - blackTolarance > blue || green + blackTolarance < blue)
        && intensity < maxIntensity ){
        return false;
    }else{
        //sets pixels detected as black to green
        set_pixel(x, y, 0, 255, 0);
        return true;
    }
}


//takes the row number
//returns a rowInfo struct
rowInfo blackRowRead(int rowToRead){
    //creates a rowInfo object to store all the info
    rowInfo output = new rowInfo;
    bool firstBlackPixel = true;
    for(int currentXPixel =0; currentXPixel<=totalXPixels; currentXPixel+=1){
        if(firstBlackPixel){
            output.firstIndex = currentXPixel;
            firstBlackPixel = false;
        }
        output.lastIndex = currentXPixel;
        output.rowList[currentXPixel] = isblack(currentXPixel,rowToRead)
    }

    output.averageIndex = (output.lastIndex-output.firstIndex)/2;

    return output;
}



void drawBox(int left,int right,int top,int bottom) {
	//This draws the green left and right lines
	for (int currentRow = 0; currentRow < (bottom - top) ; currentRow += 1) {
		set_pixel(top + currentRow, left, 0, 255, 0);
		set_pixel(top + currentRow, right, 0, 255, 0);
	}
	//This draws the blue top and bottom lines
	for (int currentCol = 0; currentCol < (right - left) ; currentCol += 1) {
		set_pixel(top, left + currentCol, 0, 0, 255);
		set_pixel(bottom, left + currentCol, 0, 0, 255);

	}
}

//maximum vlaue is the maximum value for the input range
//minimum value is the minimum value for the input range
//current vlaue is the value inisde the range to be written to the motors
//revers is weather to reverse the motor
//port is the port to be written to 
//returns nothing
void motorDrive(int maxValue, int minValue, int currentValue, bool reverse, int port) {
    //-------------------------------------- Input Checking -------------------------------------- 
    //checks to make sure current value is not negative as this will mess up maths
    if (currentValue <= 0) {
        cout << "motorDrive:value is negative" << endl;
        cout << "motorDrive:value :" << currentValue << endl;
        return;
    };
    //checks to make sure min value is not negative as this will mess up maths
    if (minValue < 0) {
        cout << "motorDrive:min is negatibe" << endl;
        cout << "motorDrive:min:" << minValue << endl;
        return;
    };
    //checsk to make sure the min is lower than the max
    if (maxValue < minValue) {
        cout << "motorDrive:min must be greater than 0" << endl;
        cout << "motorDrive:Current value of min:" << minValue << endl;
        return;
    };
            
    //-------------------------------------- Actual function -------------------------------------- 

    //casted to doubles because other wise interger divisoion is truncated
    //rounding used here so that 2.6 doesnt go to 2 but 3 so motors are more accurite
    int pwm = round(currentValue / ((double)(maxValue - minValue) / (double)(30)) + 30);
    cout << "pwm" << pwm << endl;

    //checks to make sure pwm is greater than 30 and less than 60 to not fry motors
    if (pwm < 30 && pwm > 60) {
        cout << "motorDrive:pwm out of range (30 to 60)" << endl;
        cout << "motorDrive:Current value of pwm:" << pwm << endl;
        return;
    }else {
        //fips pwm for reverse
        if (reverse) {
            pwm = 60 - (pwm - 30);
            set_motors(port,pwm);
        }
        else {
            set_motors(port,pwm);
        }
    } 
}







//------- Dynamic Functions ---------
void openGate(){
    //attempts to connect to gate, will loop untill connected
    int connected = 0;
    while(connected == 0){
        connected = connect_to_server(serverAddress,serverPort);
        cout << "error connectiong to gate, code:" << connected << endl;
    }

    //sends request to server
    char[] pleaseMessage = {'P','l','e','a','s','e'};
    send_to_server(pleaseMessage);

    //reccives password from server
    char[] password = new char[];
    receive_from_server(password&);
    cout << "password:" << password << endl;

    //sends password to server
    send_to_server(password);
}

void followLine(){
    take_picture();

    drawBox(leftOfBox,rightOfBox,topOfBox,bottomOfBox);
    rowInfo topRow = blackRowRead(topOfBox);
    rowInfo bottomRow = blackRowRead(bottomOfBox);

    int deltaX = topRow.rowList.[topRow.averageIndex]-bottomRow.rowList.[bottomRow.averageIndex]; // change in x of the averages
    int deltaY = bottomOfBox - topOfBox; //change in y (diffrance between top and bottom of box)
    double gradent = deltaY / deltaX; // the diffrance between the middle of the top and bottom

    //sets the top average pixel to red
    set_pixel(topOfBox, topRow.rowList.[topRow.averageIndex], 255, 0, 0);
    //sets the bottom average pixel to red
    set_pixel(bottomOfBox, bottomRow.rowList.[bottomRow.averageIndex], 255, 0, 0);

    //sets the left motor speed 
    motorDrive(320,0,gradent,false,leftMotorPort);
    //sets the right motor speed
    motorDrive(320,0,gradent,true,rightMotorPort);
    
    update_screen();

    //drives the motors
    hardware_exchange();
}
void intersections(){
    
}
void pushPole(){
    
}


//------- Main ---------
int main() {
    openGate()
    cout << "open gate passed" << endl;


    while(true){
        followLine();
        sleep1(20);
    }
    cout << "follow line passed" << endl;

    
    return 0;
}