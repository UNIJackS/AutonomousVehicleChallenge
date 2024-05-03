
//refrance page : https://ecs.wgtn.ac.nz/Courses/ENGR101_2024T1/AVC_manuals
//This doccument uses camelCase 


//------- Imports -------
#include <iostream> // input-output library 
#include "E101.h" // VUW camera library

using namespace::std;


//------- Global Varables ---------
//used for camera reading
const int totalXPixels = 320;
const int totalYPixesl = 240;

const int leftMotorPort = 1;  //port number for the left motor
const int rightMotorPort = 2; //port number for the right motor
const int cameraMotorPort = 3;//port number for the camera motor

const float blackTolarance = 10; // The value by which red green and blue can differ and still be black
const int maxIntensity  = 20; // The maximum value of red + green + blue values to still be black


//Sets the edges of the box to check
const int leftOfBox = 50; //px
const int rightOfBox = 100; //px
const int topOfBox = 50; //px
const int bottomOfBox = 100; //px



char[] serverAddress = {} // server adress

//used to store instructions to drive each motor
struct motorInstructions{
    int  minValue;
    int  maxValue;
    int  currentValue;
    int  port;
    bool isContinus; //true if this is a continus motor; flase if it is not
};

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
        output.rowList[currentXPixel] = isblack(currentXPixel,rowToRead)
        output.lastIndex = currentXPixel;
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

//takes a motor drive struct 
//returns nothing
void motorDrive(motorInstructions instructions){
    if(motorInstructions.maxValue > motorInstructions.minValue){
        int range  = motorInstructions.maxValue - motorInstructions.minValue;
        int value = motorInstructions.currentValue - motorInstructions.minValue;


        set_motors(motorInstructions.port,pwm);


    }else{
        cout << "min is greater than max in motor drive call" << endl;
    }
    

    
}







//------- Dynamic Functions ---------
void openGate(){
    //attempts to connect to gate, will loop untill connected
    int connected = 0;
    while(connected == 0){
        connected = connect_to_server( char serverAddress[15],int port);
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
    int topRowAverage = blackRowRead(topOfBox);
    int bottomRowAverage = blackRowRead(bottomOfBox);




    


    update_screen();
}
void intersections(){
    
}
void pushPole(){
    
}


//------- Main ---------
int main() {
    openGate()
    cout << "open gate passed" << endl;
    
    return 0;
}