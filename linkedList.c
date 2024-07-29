#include <stdlib.h>
#include <stdio.h>

struct Node{
    int data;
    struct Node* next;
};  

void insertAtBeginning(struct Node** head_ref, int new_data){
    //allocate memory
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    //ssign data into the new node
    new_node->data = new_data;

    //make next head
    new_node->next = *head_ref;

    //move the head to point to the new node
    *head_ref  = new_node;

}

void insertAtTheEnd(struct Node** head_ref, int new_data){
    //allocate memory
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    struct Node* last = *head_ref;

    new_node->data = new_data;

    new_node->next = NULL;

    if(*head_ref == NULL){
        *head_ref = new_node;
        return;
    }

    while(last->next != NULL){
        last = last->next;
    }

    last->next = new_node;

}

void insertAfter(struct Node** head_ref, int new_data, int prev_data){
    //allocate momery
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = new_data;

    struct Node* prev_node = *head_ref;

    while(prev_node->data != prev_data){
        prev_node = prev_node->next;
    }

    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

void deleteNode(struct Node** head_ref, int valueToDelete){
    struct Node* temp = *head_ref;
    if(temp->data == valueToDelete){
        *head_ref = temp->next;
        temp->next = NULL;
        free(temp);
        return;
    }

    while(temp->next->data != valueToDelete){
        temp = temp->next;
    }

    temp->next = temp->next->next;
    free(temp->next);
    
}

void makeLinkedListIncreasingManner(int value, struct Node** head_ref){
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = value;

    struct Node* head = *head_ref;
    
    if(head == NULL){   //head node
        new_node->next = NULL;
        *head_ref = new_node;
        return;
    }   

    while(head->next != NULL && head->next->data < value){
        head=head->next;
    }

    if(head->next==NULL){  //head is the last
        new_node->next = NULL;
        head->next = new_node;
    }
    else{      //in between
        new_node->next = head->next;
        head->next = new_node;
    }
    
}

int member(int value, struct Node** head){
    struct Node* current_node = *head;
    if(current_node->data == value){
        return current_node->data;
    }else{
        while(current_node->next != NULL && current_node->next->data != value){
            current_node = current_node->next;
        }

        if(current_node->next == NULL){
            return -1;
        }else{
            return current_node->next->data;
        }

    }
}


int main(){
    struct Node* head = NULL;

    insertAtBeginning(&head, 1);
    insertAtBeginning(&head, 2);
    insertAtBeginning(&head, 3);
    insertAtBeginning(&head, 4);
    insertAtBeginning(&head, 5);
    insertAtBeginning(&head, 6);
    insertAtBeginning(&head, 7);
    insertAtBeginning(&head, 8);
    insertAtBeginning(&head, 9);
    insertAtBeginning(&head, 10);

    insertAtTheEnd(&head, 11);
    insertAtTheEnd(&head, 12);
    insertAtTheEnd(&head, 13);
    insertAtTheEnd(&head, 14);
    insertAtTheEnd(&head, 15);
    insertAtTheEnd(&head, 16);
    insertAtTheEnd(&head, 17);
    insertAtTheEnd(&head, 18);
    insertAtTheEnd(&head, 19);
    insertAtTheEnd(&head, 20);

    insertAfter(&head, 21, 10);
    insertAfter(&head, 22, 11);
    insertAfter(&head, 23, 12);
    insertAfter(&head, 24, 13);
    insertAfter(&head, 25, 14);
    insertAfter(&head, 26, 15);
    insertAfter(&head, 27, 16);
    insertAfter(&head, 28, 17);
    insertAfter(&head, 29, 18);
    insertAfter(&head, 30, 19);

    // deleteNode(&head, 20);
    // deleteNode(&head, 19);
    
    // makeLinkedListIncreasingManner(2, &head);
    // makeLinkedListIncreasingManner(56, &head);
    // makeLinkedListIncreasingManner(9, &head);
    // makeLinkedListIncreasingManner(10, &head);
    //deleteNode(&head, 10);

    int result = member(10, &head);
   
    if(result != -1){
         printf("result %d" , result);
    

    struct Node* temp = head;
    while(temp != NULL){
        printf("%d\n", temp->data);
        temp = temp->next;
    }

    return 0;
}
}