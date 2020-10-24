/* === Data structures === */
typedef struct hyperParameters{
	int runs;				// amount of runs / episodes
	int epochs;				// amount of epochs per run / episode
	int numFloors;			// number of traversable floors
	int numElevators;		// number of elevators traversing floors
	int elevatorCapacity;	// max amount of passengers allowed in the elevator at the same time
	int R; 					// delivery reward value
	int spawnRate;			// Passenger spawn rate
	double epsilon;			// chance [0-1] of random action selection (exploration rate)
	double alpha;			// 
	double gamma;			// 
	int algorithm;			// algorthm: 0 = basic, 1 = q-learning, 2 = SARSA
}hyper;

typedef struct passenger{
	char* name;				// passenger name, makes it easier to track, however not unique
	int currentFloor;		// passenger current floor - spawn and after LOADing same as elevator current floor
	int goalFloor;			// passenger goal floor
	int waitingTime;		// ticks spent waiting until LOADed
	int totalWaitingTime;	// ticks spent in the simulation in total
	int activeWaitingTime;	// ticks spent not moving in the direction of the goal, mirrors the negative reward
	bool inElevator;		// is the passenger in the elevator?
}passenger;

typedef struct ListNode {	// standard list construction, used for waiting list and occupants list
	passenger p;			// each node carries a passenger object, 
	struct ListNode* next; 
}* List;

/* Actions */
typedef enum action{		// all available actions as enum for convenience & readability of the code
	MOVE_UP, MOVE_DOWN, HOLD, LOAD, UNLOAD
}action;

/* Data struct */
typedef struct elevator{
	action plannedAction;	// action enum MOVE_UP, MOVE_DOWN, HOLD, LOAD, UNLOAD, what the algorithm will do next
	int currentFloor;		// current floor of the elevator
	int* floorSignals;		// signals from waiting passengers, for each floor either -1 (I want to go down), 0 (no waiting), or 1 (want up)
	int numOccupants;		// number of occupants in the elevator (REDUNDANT THROUGH listSize(Occupants))
	List occupants; 		// list of occupants (passenger objects) currently in the elevator
	int reward;				// reward this epoch
	int totalReward;		// total reward
	double** Q; 			// [stateID|number of states][5] in the following FIXED order: MOVE_UP, MOVE_DOWN, HOLD, LOAD, UNLOAD
}elevator;

typedef struct statistics{
	int* actions;			// list of size epochs with each action that t
	int* actionCount;		// amount of times an action is selected
	int* stepReward;		// reward in this timeStep
	int* cumReward;			// cumulative reward at this timestep (graphable, should indicate performance increase over time)	
	int delivered; 			// amount of total passengers delivered
	int waitingTime;		// total waiting time of delivered passengers
	int* stepDelivered; 	// amount of passengers deliverd here this timeStep
	int* stepWaitingTime;	// waiting time of passengers delivered this timeStep
	int* stepTotalWaitingTime;		// total waiting time of passengers delivered this timeStep
	int* stepActiveWaitingTime;		// active waiting time of passengers delivered this timeStep
}stats;

/* === Data structure constructors === */
hyper makeHyperParameters(int ru, int ep, int numF, int numE, int elC, int R, int spr, double eps, double alp, double gam, int alg){
	hyper unit;
	unit.runs = ru;
	unit.epochs = ep;
	unit.numFloors = numF;
	unit.numElevators = numE;
	unit.elevatorCapacity = elC;
	unit.R = R;
	unit.spawnRate = spr; 
	unit.epsilon = eps;
	unit.alpha = alp;
	unit.gamma = gam;
	unit.algorithm = alg;
	return unit;
}

stats makeStatistics(int epoch){
	stats unit;
	unit.actions = (int*) calloc(epoch, sizeof(int));
	unit.actionCount = (int*) calloc(5, sizeof(int));
	unit.stepReward = (int*) calloc(epoch, sizeof(int));
	unit.cumReward = (int*) calloc(epoch, sizeof(int));
	unit.delivered = 0;
	unit.waitingTime = 0;
	unit.stepDelivered = (int*) calloc(epoch, sizeof(int));
	unit.stepWaitingTime = (int*) calloc(epoch, sizeof(int));
	unit.stepTotalWaitingTime = (int*) calloc(epoch, sizeof(int));
	unit.stepActiveWaitingTime = (int*) calloc(epoch, sizeof(int));
	return unit;
}
