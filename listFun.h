/* === List functions === */
#include <assert.h>

//All functions are fairly standard and self-evident in their function
List newEmptyList () {
	return NULL; 
}

List addItem(passenger p, List li) {
	List newList = malloc(sizeof(struct ListNode));
	assert(newList!=NULL);
	newList->p = p; 
	newList->next = li; 
	return newList;
}

List removeFirstNode(List li){
	List newList = li->next;
	free(li);
	return newList;
}

void freeList(List li) {
	List li1;
	while ( li != NULL ) {
		li1 = li->next; 
		free(li);
		li = li1;
	}
	return; 
}

int listSize(List li) {
	List li1 = li;
	int count = 0;
	while (li1 != NULL) {
		count++;
		li1 = li1->next; 
	}
	return count; 
}

/* Passenger assertion */
bool isSamePassenger(passenger p1, passenger p2){
	if (p1.goalFloor == p2.goalFloor && p1.waitingTime == p2.waitingTime && p1.inElevator == p2.inElevator)
		return true;
	return false;
}

List removeItem(passenger p, List li) {
	if (li == NULL) 
		return li; 
	if (isSamePassenger(li->p, p)) 
		return removeFirstNode(li);
	List li1 = li;
	while (li1->next != NULL && !isSamePassenger((li1->next)->p, p)) 
		li1 = li1->next;
	if (li1->next != NULL)  					/* so (li1->next)->item == n */
		li1->next = removeFirstNode(li1->next);
	return li;
}

void printListNames(List li){
	List li1 = li;
	while (li1 != NULL){
		printf("%s cf:%i gf:%i wt:%i ie:%i\n", li1->p.name, li1->p.currentFloor, li1->p.goalFloor, li1->p.waitingTime, li1->p.inElevator);
		li1 = li1->next;
	}
}

void printPassengerList(List li){
	printf("gFloor   wTime    inElev\n");
	List li1 = li;
	while (li1 != NULL){
		printf("%i, %i, %d\n", li1->p.goalFloor, li1->p.waitingTime, li1->p.inElevator);
		li1 = li1->next;
	}
}
