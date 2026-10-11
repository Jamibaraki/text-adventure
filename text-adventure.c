#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "text-adventure.h"

#define STRUCT_SIZE 10
#define BUFFER_SIZE 32


//TODO: drop? inventory?

#define FOREACH_VERB(VERB) \
        VERB(get)   \
        VERB(take)  \
        VERB(use)   \
        VERB(put)   \
        VERB(eat)  \
        VERB(read)  \
        VERB(smell)  \
        VERB(examine)  \
        VERB(key)  \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum verbs {
    FOREACH_VERB(GENERATE_ENUM)
};

static const char *verb_string[] = {
    FOREACH_VERB(GENERATE_STRING)
};

struct exitlist {
    int n;
    int s;
    int w;
    int e;
};

typedef struct exitlist Exitlist;

//item structure - have an id, and responses to certain verbs.
struct item {
    int id;
    int room;
    int target;
    bool isHeld;
    const char* eat;
    const char* read;
    const char* smell;
    const char* name;
    const char* description;
    const char* pronoun;
};

typedef struct item Item;


int current_room = 1;

int secondw = -1;

int itemsLength = 0;

char *blockmessage = "Not thataway!"; // "you cannot go that way, fool!"

int highest_verb = examine;

struct room {

    const char *name;
    Exitlist exits;
    int id;
    const char *description;

};

typedef struct room Room;

Room rooms[STRUCT_SIZE];
Item items[STRUCT_SIZE];

int *tokens;

int main(){
    bool game_on = true;


    //no room 0, using that as a null exit
    // exits - n,s,e,w
    createRoom(1, "hallway", "It's a long, dusty hall, with green and brown wallpaper. There's a door to the north and stairs to the east",2,0,3,0 );
    //createRoom(0, &"hallway");
    createRoom(2 , "kitchen", "Dull knives hang from the ceiling. A cat peers up at you. A door leads south.",0,1,0,0);
    createRoom(3, "stairs", "The stinky stairs have a faint pong about them. Or is that you? Head west to descend. North to go up to the landing.",4,0,0,1);
    createRoom(4,"Landing","A small landing. The stairs are to the south. There's a door to the west.",0,3,0,-1);

    createItem(1,2,3,"key","a small and golden key","a","it wouldn't agree with you","it smells metallic","do you need glasses?");
    itemsLength = 2; //currently 1 higher than actual due to index not starting at 0..

    //myprint("Welcome to the incredible world of binkus");
    printf("Welcome to the incredible world of Binkus\n");

    while( game_on == true ){
        myprint(rooms[current_room].name);
        myprint(rooms[current_room].description);

        //print inventory TODO - move elsewhere and show this on 'i'/'inventoory' command
        printf("You are carrying ");
        bool isCarrying = false;
        //go through items.. set is carrying true if find anything and print them.

        for (int j = 0; j < itemsLength; j++) {
            if(items[j].isHeld == true ){
                isCarrying = true;
                printf("%s %s ", items[j].pronoun, items[j].name);
            }
        }

        if (isCarrying == false){ printf( "nothing!");}
        printf("\n");

        //print any items in room
        for (int i = 0; i < itemsLength; i++) {
            if( current_room == items[i].room){
                printf("There is a ");
                printf(items[i].name);
                printf(" here.\n");
            }
        }


        myprint("What will you do?");
        char input[BUFFER_SIZE];

        fgets( input, sizeof(input),stdin);
        input[strcspn(input, "\n")] = 0;


        //pre-parser. May fold this in later
        if ( strcmp(input,"q") == 0 ) {
            game_on = false;
        } else if(strcmp(input,"n") == 0 || strcmp(input,"north") == 0 ){
            printf("north, eh?\n");
            moveRoom(rooms[current_room].exits.n);
        } else if(strcmp(input,"s") == 0 || strcmp(input,"south") == 0){
            printf("south, eh?\n");
            moveRoom(rooms[current_room].exits.s);
        } else if(strcmp(input,"w") == 0 || strcmp(input,"west") == 0 ){
            printf("west, eh?\n");
            moveRoom(rooms[current_room].exits.w);
        } else if(strcmp(input,"e") == 0 || strcmp(input,"east") == 0 ){
            printf("east, eh?\n");
            moveRoom(rooms[current_room].exits.e);
        } else if(strcmp(input,"redacted for GitHub!") == 0){
            printf("What a terrible smell. What a terrible person!\n");
        } else if(strcmp(input,"also redacted") == 0 || strcmp(input,"redacted") == 0){
            printf("Gosh, how awful. You didn't even take your trousers off!\n");
        } else {
            //myprint("I do not know what in the name of Peter you're on about!");
            //do the proper parsing if it isn't one of these obvious phrases.
            tokens = parseInput(input);
            processTokens(tokens);
            free(tokens);
        }
    }
    return 0;
}

//check if item is in inventory
int isOwned( int target ){
    target -= highest_verb;
    return items[target].isHeld;
}

//try moving to target room
int moveRoom( int target){
    if (target == 0 ) {
        myprint(blockmessage);
    } else if (target < 0){
        myprint("There's a locked door there!");
    } else {
        current_room = target;
    }
}

//this function deals with the tokens that represent the content of the users input, 2 ints, typically a verb and noun
void processTokens(int* input){

    //check at least 1 token is valid
    if (input[0]==-1 && input[1]==-1){
        printf("What a silly comment.\n");
        return;
    }

    //if we just have 1 valid token, put it in the first one, then process it
    if (input[0]==-1 || input[1]==-1){
        input[0] = fmax(input[0],input[1]);

        //handle input with just 1 valid token
        switch(input[0]){
            case 0:
                printf("Get what??\n");
                break;
            case 1:
                printf("Take what exactly?\n");
                break;
            case 2:
                printf("Put.. something??\n");
                break;
            case 3:
                printf("Eat.. what??\n");
                break;
            case read:
                printf("You mean a book?\n");
                break;
            case smell:
                printf("A general sniff reveals little.\n");
                break;
            case examine:
                printf("Examine what??\n");
                break;
            default:
                printf("Do that to what?\n");

        }
        return;
    }


    //setting the highest verb to differentiate between that and nouns (objects in the world to be manipulated)
    if ( input[0] > highest_verb && input[1] > highest_verb ){
        printf("How about a verb in the sentence, deary?");
        return;
    }
    if ( input[0] <= highest_verb && input[1] <= highest_verb ){
        printf("How about a noun somewhere. All these verbs confuse me.");
        return;
    }


    //swap the words over if they are inverted
    if (input[0] > highest_verb ) {
        int temp = input[0];
        input [0] = input[1];
        input[1] = temp;
    }

    //OK, the above should work if we had two tokens, but not if there was just one valid token (or zero, dunno if that's possible here!
    //the big 2 word parser
    // MAIN ACTUAL VERB HANDLING AND IMPLEMENTATION
    //probably should do a check on the getitem?? perhaps test for ownership first too and mention if it isn't present.
    switch(input[0]){
    //get or take
    case get:
    case take:
        printf("Getting or taking something\n");
        //check the target item is in the room
        if( getItem(input[1])->room == current_room ){
            printf("it's here!\n");
            struct item* this_item = &items[input[1]-highest_verb];
            this_item->isHeld = true;
            this_item->room = -1;
        } else {
            printf("it's not here!\n");
        }
        break;
    case use:
        myprint("using somehow..");
        //use the target property to check if the item target is the current room
        if( getItem(input[1])->target == current_room ){
            implementAction( getItem(input[1])->target );
        }
        //could possibly also check if the target property is an item
        break;
    case examine:
        myprint( getItem(input[1])->description );
        break;
    case smell:
        myprint( getItem(input[1])->smell );
        break;
    case eat:
        if( isOwned(input[1])){
                myprint( getItem(input[1])->eat );
        } else {
            myprint("You don't have it");
        }

        break;

    }


}

//actually implement actions when conditions are met
int implementAction(int action){
    switch(action){

    case 3:
        //using the key to unlock a door
        printf("you unlock the door\n");
        break;
    }

    return;

}

//returns item from list TODO currently just points at only item
struct item* getItem(int input){
    return &items[input-highest_verb];
}


//parser!
int* parseInput(char* input){

    //declare the tokens array with blank tokens
    int tokens[2] = {-1,-1};

    int stleng = strlen(input);


    char verb[BUFFER_SIZE];
    char noun[BUFFER_SIZE];
    char thirdword[BUFFER_SIZE];

    int firstw = strcspn(input, " ");

    //create a verb string based on its length
    strncpy(verb,input,firstw);
    verb[firstw] = '\0';  //adding the null terminator to the end of the word

    //attempt to find the second space by using firstw added to input to start the check from there..
    int secondw = strcspn(input+firstw+1," ");

    int vcode = -1;
    int ncode = -1;

    strncpy(noun,input+firstw+1,secondw);
    noun[secondw] = '\0'; //adding null terminator here, too

    int thirdwindex =-1;

    //do we look for pronoun? exclude for now as can be disregarded
    //if the second space isn't at the end, do this
    if( ( stleng > (secondw+firstw+1) ) && ( isPronoun(&noun) ) ) {

        //get the third word!
        thirdwindex = strcspn(input+firstw+secondw+2," ");
        strncpy(thirdword,input+firstw+secondw+2,thirdwindex);
        thirdword[thirdwindex] = '\0';
    }

    vcode = findToken (verb);

    if ( thirdwindex > 0 ) {
        ncode = findToken(thirdword);
    } else if ( secondw > 0 ) {
        ncode = findToken(noun);
    }

    int* result = malloc(2*sizeof(int));
    result[0] = vcode;
    result[1] = ncode;
    return result;
}

//get the int token from the text input
int findToken( char* input){

    //get char[] size..
    size_t count = sizeof(verb_string)/sizeof(verb_string[0]);
    //return token int if it matches something in the verb string array
    for(size_t i=0;i<count;i++){
        if(strcmp(input,verb_string[i]) == 0 ){
            return i;
        }
    }

    return -1;
}



bool isPronoun(char* input){
    if(strcmp(input,"the") == 0){
            return true;
    } else if(strcmp(input,"a") == 0){
        return true;
    } else if(strcmp(input,"on") == 0){
        return true;
    } else if(strcmp(input,"an") == 0){
        return true;
    } else if(strcmp(input,"my") == 0){
        return true;
    } else if(strcmp(input,"this") == 0){
        return true;
    } else if(strcmp(input,"that") == 0){
        return true;
    } else if(strcmp(input,"it") == 0){ //note IT could be handled separately > rather complex
        return true;
    }

    return false;
}

void createItem(int a_id, int room, int target, char* name,char* description, char* pronoun, char* eat, char* smell, char* read){
    Item myitem;
    myitem.id = a_id;
    myitem.room = room;
    myitem.target = target;
    myitem.name = name;
    myitem.description = description;
    myitem.pronoun = pronoun;
    myitem.eat = eat;
    myitem.smell = smell;
    myitem.read = read;
    myitem.isHeld = false;
    items[a_id] = myitem;
}

void createRoom(int a_id, char* name, char* description, int n, int s, int e, int w){

    Exitlist exits;
    exits.n = n;
    exits.s = s;
    exits.w = w;
    exits.e = e;

    Room myroom;
    myroom.id = a_id;
    myroom.name = name;
    myroom.description = description;
    myroom.exits = exits;
    rooms[a_id] = myroom;
}


int myprint(char* input){
    printf(input);
    printf("\n");
    return 0;

}
