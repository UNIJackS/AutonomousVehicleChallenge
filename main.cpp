

//refrance page : https://ecs.wgtn.ac.nz/Courses/ENGR101_2024T1/AVC_manuals
//This doccument uses camelCase 


//------- Imports -------
#include <iostream> // input-output library 
#include <math.h> //used for rounding function
#include "E101.h" // VUW camera library
#include <chrono> //used for 

using namespace  std;

//------------------------- Global Varables ---------------------------
int cruisingSpeed = 55; //the speed the robot moves at 

//Line following varables
const double kp = 0.047; //the gain of the proption section
const double ki = 0.0; //the gain of the Integral section
const double kd = 3.3; //tthe gain of the derivative section

const int rowToCheck = 120; //The row that is checked to generate error

//Read varables
const int leftOfBox = 20;    //px
const int rightOfBox = 300;  //px
const int topOfBox = 20;     //px
const int bottomOfBox = 200; //px

//Motor varables
const int leftMotorPort = 5;  //port number for the left motor
const int rightMotorPort = 3; //port number for the right motor
const int cameraMotorPort = 1;//port number for the camera motor
const bool flipMotors = false;

//Black detection Varables
const float blackTolarance = 10; // The value by which red green and blue can differ and still be black
const int maxIntensity = 60; // The maximum value of red + green + blue values to still be black

//Red detection Varables
const float redTolarance =1.5;
const int redThreshold = 2000; // The number of red pixels required for the section to change 


//Dont Change These varables
//used for camera reading
const int totalXPixels = 320;
const int totalYPixels = 240;

char turnOrder = {"R","R","L","L","R","L","R"};

char serverAddress[15] = { '1','3','0','.','1','9','5','.','3','.','5','3' };  // server address
int serverPort = 1024;  //server port
//------------------------- Testing Functions ---------------------------

//Check motor polarity
void testMotors() {
    int pwm = 55;
    cout << "if flipMotors is true" << endl;
    if (true) {
        set_motors(leftMotorPort, 65 - (pwm - 31));
        set_motors(rightMotorPort, pwm);
    }
    else {
        set_motors(leftMotorPort, pwm);
        set_motors(rightMotorPort, 65 - (pwm - 31));
    }
    hardware_exchange();
    cout << "driving for 5 seconds ..." << endl;
    sleep1(5000);

    cout << "if flipMotors is false" << endl;
    if (false) {
        set_motors(leftMotorPort, 65 - (pwm - 31));
        set_motors(rightMotorPort, pwm);
    }
    else {
        set_motors(leftMotorPort, pwm);
        set_motors(rightMotorPort, 65 - (pwm - 31));
    }
    cout << "driving for 5 seconds ..." << endl;
    hardware_exchange();
    sleep1(5000);
   
}




//------------------------- Static Functions ---------------------------

//Checks if a pixel is black, makes black pixels green
bool isBlack(int x, int y) {
    int red = get_pixel(y, x, 0);
    int green = get_pixel(y, x, 1);
    int blue = get_pixel(y, x, 2);
    int intensity = get_pixel(y, x, 3);

    //checsk if the red green and blue values are within + or - blackTolarance of each other 
    //and intesity is less than the maximum intesity
    if ((red - blackTolarance < green || red + blackTolarance > green)
        && (red - blackTolarance < blue || red + blackTolarance > blue)
        && (green - blackTolarance < blue || green + blackTolarance > blue)
        && intensity > maxIntensity) {
        return false;

    }
    else {
        //sets pixels detected as black to green
        set_pixel(y, x, 0, 255, 0);
        return true;
    }
}

void printScreen(){
    int pixelsPerChunk = 5;

    cout << "totalXPixels" << totalXPixels << endl;
    cout << "totalYPixels" << totalYPixels << endl;
    cout << "pixelsPerChunk" << pixelsPerChunk << endl;
    cout << "number of x chunks" << totalXPixels/pixelsPerChunk << endl;
    cout << "number of y chunks" << totalYPixels/pixelsPerChunk << endl;
    
    for(int currentXChunk = 0; currentXChunk < totalXPixels/pixelsPerChunk; currentXChunk +=1){
        for(int currentYChunk = 0; currentYChunk < totalYPixels/pixelsPerChunk; currentYChunk +=1){
            //cout << currentXChunk << "," <<currentYChunk <<" ";
            int blackPixelsInChunk = 0;
            int nonBlackPixelsInChunk = 0;
            for(int currentXPixel = currentXChunk*pixelsPerChunk; currentXPixel < pixelsPerChunk + currentXChunk*pixelsPerChunk; currentXPixel +=1){
                for(int currentYPixel = currentYChunk*pixelsPerChunk; currentYPixel < pixelsPerChunk + currentYChunk*pixelsPerChunk; currentYPixel +=1){
                    //cout << currentXPixel << currentYPixel << endl;
                    if(isBlack(currentXPixel,currentYPixel)){
                        blackPixelsInChunk +=1;
                    }else{
                        nonBlackPixelsInChunk +=1;
                    }

                }
            }
            
            
            if(blackPixelsInChunk >= nonBlackPixelsInChunk){
                cout << "1";
            }else{
                cout << "0";
            }
            
        }
        cout << endl;
    }
     cout << endl<< endl<< endl<< endl<< endl;

     set_motors(leftMotorPort, 48);
     set_motors(rightMotorPort, 48);
     hardware_exchange();
     
     int input =0;
     cin >> input;
    
}

//checks if a pixel is red, makes red pixels blue
bool isRed(int x,int y){
    //cout << "x:" << x << "y:" << y << endl;
	int red = (int)get_pixel(y,x, 0);
	int blue = (int)get_pixel(y,x, 1);
	int green = (int)get_pixel(y,x, 2);
	
	int intesity = green+blue;
	
	
	if(red > redTolarance*blue && red >redTolarance*green && intesity>5){
		set_pixel(y,x,0,0,255); 
		return(true);
	}
	return false;
}

//Checks if a row or column contains red or black
//ToRead = the row or column to check
//present = if at least 1 pixel of the color is detected
//col = if a column is being read
//returns the mean of the black pixels in the row or col
int read(int ToRead,bool &present) {

	int firstPixel = leftOfBox;
	int finalPixel = rightOfBox;
	int totalPixels = totalXPixels;

    int count = 0;
    int total = 0;

    for (int currentPixel = firstPixel; currentPixel < finalPixel; currentPixel += 1) {
        //cout << "x:" <<  currentPixel << "y:" << ToRead << endl;
        if (isBlack(currentPixel,ToRead)) {
            count +=1;
            total += currentPixel;
        }

    }
    
    if(count > 0 ){
        //set_pixel(ToRead, (total/count), 255, 0, 0);
		return ((total/count)- totalPixels / 2);
	}else{
		present = false;
		return 160;
	}    
}

//Draws a green or red box to the screen
void drawBox(int left, int right, int top, int bottom,bool green) {
    int greenValue =0;
    int redValue = 255;
    if(green){
        greenValue = 255;
        redValue = 0;
    }
    //This draws the yellow left and right lines
    for (int currentRow = top; currentRow < bottom; currentRow += 1) {
        set_pixel(currentRow, left, redValue, greenValue, 0);
        set_pixel(currentRow, right, redValue, greenValue, 0);
    }
    //This draws the blue top and bottom lines in yellow
    for (int currentCol = left; currentCol < right; currentCol += 1) {
        set_pixel(top, currentCol, redValue, greenValue, 0);
        set_pixel(bottom, currentCol, redValue, greenValue, 0);

    }
}

//Checks if a box contains over a threshold of a colour of pixel
bool checkBox(bool checkBlack,int threshhold,int left,int width,int top, int height ){
    int total = 0;
    for(int currectXPixel = left; currectXPixel < left+width; currectXPixel +=1){
        for(int currectYPixel = top; currectYPixel < top +height; currectYPixel +=1){
            if(checkBlack){
                if(isBlack(currectXPixel,currectYPixel)){
                    total+=1;
                }
            }else{
                if(isRed(currectXPixel,currectYPixel)){
                    total+=1;
                }
            }
        }
    }
    
    drawBox(left, left+width, top , top+height,total > threshhold);
    return (total > threshhold);
}
    

//Drives the left and right motors
void driveMotors(int leftOutput, int rightOutput,int diffarental){
    //checks if the diffarentla is too large
	if(diffarental > 65 - cruisingSpeed){
		diffarental = 65 - cruisingSpeed;
	}else if(diffarental < -(65 - cruisingSpeed)){
		diffarental = -(65 - cruisingSpeed);
	}
	//flips the motors and applys the diffarental the opposite way
    if(flipMotors){
        set_motors(leftMotorPort, 65 -((leftOutput-diffarental)-31));
        set_motors(rightMotorPort, (rightOutput+diffarental));
    }
    else {
        set_motors(leftMotorPort, (leftOutput+diffarental));
        set_motors(rightMotorPort, 65 - ((rightOutput-diffarental)- 31));
    }
}
//Initiates a left turn
void turnLeft(){
    cout << "Left turn called" << endl;
    bool left = true;
    //drives forward untill no more pixels are detected in left side
    while(left){
        take_picture();
        left = checkBox(true,300,70,20,110,100);

        driveMotors(55,55,0);

        hardware_exchange();
        update_screen();
        sleep1(20);
    }
    cout << "Left no longer detected" << endl;

    long long timeTurnStarted = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    bool forward = true;
    while(forward){
        take_picture();

        //Only starts checking the box after 500 milli seconds
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeTurnStarted > 400){
            cout << "Searching for black" << endl;
            forward = !checkBox(true,300,70,20,50,20);
        }else{
            cout << "not Searching for black" << endl;
        }
        
        driveMotors(48,48,-5);

        hardware_exchange();
        update_screen();
        sleep1(20);
    }

    //printScreen();
    cout << "left turn finished" << endl;

    




}
//Initiates a right turn
void turnRight(){
    cout << "Right turn called" << endl;
    bool right = true;
    //drives forward untill no more pixels are detected in left side
    while(right){
        take_picture();
        right = checkBox(true,300,230,20,110,100);

        driveMotors(55,55,0);

        hardware_exchange();
        update_screen();
        sleep1(20);
    }
    cout << "Right no longer detected" << endl;

    long long timeTurnStarted = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    bool forward = true;
    while(forward){
        take_picture();

        //Only starts checking the box after 500 milli seconds
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeTurnStarted > 400){
            cout << "Searching for black" << endl;
            forward = !checkBox(true,300,230,20,50,20);
        }else{
            cout << "not Searching for black" << endl;
        }
        
        driveMotors(48,48,5);

        hardware_exchange();
        update_screen();
        sleep1(20);
    }
    cout << "Right turn finished" << endl;

}
//Turns the robot back around
void turnAround(){
    //turns the robot untill the top black pixel is in the center 
    bool forward = true;
    while(forward){
        take_picture();

        forward = !checkBox(true,300,150,20,50,20);

        driveMotors(55,41,0);

        hardware_exchange();
        update_screen();
        sleep1(20);
    }
}



//------------------------- Dynamic Functions ---------------------------

//Checks if the number of red pixels in the image is above the red threshold
bool sectionChange(){
    return(!checkBox(false,redThreshold,0,totalXPixels-1,0,totalYPixels-1));
} 

void openGate() {
    //attempts to connect to gate
    int connected = connect_to_server(serverAddress, serverPort);
    if (connected != 0) {
        cout << "error connectiong to gate, code:" << connected << endl;
    }
    //sends request to server for password
    char pleaseMessage[] = { 'P','l','e','a','s','e' };
    send_to_server(pleaseMessage);
    //reccives password from server
    char password[24];
    receive_from_server(password);
    cout << "password:" << password << endl;
    //Sends password back to server
    send_to_server(password);
}

void followLine(long long &prevousTime, double &totalPastIntegral,double &prevousError) {
    //https://www.ctrlaltftc.com/the-pid-controller/the-integral-term
    //returns the time in milliseconds since the epoch
    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    long long timeBewtweenMeasueres = currentTime - prevousTime;

    bool blackpresent = true;
    //The distance from the center of the screen to the average black pixel (center is 160)
    double error = (double)(read(rowToCheck,blackpresent));

	if(blackpresent){

		double output = kp * error + ki * ((error * timeBewtweenMeasueres) + totalPastIntegral) + kd * ((error - prevousError) / timeBewtweenMeasueres);
		//Sets values for next time
		totalPastIntegral += error * totalPastIntegral;
		prevousTime = currentTime;
		prevousError = error;
		
        driveMotors(cruisingSpeed,cruisingSpeed,round(output));
	}
}

void intersections(long long &timeOfLastTurn, long long &lastTimeAnyDetected) {
    bool left = checkBox(true,300,50,20,110,20);
    bool right = checkBox(true,300,250,20,110,20);

    bool forward = checkBox(true,300,150,20,50,20);
    bool back = checkBox(true,300,150,20,170,20);

    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    

    if (currentTime-timeOfLastTurn < 5000){
        cruisingSpeed = 49;
    }else{
        cruisingSpeed = 55;
        if(left){
            timeOfLastTurn = currentTime;
            cout <<"Left turn avalable" << endl;
            turnLeft();
        }else if(right){
            timeOfLastTurn = currentTime;
            cout <<" right turn avlable" << endl;
            turnRight();
        }

    }

    
    bool blackpresent = true;

    (double)(read(200,blackpresent));

    if(blackpresent){
        lastTimeAnyDetected = currentTime;
        cout << "something detectd" << endl;
    }

    if(currentTime-lastTimeAnyDetected >500){
        lastTimeAnyDetected = currentTime;
        timeOfLastTurn = currentTime;
        
        cout <<"blackpresent for 500  millisecond" << endl;
        turnAround();
    }
    
    

}

void pushPole() {

}


//------- Main ---------
int main() {
    set_motors(1, 48);
    set_motors(3, 48);
    set_motors(5, 48);
    int err = init(0);
    cout << "error:" << err << endl;
    open_screen_stream();
    //Opens the gate
    //openGate();
    cout << "Gate Opened, Following line" << endl;

    //flips the camera up
    set_motors(cameraMotorPort, 50);
    hardware_exchange();
    
    long long prevousTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double totalPastIntegral = 0;
    double prevousError = 0;
    //follows the line
    bool change = true;
    /*while(change){
		take_picture();

		followLine(prevousTime, totalPastIntegral, prevousError);
        change = sectionChange();

        hardware_exchange();
        update_screen();
        sleep1(20);
    }
    cout << "First section change detected" << endl;
    
    sleep1(1000);
    */
    
    cout << "Intersection code starting" << endl;
    long long timeOfLastTurn = 0;
    long long lastTimeAnyDetected = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    change = true;
    while(change){
		take_picture();


        followLine(prevousTime, totalPastIntegral, prevousError);
		intersections(timeOfLastTurn,lastTimeAnyDetected);

        cout << cruisingSpeed << endl;

        hardware_exchange();
        update_screen();
        sleep1(20);
	}
    cout << "Second section change detected" << endl;

    return 0;
}
