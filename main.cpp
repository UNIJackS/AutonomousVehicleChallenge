
//refrance page : https://ecs.wgtn.ac.nz/Courses/ENGR101_2024T1/AVC_manuals
//This doccument uses camelCase 


//------- Imports -------
#include <iostream> // input-output library 
#include <math.h> //used for rounding function
#include "E101.h" // VUW camera library
#include <chrono> //used for 

using namespace  std;


//------- Global Varables ---------
//Line following varables
const double kp = 0.055; //the gain of the proption section
const double ki = 0.0; //the gain of the Integral section
const double kd = 0.0; //tthe gain of the derivative section

const int rowToCheck = 120; //The row that the fucntion checks to generate its error

/*
kp values testing
0.5 worked but excesive reversing due to too low
0.53 worked but still can go higher
0.55 worked well everywhere but the final big turn
0.6 is too far was ossliationg /over turning at some corners
0.58 worked best

found that the intergtil and derivative where not needed (works so why change)

the camera postion affected this alot as well so i took a photo of the positon that worked best
*/


//Intersection varables
const int leftOfBox = 20; //px
const int rightOfBox = 300; //px
const int topOfBox = 20; //px
const int bottomOfBox = 200; //px


//Motor varables
const int leftMotorPort = 5;  //port number for the left motor
const int rightMotorPort = 3; //port number for the right motor
const int cameraMotorPort = 1;//port number for the camera motor
const bool flipMotors = false;

//Black detection Varables
const float blackTolarance = 10; // The value by which red green and blue can differ and still be black
const int maxIntensity = 60; // The maximum value of red + green + blue values to still be black



//Dont Change These varables
//used for camera reading
const int totalXPixels = 320;
const int totalYPixels = 240;

char serverAddress[15] = { '1','3','0','.','1','9','5','.','3','.','5','3' }; // server address
int serverPort = 1024; //server port



//------------------------- Static Functions ---------------------------

//takes an the x and y value of a pixel and checks if its black
//if it is then it makes it green
bool isblack(int x, int y) {
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

//Takes
//the row or column number to check
//A pointer for if there is at least 1 black pixel
//If it is to check a column
//returns the mean of the black pixels in the row or col
int read(int ToRead,bool &present,bool col) {

    int count = 0;
    int total = 0;
    
    int firstPixel;
    int finalPixel;
    int totalPixels;
    if(col){
		//cout << "column being checked" << ToRead << endl;
		firstPixel = topOfBox;
		finalPixel = bottomOfBox;
		totalPixels = totalYPixels;
	}else{
		//cout << "row being checked" << ToRead << endl;
		firstPixel = leftOfBox;
		finalPixel = rightOfBox;
		totalPixels = totalXPixels;
	}
	

    for (int currentPixel = firstPixel; currentPixel < finalPixel; currentPixel += 1) {
		if(col){
			if (isblack(ToRead, currentPixel)) {
            count +=1;
            total += currentPixel;
			}
			
		}else{
			if (isblack(currentPixel, ToRead)) {
            count +=1;
            total += currentPixel;
			}
		}
        
    }
    
    

    if(count > 0 ){
		if(col){
			set_pixel((total/count),ToRead , 255, 0, 0);
		}else{
			set_pixel(ToRead, (total/count), 255, 0, 0);
		}
		
		return ((total/count)- totalPixels / 2);
	}else{
		present = false;
		return 160;
	}    
}

//This takes a left, rght, top and bottom side and draws a box
void drawBox(int left, int right, int top, int bottom) {
    //This draws the green left and right lines
    for (int currentRow = 0; currentRow < (bottom - top); currentRow += 1) {
        set_pixel(top + currentRow, left, 0, 255, 0);
        set_pixel(top + currentRow, right, 0, 255, 0);
    }
    //This draws the blue top and bottom lines
    for (int currentCol = 0; currentCol < (right - left); currentCol += 1) {
        set_pixel(top, left + currentCol, 0, 0, 255);
        set_pixel(bottom, left + currentCol, 0, 0, 255);

    }
}

//used to double check motor polarity
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





//------- Dynamic Functions ------``---
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

    send_to_server(password);
}

void followLine(long long &prevousTime, double &totalPastIntegral,double &prevousError) {
    //https://www.ctrlaltftc.com/the-pid-controller/the-integral-term
    

    //draws the box of where it is checking
    drawBox(leftOfBox, rightOfBox, topOfBox - 1, bottomOfBox + 1);

    //returns the time in milliseconds since the epoch
    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    long long timeBewtweenMeasueres = currentTime - prevousTime;

    bool blackpresent = true;

    //The distance from the center of the screen to the average black pixel (center is 160)
    double error = (double)(read(rowToCheck,blackpresent,false));

	if(blackpresent){
		
		//REMOVE
		cout << "error:" << error << endl;
		//REMOVE

		double output = kp * error + ki * ((error * timeBewtweenMeasueres) + totalPastIntegral) + kd * ((error - prevousError) / timeBewtweenMeasueres);


		//REMOVE
		cout << "output:" << output << endl;
		//REMOVE

		//Sets values for next time
		totalPastIntegral += error * totalPastIntegral;
		prevousTime = currentTime;
		prevousError = error;

		int intOutput = round(output);
		
		if(intOutput > 10){
			intOutput = 10;
		}else if(intOutput < -10){
			intOutput = -10;
		}
		
		if(flipMotors){
			set_motors(leftMotorPort, 65 -((55-intOutput)-31));
			set_motors(rightMotorPort, 55 +intOutput);
		}
		else {
			set_motors(leftMotorPort, 55 +intOutput);
			set_motors(rightMotorPort, 65 - ((55-intOutput) - 31));
		}
		
		
	//Reversese if no black pixels are detected
	}else{
		int intOutput = 40;
		if(flipMotors){
			set_motors(leftMotorPort, 65 -(intOutput-31));
			set_motors(rightMotorPort, intOutput);
		}
		else {
			set_motors(leftMotorPort,intOutput);
			set_motors(rightMotorPort, 65 - (intOutput- 31));
		}
	}
}

void intersections() {
    drawBox(leftOfBox-1, rightOfBox+1, topOfBox - 1, bottomOfBox + 1);

    bool forward = true;
    bool left = true;
    bool right = true;

    double forwardError = (double)(read(topOfBox,forward,false));
    double leftError = (double)(read(leftOfBox,left,true));
    double rightError = (double)(read(rightOfBox,right,true));


    if(left && leftError < 10 && leftError > -10){
        cout <<"Left turn avalable" << endl;
    }

    if(right && rightError < 10 && rightError > -10 ){
        cout <<" right turn avlable" << endl;
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

	//testMotors();

    //Opens the gate
    //openGate();




    long long prevousTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double totalPastIntegral = 0;
    double prevousError = 0;
    /*while (true) {
		take_picture();
		followLine(prevousTime, totalPastIntegral, prevousError);
		
		update_screen();
		hardware_exchange();
        sleep1(20);
    }
    */
    while(true){
		take_picture();
		intersections();
		update_screen();
		hardware_exchange();
		sleep1(20);
	}

    return 0;
}
