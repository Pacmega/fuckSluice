#include <iostream>
#include <signal.h>

#include "Sluice.h"
#include "lib/returnValues.h"

int currentSluice = 0;
Sluice normalSluice1 = Sluice(5555);
Sluice normalSluice2 = Sluice(5556);
Sluice fastLockSluice = Sluice(5557);
Sluice pulseMotorSluice = Sluice(5558);

void ctrlCHandler(int sig){
    switch(currentSluice)
    {
        case 1:
            std::cout << "\nEmergency button pressed for sluice 1 (standard)." << std::endl;
            normalSluice1.passInterrupt();
            break;
        case 2:
            std::cout << "\nEmergency button pressed for sluice 2 (standard)." << std::endl;
            normalSluice2.passInterrupt();
            break;
        case 3:
            std::cout << "\nEmergency button pressed for sluice 3 (locking doors)." << std::endl;
            fastLockSluice.passInterrupt();
            break;
        case 4:
            std::cout << "\nEmergency button pressed for sluice 4 (different motor)." << std::endl;
            pulseMotorSluice.passInterrupt();
            break;
        default:
            std::cout << "\nNo sluice selected to stop." << std::endl;
            break;
    }
}

void entryExitInterpreter(int value)
{
    switch(value)
    {
        case success:
            std::cout << "Action completed." << std::endl;
            break;
        case incorrectDoorState:
            std::cout << "Door is not currently in a state where it can be moved." << std::endl;
            break;
        case incorrectWaterLevel:
            std::cout << "Water level is not at a level where passing through can be allowed." << std::endl;
            break;
        case noAckReceived:
            std::cout << "A message was not received by the simulator." << std::endl;
            break;
        case interruptReceived:
            std::cout << "An emergency stop was triggered." << std::endl;
            break;
        case invalidLightState:
            std::cout << "An invalid light state (not green or red) was returned by the simulator." << std::endl;
            break;
        case motorDamaged:
            std::cout << "Door is damaged. Unable to open." << std::endl;
            break;
        default:
            std::cout << "Warning - sluice returned an unknown value: " << value << std::endl;
            break;
    }
}

void startInterpreter(int value)
{
    switch(value)
    {
        case success:
            std::cout << "Sluicing process completed." << std::endl;
            break;
        case invalidCall:
            std::cout << "It is not possible to do this right now. Is the emergency mode active?" << std::endl;
            break;
        case incorrectWaterLevel:
            std::cout << "Can't start moving a boat that can't possibly have entered." << std::endl;
            break;
        case noAckReceived:
            std::cout << "A message was not received by the simulator." << std::endl;
            break;
        case interruptReceived:
            std::cout << "An emergency stop was triggered." << std::endl;
            break;
        case invalidWaterLevel:
            std::cout << "An invalid water level was returned by the simulator." << std::endl;
            break;
        case invalidLightState:
            std::cout << "An invalid light state (not green or red) was returned by the simulator." << std::endl;
            break;
        default:
            std::cout << "Warning - sluice returned an unknown value: " << value << std::endl;
            break;
    }
}

int main(int argc, char const *argv[])
{
    signal (SIGINT,&ctrlCHandler);

    int choice = ' ';
    
    int rtnval;
    char line[1];
    while (choice != 'q')
    {
        std::cout << "\n==Menu==\n"
        << "[1] Manage sluice 1 (standard)\n"
        << "[2] Manage sluice 2 (standard)\n"
        << "[3] Manage sluice 3 (locking doors)\n"
        << "[4] Manage sluice 4 (different motor)\n"
        << "[q] Quit\n"
        << "Enter your choice: ";
        std::cin >> line;
        choice = line[0];
        std::cout << std::endl;

        switch (choice)
        {
            case '1':
                currentSluice = 1;
                std::cout << "\n==Sluice 1 (standard)==\n";
                while (choice != '9')
                {
                    std::cout << "[1]   Allow entry\n"
                              << "[2]   Move boat up/down\n"
                              << "[3]   Allow exiting\n"
                              << "[9]   Return to main menu\n"
                              << "Enter your choice: ";
                    std::cin >> line;
                    choice = line[0];
                    std::cout << std::endl;
                    switch (choice)
                    {
                        case '1':
                            std::cout << "Allowing entry into sluice.\n" << std::endl;
                            rtnval = normalSluice1.allowEntry();
                            entryExitInterpreter(rtnval);
                            break;
                        case '2':
                            std::cout << "Moving boat up or down, depending on current position.\n" << std::endl;
                            rtnval = normalSluice1.start();
                            startInterpreter(rtnval);
                            break;
                        case '3':
                            std::cout << "Allowing exiting the sluice.\n" << std::endl;
                            rtnval = normalSluice1.allowExit();
                            entryExitInterpreter(rtnval);
                            break;
                        default:
                            std::cout << "Invalid input." << std::endl;
                            break;
                    }
                }
                break;
            case '2':
                currentSluice = 2;
                std::cout << "\n==Sluice 2 (standard)==\n";
                while (choice != '9')
                {
                    std::cout << "[1]   Allow entry\n"
                              << "[2]   Move boat up/down\n"
                              << "[3]   Allow exiting\n"
                              << "[9]   Return to main menu\n"
                              << "Enter your choice: ";
                    std::cin >> line;
                    choice = line[0];
                    std::cout << std::endl;
                    switch (choice)
                    {
                        case '1':
                            std::cout << "Allowing entry into sluice.\n" << std::endl;
                            rtnval = normalSluice2.allowEntry();
                            entryExitInterpreter(rtnval);
                            break;
                        case '2':
                            std::cout << "Moving boat up or down, depending on current position.\n" << std::endl;
                            rtnval = normalSluice2.start();
                            startInterpreter(rtnval);
                            break;
                        case '3':
                            std::cout << "Allowing exiting the sluice.\n" << std::endl;
                            rtnval = normalSluice2.allowExit();
                            entryExitInterpreter(rtnval);
                            break;
                        default:
                            std::cout << "Invalid input." << std::endl;
                            break;
                    }
                }
                break;
            case '3':
                currentSluice = 3;
                std::cout << "\n==Sluice 3 (locking doors)==\n";
                while (choice != '9')
                {
                    std::cout << "[1]   Allow entry\n"
                              << "[2]   Move boat up/down\n"
                              << "[3]   Allow exiting\n"
                              << "[9]   Return to main menu\n"
                              << "Enter your choice: ";
                    std::cin >> line;
                    choice = line[0];
                    std::cout << std::endl;
                    switch (choice)
                    {
                        case '1':
                            std::cout << "Allowing entry into sluice.\n" << std::endl;
                            rtnval = fastLockSluice.allowEntry();
                            entryExitInterpreter(rtnval);
                            break;
                        case '2':
                            std::cout << "Moving boat up or down, depending on current position.\n" << std::endl;
                            rtnval = fastLockSluice.start();
                            startInterpreter(rtnval);
                            break;
                        case '3':
                            std::cout << "Allowing exiting the sluice.\n" << std::endl;
                            rtnval = fastLockSluice.allowExit();
                            entryExitInterpreter(rtnval);
                            break;
                        default:
                            std::cout << "Invalid input." << std::endl;
                            break;
                    }
                }
                break;
            case '4':
                currentSluice = 4;
                std::cout << "\n==Sluice 4 (different motor)==\n";
                while (choice != '9')
                {
                    std::cout << "[1]   Allow entry\n"
                              << "[2]   Move boat up/down\n"
                              << "[3]   Allow exiting\n"
                              << "[9]   Return to main menu\n"
                              << "Enter your choice: ";
                    std::cin >> line;
                    choice = line[0];
                    std::cout << std::endl;
                    switch (choice)
                    {
                        case '1':
                            std::cout << "Allowing entry into sluice.\n" << std::endl;
                            rtnval = pulseMotorSluice.allowEntry();
                            entryExitInterpreter(rtnval);
                            break;
                        case '2':
                            std::cout << "Moving boat up or down, depending on current position.\n" << std::endl;
                            rtnval = pulseMotorSluice.start();
                            startInterpreter(rtnval);
                            break;
                        case '3':
                            std::cout << "Allowing exiting the sluice.\n" << std::endl;
                            rtnval = pulseMotorSluice.allowExit();
                            entryExitInterpreter(rtnval);
                            break;
                        default:
                            std::cout << "Invalid input." << std::endl;
                            break;
                    }
                }
                break;
            case 'q':
                std::cout << "Shutting down." << std::endl;
                break;
            default:
                std::cout << "Invalid input." << std::endl;
                break;
        }
    }
    return 0;
}