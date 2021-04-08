
extern int isGame, client_cnt, turn, wait_t, wait_s;
extern int client_socks[];
extern char* ids[];
extern pthread_mutex_t mutx;
extern void gamecall(int*,int,char**);
int onCi, live, isYes;
int all[56], eachCards[4][56], eachEnd[4], onCnt[4], onCards[56], onn[4];

void set(int*);
void shuffle(int*);
void holli_st();
void card_draw();
void get_oncards(int, int);
void give_mycards(int);
void chk_end();
void during_game(int, char*);
void make_start(int);
