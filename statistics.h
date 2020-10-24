
stats deliveryStats(stats s, elevator e, int t){ // Update passenger delivery statistics while running
	List li1 = e.occupants;
	while (li1 != NULL){
		if (li1->p.goalFloor == e.currentFloor){
			s.delivered += 1;
			s.stepDelivered[t] += 1;
			s.waitingTime += li1->p.waitingTime;
			s.stepWaitingTime[t] += li1->p.waitingTime;
			s.stepTotalWaitingTime[t] += li1->p.totalWaitingTime;
			s.stepActiveWaitingTime[t] += li1->p.activeWaitingTime;
		}
		li1 = li1->next;
	}
	return s;
}

void multiRunStats(stats* s, hyper h){
	int outputSize = 10000;					// Amount of datapoints of the output file averaging over the intervals of size epochs/outputSize, and for each run
	if (h.epochs <= outputSize){
		printf("WARNING: unreliable data - decrease output size or increase epochs\n");
	}
	int* intervalDelivered = (int*) calloc(h.runs, sizeof(int));
	int* intervalActiveWaitingTime =  (int*) calloc(h.runs, sizeof(int));
	int* intervalCumReward = (int*) calloc(h.runs, sizeof(int));
	double* avCumData = (double*) calloc(outputSize, sizeof(double));
	double* avData = (double*) calloc(outputSize, sizeof(double));
	int interval = h.epochs / outputSize;

	for (int i = 0; i < h.epochs; i++){
		for (int j = 0; j < h.runs; j++){
			intervalDelivered[j] += s[j].stepDelivered[i];
			intervalActiveWaitingTime[j] += s[j].stepActiveWaitingTime[i];
			intervalCumReward[j] += s[j].cumReward[i];
		}
		if (i%interval == 0 && i > 0){
			for (int j = 0; j < h.runs; j++){
				if (intervalDelivered[j])
					avData[i/interval] += (double)intervalActiveWaitingTime[j] / intervalDelivered[j];
				avCumData[i/interval] += (double)intervalCumReward[j] / interval;
				intervalActiveWaitingTime[j] = 0;
				intervalDelivered[j] = 0;
				intervalCumReward[j] = 0;
			}
			avData[i/interval] /= h.runs;
			avCumData[i/interval] /= h.runs;
		}
	}
	writeGraphData(avData, outputSize, "data", "active_waiting_time");
	writeGraphData(avCumData, outputSize, "cumReward", "cumulative_reward");
	free(avData);
	free(intervalDelivered);
	free(intervalActiveWaitingTime);
}

void printStats(stats s, elevator e, int epoch){	// Container statistics calculation
	/*
	printf("MOVE_UP: MOVE_DOWN: HOLD: LOAD: UNLOAD: \n");
	printf("Actions selection count: \n");
	printf("%i %i %i %i %i \n", s.actionCount[0], s.actionCount[1], s.actionCount[2], s.actionCount[3], s.actionCount[4]);
	
	int totAcRewards[5] = {0,0,0,0,0};
	for (int i = 0; i < epoch; i++)
		totAcRewards[s.actions[i]] += s.stepReward[i];

	double avAcRewards[5] = {0.0,0.0,0.0,0.0,0.0};
	for (int i = 0; i < 5; i++)
		avAcRewards[i] = (double)totAcRewards[i] / s.actionCount[i];
	
	printf("Tot Reward: \n");
	printf("%i %i %i %i %i \n",totAcRewards[0],totAcRewards[1],totAcRewards[2],totAcRewards[3],totAcRewards[4]);

	printf("Avg Reward: \n");
	printf("%.2f %.2f %.2f %.2f %.2f \n",avAcRewards[0],avAcRewards[1],avAcRewards[2],avAcRewards[3],avAcRewards[4]);
	
	printf("\nPassengers delivered: %i \navg time: %.2f\n", s.delivered, ((double)s.waitingTime / s.delivered));
	printf("Total reward: %i\n",e.totalReward);
	*/
	double performance = 0.0;
	int intervalDelivered = 0;
	int intervalWaitingTime = 0;
	int intervalTotalWaitingTime = 0;
	int intervalActiveWaitingTime = 0;
	int interval = epoch/10;
	double* data = (double*) calloc(epoch/interval, sizeof(double));
	for (int i = 0; i < epoch; i++){
		performance += s.stepReward[i];
		intervalDelivered += s.stepDelivered[i];
		intervalWaitingTime += s.stepWaitingTime[i];
		intervalTotalWaitingTime += s.stepTotalWaitingTime[i];
		intervalActiveWaitingTime += s.stepActiveWaitingTime[i];
		if (i%interval == 0 && i > 0){
			//printf("interval %i: %f\n", (i/interval), performance/interval);
			//printf("avg time: %f\n", (double)intervalWaitingTime/intervalDelivered);
			//printf("avg tot time: %f\n", (double)intervalTotalWaitingTime/intervalDelivered);
			printf("avg act time: %f\n", (double)intervalActiveWaitingTime/intervalDelivered);
			data[i/interval] = (double)intervalActiveWaitingTime / intervalDelivered;
			performance = 0.0;
			intervalDelivered = 0;
			intervalWaitingTime = 0;
			intervalTotalWaitingTime = 0;
			intervalActiveWaitingTime = 0;
		} 
	}
	//writeGraphData(data, interval, "actwait100m", "active waiting time");
}




