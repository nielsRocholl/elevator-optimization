/* === Helper functions === */
// functions designed to assist elsewhere, general operations, translations, allocations
char* printAction(action a){
	switch(a){
		case MOVE_UP:
			return "MOVE_UP";
			break;
		case MOVE_DOWN:
			return "MOVE_DOWN";
			break;
		case HOLD:
			return "HOLD";
			break;
		case LOAD:
			return "LOAD";
			break;
		case UNLOAD:
			return "UNLOAD";
			break;
	}
	printf("error value: %u\n", a);
	return "ERROR";
}

// Translate state to pseudo-binary unique integer ID
int stateToID(elevator e, int nF, int mC){
	int id = 0;
	int scalar = 1;
	id += e.currentFloor * scalar;				// Translate floor number
	scalar = nF * scalar;						// 0 to nf-1 * scalar
	for (int i = 0; i < nF; i++){				// Translate floor signals
		id += ((e.floorSignals[i]+1) * scalar);	// 0, 1 or 2 * scalar
		scalar *= 3;
	}
	List li1 = e.occupants;
	for (int i = 0; i < mC; i++){				// Translate occupant goals
		while (li1 != NULL){
			id += (li1->p.goalFloor * scalar);
			li1 = li1->next;
			i++;
			scalar *= (nF + 1);					// number of floors + no waiting signal
		}
		if (i < mC){
			id += (nF * scalar);				// No occupant 
			scalar *= (nF + 1);
		}
	}
	return id;
}

void printState(elevator e){					// Print current state variables
	printf("cf: %i, sig: ", e.currentFloor);
	for (int i = 0; i < 5; i++){
		printf("%i ", e.floorSignals[i]);
	}
	printf("occ: ");
	List li1 = e.occupants;
	while (li1 != NULL){
		printf("%i ", li1->p.goalFloor);
		li1 = li1->next;
	}
	printf("\n");
}

action intToAction(int i){  					//0 = MOVE_UP, 1 = MOVE_DOWN, 2 = HOLD, 3 = LOAD, 4 = UNLOAD
	if (i > 4)
		printf("ERROR: Illegal int to action conversion.\n");
	action actions[5] = {MOVE_UP, MOVE_DOWN, HOLD, LOAD, UNLOAD};
	return actions[i];
}

char* intToAlgorithm(int i){
	if (i > 3)
		printf("ERROR: Illegal int to algorithm conversion.\n");
	char* algorithms[4] = {"basic", "Q-Learning", "SARSA", "Classic"};
	return algorithms[i];
}

int actionToInt(action a){						// Translate action enum to corresponding action int
	switch(a){
		case MOVE_UP:
			return 0;
			break;
		case MOVE_DOWN:
			return 1;
			break;
		case HOLD:
			return 2;
			break;
		case LOAD:
			return 3;
			break;
		case UNLOAD:
			return 4;
			break;
	}
}

void writeGraphData(double* data, int size, char* name, char* title){		/* Write the data required for the graphs to a .csv file */
    char* extension = ".csv";
    char fileName[strlen(name)+strlen(extension)+1];

    snprintf(fileName, sizeof(fileName), "%s%s", name, extension);
    FILE *fptr = fopen(fileName, "w");

    fprintf(fptr, "%s\n", title); 
    for (int i = 0; i < size; i++) {
        fprintf(fptr,"%f\n", data[i]);
    }
    fclose(fptr);
}

void allocate_mem(double*** arr, int n, int m){
  *arr = (double**)calloc(n,sizeof(double*));
  for(int i=0; i<n; i++)
    (*arr)[i] = (double*)calloc(m,sizeof(double));
} 

void deallocate_mem(double*** arr, int n){
    for (int i = 0; i < n; i++)
        free((*arr)[i]);
    free(*arr); 
}

void freeStats(stats* s, int r){
	for (int i = 0; i < r; i++){
		free(s[i].actions);
		free(s[i].actionCount);
		free(s[i].stepReward);
		free(s[i].cumReward);
		free(s[i].stepDelivered);
		free(s[i].stepWaitingTime);
		free(s[i].stepTotalWaitingTime);
		free(s[i].stepActiveWaitingTime);
	}
}

double** initializeQ(int numFloors, int elevatorCapacity){
	int size = numFloors * pow(3, numFloors) * pow((numFloors+1),elevatorCapacity);
	int actions = 5;
	double** Q; 
	allocate_mem(&Q, size, actions);
	return Q;
}

