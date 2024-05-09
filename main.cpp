
//refrance page : https://ecs.wgtn.ac.nz/Courses/ENGR101_2024T1/AVC_manuals
//This doccument uses camelCase 


//------- Imports -------
#include <iostream> // input-output library 
#include <math.h> //used for rounding function
#include "E101.h" // VUW camera library
#include <chrono>

using namespace::std;


//------- Global Varables ---------
//used for camera reading
const int totalXPixels = 320;
const int totalYPixesl = 240;

//Motor varables
const int leftMotorPort = 5;  //port number for the left motor
const int rightMotorPort = 3; //port number for the right motor
const int cameraMotorPort = 1;//port number for the camera motor
const bool flipMotors = true;

//Black detection Varables
const float blackTolarance = 10; // The value by which red green and blue can differ and still be black
const int maxIntensity = 60; // The maximum value of red + green + blue values to still be black

const int leftOfBox = 20; //px
const int rightOfBox = 300; //px
const int topOfBox = 20; //px
const int bottomOfBox = 200; //px

const int maxOffSet = rightOfBox - leftOfBox;



char serverAddress[15] = { '1','3','0','.','1','9','5','.','3','.','5','3' }; // server address
int serverPort = 1024; //server port

//used to store instructions to drive each motor


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

//takes the row number
//returns a rowInfo struct
int readRow(int rowToRead) {

    bool firstBlackPixel = true;

    int lastBlackPixelX;
    int firstBlackPixelX;

    for (int currentXPixel = leftOfBox; currentXPixel < rightOfBox; currentXPixel += 1) {
        if (isblack(currentXPixel, rowToRead)) {
            //sets the first black pixel if this is the first black pixel detected
            if (firstBlackPixel) {
                output.firstBlackPixelX = currentXPixel;
                firstBlackPixel = false;
            }
            lastBlackPixelX = currentXPixel;
        }
    }

    return (lastBlackPixelX - firstBlackPixelX) / 2 + firstBlackPixelX;
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
    if (flipMotors) {
        set_motors(leftMotorPort, 65 - (pwm - 31));
        set_motors(rightMotorPort, pwm);
    }
    else {
        set_motors(leftMotorPort, pwm);
        set_motors(rightMotorPort, 65 - (pwm - 31));
    }
}





//------- Dynamic Functions ---------
void openGate() {
    //attempts to connect to gate
    connected = connect_to_server(serverAddress, serverPort);
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
    take_picture();

    //draws the box of where it is checking
    drawBox(leftOfBox, rightOfBox, topOfBox - 1, bottomOfBox + 1);

    //returns the time in milliseconds since the epoch
    long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    long long timeBewtweenMeasueres = currentTime - prevousTime;

    //The distance from the center of the screen to the average black pixel
    double error = (double)(readRow(bottomOfBox) - totalXPixels / 2);

    //sets the bottom average pixel to red
    set_pixel(bottomOfBox, error, 255, 0, 0);

    //Gains for the PID
    double kp = 0.25; //the gain of the proption section
    double ki = 0.25; //the gain of the Integral section
    double kd = 0.25; //the gain fo the derivative section
    double output = kp * error + ki * ((error * timeBewtweenMeasueres) + totalPastIntegral) + kd * ((error - prevousError) / timeBewtweenMeasueres);

    cout << "output:" << output << endl;

    //Sets values for next time
    totalPastIntegral += error * totalPastIntegral;
    prevousTime = currentTime;
    prevousError = error;

    int pwm = 55 + output;
    //restricts pwm to writable values
    if (pwm > 65) {
        pwm = 65;
    }
    else if (pwm < 30) {
        pwm = 30;
    }

    if(flipMotors){
        set_motors(leftMotorPort, 65 -(pwm-31));
        set_motors(rightMotorPort, pwm);
    }
    else {
        set_motors(leftMotorPort, pwm);
        set_motors(rightMotorPort, 65 - (pwm - 31));
    }
    
    
    update_screen();
    hardware_exchange();
}
void intersections() {



}
void pushPole() {

}


//------- Main ---------
int main() {

    int err = init(0);
    cout << "error:" << err << endl;
    open_screen_stream();

    //set_motors(leftMotorPort,56);
    //set_motors(rightMotorPort,40);
    //hardware_exchange();

    openGate();
    cout << "open gate passed" << endl;


    long long prevousTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double totalPastIntegral = 0;
    double prevousError = 0;
    while (true) {
        testMotors();

        //followLine(prevousTime, totalPastIntegral, prevousError);


        sleep1(20);
    }

    cout << "follow line passed" << endl;


    return 0;
}
