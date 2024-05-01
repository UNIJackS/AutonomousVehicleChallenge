
//refrance page : https://ecs.wgtn.ac.nz/Courses/ENGR101_2024T1/AVC_manuals



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

//used to store instructions to drive each motor
struct motorInstructions{
    int  minValue;
    int  maxValue;
    int  currentValue;
    int  port;
    bool isContinus; //true if this is a continus motor; flase if it is not
}

//------------------------- Static Functions ---------------------------
//takes 0,1,2,3 for red,green,blue and intesity
//returns a 2d array of the iamge
int[][] wholeImageRead(int type){
    int[][] output2DList = new int[totalXPixels][totalYPixesl]
    for(int currentXPixel =0; currentXPixel<=totalXPixels; currentXPixel+=1){
        for(int currentYPixel =0; currentYPixel<=totalYPixels; currentYPixel+=1){
            output2DList[currentXPixel][currentYPixel] = get_pixel(currentXPixel,currentYPixel,type)
        }
    }
}

//takes the row number to read and 0,1,2 or 3 for red,green,blue and intesity
//returns a array of the row
int[] rowRead(int rowToRead,int type){
    int[] output = new int[totalXPixesl]
    for(int currentXPixel =0; currentXPixel<=totalXPixels; currentXPixel+=1){
        output[currentXPixel] = get_pixel(rowToRead,currentYPixel,type)
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




//------- Main ---------
int main() {
    take_picture();




    update_screen();
    return 0;
}