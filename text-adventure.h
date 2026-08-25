void createRoom(int a_id, char* name, char* description, int n, int s, int e, int w);

void createItem(int a_id, int room, int target, char* name, char* description, char* pronoun, char* eat, char* smell, char* read);

//my custom print function automatically does \n.
int myprint(char* input);

//process the input from the user
int* parseInput(char* input);

//checks if a word is a pronoun - current approach is to disregard them
bool isPronoun(char* input);

//returns an int token based on a single word extracted from text input
int findToken(char* input);

//handles the tokens and convert them into meaningful actions, such as taking an item
void processTokens(int* input);

//TODO get item
struct item* getItem(int input);

//TODO turns input actions into effects on game world
int implementAction(int action);

//moves to target room, or raises the appropriate objection
int moveRoom( int target);


//check if item is in inventory
int isOwned( int target );
