
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
const int leftMotorPort = 5;  //port number for the left motor
const bool leftMotorReversed = false; // is the left motor fliped

const int rightMotorPort = 3; //port number for the right motor
const bool rightMotorReversed = true; // is the right motor fliped

const int cameraMotorPort = 1;//port number for the camera motor
const bool cameraMotorReversed = false; // is the camera motor fliped

//Black detection Varables
const float blackTolarance = 10; // The value by which red green and blue can differ and still be black
const int maxIntensity  = 100; // The maximum value of red + green + blue values to still be black

const int leftOfBox = 110; //px
const int rightOfBox = 210; //px
const int topOfBox = 82; //px
const int bottomOfBox = 158; //px


char serverAddress[15] = {'1','3','0','.','1','9','5','.','3','.','5','3'}; // server address
int serverPort = 1024; //server port

//used to store instructions to drive each motor

struct rowInfo{
    int firstBlackPixelX;
    int lastBlackPixelX;
    int averageX;
    bool rowList[totalXPixels];
};

//------------------------- Static Functions ---------------------------
bool isblack(int x,int y){
    int red = get_pixel(y,x,0);
    int green = get_pixel(y,x,1);
    int blue = get_pixel(y,x,2);
    int intensity = get_pixel(y,x,3);

    //checsk if the red green and blue values are within + or - blackTolarance of each other 
    //and intesity is less than the maximum intesity
    if((red - blackTolarance < green || red + blackTolarance > green) 
       && (red - blackTolarance < blue || red + blackTolarance > blue) 
       && (green - blackTolarance < blue || green + blackTolarance > blue)
        && intensity > maxIntensity ){
       return false;
   
    }else{
        //sets pixels detected as black to green
        set_pixel(y, x, 0, 255, 0);
        return true;
    }
}


//takes the row number
//returns a rowInfo struct
rowInfo blackRowRead(int rowToRead){
    //creates a rowInfo object to store all the info
    rowInfo output;
    bool firstBlackPixel = true;
    for(int currentXPixel =leftOfBox; currentXPixel< rightOfBox; currentXPixel+=1){
		if(isblack(currentXPixel,rowToRead)){
			if(firstBlackPixel){
				output.firstBlackPixelX = currentXPixel;
				firstBlackPixel = false;
			}
			output.lastBlackPixelX = currentXPixel;
			output.rowList[currentXPixel] = true;
		}else{
			output.rowList[currentXPixel] = false;
		}
        
    }

    output.averageX = (output.lastBlackPixelX - output.firstBlackPixelX)/2 + output.firstBlackPixelX;


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
        cout << "motorDrive:min is negative" << endl;
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
    //while(connected == 0){
	connected = connect_to_server(serverAddress,serverPort);
    cout << "error connectiong to gate, code:" << connected << endl;
    //}

    //sends request to server
    char pleaseMessage[] = {'P','l','e','a','s','e'};
    send_to_server(pleaseMessage);

    //reccives password from server
    char password[24];
    receive_from_server(password);
    cout << "password:" << password << endl;

    //sends password to server
    send_to_server(password);
}

void followLine(){
    take_picture();
    
    rowInfo topRow = blackRowRead(topOfBox);
    rowInfo bottomRow = blackRowRead(bottomOfBox);

    int deltaX = topRow.averageX-bottomRow.averageX; // change in x of the averages
    int deltaY = bottomOfBox - topOfBox; //change in y (diffrance between top and bottom of box)
	
    double gradent;
    if(deltaX != 0){
		gradent = deltaY / deltaX; // the diffrance between the middle of the top and bottom
	}else{
		gradent = 0;
    }
    //draws the box of where it is checking
    drawBox(leftOfBox,rightOfBox,topOfBox-1,bottomOfBox+1);

    //sets the top average pixel to red
    set_pixel(topOfBox, topRow.averageX, 255, 0, 0);
    //sets the bottom average pixel to red
    set_pixel(bottomOfBox, bottomRow.averageX, 255, 0, 0);
    
    
	cout << "motor drive started" << endl;
	if(gradent == 0){
		//set_motors(leftMotorPort,45);
		//set_motors(rightMotorPort,45);
	}
		
	if(gradent > 0){
		cout << "go right" << endl;
		
		//sets the left motor speed 
		//motorDrive(320,0,gradent,false,leftMotorPort);
		//sets the right motor speed
		//motorDrive(320,0,gradent,true,rightMotorPort);
		
	}
	if(gradent < 0){
		
		cout << "go left" << endl;
		//sets the left motor speed 
		//motorDrive(320,0,abs(gradent),true,leftMotorPort);
		//sets the right motor speed
		//motorDrive(320,0,abs(gradent),false,rightMotorPort);
	}
    
	cout << "motor drive finished" << endl;
    //updates the screen
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
	
	int err = init(0);
	cout << "error:" << err << endl;
	open_screen_stream();
	
    //openGate();
    cout << "open gate passed" << endl;

	for(int k = 0; k < 100; k +=1){
        followLine();
        //take_picture();
        //for(int currentX =0; currentX < totalXPixels; currentX +=1){
		//	for(int currentY =0; currentY < totalYPixesl; currentY +=1){
		//		isblack(currentX,currentY);
		//	}
		//}
        
        
        sleep1(20);
    }
    
    //set_motors(leftMotorPort,45);
    //set_motors(rightMotorPort,45);

    cout << "follow line passed" << endl;

    
    return 0;
}
