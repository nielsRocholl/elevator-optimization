
int largest(double* Q_S){								/* calculate largest double of array, returns index */
	int index = rand() % 5; 							/* default values to compare to, default 0 -just in case-*/
	double max = -1000.0;
	for (int i = 0; i < 5; i++){
		if (Q_S[i] == 0.000000)
			Q_S[i] = ((((double)rand())/RAND_MAX) / 1000.0); // Fuzzing for equality at 0
		if (Q_S[i] > max){					
			max = Q_S[i];
			index = i;
		}
	}
	return index;
}

int eGreedy(double* Q_S, int* allowedActions, double epsilon){			/* Epsilon Greedy algorithm */
	if ((((double)rand())/RAND_MAX) < epsilon){
		int random = rand() % 5;
		while(!allowedActions[random]){
			random = (random + 1) % 5; 					// not truly random action selection, perhaps make more elegant later (select random from availible)
		}
		printf("Action Selected Random\n");
		return random;
	} else {
		return largest(Q_S);
	}
}

int* filterActions(int* actions, elevator e, hyper h){ //order: 0 = MOVE_UP, 1 = MOVE_DOWN, 2 = HOLD, 3 = LOAD, 4 = UNLOAD
	for (int i = 0; i < 4; i++)							// Set all availible to disallow below, except UNLOAD, which does the reverse
		actions[i] = 1;
	if (e.currentFloor == h.numFloors-1)				// on top floor remove MOVE_UP
		actions[0] = 0;
	if (e.currentFloor == 0)							// on bottom floor remove MOVE_DOWN
		actions[1] = 0;
	if (e.floorSignals[e.currentFloor] == 0 || listSize(e.occupants) == h.elevatorCapacity) // No waiting or full remove LOAD
		actions[3] = 0;
	List li1 = e.occupants;
	while (li1 != NULL){
		if (li1->p.goalFloor == e.currentFloor)			// someone wants out allow UNLOAD
			actions[4] = 1;
		li1 = li1->next;
	}
	return actions;
}

elevator applyFilter(elevator e, int* allowedActions, int stateID){	//applies filter every time, to speed up check if its neccesairy
	for (int i = 0; i < 5; i++){
		if (allowedActions[i] == 0)
			e.Q[stateID][i] = -10000.0;
	}
	return e;
}

elevator selectActionQ(elevator e, hyper h){			// Action selection function, only call this from elsewhere 
	int stateID = stateToID(e, h.numFloors, h.elevatorCapacity);
	int* allowedActions = filterActions((int*) calloc(5, sizeof(int)), e, h);
	e = applyFilter(e, allowedActions, stateID);		// Remove this after the first tick. -- after this it is applied for each Q update
	int selAction = eGreedy(e.Q[stateID], allowedActions, h.epsilon);

	e.plannedAction = intToAction(selAction);
	return e;
}

elevator updateQ(elevator e, hyper h, int oldStateID){ 	// Q(S,A) <- Q(S,A) + alpha[R + gamma*Max_aQ(S',a) - Q(S,A)]
	int newStateID = stateToID(e, h.numFloors, h.elevatorCapacity);
	int* allowedActions = filterActions((int*) calloc(5, sizeof(int)), e, h); 	// Find allowed actions for the new state
	e = applyFilter(e, allowedActions, newStateID);								// Remove unavailible actions
	
	printf("state parameters before\n");
	for (int i = 0; i < 5; i++){
		printf("%f ",e.Q[oldStateID][i]);
	}
	printf("\n");
	
	double old = e.Q[oldStateID][actionToInt(e.plannedAction)];
	printf("%f + (%f * (%i + (%f * %f) - %f) = ",old, h.alpha, e.reward, h.gamma, e.Q[newStateID][largest(e.Q[newStateID])], old);
	e.Q[oldStateID][actionToInt(e.plannedAction)] = old + (h.alpha * (e.reward + (h.gamma * e.Q[newStateID][largest(e.Q[newStateID])]) - old)); //One step Q learning
	printf("%f\n",e.Q[oldStateID][actionToInt(e.plannedAction)]);
	printf("after\n");
	for (int i = 0; i < 5; i++){
		printf("%f ",e.Q[oldStateID][i]);
	}
	printf("\n");
	
	return e;
}

/*

select A

observe R & S 

update Q
*/
