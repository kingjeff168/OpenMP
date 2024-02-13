/*
Last Date Modified: 1/9/2024
*/
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <chrono>
#include <cctype>

#include <omp.h>

using namespace std;

vector<string> vInputChargeDimension;
vector<string> vInputSeparationCharge;
vector<string> vInputCharge;
vector<string> vInputTargetLocation;
vector<string> vInputThreads;
vector<double> v1; // Save the difference between charges and the target point in x-direction.
vector<double> v2; // Save the difference between charges and the target point in y-direction.
int qPositionMatch; // Check if the target point is on the charge.
double Ex = 0; // Electric field in the x-direction.
double Ey = 0; // Electric field in the y-direction.
double Ez = 0; // Electric field in the z-direction.
double positionZ = 0;
double kConstant; // Coulomb's constant multiply with charge because it would be a constant after setting up by the user.


void splitInput(const string& input, char space, vector<string>& results)
{
    // This function splits the input through detecting the space, and it puts every piece of string in the vector.
    bool isInteger = true;
    if (input.empty())
    {
        isInteger = false;
    }
    else
    {
        string token;
        istringstream tokenStream(input);

        while (getline(tokenStream, token, space)) 
        {
            results.push_back(token);
        }
    }

}

bool isPositiveInteger(const string& str) 
{
    // This for-loop is to check if the input is an integer.
    for (char c : str) 
    {
        if (!isdigit(c)) 
        {
            return false;
        }
    }
    return true;
}

bool isValidInput(const string& str) 
{
    // This for-loop is to check if the input has char or other invalid things than numbers.      
    istringstream iss(str);
    double value;
    
    if (str.empty()) 
    {
        return false;
    }

    iss >> noskipws >> value;

    if (!iss.fail() && iss.eof()) 
    {
        return true;
    }
    return false;
}

void convertToScientificNotation(double input, double & number, int & exponent) 
{
    exponent = 0;
    number = input;

    // This function is to convert numbers to scientific notation.
    // It will check the number, if it is bigger than 10, it will be divided, and the exponent will be added up until it is smaller than 10.
    // if input is zero, all variable should be 0.
    // If the number is smaller than 1, it will be multiplied, and the exponent will be deducted until it is bigger than 1.
    if (abs(input) >= 1) 
    {
        while (abs(number) >= 10.0) 
        {
            number /= 10.0;
            exponent++;
        }
    }
    else if (input == 0)
    {
        number = 0;
    }
    else 
    {
        while (abs(number) < 1.0) 
        {
            number *= 10.0;
            exponent--;
        }
    }

    number = round(number * 10000) / 10000;
}

int checkPositionMatch(double x, double y, double z)
{
    // If all positions in x, y ,and z are no difference, which equals to 0 and return true, and it means 
    // the target point has same coordinates of a charge.
    double alpha = 1e-6;
    qPositionMatch = 0;
    if ((abs(x) < alpha) && (abs(y) < alpha) && (abs(z) < alpha))
    { 
        qPositionMatch = 1;
        return 1;
    }
    return 0;
}

void setLocation(double x, double y, double z, double separationX, double separationY, int N, int M)
{
    double positionX = 0;
    double positionY = 0;

    v1.clear();
    v2.clear();


    // This 2-level for-loop will calculate the position of the charge, 
    // and then it calculate the difference between the charge and target points in x and y direction.
    // This function will split the position into x and y direction respectively.
    for (int i = 0; i < M; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            // For if-statement, if it is even number, need to use this part to get the x value of top-left point, 
            // and it will go through the whole row using (separationX * j).
            // For else-statement, if it is odd number, use this part to get the x value of top-left point, 
            // and it will go through the whole row using (separationX * j).
            if (N % 2 == 0)
            {
                positionX = (-1)*((((N / 2) - 1) * separationX) + (separationX/2)) + (separationX * j);
            }
            else
            {
                positionX = (-1)*((((N - 1) / 2) * separationX)) + (separationX * j);
            }

            positionX = x - positionX;
            
            // For if-statement, if it is even number, need to use this part to get the y value of top-left point, 
            // and it will go through the whole row using (separationY * i).
            // For else-statement, if it is odd number, use this part to get the x value of top-left point, 
            // and it will go through the whole row using (separationY * i).
            if (M % 2 == 0)
            {
                positionY = ((((M / 2) - 1) * separationY) + (separationY/2)) - (separationY * i);
            }
            else
            {
                positionY = (((M - 1) / 2) * separationY) - (separationY * i);
            }

            positionY = y - positionY;


            // Check whether any charge has the same value as the target point.
            positionZ = z;
            if(checkPositionMatch(positionX, positionY, positionZ) == 1)
            {
                break;
            }

            // Save the postion of x and y coordinates, which are already deducted with target point, in vector respectively.
            v1.push_back(positionX);
            v2.push_back(positionY);

        }
        if(checkPositionMatch(positionX, positionY, positionZ) == 1)
        {
            break;
        }
    }
}


int main()
{
    string inputThreads, inputArray, inputSeparation, inputCharge, inputLocation, inputYesNo;
    double numberX, numberY, numberZ, numberTotal, dX, dY, xPosition, yPosition, zPosition;
    double sCharge;
    int exponentX, exponentY, exponentZ, exponentTotal, numThreads;
    uint64_t N, M;
    char cSpace = ' ';


// From here, users enter the variables they want.
    while (1)
    {
        while (1)
        {
            // This while loop is to check if the user enter the right numbers of threads.
            vInputThreads.clear();
            inputThreads.clear();
            cout << "Please enter the number of concurrent threads to use: ";
            getline(cin, inputThreads);
            if (inputThreads.length() == 0)
            {
                cout << "Invalid input!" << endl; 
            }
            else
            {
                splitInput(inputThreads, cSpace, vInputThreads);
                if (vInputThreads.size() == 1)
                {
                    if (isPositiveInteger(vInputThreads[0]) == true)
                    {
                        // If the input is passed, it will check whether the numThreads is 0 because it is invalid.
                        numThreads = stoi(vInputThreads[0]);
                        omp_set_num_threads(numThreads);
                        if (numThreads == 0)
                        {
                            // If the number of threads is 0, it will clear the vector and back to the beginning of this loop.
                            vInputThreads.clear();
                            inputThreads.clear();
                            cout << "Invalid input!" << endl; 
                        }
                        else
                        {
                            // If the input is passed all, it will leave this loop.
                            break;
                        }
                    }
                    else
                    {
                        // If the input is invalid, it will clear the vector and back to the beginning of this loop.
                        vInputThreads.clear();
                        inputThreads.clear();
                        cout << "Invalid input!" << endl;
                    }
                }
                else
                {
                    // If the size of input is invalid, it will clear the vector and back to the beginning of this loop.
                    vInputThreads.clear();
                    inputThreads.clear();
                    cout << "Invalid input!" << endl;
                }
            }
        }

        while (1)
        {
            // This while loop is to check if the user enter right number of arguments of N and M.
            vInputChargeDimension.clear();
            inputArray.clear();
            cout << "Please enter the number of rows and columns in the N x M array: ";
            getline(cin, inputArray);
            if (inputArray.length() == 0)
            {
                cout << "Invalid input!" << endl; 
            }
            else
            {
                // If the user enters more than two parameters, it will say "Invalid input!" and ask the user to enter new parameters again.
                splitInput(inputArray, cSpace, vInputChargeDimension);
                if (vInputChargeDimension.size() == 2)
                {
                    if ((isPositiveInteger(vInputChargeDimension[0]) == true) && (isPositiveInteger(vInputChargeDimension[1]) == true))
                    {
                        // If the input is passed, it will check whether the N or M is 0 because it is invalid.
                        N = stoi(vInputChargeDimension[0]);
                        M = stoi(vInputChargeDimension[1]);
                        if ((N == 0) || (M == 0))
                        {
                            // If either N or M is 0, it will clear the vectors first and then back to the beginning of the loop.
                            vInputChargeDimension.clear();
                            inputArray.clear();
                            cout << "Invalid input!" << endl; 
                        }
                        else
                        {
                            // If the input is passed all, it will leave this loop.
                            break;
                        }
                    }
                    else
                    {
                        // If 2 inputs are not positive integers, it will clear the vectors first and then back to the beginning of this loop.
                        vInputChargeDimension.clear();
                        inputArray.clear();
                        cout << "Invalid input!" << endl;
                    }
                }
                else
                {
                    // If numbers of inputs are more than 2, it will clear the vectors first and then back to the beginning of this loop.
                    vInputChargeDimension.clear();
                    inputArray.clear();
                    cout << "Invalid input!" << endl;
                }
            }
        }

        while (1)
        {
            // This while loop is to check if the user enter right arguments of separation of x and y.
            vInputSeparationCharge.clear();
            inputSeparation.clear();
            cout << "Please enter the x and y separation distances in meters: ";
            getline(cin, inputSeparation);
            if (inputSeparation.length() == 0)
            {
                cout << "Invalid input!" << endl; 
            }
            else
            {
                // If the user enters more than two parameters, it will say "Invalid input!" and ask the user to enter new parameters again.
                splitInput(inputSeparation, cSpace, vInputSeparationCharge);
                if (vInputSeparationCharge.size() == 2)
                {
                    // If the input is passed, it will check whether the x or y separation is smaller than 0 because it is invalid.
                    if ((isValidInput(vInputSeparationCharge[0]) == true) && (isValidInput(vInputSeparationCharge[1]) == true))
                    {
                        dX = stod(vInputSeparationCharge[0]);
                        dY = stod(vInputSeparationCharge[1]);
                        if ((dX <= 0) || (dY <= 0))
                        {
                            // If the values of separation in x and y-direction are smaller than 0, it will let the user enter new values again.
                            vInputSeparationCharge.clear();
                            inputSeparation.clear();
                            cout << "Invalid input!" << endl;
                        }
                        else
                        {
                            // If the input is passed all, it will leave this loop.
                            break;
                        }
                    }
                    else
                    {
                        // It finds that the values of separation are not valid, it will clear the vector first and then back to the beginning of this loop.
                        vInputSeparationCharge.clear();
                        inputSeparation.clear();
                        cout << "Invalid input!" << endl;
                    }
                }
                else
                {
                    //Because input parameters are more than 2, it will clear the vector first and then back to the beginning of this loop.
                    vInputSeparationCharge.clear();
                    inputSeparation.clear();
                    cout << "Invalid input!" << endl;
                }
            }
        }
        
        while (1)
        {
            vInputCharge.clear();
            inputCharge.clear();
            cout << "Please enter the common charge on the points in micro C: ";
            getline(cin, inputCharge);
            if (inputCharge.length() == 0)
            {
                inputCharge.clear();
                cout << "Invalid input!" << endl; 
            }
            else
            {
                // If the user enters more than one parameter, it will say "Invalid input!" and ask the user to enter a new parameter again.
                splitInput(inputCharge, cSpace, vInputCharge);
                if (vInputCharge.size() == 1)
                {
                    sCharge = stod(vInputCharge[0]);
                    if ((isValidInput(vInputCharge[0]) == true) && (sCharge != 0))
                    {
                        // If the input is passed, it will leave this loop.
                        cout << endl;
                        break;
                    }
                    else
                    {
                        // If the input isn't passed, it will let the user enter new values again.
                        vInputCharge.clear();
                        inputCharge.clear();
                        cout << "Invalid input!" << endl; 
                    }
                }
                else
                {
                    // If there is unexpected error, it will come here, and it will let the user enter new values again.
                    vInputCharge.clear();
                    inputCharge.clear();
                    cout << "Invalid input!" << endl; 
                }
            }
        }
        
        while (1)
        {
            while(1)
            {
                vInputTargetLocation.clear();
                inputLocation.clear();
                v1.clear();
                v2.clear();
                cout << "Please enter the location in space to determine the electric field (x y z) in meters: ";
                getline(cin, inputLocation);
                if (inputLocation.length() == 0)
                {
                    inputLocation.clear();
                    cout << "Invalid input!" << endl; 
                }
                else
                {
                    // If the user enters more than 3 parameters, it will say "Invalid input!" and ask the user to enter a new parameter again.
                    splitInput(inputLocation, cSpace, vInputTargetLocation);
                    if(vInputTargetLocation.size() == 3)
                    {
                        if ((isValidInput(vInputTargetLocation[0]) == true) && (isValidInput(vInputTargetLocation[1]) == true) && (isValidInput(vInputTargetLocation[2]) == true))
                        {
                            // If the inputs are passed, we will start to set up the value for calculation of the electric field.
                            xPosition = stod(vInputTargetLocation[0]);
                            yPosition = stod(vInputTargetLocation[1]);
                            zPosition = stod(vInputTargetLocation[2]);
                            kConstant = sCharge * 9000;
                            setLocation(xPosition, yPosition, zPosition, dX, dY, N, M);

                            // Once the position of charges are calculated, we will check whether the target point is at the charge.
                            if ((qPositionMatch) == 1)
                            {
                                vInputTargetLocation.clear();
                                inputLocation.clear();
                                cout << "Invalid location! Location matches a point charge position!" << endl;
                            }
                            else
                            {
                                // If the target point is not at the charge, then it will leave this loop.
                                break;
                            }
                        }
                        else
                        {
                            // If any parameters are invalid, it will clear the vector and back to the beginning this loop.
                            vInputTargetLocation.clear();
                            inputLocation.clear();
                            cout << "Invalid input!" << endl; 
                        }
                    }
                    else
                    {
                        // If number of input parameters is more than 3, it will clear the vector and back to the beginning this loop.
                        vInputTargetLocation.clear();
                        inputLocation.clear();
                        cout << "Invalid input!" << endl; 
                    }
                }
            }
        
            // Here is to calculate the time and electric field.
            Ex = 0;
            Ey = 0;
            Ez = 0;
            chrono::high_resolution_clock::time_point stopTimer;
            chrono::high_resolution_clock::time_point startTimer;

            // Calculate the electric field using OpenMP.
            #pragma omp parallel
            {
                // Start the time.
                #pragma omp master
                {
                    startTimer = chrono::high_resolution_clock::now();
                }
                
                // Different Threads will have their local copy of Ex, Ey and Ez. Each thread will do their own calculation.
                // At the end of each iteration of the loop, the values ​​of these local copies will be summed to the global Ex, Ey, and Ez.
                #pragma omp for reduction(+:Ex, Ey, Ez)
                for (int i = 0; i < v1.size(); ++i)
                {
                    double dEx;
                    double dEy;
                    double dEz;
                    double r = sqrt((v1[i] * v1[i]) + (v2[i] * v2[i]) + (positionZ * positionZ));
                    double r3 = r * r * r;
                

                    dEx = (kConstant*(v1[i]))/(r3); // Temporary value of electric field in x-direction. 
                    dEy = (kConstant*(v2[i]))/(r3); // Temporary value of electric field in y-direction. 
                    dEz = (kConstant*(positionZ))/(r3); // Temporary value of electric field in z-direction. 

                    Ex += dEx;
                    Ey += dEy;
                    Ez += dEz;
                }

                // Stop the time.
                #pragma omp master
                {
                    stopTimer = chrono::high_resolution_clock::now();
                }
            
            }
            auto totalTime = chrono::duration_cast<chrono::microseconds>(stopTimer - startTimer);

            // From here, the program will output the results.    
            cout << "The electric field at (" << xPosition << ", " << yPosition << ", " << zPosition << ") in V/m is " << endl;

            convertToScientificNotation(Ex, numberX, exponentX);
            convertToScientificNotation(Ey, numberY, exponentY); 
            convertToScientificNotation(Ez, numberZ, exponentZ);
            double eTotal = sqrt(((Ex * Ex) + (Ey * Ey) + (Ez * Ez)));
            convertToScientificNotation(eTotal, numberTotal, exponentTotal);
            
            cout << "Ex = " << numberX << " * 10^" << exponentX << endl;
            cout << "Ey = " << numberY << " * 10^" << exponentY << endl;
            cout << "Ez = " << numberZ << " * 10^" << exponentZ << endl;
            cout << "|E| = " << numberTotal << " * 10^" << exponentTotal << endl;

            cout << "The calculation took "<< totalTime.count() << " microsec!" << endl;

            while (1)
            {
                // This loop is to check whether the user wants to calculate the electric field again.
                vInputTargetLocation.clear();
                inputLocation.clear();
                cout << "Do you want to enter a new location (Y/N)? ";
                getline(cin, inputYesNo);
                if (inputYesNo == "y" || inputYesNo == "Y")
                {
                    // If the user says yes, then it will do it again.
                    inputYesNo.clear();
                    vInputTargetLocation.clear();
                    inputLocation.clear();
                    cout << endl;
                    break;
                }
                else if (inputYesNo == "n" || inputYesNo == "N")
                {
                    // If the user says no, it will break out the loop and terminate the program.
                    cout << "Bye!" << endl;
                    break;
                }
                else
                {
                    inputYesNo.clear();
                    cout << "Invalid input!" << endl;
                }
            }
            if (inputYesNo == "n" || inputYesNo == "N")
            {
                break;
            }
        }
        if (inputYesNo == "n" || inputYesNo == "N")
        {
            break;
        }
    }
    return 0;
}