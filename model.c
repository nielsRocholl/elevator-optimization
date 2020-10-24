/* Elevator problem == Reinforcement Learning Practical == Niels Rochell && Ivo Steegstra*/
#include "headers.h"

stats run(hyper h){

	// Initialize
	elevator e = makeElevator(0, h.numFloors, h.elevatorCapacity);
	List waiting = newEmptyList(); 	
	stats s = makeStatistics(h.epochs);

	// First round passenger
	passenger p = spawnPassenger(waiting, h.numFloors);
	e.floorSignals[p.currentFloor] = pingSignal(p);
	waiting = addItem(p, waiting);

	for (int ticks = 1; ticks < h.epochs; ticks++){
		int stateID = stateToID(e, h.numFloors, h.elevatorCapacity);	// state ID
		
		// Plan
		e = planAction(e, h, ticks);

		s.actions[ticks-1] = actionToInt(e.plannedAction);				// statistics
		s.actionCount[actionToInt(e.plannedAction)]++;					// statistics
		
		// Execute
		e = reward(e, waiting, h.R); 									// Reward = 'previous' state + planned action 
		e.totalReward += e.reward;

		if (e.plannedAction == UNLOAD)
			s = deliveryStats(s, e, ticks-1);
		s.stepReward[ticks-1] = e.reward;								// statistics
		s.cumReward[ticks-1] = e.totalReward;							// statistics

		e = executeAction(e, waiting);									// Execute = perform action, move passengers and elevator

		// Spawn + Observe
		if (ticks%h.spawnRate == 0 && listSize(waiting) < h.numFloors){	// Spawn = create new waiting
			passenger p = spawnPassenger(waiting, h.numFloors);
			e.floorSignals[p.currentFloor] = pingSignal(p);				// Ping floor with direction
			waiting = addItem(p, waiting);
		}

		// Update algorithm
		e = updateAlgorithm(e, h, stateID);

		// Tick
		waiting = removeInElevator(waiting); 							// remove passengers from the waiting list
		waiting = tickPassengers(waiting);								// increase waiting time for all on the waiting list
		e.occupants = tickOccupants(e);
	}

	printStats(s,e, h.epochs);

	// free memory
	freeList(waiting);
	freeList(e.occupants);
	deallocate_mem(&e.Q, h.numFloors * pow(3, h.numFloors) * pow((h.numFloors+1),h.elevatorCapacity));
	return s;
}

int main(int argc, char *argv[]){
	srand((unsigned int)time(NULL));
	// for viewing ease & easy control: 
	int runs = 10;				// CHANGE -- be careful with time: 10m cycles (runs * epoch) are about 6 seconds
	int epochs = 1000000;		// CHANGE
	int numFloors = 5;			// DON'T CHANGE | 5
	int numElevators = 1;		// DON'T CHANGE | 1
	int elevatorCapacity = 3;	// DON'T CHANGE | 3
	int R = 8;					// CHANGE
	int spawnRate = 8;			// CHANGE 
	double epsilon = 0.001; 	// CHANGE -- eps-greedy
	double alpha = 1.0;			// CHANGE -- 
	double gamma = 0.8;			// CHANGE --
	int algorithm = 1; 			// CHANGE 0 = basic, 1 = q-learning, 2 = SARSA, 3 = classic

	hyper h = makeHyperParameters(runs, epochs, numFloors, numElevators, elevatorCapacity, R, spawnRate, epsilon, alpha, gamma, algorithm);
	
	printf("Elevator Model\n %s\n Runs: %i\n Epochs: %i\n", intToAlgorithm(algorithm), runs, epochs);

	stats s[runs];
	for (int i = 0; i < runs; i++){
		s[i] = run(h);	
	}

	// print stats
	multiRunStats(s, h);
	freeStats(s, runs);	
	return 0;
}

/* PSEUDOCODE

Make elevators

time loop 
	observe environment / recieve signals
	plan action
	excecute plan / change environment
	tick time

*/
