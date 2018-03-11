#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

int currentState;
char permittion;
FILE *fptr;

typedef struct datesPattern {
    unsigned int year;
    unsigned int month;
    unsigned int day;
}publishingDate;
typedef struct booksPattern{
    unsigned int hash;
//    PublishingDate date;
    unsigned int day;
    char author[20];
    char title[20];
    unsigned int month;
    unsigned int year;
    unsigned int pagesQuantity;
    struct booksPattern *link;
} Book;


void act_manual();
void act_create_library ();
void act_read_from_library ();
void act_book_add ();
void act_book_find();
void act_book_del();

Book * readListFromLibrary ();
Book * createBooksList ();
void displayList (Book * head);
void writeBooksList2File (Book *head);
int polynomialHash (char *str, int k);
int deleteSpecificItem(Book** head, unsigned int hash);

int main() {
    void (*Action[5]) ();
    Action[0]=act_manual;
    Action[1]=act_create_library;
    Action[2]=act_read_from_library;
    Action[3]=act_book_add;
    Action[4]=act_book_del;
    Action[5]=act_book_find;

    act_manual();
    scanf("%d",&currentState);
    (*Action[currentState]) ();

    for(int i=0;i<5;i++){
        Action[i]=0;
    }
    free(*Action);
    printf("program was closed");
    return 0;
}

void act_manual(){
//    printf("act_manual -> 0 state");
    printf("\nManual for this program:\n");
//    printf("0 - enter this value to call manual again\n");
    printf("=====================================================\n");
    printf("1 - enter this value to create a new file\n");
    printf("2 - enter this value to open exist book's library\n");
    printf("3 - enter this value to add book to library\n");
    printf("4 - enter this value to delete book from library\n");
    printf("5 - enter this  value to find book from library\n");
    printf("-----------------------------------------------------\n");
}
void act_create_library (){
    fptr = fopen("BooksLibrary.txt","w");
    printf("\nFile BooksLibrary.txt was created\n"
                   "Would you like to add few books to new library? y/n\n");
    scanf("%s", &permittion);
    if (permittion=='y') {
        permittion='z';
        act_book_add();
    }
    if (permittion=='n') {
        permittion='z';
        exit(1);
    }
}
void act_read_from_library (){
//    printf("act_showLibrary -> 1st state\n");
    if ((fptr = fopen("BooksLibrary.txt","r")) == NULL){
        printf("----------------------------------------------------\n");
        printf("Error! opening file. Perhaps file doesn't exist\n");
        printf("----------------------------------------------------\n");
        printf("Would you like to create new library? y/n\n");
        scanf("%s",&permittion);
        if (permittion=='y') {
            permittion='z';
            act_create_library();
        }
        if (permittion=='n') {
            permittion='z';
            exit(1);
        }
    } else {
        Book * HEAD= NULL;
        HEAD=readListFromLibrary();
        displayList(HEAD);
    }
}
void displayList (Book * head){
    Book *swap=head;
    int BooksCounter=1;
    while(swap!=NULL){
        printf("Book #%d\n",BooksCounter);
        printf("HASH: %u\n",swap->hash);
        printf("Author: %s\n",swap->author);
        printf("Title: %s\n",swap->title);
        printf("Pages: %u\n",swap->pagesQuantity);
        printf("Date: %u/%u/%u\n", swap->day,swap->month, swap->year);
        printf("====================================================\n");
        swap=swap->link;
        BooksCounter++;
    }
}
void writeBooksList2File (Book *head){
    fptr = fopen("BooksLibrary.txt","a");
    Book *temp=head;
    while(temp!=NULL){
        fprintf(fptr,"%u\t",temp->hash);
        fprintf(fptr,"%s\t",temp->author);
        fprintf(fptr,"%s\t",temp->title);
        fprintf(fptr,"%u\t",temp->pagesQuantity);
        fprintf(fptr,"%u\t%u\t%u\n", temp->day,temp->month, temp->year);
        temp=temp->link;
    }
}
Book * readListFromLibrary (){
    Book *head = NULL;
    Book *temp = NULL;
    Book *p = NULL;
    int eof = 0;
    do {
        temp=(Book*)malloc(sizeof(Book));
        if(fscanf(fptr, "%u\t%s\t%s\t%u\t%u\t%u\t%u\n",
                  &temp->hash,temp->author,temp->title,
                  &temp->pagesQuantity,&temp->day,
                  &temp->month, &temp->year) == EOF){
            eof=1;
        }
//        printf("%u\t%s\t%s\t%u\t%u\t%u\t%u\n",
//               temp->hash,temp->author,temp->title,temp->pagesQuantity,temp->day,temp->month, temp->year);
        temp->link=NULL;
        if(head==NULL){ // if list currently empty as first Book
            head = temp;
        }
        else {
            p = head;
            while (p->link != NULL) {
                p=p->link;
            }
            p->link=temp;
        }
    }while (eof==0);
    fclose(fptr);
    return head;

}
Book * createBooksList () {
    int i=1;
    Book *head = NULL;
    Book *temp = NULL;
    Book *p = NULL;
    permittion='y';
    while(permittion=='y'){
        //individual isolated node
        temp=(Book*)malloc(sizeof(Book));
        printf("Enter the data for Book's number: %d\n"
                       "\tAuthor: ", i++);
        temp->hash=(unsigned int)polynomialHash(temp->author,2);
        scanf("%s",temp->author);
        printf("\tTitle: ");
        scanf("%s",temp->title);
        printf("\tPages' quantity: ");
        scanf("%u", &(temp->pagesQuantity));
        printf("\n Date in format -> dd/mm/yyy: ");
        scanf("%u%*c%u%*c%u", &(temp->month), &(temp->day), &(temp->year));
        temp->link=NULL;
        if(head==NULL){ // if list currently empty as first Book
            head = temp;
        }
        else {
            p = head;
            while (p->link != NULL) {
                p=p->link;
            }
            p->link=temp;
        }
        printf("\nWould you like to add one more book? y/n\n");
        scanf("%s",&permittion);
        if(permittion=='n')break;
    }
    return head;
}
int deleteSpecificItem(Book** head, unsigned int hash) {
    if (!head || !(*head)) return -1;

    Book* tmp = *head;
    Book* prev = NULL;

    while (tmp->hash != hash && tmp->hash != NULL)
    {
        prev = tmp;
        tmp = tmp->link;
    }

    if (tmp->hash == hash)
    {
        if (prev)
        {
            prev->link = tmp->link;
        }
        else
        {
            *head = tmp->link;
        }
        free(tmp);
        return hash;
    }

    return -1;
}
void act_book_add (){
//    printf("act_book_add -> 2d state\n");
    Book * HEAD= NULL;
    HEAD=createBooksList();
//    displayList(HEAD);
    writeBooksList2File(HEAD);
}
void act_book_del (){
    printf("act_book_del -> 3d state\n");
    unsigned int HASH;
    permittion = 'z';

    Book * HEAD= NULL;
    HEAD=readListFromLibrary();
    displayList(HEAD);

//    printf("\n-------------------------------------------------------------\n");
    printf("\nInput unique books' hash code to delete book fron library\n"
                   "HASH:\t");
    scanf("%u",&HASH);
//    printf("\n-------------------------------------------------------------\n");
    printf("Are you sure that you want to delete this book? y/n\n");

//    act_book_find();

    scanf("%c",&permittion);
    if (permittion=='y'){
        ////deleteSpecificItem(Book** head, unsigned int hash)
        printf("The book with HASH: %u was deleted\n",deleteSpecificItem( &HEAD, HASH));
        displayList(HEAD);
    }
    if (permittion=='n'){
        exit(1);
    }
}
void act_book_find(){
    printf("act_book_find -> 4th state\n");
}

int polynomialHash (char *str, int k){
    int hash=0;
    int codes[20];
    for (int i = 0; i <20; i++) {
        codes[i] = (int)str[i]*(int)pow(k,i);
    }
    for (int j = 0; j <20 ; j++) {
        hash+=codes[j];
    }
    hash%=(unsigned int)pow(2,20);
    return hash;
}