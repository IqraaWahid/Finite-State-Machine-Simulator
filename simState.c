#include <stdio.h> // allows printf and fgets
#include <string.h> // allows string operations (strlen, strcspn)
#include <stdlib.h> // allows other functions (exit())
#include <ctype.h> // allows character type finctions (toupper())

#define MAX_STATES 8 // maximum number of states the machine can have (A, B, C, D, E, F, G, H)

// the following structure represents a single state in the state machine
typedef struct {
    char name;           // Stores the name of the current state (ex. 'A', 'B', 'C', ...)
    char next0;          // Stores the next state to move to on input '0'
    char next1;          // Stores the next state to move to on input '1'
    int deleted;         // Determines if a state has been deleted (0 --> not deleted and 1 --> deleted)
    int reachable;       // Used to check for garbage state (1 --> reacheable, 0 --> unreachable)
} State; // creates a new data type called "State" which is used later

// Global variables:
State states[MAX_STATES]; // array storing all states of the state machine
int numStates = 0; // stores number of states of the state machine (updates each time) 
char currentState; // storing the current state of the state machine

//--------- functions used in the main method ---------:
// reads the "machine.txt" file and fills the states array
void readMachineFile() {
    FILE *file = fopen("machine.txt", "r"); //open the file "machine.txt" in reading mode
    
    //if file does not open, print an error message terminate the program
    if (file == NULL) {
        printf("Error: Cannot open machine.txt\n");
        exit(1); // terminate the program
    }
    
    char line[100]; // stores each line read from the file
    numStates = 0; // reset the number of states to zero, before reading the file
    
    // read the file line by line until end of file (EOF) has been reached or until MAX_STATES number of lines have been read 
    while (fgets(line, sizeof(line), file) != NULL && numStates < MAX_STATES) {
        char name, n0, n1; // to temporarily store each lines information from file
        if (sscanf(line, "%c %c %c", &name, &n0, &n1) == 3) {
            
            // assign values to the state arrays at index numStates (creating a structure for each state)
            states[numStates].name = name; // setting states name at index 'numStates'
            states[numStates].next0 = n0; // setting next state when input is zero at index 'numStates'
            states[numStates].next1 = n1; // setting next state when input is one at index 'numStates'
            states[numStates].deleted = 0; //marking the state as UNdeleted
            states[numStates].reachable = 0; // intialize the state to be unreachable
            numStates++; // increase number of states loaded each time
        }
    }
    fclose(file); //close the machine.txt" file
}

// searches through the 'states' array and returns the index of the state with the name given in the formal
// parameters, only if the state exists (is undeleted); otherwise, is returns -1 and ends the function
int findStateIndex(char name) {
    for (int i = 0; i < numStates; i++) { //checking the "states" array
        if (states[i].name == name && !states[i].deleted) { //if the name of the state matches one in the "states" array AND that state is not deleted
            return i; //return the index of that state
        }
    }
    return -1; // if not found return -1
}

// searches through the 'states' array and returns the index of the state with the name given in the formal
// parameters (even if the state is deleted)
int findStateIndexIncludeDeleted(char name) {
    for (int i = 0; i < numStates; i++) { //searches through the array
        if (states[i].name == name) { // if state name from array matches that of the formal parameter
            return i; //return the index of the state
        }
    }
    return -1; //if no matches found, return -1
}

// prints the current state
void printCurrentState() {
    printf("%c\n", currentState); //prints the current state to the terminal (stdout)
}

// this function updates the current state value based on the user input '0' or '1'
void processZeroOne(char input) {
	
    int index = findStateIndex(currentState); // find and store the index of the currentState by calling the findStateIndex(currentState) method
    if (index == -1) return; // if currentState not found return nothing (ends function)
    
    char nextState; //variable to store the appropriate nextState value
    // determine nextState of the currentState depending on the input
    if (input == '0') { // if input is '0' set 'nextState' value accordingly
        nextState = states[index].next0;
    }
    else { // otherwise, if input is '1' set 'nextState' value accordingly
        nextState = states[index].next1; // the a states array at given index's 'next1' value if stored into the nextState since input is 1  
    }
    
    printf("%c\n", nextState); // output the nextState onto the terminal (stdout)
    currentState = nextState; // update the currentState value to the new one (nextState)
}

// appropriately handles the change command
// this function changes the transition of the current state. For example, change input '1' of the currentState to lead to a different state
void processChange(char input, char newState) {
    // Find the current state
    int currentIndex = -1;
    
    //find index of the currenState in the array
    for (int i = 0; i < numStates; i++) {
        if (states[i].name == currentState && !states[i].deleted) { //if found: name matches and state is NOT deleted
            currentIndex = i; //set currentIndex to that idex
            break; // stop searching (end for loop)
        }
    }
    
    if (currentIndex == -1) return; // if current state not found, do nothing leave the function
    
    // Other wise, update the transition based on the user input
    if (input == '0') { //if input is zero
        states[currentIndex].next0 = newState; // change the 'next0' value of the 'states' array at index 'currentIndex' to the newState chosen by user
    }
    else if (input == '1') { // otherwise, if user inputted '1'
        states[currentIndex].next1 = newState; // update the 'next1' value of the array at the index 'currentIndex' to what user wants to change it to
    }
}

// prints all undeleted states present and their transitions
void printConfiguration() {
    for (int i = 0; i < numStates; i++) { // go through the whole array
        if (!states[i].deleted) { // if states are NOT deleted
            printf("%c %c %c\n", states[i].name, states[i].next0, states[i].next1); // output to the terminal (stdout) the state's name and its transitions when '0' or  '1' selected
			// format of the output: <stateName> <next0> <next1>
        }
    }
}

// resets reachability determination so they can be redetermined through the markReachable() method
void resetReachable() {
    for (int i = 0; i < numStates; i++) {
        states[i].reachable = 0; //resetting 'reachable' value of all elements in the 'states' array
    }
}

// starts from the given formal parameter state, and recursively marks every state that can be reached through transitions
void markReachable(char state) {
    int index = findStateIndexIncludeDeleted(state); // find the index of the given state
    if (index == -1 || states[index].reachable || states[index].deleted) {
        return; // return nothing (end function) if not found in the array
    }
    
    states[index].reachable = 1; //otherwise, set reachability as 1 (meaning reachable)
    // recursively follow both transitions and determine their reachability 
    markReachable(states[index].next0);
    markReachable(states[index].next1);
}

// find the garbage (unreachable) states
void processGarbage() {
    resetReachable(); // firstly, reset Reacability of the states
    markReachable(currentState); // starting from the current state, mark all the state's reachability  
    
    int garbageFound = 0; // default value be zero, and remain zero if no garbage is found
    char garbage[MAX_STATES]; // array that will store the names of the garbage states
    int garbageCount = 0; // counter to track the number of states that can garbage collected
    
    for (int i = 0; i < numStates; i++) { // go trhough the 'states' array
        if (!states[i].deleted && !states[i].reachable) { // if a state in the 'states' array is NOT deleted and is marked NOT reachable 
            garbage[garbageCount++] = states[i].name; // store those state's names into the garbage array and simultaneously increase the size of the garbage array 
            garbageFound = 1; // set this variable to 1 if garbage found
        }
    }
    
    if (!garbageFound) { // if no garbage found (garbageFound = 0) output accordingly
        printf("No garbage\n");
    }
    else { // otherwise, if garbage was found
        printf("Garbage: ");
        for (int i = 0; i < garbageCount; i++) { // print out into the terminal (stdout) the garbage array (garbage/unreachable states)
            printf("%c", garbage[i]);
        }
        printf("\n");
    }
}

// deletes all garbage (unreachable) states
void processDeleteAll() {
    resetReachable(); // firstly, reset reacability of the states
    markReachable(currentState); // starting from the current state, mark all the state's reachability properly
    
    char deleted[MAX_STATES]; // array to store the names of the unreachable states that get deleted
    int deletedCount = 0; // counter to track the number of states that get deleted
    
    for (int i = 0; i < numStates; i++) { // search the 'states' array
        if (!states[i].deleted && !states[i].reachable) { // if there is a UNdeleted and NONreachable state
            states[i].deleted = 1; // change that state's deleted status to 1 (meaning deleted)
            deleted[deletedCount++] = states[i].name; // add that state's name into the array that stores all deleted states, whilst increasing the number of deleted states 
        }
    }
    
    if (deletedCount == 0) { // if no deleted states
        printf("No states deleted\n");
    }
    else { // otherwise, if there are deleted states
        printf("Deleted: ");
        for (int i = 0; i < deletedCount; i++) { //output the deleted[i] array meaning the array that stores the deleted states
            printf("%c", deleted[i]);
        }
        printf("\n");
    }
}

// delete a specific state given the stateToDelete (only if it is unreachable)
void processDeleteSpecific(char stateToDelete) {
    int index = findStateIndexIncludeDeleted(stateToDelete); // obtain the index of the stateToDelete
    
    if (index == -1 || states[index].deleted) { // if state is not found, return nothing, meaning end function
        printf("Not deleted\n");
        return;
    }
    
    resetReachable(); // otherwise, reset reachability
    markReachable(currentState); // recompute the reachability of all states from the currentState
    
    if (states[index].reachable) { // if stateToDelete is reachable
        printf("Not deleted\n");
    }
    else { // if stateToDelete is UNreachable
        states[index].deleted = 1; // state deleted status is changed to 1 (meaning deleted)
        printf("Deleted\n");
    }
}

// main method: where the program starts to execute
int main() {
    readMachineFile(); // reads all the states and their respective transitions and stores them into the 'states' array
    currentState = 'B';  // assigning the starting state of my customized state machine
    printCurrentState(); // printing the starting state (B) to the terminal (stdout)
    
    char line[100]; // character array to store user input commands
    // reads user input until the program exits; 'fgets' reads an entire input line until enter key is pressed 
    while (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = 0; // removing the newline character "\n" which is added by the fgets function
        if (strlen(line) == 0) continue; // if user entered nothing, skip and wait for next command
        
        switch(line[0]) { // switch case used to check the first character user entered and according to that performing steps
            case '0': // if user entered 0
            case '1': // or if user entered 1
                processZeroOne(line[0]); // perform state transition: moving to the next state depending on the input
                break;
                
            case 'c': // if user entered "c" --> change command
                // Format of this input: "c 0 X" or "c 1 X" (change zero/one transition of currentState to X)
                if (strlen(line) >= 4) { // if the input is greater than or equal to 4
                    char input = line[2]; // store the input 0 or 1 (determines the nextState) into the character variable 'input' 
                    char newState = toupper(line[4]); // covert the newState the user inputted to upper case (since it's a state)
                    if ((input == '0' || input == '1') && // checks if first (zero in array) character input is either a zero or one
                        newState >= 'A' && newState <= 'H') { // checks if the newState (fourth index character in the array) is beterrn 'A' and 'H' since thats where the states are up until 
                        processChange(input, newState); // allow transition change
                    }
                }
                break;
                
            case 'p': // if user enters 'p' as first character input
                printConfiguration();  // print the current configuration of the machine all reachable stated and their transitions)
                break;
                
            case 'g': // if user enters 'g' as first character input
                processGarbage(); // call garbage detection function
                break;
                
            case 'd': // if user enters 'd' as first character input
                if (strlen(line) == 1) { // if user entered only 'd'
                    processDeleteAll(); // delete all UNreachable states
                }
                else if (strlen(line) == 2) { // if user entered like "dA" to delete state A
                    processDeleteSpecific(toupper(line[1])); // delete state A is valid (if its UNreachable)
                }
                else if (strlen(line) == 3 && line[1] == ' ') { // if user entered like "d A" to delete state A
                    processDeleteSpecific(toupper(line[2])); // delete state A is valid (if its UNreachable)
                }
                break;
                
            case 'e': // otherwise when user enters "e" as first character input
                return 0; // terminate the program 
        }
    }
    return 0; // end program
}
