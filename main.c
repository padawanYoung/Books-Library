#include <stdio.h>
#include <stdlib.h>

#define LENGTH 20
#define TABLE_SIZE 16

typedef enum {
    EXIT = 0,
    createNew = 1,
    read = 2,
    add = 3,
    del = 4,
    find = 5,
    Main = 6,
    SubMenu = -2,
    idle_state = -1
} finite_Machine_states;

typedef enum {
    yes = 'y', no = 'n', z_state = 'z'
} permissions;

typedef unsigned int HashType;

typedef struct {
    unsigned int year;
    unsigned int day;
    unsigned int month;
} publishingDate;

typedef struct booksPattern {
    HashType hash;
//    char author[LENGTH];
    char *author;
//    char title[LENGTH];
    char *title;
    publishingDate *date;
    unsigned int pagesQuantity;
    struct booksPattern *next;
    struct booksPattern *prev;
} Book;

Book *TABLE[TABLE_SIZE];

FILE *fptr;

finite_Machine_states currentState;
permissions permission;

HashType hashIndex;
HashType hash;

void act_mainMenu();

void act_subMenu();

void act_create_library();

void act_read_from_library();

void act_book_add();

void act_book_find();

void act_book_del();

void createBooksList();

void displayList();

void writeBooksList2File();

void delete_book_from_list( Book ** book , HashType hash);

void displaySelectedBook(Book *SelectedBook);

Book * readListFromLibrary(Book * table[]);

Book *findBook(Book *table[], HashType hash, HashType hashIndex);

float power(float value, int argument);

HashType Hash(char *str);

HashType HashIndex(HashType hash);

char * AllocateFlexibleString(FILE *pointer, char SeparateSymbol);

char * ReadStringFromFile(FILE *pointer, char SeparateSymbol);

void printString (char * string);

void clearTable();

int main() {
    setbuf(stdout, 0);

    for (int j = 0; j < TABLE_SIZE; ++j) TABLE[j] = NULL;

    void (*Action [6])();
    Action[createNew] = act_create_library;
    Action[read] = act_read_from_library;
    Action[add] = act_book_add;
    Action[del] = act_book_del;
    Action[find] = act_book_find;
    Action[Main] = act_mainMenu;

    currentState = idle_state;

    while (1) {
        if (currentState == idle_state) {
            act_mainMenu();
            scanf("%d", &currentState);
            (*Action[currentState])();
        }
        if (currentState == SubMenu) {
            act_subMenu();
            scanf("%d", &currentState);
            (*Action[currentState])();
        }
        if (currentState == EXIT){
            break;
        }
    }
    clearTable();
    printf("\nProgram was closed\n");
    return 0;
}

void act_mainMenu() {
    printf("1 - Create a new file\n");
    printf("2 - Open exist book's library\n");
    printf("3 - Add book to library\n");
    printf("4 - Delete book from library\n");
    printf("5 - Find book from library\n");
    printf("0 - Close the program\n");
    printf("------------------------------------------------------\n");
}

void act_subMenu() {
    printf("-{3 ADD}--{4 DEL}--{5 FIND}--{6 MAIN MENU}--{0 EXIT}-  \n");
    printf("====================================================\n");
}

void act_create_library() {
    fptr = fopen("BooksLibrary.txt", "w+");
    fclose(fptr);
    fptr=NULL;
    printf("\nFile BooksLibrary.txt was created\n"
                   "Would you like to add few books to new library? y/n\n");
    scanf("%s", &permission);
    if (permission == yes) {
        permission = z_state;
        currentState = SubMenu;
        act_book_add();
    }
    if (permission == no) {
        permission = z_state;
        currentState = idle_state;
    }
}

void act_read_from_library() {

    if ((fptr = fopen("BooksLibrary.txt", "r")) == NULL) {
        printf("----------------------------------------------------\n");
        printf("Error! opening file. Perhaps file doesn't exist\n");
        printf("----------------------------------------------------\n");
        printf("Would you like to create new library? y/n\n");
        scanf("%s", &permission);
        if (permission == yes) {
            permission = z_state;
            act_create_library();
        }
        if (permission == no) {
            permission = z_state;
            currentState = idle_state;
        }
    } else {
        *TABLE = readListFromLibrary(TABLE);
        displayList();
        currentState = SubMenu;
    }
    fclose(fptr);
}

void displayList() {
   Book * temp = NULL;
    for (int i = 0; i < TABLE_SIZE; i++) {
        temp = TABLE[i];
        while (temp != NULL) {
            printf("Book #%d\n", i);
            printf("Author: ");
            printString(temp->author);
            printf("Title: ");
            printString(temp->title);
            printf("HASH: %u\n", temp->hash);
            printf("Pages: %u\n", temp->pagesQuantity);
            printf("Date: %u/%u/%u\n", temp->date->day, temp->date->month, temp->date->year);
            printf("====================================================\n");
            temp = temp->next;
        }
    }
}

void writeBooksList2File() {
    Book *temp = NULL;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        temp = TABLE[i];
        while (temp != NULL) {
            fprintf(fptr, "%u\t", temp->hash);
            fprintf(fptr, "%s{%s}", temp->author, temp->title);
            fprintf(fptr, "%u\t", temp->pagesQuantity);
            fprintf(fptr, "%u\t%u\t%u\n", temp->date->day, temp->date->month, temp->date->year);
            temp = temp->next;
        }
    }
}

Book * readListFromLibrary(Book * table[]) {
    Book *temp = NULL;
    Book *p = NULL;
    clearTable();
    temp = (Book *) malloc(sizeof(Book));
    temp->date = (publishingDate *) malloc(sizeof(publishingDate));

    while (fscanf(fptr, "%u\t", &temp->hash) != EOF) {

        hashIndex = HashIndex(temp->hash);
        temp->author = ReadStringFromFile(fptr, '{');
        temp->title = ReadStringFromFile(fptr, '}');
        fscanf(fptr, "%u\t%u\t%u\t%u\n",
                  &temp->pagesQuantity,
               &temp->date->day, &temp->date->month, &temp->date->year);
            temp->next = NULL;
            temp->prev = NULL;

        if (table[hashIndex] == NULL) { // if list currently empty as first Book
            table[hashIndex] = temp;
        } else {
            p = table[hashIndex];
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = temp;
            p->next->prev = p;
        }
        temp = (Book *) malloc(sizeof(Book));
        temp->date = (publishingDate *) malloc(sizeof(publishingDate));
    }
    return *table;
}

void createBooksList() {
    int i = 1;
    Book *temp = NULL;
    Book *p = NULL;
    permission = yes;

    while (permission == yes) {
        permission = z_state;
        temp = (Book *) malloc(sizeof(Book));
        temp->date = (publishingDate *) malloc(sizeof(publishingDate));
        printf("Enter the data for Book's number: %d\n"
                       "\tAuthor: ", i++);
        temp->author = AllocateFlexibleString(stdin,'\n');
        printf("\tTitle: ");
        temp->title = AllocateFlexibleString(stdin,'\n');
        printf("\tPages' quantity: ");
        scanf("%u", &(temp->pagesQuantity));
        printf("\n Date in format -> dd/mm/yyy: ");
        scanf("%u%*c%u%*c%u", &(temp->date->month), &(temp->date->day), &(temp->date->year));

        temp->hash = Hash(temp->title);
        hashIndex = HashIndex(temp->hash);

        temp->next = NULL;
        temp->prev = NULL;

        if (TABLE[hashIndex] == NULL) {
            TABLE[hashIndex] = temp;
        } else {
            p = TABLE[hashIndex];
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = temp;
            p->next->prev = p;
        }
        printf("\nWould you like to add one more book? y/n\n");
        if (scanf("%s", &permission) == no)break;
    }
    permission = z_state;
}

void delete_book_from_list( Book ** book , HashType hash){
    Book * prevNode=NULL;
    Book * nextNode=NULL;
    Book * selectedNode = *book;

    if (selectedNode != NULL && selectedNode->hash == hash){
        *book=selectedNode->next;
//        (*book)->prev = NULL;
        free(book);
        book=NULL;
        return;
    }
    while (selectedNode!=NULL && selectedNode->hash!=hash){
        prevNode = selectedNode;
        selectedNode=selectedNode->next;
    }
    if (selectedNode == NULL) {
        printf("Doesn't exist\n");
        return;
    }
    if (selectedNode->next==NULL && selectedNode->hash == hash){
        prevNode = selectedNode->prev;
        prevNode->next = NULL;
    } else{
        nextNode = selectedNode->next;
        prevNode->next = nextNode;
        nextNode->prev = prevNode;
    }
    free(selectedNode);
    selectedNode = NULL;
}

void act_book_add() {
    if ((fptr = fopen("BooksLibrary.txt", "r")) != NULL){
        *TABLE=readListFromLibrary(TABLE);
    }else {
        printf("File is empty\n");
    }
    fclose(fptr);

    fptr = fopen("BooksLibrary.txt", "w+");
    createBooksList();
    displayList();
    writeBooksList2File();
    fclose(fptr);
    currentState = idle_state;
}

void displaySelectedBook(Book *SelectedBook) {
    printf("Author: ");
    printString(SelectedBook->author);
    printf("Title: ");
    printString(SelectedBook->title);
    printf("HASH: %u\n", SelectedBook->hash);
    printf("Pages: %u\n", SelectedBook->pagesQuantity);
    printf("Date: %u/%u/%u\n",
           SelectedBook->date->day,
           SelectedBook->date->month,
           SelectedBook->date->year);
    printf("====================================================\n");
}

Book * findBook(Book *table[], HashType hash, HashType hashIndex) {
    Book *SelectedBook = table[hashIndex];
    while (SelectedBook != NULL) {
        if (SelectedBook->hash == hash){
            return SelectedBook;
        }
        SelectedBook = SelectedBook->next;
    }
    return SelectedBook;
}

void act_book_del() {
    permission = z_state;
    hashIndex = 0;
    char * title;
    Book * SelectedBook=NULL;

    if ((fptr = fopen("BooksLibrary.txt", "r")) != NULL) {
        *TABLE=readListFromLibrary(TABLE);
    }else {
        printf("Error! opening file. Perhaps file doesn't exist\n");
        return;
    }
    fclose(fptr);

        printf("Please, enter book's title: \n");
        title = AllocateFlexibleString(stdin, '\n');

        hash = Hash(title);
        hashIndex = HashIndex(hash);

        SelectedBook = findBook(TABLE, hash, hashIndex);
        displaySelectedBook(SelectedBook);

        printf("Are you sure that you want to delete this book from library? y/n\n");
        scanf("%s", &permission);
        if (permission == yes) {
            permission = z_state;
            delete_book_from_list(&TABLE[hashIndex], hash);
            printf("Book was deleted\n");

            fptr = fopen("BooksLibrary.txt", "w+");
            writeBooksList2File();
            fclose(fptr);

            printf("Lirary was updated\n");
        }
    currentState = idle_state;
}

void act_book_find() {
    hashIndex = 0;
    char  * title;
    Book * SelectedBook=NULL;
    if ((fptr = fopen("BooksLibrary.txt", "r")) != NULL){
        *TABLE=readListFromLibrary(TABLE);
        printf("Please, enter book's title: \n");
        fflush(stdin);
        title = AllocateFlexibleString(stdin,'\n');
        hash = Hash(title);
        hashIndex = HashIndex(hash);
        SelectedBook = findBook(TABLE, hash, hashIndex);
        if (SelectedBook != NULL)
            displaySelectedBook(SelectedBook);
        else printf("Book with title %s wasn't found\n", title);
    } else printf("Error! opening file. Perhaps file doesn't exist\n");
    fclose(fptr);
    currentState = SubMenu;
}

float power(float value, int argument) {
    float result = 1;
    for (int i = 0; i < argument; ++i) {
        result *= value;
    }
    return result;
}

HashType Hash(char *str) {
    HashType h = 0;
    int argument = 0;
    float polynomialKoefficient = 1.1;
    while (*str) h += *str++ * power(polynomialKoefficient, argument++);
    return h;
}

HashType HashIndex(HashType hash) {
    return hash % TABLE_SIZE;
}

void clearTable() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        free(TABLE[i]);
        TABLE[i] = NULL;
    }
}
char * AllocateFlexibleString(FILE *pointer, char SeparateSymbol){
    char * string;
    int i = 0;
    int j = 1;
    int temp = 0;
//    fflush(pointer);
    string = (char*) malloc(sizeof(char));
    while ((char)temp != SeparateSymbol){
        temp =getc(pointer);
        string = (char*) realloc(string,j++ * sizeof(char));
        string[i] = (char)temp;
        i++;
    }
    string[i-1]='\0';
    return string;
}

char * ReadStringFromFile(FILE *pointer, char SeparateSymbol){
    char *string;
    int i = 0;
    int j = 1;
    int temp = 0;
    string = (char*) malloc(sizeof(char));
    while ((char)temp != SeparateSymbol){
        temp =getc(pointer);
        string = (char*) realloc(string,j++ * sizeof(char));
        string[i++] = (char)temp;
    }
    string[i-1]='\0';
    return string;
}

void printString (char * string){
    while (*string){
//        if (*string == '_') {
//            *string = ' ';
//        }
        printf("%c",*string++);
    }
    printf("\n");
}