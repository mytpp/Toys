typedef struct LISTNODE {
	int number;
	struct LISTNODE* next;
}LISTNODE;
LISTNODE * create();
void sort(LISTNODE**);               //A function to sort a existing linked list accroding to the value of the number of each node
void printList(LISTNODE*);           //A function to output every number in a linked node
void freeList(LISTNODE*);


int main()
{
	LISTNODE* headPtr;
	headPtr = create();
	sort(&headPtr);
	printList(headPtr);
	freeList(headPtr);
	system("pause");
	return 0;
}


	LISTNODE* headPtr = NULL, *currentPtr = NULL, *lastPtr = NULL;
	printf("Please input an list(end by -1):");
	scanf("%d", &num);
	while (num != -1)
	{
		currentPtr = malloc(sizeof(LISTNODE));
		if (currentPtr != NULL)
		{
			currentPtr->number = num;
			if (headPtr == NULL)
			{
				headPtr = currentPtr;
				lastPtr = currentPtr;
			}
			else
			{
				lastPtr->next = currentPtr;
				lastPtr = currentPtr;
			}
		}
		else
			printf("Application for internal memories failed.");
		scanf("%d", &num);
/*		lastPtr->next = NULL;
	}
	return headPtr;
}

// /*Insertion Sort*/
// /*We need a pointer to the pointer to the head of a linked list that is named 'headPtr'.*/
// void sort(LISTNODE** headPtr)
// {
// 	/*'previousPtr' and 'currentPtr' are declared to locate the insertion poin.t*/
// 	/*'headPtr' and 'lastPtr' point to the first and the last node respectively in the sub sequcece that has been already sorted.*/
// 	LISTNODE* previousPtr = NULL, *currentPtr = *headPtr, *lastPtr = *headPtr, *newPtr = *headPtr;
// 	while (lastPtr->next != NULL)
// 	{
// 		previousPtr = NULL;
// 		currentPtr = *headPtr;
// 		newPtr = lastPtr->next;
// 		while (currentPtr != newPtr&&currentPtr->number < newPtr->number)
// 		{           //Find a position to insert 'newPtr', which is between 'previousPtr' and 'currentPtr'
// 			previousPtr = currentPtr;
// 			currentPtr = currentPtr->next;
// 		}
// 		if (previousPtr == NULL)
// 		{           //If the insertion point is before headPtr, delete newPtr from its former position and make it head of the list
// 			lastPtr->next = newPtr->next;
// 			newPtr->next = *headPtr;
// 			*headPtr = newPtr;
// 		}
// 		else if (currentPtr == newPtr)
// 		{           //If the insertion point is just after the tail of the sorted linked list, move lastPtr backward for one node
// 			lastPtr = currentPtr;
// 		}
// 		else        //If the insertion point is in the middle of the sub sequence, delete newPtr from its former position and insert it in the new-found position
// 		{
// 			lastPtr->next = newPtr->next;
// 			previousPtr->next = newPtr;
// 			newPtr->next = currentPtr;
// 		}
// 	}
// }


// void freeList(LISTNODE* headPtr)
// {
// 	LISTNODE* tempPtr = NULL;
// 	while (headPtr != NULL)
// 	{
// 		tempPtr = headPtr;
// 		headPtr = headPtr->next;
// 		free(tempPtr);
// 	}
// }


// /*'currentPtr' points to the first node of the linked list whose numbers we will be output one by one.*/
// void printList(LISTNODE* currentPtr)
// {
// 	printf("The new list is:");
// 	while (currentPtr->next != NULL)//All the output numbers but the last one is followed by a blank space, so break the loop when 'currentPtr' reach the last node
// 	{
// 		printf("%d ", currentPtr->number);
// 		currentPtr = currentPtr->next;
// 	}
// 	printf("%d\n", currentPtr->number);//The last number should be followed by a newline
// }
