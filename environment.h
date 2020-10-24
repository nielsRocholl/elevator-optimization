/* === Environment === */

/* Data struct */
#include <string.h>

/* Constructors */
char* nameGenerator(){
	char* names[10] = {"Freddy","Sjon","Gerrie","Evert","Marloes","Knor","Pieter","Tamar","Boes","Riemer"};
	int n = rand() % 10;
	return names[n];
}

passenger makePassenger(int start, int goal){
	passenger unit;
	unit.name = nameGenerator();
	unit.currentFloor = start;
	unit.goalFloor = goal;
	unit.waitingTime = 0;
	unit.totalWaitingTime = 0;
	unit.activeWaitingTime = 0;
	unit.inElevator = false;
	return unit;
}

/* Passenger has spawned at a floor and presses the button to go up or down */
int pingSignal(passenger p){
	if (p.currentFloor < p.goalFloor)
		return 1;
	if (p.currentFloor > p.goalFloor)
		return -1;
	return 1;
}

/* Remove in elevator passengers from list */
List removeInElevator(List li){
	List li1 = li;
	while (li1 != NULL){
		if (li1->p.inElevator){
			li = removeItem(li1->p, li);
		}
		li1 = li1->next;
	}
	return li;
}

/* Spawn passenger according to a set of rules */
passenger spawnPassenger(List waiting, int numFloors){
	int* occupiedFloors = (int*) calloc(numFloors, sizeof(int)); 
	List li1 = waiting;
	for (int i = 0; i < listSize(waiting); i++){	// If [floor] = false -> free
		occupiedFloors[li1->p.currentFloor] = 1;
		li1 = li1->next;
	}
	int start = rand() % numFloors;
	while(occupiedFloors[start]){					// If [floor] = false -> free
		start = (start + 1) % numFloors;
	}
	int goal = rand() % numFloors;
	while (goal == start){							// can make random guesses as complexity will not increase. 
		goal = rand() % numFloors;
	}

	return makePassenger(start, goal);
}

/* Increases waiting time for all people not in an elevator */
List tickPassengers(List li){
	List li1 = li;
	while (li1 != NULL){
		li1->p.waitingTime += 1;
		li1->p.totalWaitingTime += 1;
		li1->p.activeWaitingTime += 1;
		li1 = li1->next;
	}
	return li;
}

/* Increase waiting time for all people in an elevator */
List tickOccupants(elevator e){
	List li1 = e.occupants;
	while (li1 != NULL){
		li1->p.totalWaitingTime += 1;
		li1->p.activeWaitingTime += 1;				// decreased in the reward function, as it mirrors the negative reward 
		li1 = li1->next;
	}
	return e.occupants;
}

/* Passengers in the elevator move with the elevator */
List moveAlong(List li, int direction){
	List li1 = li;
	while (li1 != NULL){
		li1->p.currentFloor += direction;
		li1 = li1->next;
	}
	return li;
}


