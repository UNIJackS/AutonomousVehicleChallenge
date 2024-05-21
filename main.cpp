

//refrance page : https://ecs.wgtn.ac.nz/Courses/ENGR101_2024T1/AVC_manuals
//This doccument uses camelCase 


//------- Imports -------
#include <iostream> // input-output library 
#include <math.h> //used for rounding function
#include "E101.h" // VUW camera library
#include <chrono> //used for 

using namespace  std;

//--------------------------- Tuneable varables ---------------------------
const float blackTolarance = 10; // The value by which red green and blue can differ and still be black
const int maxBlackIntensity = 60; // The maximum value of red + green + blue values to still be black


const float blueTolarance = 10;
const int maxBlueIntensity = 150;

const float redTolarance = 1.5;
const float greenTolarance = 1.5;

int cruisingSpeed = 55; //the speed the robot moves at 

const int redThreshold = 2000; // The number of red pixels required for the section to change 

//Line following varables
const double lkp = 0.047; //the gain of the proption 
const double lki = 0.0; //the gain of the Integral 
const double lkd = 3.3; //tthe gain of the derivative 
const int rowToCheck = 120; //The row that is checked to generate error

//Intersection Varables
char turnOrder[] = {'R','R','L','L','R','L','R'};

//Pole pushing Varables
const double pkp = 0.047; //the gain of the proption 
const double pki = 0.0; //the gain of the Integral 
const double pkd = 3.3; //tthe gain of the derivative 
char poleOrder[] = {'R','G','B','R'}; //The order for colours to look for
int poleThreshhold = 190; //Number of pixels to be close enough to pole


//--------------------------- Dont Touch varables ---------------------------
const int leftMotorPort = 5;   //port number for the left motor
const int rightMotorPort = 3;  //port number for the right motor
const int cameraMotorPort = 1; //port number for the camera motor
const bool flipMotors = false; //Used in conjunction with testMotors to check which motor to reverse

const int totalXPixels = 320; // Total number of columns of pixels the camera reads
const int totalYPixels = 240; // Total number of row of pixels the camera reads

char serverAddress[15] = { '1','3','0','.','1','9','5','.','3','.','5','3' };  // server address
int serverPort = 1024;  //server port

//------------------------- Static Functions ---------------------------

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
        && intensity > maxBlackIntensity) {
        return false;

    }
    else {
        //sets pixels detected as black to green
        set_pixel(y, x, 0, 255, 0);
        return true;
    }
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

//checks if a pixel is red, makes red pixels blue
bool isGreen(int x,int y){
    //cout << "x:" << x << "y:" << y << endl;
	int red = (int)get_pixel(y,x, 0);
	int blue = (int)get_pixel(y,x, 1);
	int green = (int)get_pixel(y,x, 2);
		
	if(green > round(1.3*red) && green > 2*blue){
		set_pixel(y,x,0,0,255); 
		return(true);
	}
	return false;
}


//checks if a pixel is red, makes red pixels blue
bool isBlue(int x,int y){
    //cout << "x:" << x << "y:" << y << endl;
	int red = (int)get_pixel(y,x, 0);
	int blue = (int)get_pixel(y,x, 1);
	int green = (int)get_pixel(y,x, 2);
	
	int intesity = green+blue;
	
	
    if ((red - blueTolarance < green || red + blueTolarance > green)
        && (red - blueTolarance < blue || red + blueTolarance > blue)
        && (green - blueTolarance < blue || green + blueTolarance > blue)
        && intesity > maxBlueIntensity) {
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
    for(int currentXChunk = 0; currentXChunk < totalXPixels/pixelsPerChunk; currentXChunk +=1){
        for(int currentYChunk = 0; currentYChunk < totalYPixels/pixelsPerChunk; currentYChunk +=1){
            int blackPixelsInChunk = 0;
            int nonBlackPixelsInChunk = 0;
            for(int currentXPixel = currentXChunk*pixelsPerChunk; currentXPixel < pixelsPerChunk + currentXChunk*pixelsPerChunk; currentXPixel +=1){
                for(int currentYPixel = currentYChunk*pixelsPerChunk; currentYPixel < pixelsPerChunk + currentYChunk*pixelsPerChunk; currentYPixel +=1){
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

//Checks if a row or column contains red or black
//ToRead = the row or column to check
//present = if at least 1 pixel of the color is detected
//col = if a column is being read
//colour == the colour to check (K,R,G or B)
//returns the mean of the black pixels in the row or col
int readRow(int rowToRead,bool &present) {
    int count = 0;
    int total = 0;


    for (int currentPixel = 20; currentPixel < 300; currentPixel += 1) {
        if (isBlack(currentPixel,rowToRead)) {
            count +=1;
            total += currentPixel;
        }
    }


    
    if(count > 0 ){
        set_pixel(rowToRead, (total/count), 255, 0, 0);
		return ((total/count)- totalXPixels / 2);
	}else{
		present = false;
		return 160;
	}    
}


int readPoles(bool notSearching, char colour,int &countColumnsAboveThreshold){
    int threshHold = 130; // 70% of 240
    countColumnsAboveThreshold = 0;
    int totalValue = 0;

    cout << "colour:" << colour << endl;

    for (int currentXPixel = 0; currentXPixel < 320; currentXPixel += 1) {
        int pixelsInColumn = 0;

        for (int currentYPixel = 0; currentYPixel < 240; currentYPixel += 1) {
            switch (colour)
            {
            case 'R':
                if (isRed(currentXPixel,currentYPixel)) {
                    pixelsInColumn +=1;
                }
                break;

            case 'G':
                if (isGreen(currentXPixel,currentYPixel)) {
                    pixelsInColumn +=1;
                }
                break;

            case 'B':
                if (isBlue(currentXPixel,currentYPixel)) {
                    pixelsInColumn +=1;
                }
                break;

            }
        }
        if (pixelsInColumn > threshHold){
            set_pixel(totalYPixels/2, currentXPixel, 255, 0, 0);
            countColumnsAboveThreshold += 1;
            totalValue += currentXPixel;
        }
        
    }
    

    //cout << "countColumnsAboveThreshold" << countColumnsAboveThreshold << endl;
    if(countColumnsAboveThreshold > 0 ){
        
		return ((totalValue/countColumnsAboveThreshold)- totalXPixels / 2);
	}else{
		notSearching = false;
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

    long long timeTurnStarted = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    bool forward = true;
    while(forward){
        take_picture();
        //Only starts checking the box after 500 milli seconds
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeTurnStarted > 400){
            forward = !checkBox(true,300,70,20,50,20);
        }else{
        }
        driveMotors(48,48,-5);
        hardware_exchange();
        update_screen();
        sleep1(20);
    }
}

//Initiates a right turn
void turnRight(){
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
    long long timeTurnStarted = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    bool forward = true;
    while(forward){
        take_picture();
        //Only starts checking the box after 500 milli seconds
        if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeTurnStarted > 400){
            forward = !checkBox(true,300,230,20,50,20);
        }else{
        }
        
        driveMotors(48,48,5);

        hardware_exchange();
        update_screen();
        sleep1(20);
    }
}



//------------------------- Dynamic Functions ---------------------------

//Checks if the number of red pixels in the image is above the red threshold
bool sectionChange(){
    if(!checkBox(false,redThreshold,0,totalXPixels-1,0,totalYPixels-1)){
        return 0;
    }else{
        cout << "Section change tripped" << endl;
        driveMotors(55,55,0);
        hardware_exchange();
        sleep1(1000);
        return 1;
    }
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

void followLine(long long &prevousTime,double &prevousError) {
    //https://www.ctrlaltftc.com/the-pid-controller/the-integral-term
    //returns the time in milliseconds since the epoch
    bool blackpresent = true;
    //The distance from the center of the screen to the average black pixel (center is 160)
    double error = (double)(readRow(rowToCheck,blackpresent));

	if(blackpresent){
        long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		double output = lkp * error + lkd * ((error - prevousError) / (currentTime - prevousTime));
		//Sets values for next time
		prevousTime = currentTime;
		prevousError = error;
		
        driveMotors(cruisingSpeed,cruisingSpeed,round(output));
	}
}

void intersections(long long &timeOfLastTurn,int &turnNumber) {
    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if (currentTime-timeOfLastTurn < 4000){
        //Changes cursing speed for 4 seconds after a turn for auto correction
        cruisingSpeed = 49;
    }else{
        cruisingSpeed = 55;
        if(checkBox(true,300,50,20,110,20) && turnOrder[turnNumber] == 'L'){
            turnNumber+=1;
            cout << "left called" << endl;
            turnLeft();
            timeOfLastTurn = currentTime;
        }else if(checkBox(true,300,250,20,110,20) && turnOrder[turnNumber] == 'R'){
            turnNumber +=1;
            cout << "left called" << endl;
            turnRight();
            timeOfLastTurn = currentTime;
        }

    }
}

void pushPole(int &poleNumber,long long &prevousTime,double &prevousError) {
    int total = 0;
    bool notSearching = true;
    //Check clumns as well
    double error = (double)(readPoles(notSearching,poleOrder[poleNumber], total));

    if(notSearching){ //Drive towards pole
        long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double output = pkp * error + pkd * ((error - prevousError) / (currentTime - prevousTime));
        //Sets values for next time
        prevousTime = currentTime;
		prevousError = error;
        //drives motors towards pole
        driveMotors(55,55,round(output));
        //checks if pole is close enough 
        cout << "total" << total << endl;
        if(total > poleThreshhold){
            cout << "Pole reched reversing" << endl;
            poleNumber +=1;
            //reversese for 3 seconds from the pole
            driveMotors(41,41,0);
            hardware_exchange();
            sleep1(3000);
        }


    }else{ //Spin to search for pole
        driveMotors(48,48,5);
    }

    

}


//------- Main ---------
int main() {

    int err = init(0);
    cout << "error:" << err << endl;
    open_screen_stream();
    //Opens the gate
    openGate();


    
    while(true){
        take_picture();

        int red = (int)get_pixel(totalYPixels/2,totalXPixels/2, 0);
	    int blue = (int)get_pixel(totalYPixels/2,totalXPixels/2, 1);
	    int green = (int)get_pixel(totalYPixels/2,totalXPixels/2, 2);

        if(green > round(1.3*red) ){
            
        }

        if(green > 2*blue){
	    }

        cout << red << "," << blue << "," << green << endl;

        update_screen();
        sleep1(20);
    }


    //Varables for following line
    long long prevousTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double prevousError = 0;

    //Varables for intersections
    int turnNumber = 0;
    long long timeOfLastTurn = 0;

    //Varables for pole pushing
    int poleNumber = 0;
    long long polePrevousTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double polePrevousError = 0;

    //follows the line
    int sectionNumber = 3;
    while(true){
		take_picture();

        switch (sectionNumber)
        {
        case 1:
            set_motors(cameraMotorPort, 55); //lowers camera to look at line
            followLine(prevousTime, prevousError);
            sectionNumber += sectionChange(); //Adds one to go to the next switch case if true
            break;
        
        case 2:
            set_motors(cameraMotorPort, 55); //lowers camera to look at line
            followLine(prevousTime, prevousError);
		    intersections(timeOfLastTurn,turnNumber);
            sectionNumber += sectionChange(); //Adds one to go to the next switch case if true
            break;

        case 3:
            set_motors(cameraMotorPort, 35); //raises camera to look for poles
            cout << "poleNumber" << poleNumber << endl;
            pushPole(poleNumber,polePrevousTime, polePrevousError);
            break;

        }

        hardware_exchange();
        update_screen();
        sleep1(20);
    }

    return 0;
}
