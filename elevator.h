/* === Elevator === */



/* Constructor */
elevator makeElevator(int floor, int floors, int capacity){
	elevator unit;
	unit.currentFloor = floor;
	unit.floorSignals = (int*) calloc(floors, sizeof(int));
	unit.numOccupants = 0;
	unit.occupants = newEmptyList();
	unit.reward = 0;
	unit.totalReward = 0;
	unit.Q = initializeQ(floors, capacity);
	return unit;
}

/* Action functions */
/* LOAD occupants on this floor */
elevator load(elevator e, List people){
	List li1 = people;
	while (li1 != NULL){
		if (li1->p.currentFloor == e.currentFloor && li1->p.inElevator == false){
			li1->p.inElevator = true;
			e.occupants = addItem(li1->p, e.occupants);
			e.numOccupants += 1;
		}
		li1 = li1->next;
	}
	e.floorSignals[e.currentFloor] = 0;
	return e;
}

/* UNLOAD occupant */
elevator unload(elevator e){
	List li1 = e.occupants;
	while (li1 != NULL){
		if (li1->p.goalFloor == e.currentFloor){
			li1->p.inElevator = false;
			e.occupants = removeItem(li1->p, e.occupants);
			e.numOccupants -= 1;
		}
		li1 = li1->next;
	}
	return e;
}

/* MOVE_UP with all occupants */
elevator moveUp(elevator e){
	e.currentFloor += 1;
	e.occupants = moveAlong(e.occupants, 1);
	return e;
}

/* MOVE_DOWN with all occupants */
elevator moveDown(elevator e){
	e.currentFloor -= 1;
	e.occupants = moveAlong(e.occupants, -1);
	return e;
}

/* HOLD */
elevator hold(elevator e){
	return e;
}

/* Action switch */
elevator executeAction(elevator e, List people){
	switch(e.plannedAction){
		case MOVE_UP:
			return moveUp(e);
			break;
		case MOVE_DOWN:
			return moveDown(e);
			break;
		case HOLD:
			return hold(e);
			break;
		case LOAD:
			return load(e, people);
			break;
		case UNLOAD:
			return unload(e);
			break;
	}
}

//Basic elevator function, is placeholder dumb algorithm
elevator basicModel(elevator e, hyper h){
	
	if (e.occupants != NULL){										// If you have someone, move to that floor or unload if there
		if (e.occupants->p.goalFloor > e.currentFloor)
			e.plannedAction = MOVE_UP;
		if (e.occupants->p.goalFloor < e.currentFloor)
			e.plannedAction = MOVE_DOWN;
		if (e.occupants->p.goalFloor == e.currentFloor)
			e.plannedAction = UNLOAD;
		return e;
	}
	
	for (int i = 0; i < h.numFloors; i++){							// If you dont, move to a signal or load if there, or hold (wait for a signal)
		if (e.floorSignals[i] != 0){
			if (i > e.currentFloor)
				e.plannedAction = MOVE_UP;
			if (i < e.currentFloor)
				e.plannedAction = MOVE_DOWN;
			if (i == e.currentFloor)
				e.plannedAction = LOAD;
			return e;
		}
	}
	e.plannedAction = HOLD;
	return e;
}

elevator classicalModel(elevator e, hyper h){

	List li1 = e.occupants;
	while (li1 != NULL){										// Priority 1: UNLOAD
		if (li1->p.goalFloor == e.currentFloor){
			e.plannedAction = UNLOAD;
			return e;
		}
		li1 = li1->next;
	}
	if (e.floorSignals[e.currentFloor] != 0){					// Priority 2: LOAD
		e.plannedAction = LOAD;
		return e;
	}
	li1 = e.occupants;
	if (e.plannedAction == MOVE_DOWN && e.currentFloor != 0){	// Priority 3: Keep the current direction
		while (li1 != NULL){						
			if (li1->p.goalFloor < e.currentFloor)				// Priority 3.5: occupants > waiting
				return e;										// repeat previous action
			li1 = li1->next;
		}
		for (int i = 0; i < e.currentFloor; i++){
			if (e.floorSignals[i] != 0)
				return e;
		}
	}
	if (e.plannedAction == MOVE_UP && e.currentFloor != h.numFloors-1){	// Same as above, now opposite direction
		while (li1 != NULL){						
			if (li1->p.goalFloor > e.currentFloor)				
				return e;										
			li1 = li1->next;
		}
		for (int i = e.currentFloor + 1; i < h.numFloors; i++){
			if (e.floorSignals[i] != 0)
				return e;
		}
	}
	int dist = 100;
	int closest = 0;
	while(li1 != NULL){											// Priority 4: move to closest goal, occupants > waiting
		if ((e.currentFloor - li1->p.goalFloor)*(e.currentFloor - li1->p.goalFloor) < dist){
			closest = li1->p.goalFloor;
			dist = (e.currentFloor - li1->p.goalFloor)*(e.currentFloor - li1->p.goalFloor);
		}
		li1 = li1->next;
		if (li1 == NULL){
			e.plannedAction = e.currentFloor > closest ? MOVE_DOWN : MOVE_UP;
			return e;
		}
	}
	for (int i = 0; i < h.numFloors; i++){
		if (e.floorSignals[i] != 0){
			if ((e.currentFloor - i)*(e.currentFloor - i) < dist){
				closest = i;
				dist = (e.currentFloor - i)*(e.currentFloor - i);
			}
		}
		if (i == h.numFloors-1){
			e.plannedAction = e.currentFloor > closest ? MOVE_DOWN : MOVE_UP;
			return e;
		}
	}
	e.plannedAction = HOLD;										// Priority 5: Hold
	return e;
}

elevator planAction(elevator e, hyper h, int t){					// Selects action algorithm, plans an action before it executes later
	switch(h.algorithm){
		case 0:
			return basicModel(e, h);;
			break;
		case 1:
			return selectActionQ(e, h, t);
			break;
		case 2:
			return selectActionSarsa(e, h, t);
			break;
		case 3:
			return classicalModel(e, h);
			break;
	}	
	printf("Error: no algorithm selected\n");
	return e;
}

elevator updateAlgorithm(elevator e, hyper h, int stateID){
	switch(h.algorithm){
		case 0:
			return e;
			break;
		case 1:
			return updateQ(e, h, stateID);
			break;
		case 2:
			return updateSarsa(e, h, stateID);
			break;
		case 3:
			return e;
			break;
	}	
	printf("Error: no algorithm selected\n");
	return e;
}

elevator reward(elevator e, List waiting, int R){					// -1 for all passengers not moving in the direction of their goal, +R for delivered passengers
	e.reward = 0;
	e.reward -= listSize(waiting);									// subtract -1 for all passengers and waiting, compensate with +1 for unjust punishments below
	e.reward -= listSize(e.occupants);
	List li1 = e.occupants;
	List li2 = waiting;
	while (li1 != NULL || e.plannedAction == LOAD){
		switch(e.plannedAction){
			case MOVE_UP:
				if (li1->p.currentFloor < li1->p.goalFloor)
					e.reward += 1; li1->p.activeWaitingTime -= 1;	// decrease the active wait time; it mirrors the negative reward function
				break;
			case MOVE_DOWN:
				if (li1->p.currentFloor > li1->p.goalFloor)
					e.reward += 1; li1->p.activeWaitingTime -= 1;
				break;
			case HOLD:
				break;
			case LOAD:												// Special case: LOAD cycles through waiting to compensate people loaded now, and exits.
				while (li2 != NULL){
					if (e.currentFloor == li2->p.currentFloor)
						e.reward += 1; li2->p.activeWaitingTime -= 1;
					li2 = li2->next;
				}
				return e;
				break;
			case UNLOAD:
				if (li1->p.currentFloor == li1->p.goalFloor)
					e.reward += 1 + R;								// Succesfully unloaded gain R reward | Don't decrease active waiting time, as the passenger is UNLOADed before it ticks up
				break;
		}
		li1 = li1->next;
	}
	return e;
}




