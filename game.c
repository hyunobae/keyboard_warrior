#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <pthread.h>
#define wordcnt 999
#define blank "               "
#define TTL 64
#define BUF_SIZE 30
#define MESSAGE "K E Y B O A R D W A R R I O R"
#define BLANK "				    "
#define taedori "---------------------------------------------------------------------------------------------------------------------------"
#define pado "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define udp 225.1.1.2

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void single_play(); //405
void multi_play();
void move_msg(int);
int set_ticker(int);
void show_title(); /////355
void card_flip();
void rain();
int get_ok_char();
char **word;
int count = 0;
int HP = 3;
FILE *short_input;
int rain_correct_count=0;
void life_draw();
int cardcnt=0;
int b_cnt = 0;
int w_cnt = 0;
void card(int);
void boxing();
void now_correct();
void multirain(int recv_s, int send_s, struct sockaddr_in mcast_group, char player[2]);
void multicard(int recv_sock, int send_sock, struct sockaddr_in mcast_group, char player[2]);
void multirain_set();
void error_handling(char *message);
int row;
int col;
int dir;
char GID[20];
char portnum[5];
char nowplayer[2];
int ready1 = 0;
int ready2 = 0;
int galbae=0;
int flaga=0;
int flagb=0;
void multirainend();
void draw_multi(int);
void multicard_set();

int send_sock;
struct sockaddr_in mcast_group;

struct wordinfo {
	char wordn[20];
	int row;
	int col;
	int show;
};

struct card{
	char word[20];
	int color;
	int r;
	int c;
};
struct card array[50];

struct sendarg {
	int send_sock;
	struct sockaddr_in mcast_group;
};

struct recvarg {
	int recv_sock;
	struct sockaddr_in from;
};


typedef struct Inputrem{
	char rem[1000];
}Inputrem;

struct wordinfo LOC[1000];

char *getword(int idx) {
	int i; 

	for (i = 0; i < wordcnt + 1; i++) {
		if (i == idx) {
			return word[i];
		}
	}
}

void stopitimer() {	
	struct itimerval set_time_val;

	set_time_val.it_value.tv_sec = 0;
	set_time_val.it_value.tv_usec = 0;
	set_time_val.it_interval.tv_sec = 0;
	set_time_val.it_interval.tv_usec = 0;

	setitimer(ITIMER_REAL, &set_time_val, NULL);

}

void now_correct(){
	move(LINES-5, COLS/2-100);
	printw("		     ");
	move(LINES-5, COLS/2-100);
	printw("CORRECT WORD : %d",rain_correct_count);
	refresh();
}

void draw(int signum) {

	//새로 입력할 단어 받아와서 구조체 배열에 저장
	int idx = rand()%(wordcnt + 1);
	char *w = getword(idx);
	int column = rand()%(COLS);
	char c;
	strcpy(LOC[count].wordn, w); 
	LOC[count].row = 0;
	LOC[count].col = column;
	LOC[count++].show = 1;



	//출력되어있는 단어 지우기
	for (int i = 0; i < count; i++) {
		move(LOC[i].row, LOC[i].col);
		addstr(blank);
	}
	now_correct();
	//한줄 내려서 출력하기
	for (int i = 0; i < count; i++) {
		life_draw();
		if (LOC[i].row + 1 >= LINES-10 && LOC[i].show == 1) {
			LOC[i].show = 0;
			HP = HP - 1;
			if (HP == 0) {
				stopitimer();
				clear();
				refresh();
				move(LINES/2, COLS/2-20);
				printw("YOU TYPED %d CORRECT WORD! CHEER UP!", rain_correct_count);
				move(LINES/2+15, COLS/2-20);
				printw("////PRESS ANY KEY TO GO BACK////");
				while(1) {
					c = getch();
					if (c != 0) break;
				}
				if (c != 0) exit(1);

			}

		}
		if (LOC[i].show == 1) {
			LOC[i].row++;
			move(LOC[i].row, LOC[i].col);
			addstr(LOC[i].wordn);
		}
	}

	//새로운 단어 출력
	move(LOC[count].row, column);
	addstr(LOC[count].wordn);
	refresh();
}

void draw_multi(int signum) {

	//새로 입력할 단어 받아와서 구조체 배열에 저장
	int idx = rand()%(wordcnt + 1);
	char *w = getword(idx);
	int column = rand()%(COLS);
	char c;
	strcpy(LOC[count].wordn, w); 
	LOC[count].row = 0;
	LOC[count].col = column;
	LOC[count++].show = 1;



	//출력되어있는 단어 지우기
	for (int i = 0; i < count; i++) {
		move(LOC[i].row, LOC[i].col);
		addstr(blank);
	}
	now_correct();
	//한줄 내려서 출력하기
	for (int i = 0; i < count; i++) {
		life_draw();
		if (LOC[i].row + 1 >= LINES-10 && LOC[i].show == 1) {
			LOC[i].show = 0;
			HP = HP - 1;
			if (HP == 0) {
				multirainend();
				exit(1);
			}
		}
		if (LOC[i].show == 1) {
			LOC[i].row++;
			move(LOC[i].row, LOC[i].col);
			addstr(LOC[i].wordn);
		}
	}
	//새로운 단어 출력
	move(LOC[count].row, column);
	addstr(LOC[count].wordn);
	refresh();
}

void multirainend(){

	char c;
	stopitimer();
	clear();
	refresh();

	sendto(send_sock,"END",4,0,(struct sockaddr *)&mcast_group,sizeof(mcast_group));
	move((LINES/2)-5, COLS/2-20);
	printw("YOU LOSE!!");
	move(LINES/2, COLS/2-20);
	printw("YOU TYPED %d CORRECT WORD! CHEER UP!", rain_correct_count);
	move(LINES/2+5, COLS/2-20);
	printw("////PRESS CTRL+C TO EXIT THE GAME////");
	while(1){
		c = getch();
		if(c!=0)break;
	}
	if(c!=0) exit(1);


}
void multirainendsecond(){
	char c;
	stopitimer();
	clear();
	refresh();

	move((LINES/2)-5, COLS/2-20);
	printw("You Win!!");
	move(LINES/2, COLS/2-20);
	printw("YOU TYPED %d CORRECT WORD! CHEER UP!",rain_correct_count);
	move(LINES/2+5, COLS/2-20);
	printw("////PRESS CTRL+C TO EXIT THE GAME////");
	while(1){
		c = getch();
		if(c!=0)break;
	}
	if(c!=0) exit(1);

}

void life_draw(){
	printw("		");
	move(LINES-5, COLS/2-5);
	printw("LIFE:");

	for(int i=1; i<=HP; i++){
		move(LINES-5, (COLS/2)+i+2);
		printw("O");
		refresh();
	}
}

void rain() {
	void draw(int);
	char temp[20];
	char input[20];
	char c;
	char *w;
	int fd, cnt = 0, row = 0;
	struct itimerval set_time_val;

	clear();
	refresh();
	curs_set(0);

	move(25,90);
	addstr("3");
	refresh();
	sleep(1);

	clear();
	move(25,90);
	addstr("2");
	refresh();
	sleep(1);

	clear();
	move(25,90);
	addstr("1");
	refresh();
	sleep(1);

	clear();
	move(25,90);
	addstr("START!");
	refresh();
	sleep(1);

	clear();

	// 파일에서 단어 읽어오기 - open, read
	word = (char**)malloc(sizeof(char*)*wordcnt);
	for (int i = 0; i < wordcnt; i++) {
		word[i] = (char*)malloc(sizeof(char)*20);
	}

	fd = open("term.txt", O_RDONLY);
	for (int i = 0; i < wordcnt; i++) {
		while(1) {
			read(fd, &c, sizeof(char));
			if (c  == '\n') break;
			temp[cnt++] = c;
		}
		strcpy(word[i], temp);
		cnt = 0;
		memset(temp, 0, 20);	
	}
	close(fd);


	signal(SIGALRM, draw);
	set_time_val.it_value.tv_sec = 0;
	set_time_val.it_value.tv_usec = 600000;
	set_time_val.it_interval.tv_sec = 0;
	set_time_val.it_interval.tv_usec = 600000;

	setitimer(ITIMER_REAL, &set_time_val, NULL);


	initscr();
	noecho();
	crmode();
	clear();

	move(LINES-10, 4);
	addstr(pado);

	curs_set(0);
	srand((unsigned)time(NULL));

	int idx = rand()%(wordcnt +1);
	w = getword(idx);
	int column = rand()%COLS;
	strcpy(LOC[count].wordn, w); 
	LOC[count].row = 0;
	LOC[count].col = column;
	LOC[count++].show = 1;
	move(row, column);
	addstr(w);
	refresh();


	while(1) {
		scanw("%s", input);
		for (int k = 0; k < count; k++) {
			if (strcmp(LOC[k].wordn, input) == 0) {
				move(LOC[k].row, LOC[k].col);
				addstr(blank);
				LOC[k].show = 0;
				rain_correct_count++;
			}
		}	
	}
}

void short_game(){
	struct timeval start_time,end_time;
	char rem[1000];
	char input[1000];
	int c;
	int inputnum = 20;
	Inputrem a[20];
	int i=0;
	int line=10;
	int flag=0;
	double tasu;
	int count = 0; //몇 타자를 쳤는가?
	int endwin_flag = 0; //endwin했는가?
	int whereline = 0;//몇번째 라인하는 중인가
	int correct =0; //정확도
	int index=0; //
	int backspace_flag = 0; //backspace인가?
	int start_flag = 0; //start했나?

	short_input = fopen("short_input.txt","r");

	//파일에 있는 짧은 글 a[i].rem에 저장
	for(i=0;i<inputnum;i++){
		fgets(a[i].rem,1000,short_input);
	}

	//짧은 글 보여주기
	initscr();
	curs_set(1);//////////////////
	clear();
	start_color();
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(0,COLOR_WHITE, COLOR_BLACK);

	//테두리 그리기
	move(line-1,20-2);
	addstr(taedori);

	i=0;
	//짧은 줄 출력
	move(line,20);
	attron(COLOR_PAIR(1));
	addstr(a[i++].rem);
	attroff(COLOR_PAIR(1));
	refresh();

	//입력할 때 시작하자마자 시간재서 엔터누르면 시간재기 끝
	while(1){
		move(line-1,20-2);
		addstr(taedori);

		count=0;
		correct=0;
		line+=2;
		move(line,20);

		while((c=getch()) != '\n'){ //입력
			backspace_flag = 0;
			start_flag=0;
			if(flag==0){
				gettimeofday(&start_time,NULL); //입력 시작
				flag=1;
			}
			int x,y;
			getyx(stdscr,y,x);

			if(c==127 && x<=22){ //아무것도 안친상태에서 backspace 처리
				move(y,x-2);
				addstr("  ");
				move(y,x-2);
				start_flag=1;

			}
			else if(c==127 && x>=23){ //backspace
				int x, y;
				backspace_flag=1;

				getyx(stdscr,y,x);
				move(y,x-1);
				addstr(" ");
				move(y,x-2);
				addstr(" ");
				move(y,x-3);
				addstr(" ");
				move(y,x-3);
				count-=2;
			}
			else{
				rem[count] = c;
				start_flag=0;
			}
			//	count++;
			if(backspace_flag == 0 && start_flag != 1){
				if(a[whereline].rem[count] == c){ //white
					move(line-2, 20+count);
					attron(COLOR_PAIR(0));
					printw("%c",a[whereline].rem[count]);
					attroff(COLOR_PAIR(0));
					move(line,20+count+1);

				}
				else{//red
					move(line-2,20+count);
					attron(COLOR_PAIR(1));
					printw("%c",a[whereline].rem[count]);
					attroff(COLOR_PAIR(1));
					move(line,20+count+1);

				}
				//start_flag=1;
			}
			if(start_flag==0)	count++;
		}
		//짧은 글 얼마의 인덱스인지 가져오기

		move(line+1,18);
		addstr(taedori);

		//정확도 계산
		for(int i=0;i<strlen(a[whereline].rem)-1;i++){
			if(a[whereline].rem[i] == rem[i])
				correct++;
		}


		//초기화
		whereline++;
		flag=0;
		index = 0;
		//한줄 치는 동안 걸린 시간
		gettimeofday(&end_time,NULL);
		double operating_time = (double)(end_time.tv_sec)+(double)(end_time.tv_usec)/1000000.0-(double)(start_time.tv_sec)-(double)(start_time.tv_usec)/1000000.0;

		tasu = count * (60/operating_time);

		mvprintw(39,18,"%s",taedori);
		move(40,20);
		printw("Time : %f",operating_time);
		mvprintw(41,18,"%s",taedori);
		move(42,20);
		printw("Count: %d, Tasu : %f, How many correct : %d, How many length : %d",count,tasu,correct,strlen(a[whereline-1].rem)-1);
		mvprintw(43,18,"%s",taedori);
		move(44,20);
		printw("correct Percent : %.2f%",(double)((double)correct/(double)(strlen(a[whereline-1].rem)-1))*100);
		mvprintw(45,18,"%s",taedori);
		if(line==32){ //5줄 입력
			move(48,20);
			addstr("  If you want to next page, press 'n' (if not, press any key)");
			move(50,20);
			c=getch();

			if(c=='n'){
				clear();
				refresh();
				line = 10;
				move(line,20);
				attron(COLOR_PAIR(1));
				addstr(a[i++].rem);
				attroff(COLOR_PAIR(1));
				refresh();
			}
			else{
				clear();
				refresh();
				curs_set(0);
				fclose(short_input);
				exit(1);
				//endwin();
				endwin_flag=1;
			}
		}
		else{
			line+=2;
			move(line,20);
			attron(COLOR_PAIR(1));
			addstr(a[i++].rem);
			attroff(COLOR_PAIR(1));
			refresh();
		}
		if(endwin_flag==1)
			break;
	}
	fclose(short_input);
	exit(3);
}

int set_ticker(int n_msecs){
	struct itimerval new_timeset;
	long n_sec, n_usecs;

	n_sec = n_msecs / 1000;
	n_usecs = (n_msecs % 1000) * 1000L;

	new_timeset.it_interval.tv_sec = n_sec;
	new_timeset.it_interval.tv_usec = n_usecs;
	new_timeset.it_value.tv_sec = n_sec;
	new_timeset.it_value.tv_usec = n_usecs;

	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}

int stop_ticker(){
	struct itimerval time;

	time.it_value.tv_sec = 0;
	time.it_value.tv_usec = 0;
	return setitimer(ITIMER_REAL, &time, NULL);
}

void move_msg(int signum){
	signal(SIGALRM, move_msg);
	move(row, col);
	addstr(BLANK);
	col += dir;
	move(row, col);
	addstr(MESSAGE);
	refresh();

	if(dir == -1 && col <= 60)
		dir = 1;
	else if(dir == 1 && col+40 >= 120)
		dir=-1;
}

void show_title(){
	int delay;
	int ndelay=0;
	char input;
	char temp;
	void move_msg(int);

	initscr();
	crmode();
	noecho();
	clear();
	curs_set(0);
	row=15;
	col=50;
	dir=1;
	delay=1000;

	start_color();
	attron(A_BOLD);//여기고침!!!!!!!!!!!!!!!!!!!!!
	move(row,col);
	addstr(MESSAGE);
	boxing();
	//attroff(A_STANDOUT);

	signal(SIGALRM, move_msg);
	set_ticker(delay);
	ndelay = delay/12;
	set_ticker(delay=ndelay);

	move(30,60);
	addstr("1. SINGLE PLAY");
	move(30,100);
	addstr("2. MULTI PLAY");

	while(true){
		ndelay=0;
		if(ndelay>0)
			set_ticker(delay=ndelay);

		input = tolower(get_ok_char());

		if(input == '1'){
			stop_ticker();
			single_play();
			break;
		}

		else if(input == '2'){
			stop_ticker();
			multi_play();
			break;
		}
	}
}
void boxing(){//여기고침!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for(int i=5; i<=49; i++){
		move(i,10);
		attron(A_STANDOUT);
		addstr("|");
		move(i, 170);
		addstr("|");
	}

	for(int i=10; i<=170; i++){
		move(5,i);
		addstr("-");
		move(50,i);
		addstr("-");
	}
	attroff(A_STANDOUT);
}

void multirain(int recv_s, int send_s, struct sockaddr_in mcast_group, char player[2]) {
	void draw_multi(int);
	char temp[20];
	char input[20];
	char *inputp;
	char sendinput[30];
	char play[20];
	char nowplayer[2];
	char c;
	char *w, *w1, *ptr;
	int fd, cnt = 0, row = 0, len = 0, n = 0, pid = 0;
	int start1 = 0, start2 = 0;
	struct itimerval set_time_val;
	int flaga=0;
	int flagb=0;

	strcpy(nowplayer, player);

	// 파일에서 단어 읽어오기 - open, read
	word = (char**)malloc(sizeof(char*)*wordcnt);
	for (int i = 0; i < wordcnt; i++) {
		word[i] = (char*)malloc(sizeof(char)*20);
	}

	fd = open("term.txt", O_RDONLY);
	for (int i = 0; i < wordcnt; i++) {
		while(1) {
			read(fd, &c, sizeof(char));
			if (c  == '\n') break;
			temp[cnt++] = c;
		}
		strcpy(word[i], temp);
		cnt = 0;
		memset(temp, 0, 20);	
	}
	close(fd);

	signal(SIGALRM, draw_multi);
	set_time_val.it_value.tv_sec = 0;
	set_time_val.it_value.tv_usec = 600000;
	set_time_val.it_interval.tv_sec = 0;
	set_time_val.it_interval.tv_usec = 600000;

	initscr();
	noecho();
	crmode();
	clear();

	curs_set(0);
	srand((unsigned)time(NULL));

	int idx = rand()%(wordcnt +1);
	w = getword(idx);
	int column = rand()%COLS;
	if ((pid = fork()) < 0) {
		printf("error: fork\n");
		exit(0);

	} else if (pid == 0) {
		struct sockaddr_in from;
		char message[20];
		while(1) {	
			//전송받은거 출력
			len = sizeof(from);
			strncpy(w, "\0", strlen(w));
			if ((n = recvfrom(recv_s, w, 20, 0, (struct sockaddr *)&from, &len)) > 0) {	
				flagb=0;
				if(strcmp(w,"END")==0){
					multirainendsecond();
				}
				if (strcmp(w, "READY1") == 0 || strcmp(w, "READY2") == 0) {
					char red = w[strlen(w)-1];
					if (red == '1')
						start1 = 1;
					else if (red == '2')
						start2 = 1;

					if (strcmp(w, "READY1") == 0 && strcmp(nowplayer, "1") == 0) {
						clear();
						move(LINES/2, COLS/2-20);
						printw("You're READY PLAYER 1!!!!!!!!!!!!!!");
						refresh();
					} else if (strcmp(w, "READY2") == 0 && strcmp(nowplayer, "2") == 0) {
						clear();
						move(LINES/2, COLS/2-20);
						printw("You're READY PLAYER2!!!!!!!!!!!!!!");
						refresh();
					}

					if (start1 == 1 && start2 == 1) {
						clear();
						fflush(stdin);
						sleep(1);
						flaga=1; flagb=1;
						move(LINES-10, 4);
						addstr(pado);
						refresh();
						setitimer(ITIMER_REAL, &set_time_val, NULL);
					}
				}


				if(flaga==1 && flagb==0){
					if (start1 == 1 && start2 == 1) {
						w[strlen(w)] = '\0';
						char rem[20];
						strcpy(rem,w);
						rem[strlen(rem)-1] = '\0';
						char *idx = 0;
						if (strstr(w, "2") == 0 && strcmp(nowplayer, "1") == 0) {
							for (int k = 0; k < count; k++) {
								if(strcmp(LOC[k].wordn, rem)==0){
									move(LOC[k].row, LOC[k].col);
									addstr("                   ");
									LOC[k].show = 0;
									rain_correct_count++;
								}

							}
						} else if (strstr(w, "1") == 0 && strcmp(nowplayer, "2") == 0){
							for (int k = 0; k < count; k++) {

								if(strcmp(LOC[k].wordn,rem) ==0){
									move(LOC[k].row, LOC[k].col);
									addstr("                     ");
									LOC[k].show = 0;
									rain_correct_count++;
								}		
							}
						} else if (strstr(w, "1") == 0 && strcmp(nowplayer, "1") == 0) {
							w[strlen(w)-1] = '\0';
							column = rand()%COLS;
							strcpy(LOC[count].wordn, w); 
							LOC[count].row = 0;
							LOC[count].col = column;
							LOC[count++].show = 1;
							move(row, column);
							addstr(w);
							refresh();

						} else if (strstr(w, "2") == 0 && strcmp(nowplayer, "2") == 0){
							w[strlen(w)-1] = '\0';
							column = rand()%COLS;
							strcpy(LOC[count].wordn, w); 
							LOC[count].row = 0;
							LOC[count].col = column;
							LOC[count++].show = 1;
							move(row, column);
							addstr(w);
							refresh();
						}
					}
				}
			}
		}

	} else {
		move(LINES/2, COLS/2-20);
		addstr("TYPE \"READY\" TO GET READY");
		refresh();

		while(1) {
			scanw("%s", input);
			int inputlen = strlen(input);

			if (strcmp(input, "READY") == 0) {
				strcpy(sendinput, input);			//sendinput = "input"
				strcat(sendinput, player);			//sendinput = "input1(2)"    */
				sendinput[inputlen+1] = '\0';
				len = strlen(sendinput);			
				sendto(send_s, sendinput, len, 0, (struct sockaddr *)&mcast_group, sizeof(mcast_group));

			}
			else {
				for(int k=0;k<wordcnt;k++){
					if(strcmp(word[k],input)==0){
					strcpy(sendinput, input);			//sendinput = "input"
					strcat(sendinput, player);			//sendinput = "input1(2)"    */
					sendinput[inputlen+1] = '\0';
					len = strlen(sendinput);			
					sendto(send_s, sendinput, len, 0, (struct sockaddr *)&mcast_group, sizeof(mcast_group));						
			}
				}
			}
					memset(input, 0, sizeof(input));
			memset(sendinput, 0, sizeof(sendinput));

		}
	}
	exit(1);

}

void multirain_set(){
	// 225.1.1.2 9190 1
	int recv_sock;
	int pid;
	unsigned int yes=1, ttl=32;
	struct ip_mreq mreq;
	char name[10];
	int n, len;


	memset((char *)&mcast_group, 0, sizeof(mcast_group));
	mcast_group.sin_family = AF_INET;
	mcast_group.sin_port = htons(atoi(portnum));
	mcast_group.sin_addr.s_addr = inet_addr(GID);
	if ((recv_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("error: Can't create receive socket\n");
		exit(0);
	}

	mreq.imr_multiaddr = mcast_group.sin_addr;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		printf("error: add membership\n");
		exit(0);
	}

	if (setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		printf("error: SO_REUSEADDR setsockopt\n");
		exit(0);
	}

	if (setsockopt(recv_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
		printf("error: IP_MULTICAST_TTL setsockopt\n");
		exit(0);
	}

	if (bind(recv_sock, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0) {
		printf("error: bind receive socket\n");
		exit(0);
	}

	if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("error: Can't create send socket\n");
		exit(0);
	}


	multirain(recv_sock, send_sock, mcast_group, nowplayer);

	close(recv_sock);
	close(send_sock);
	return;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void single_play(){
	char input;
	int pid;
	int wait_rv;

	echo();
	clear();
	boxing();
	refresh();
	move(15,70);
	addstr("HERE IS SINGLE PLAY ZONE!");
	move(30,60);
	addstr("1. PRACTICE SHORT SENTENCE");
	move(30,100);
	addstr("2. ACID - RAIN");
	move(45, 120);
	addstr("PRESS B KEY TO GO BACK");
	refresh();

	input= tolower(get_ok_char());
	if(input=='1'){
		if((pid = fork()) == -1){
			perror("Cannot fork");
			exit(1);
		}
		if(pid==0){
			short_game();
		}
		wait_rv = wait(NULL);
		single_play();
	}
	else if(input=='2'){
		if((pid = fork()) == -1){
			perror("Cannot fork");
			exit(1);
		}
		if(pid == 0){
			rain();
		}
		wait_rv = wait(NULL);
		single_play();
	}
	else show_title();
}

void multi_play(){
	char input;
	int pid;
	int wait_rv;

	clear();
	boxing();
	refresh();
	move(15,70);
	addstr("HERE IS MULTI PLAY ZONE!");

	move(30,60);
	addstr("1. CARD FLIP");
	move(30,100);
	addstr("2. ACID - RAIN");
	move(45, 120);
	addstr("PRESS B KEY TO GO BACK");
	refresh();

	input = tolower(get_ok_char());
	if(input=='1'){ 
		if((pid = fork()) == -1){
			perror("Cannot fork");
			exit(1);
		}
		if(pid == 0){

			multicard_set();
		}
		wait_rv = wait(NULL);
		multi_play();
	}
	else if(input=='2'){
		if((pid = fork()) == -1){
			perror("Cannot fork");
			exit(1);
		}
		if(pid == 0){
			multirain_set();
		}
		wait_rv = wait(NULL);
		multi_play();
	}
	else  show_title();
}

void multicard_set(){
	int send_sock, recv_sock;
	int pid;
	unsigned int yes=1, ttl=32;
	struct sockaddr_in mcast_group; 
	struct ip_mreq mreq;
	char name[10];
	int n, len;

	memset((char *)&mcast_group, 0, sizeof(mcast_group));
	mcast_group.sin_family = AF_INET;
	mcast_group.sin_port = htons(atoi(portnum));
	mcast_group.sin_addr.s_addr = inet_addr(GID);
	if ((recv_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("error: Can't create receive socket\n");
		exit(0);
	}

	mreq.imr_multiaddr = mcast_group.sin_addr;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		printf("error: add membership\n");
		exit(0);
	}

	if (setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		printf("error: SO_REUSEADDR setsockopt\n");
		exit(0);
	}

	if (setsockopt(recv_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
		printf("error: IP_MULTICAST_TTL setsockopt\n");
		exit(0);
	}

	if (bind(recv_sock, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0) {
		printf("error: bind receive socket\n");
		exit(0);
	}

	if ((send_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("error: Can't create send socket\n");
		exit(0);
	}


	multicard(recv_sock, send_sock, mcast_group, nowplayer);

	close(recv_sock);
	close(send_sock);
}

void card_boxing(){

	for(int i=5; i<=49; i++){
		move(i,10);
		attron(A_STANDOUT);
		addstr("|");
		move(i, 170);
		addstr("|");
	}

	for(int i=10; i<=170; i++){
		move(5,i);
		addstr("-");
		move(50,i);
		addstr("-");
		move(35,i);
		addstr("-");
	}
	attroff(A_STANDOUT);
}

void multicard(int recv_sock, int send_sock, struct sockaddr_in mcast_group, char player[2]) {
	FILE *fd1;
	int i=0;
	int r=15, c=50;
	int cnt=0;
	char input[20];
	int j=1;
	char temp[20];
	struct itimerval timer;
	pthread_t send_t, recv_t;
	void *sendfunc(void* arg);
	void *recvfunc(void* arg);
	struct sockaddr_in from;
	struct sendarg arg1;
	struct recvarg arg2;

	arg1.send_sock = send_sock;
	arg1.mcast_group = mcast_group;
	arg2.recv_sock = recv_sock;
	arg2.from = from;

	strcpy(nowplayer,player);

	clear();
	refresh();
	curs_set(0);

	initscr();
	noecho();
	crmode();
	clear();

	move(LINES/2,COLS/2-10);
	printw("Type Ready!");
	refresh();

	pthread_create(&send_t, NULL, sendfunc, (void*)&arg1);
	pthread_create(&recv_t, NULL, recvfunc, (void*)&arg2);

	pthread_join(recv_t, NULL);

	move(25,90);
	addstr("3");
	refresh();
	sleep(1);

	clear();
	move(25,90);
	addstr("2");
	refresh();
	sleep(1);

	clear();
	move(25,90);
	addstr("1");
	refresh();
	sleep(1);

	clear();
	move(25,90);
	addstr("START!");
	refresh();
	sleep(1);

	clear();

	signal(SIGALRM, card);//종료호출
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 1;//여기고침!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	timer.it_interval.tv_sec = 1;
	timer.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer, NULL);

	fd1 = fopen("file1.txt","r");
	start_color();
	init_pair(0,COLOR_WHITE,COLOR_BLACK);//white
	init_pair(1,COLOR_BLUE,COLOR_BLACK);//blue

	for(i=1; i<=40; i++){ ///////////////////////////////card print
		fgets(array[i].word, 20, fd1);
		if(i%2==0 || i==0){
			array[i].color=0;
			attron(COLOR_PAIR(0));
			move(r,c);
			array[i].r=r;
			array[i].c=c;
			c=c+20;
			addstr(array[i].word);
			attroff(COLOR_PAIR(0));
			refresh();
			cnt++;
			w_cnt++;
		}
		else{
			array[i].color=1;
			attron(COLOR_PAIR(1));
			move(r,c);
			array[i].r=r;
			array[i].c=c;
			c=c+20;
			addstr(array[i].word);
			attroff(COLOR_PAIR(1));
			refresh();
			cnt++;
			b_cnt++;
		}
		if(cnt==5){
			c=50;
			r++;
			cnt=0;
		}
	}

	card_boxing();	
	mvprintw(40,20,"WHITE SHARE: %d",w_cnt);
	mvprintw(40,140,"BLUE SHARE: %d",b_cnt);
	refresh();

	pthread_create(&recv_t, NULL, recvfunc, (void*)&arg2);

	pthread_join(send_t, NULL);
	pthread_join(recv_t, NULL);

}

void card(int signum){
	cardcnt++;
	move(10,90);
	addstr("	");
	move(10,90);
	printw("%d",30-cardcnt);
	refresh();
	if(cardcnt == 30){
		stopitimer();
		clear();
		move(10,75);
		addstr("GAME END! PRESS ANY KEY YOU WANT TO BACK!");
		refresh();

		if(strcmp(nowplayer,"1")==0){ /////////////////1이 화이트
			mvprintw(20,75,"YOU FLIPED %d!!!",w_cnt);
			if(w_cnt>b_cnt ){ //1 win
				mvprintw(30,75,"YOU WIN!!!!!");
			}
			else if(w_cnt==b_cnt){ //draw
				mvprintw(30,75,"DRAW~~~");
			}
			else{//2 win
				mvprintw(30,75,"YOU LOSE....");
			}
		}
		else if(strcmp(nowplayer,"2")==0){///////////////2 is blue
			mvprintw(20,75,"YOU FILPED %d!!!",b_cnt);
			if(b_cnt>w_cnt){//2 win
				mvprintw(30,75,"YOU WIN!!!");
			}
			else if(b_cnt == w_cnt){//draw
				mvprintw(30,75,"DRAW~~~");
			}
			else{//1 win
				mvprintw(30,75,"YOU LOSE....");
			}
		}
		refresh();
		char c;

		while(1){
			c=getch();
			if(c!=0){
				clear();
				refresh();
				exit(1);
			}
		}
	}
}


void *sendfunc(void* arg) {
	struct sendarg* arg1 = (struct sendarg*)arg;
	int send_sock = arg1->send_sock;
	struct sockaddr_in mcast_group = arg1->mcast_group;
	char input[20];
	int inputlen = 0;

	while(1) {
		//입력받아서 전송
		strcpy(input,"aa");
		scanw("%s", input);
		inputlen = strlen(input);
		if(strcmp(input,"READY")==0){
			strcat(input,nowplayer);
			input[inputlen+1]='\0';
			inputlen++;
		}

		sendto(send_sock, input, inputlen, 0, (struct sockaddr*)&mcast_group, sizeof(mcast_group));
	}
}

void *recvfunc(void* arg) {
	struct recvarg* arg2 = (struct recvarg*)arg;
	int recv_sock = arg2->recv_sock;
	struct sockaddr_in from = arg2->from;
	int len = 0;
	char w[20];
	char neww[20];
	//	int flaga=0; int flagb=0;

	while(1) {
		len = sizeof(from);
		recvfrom(recv_sock, w, 20, 0, (struct sockaddr*)&from, &len);
		w[strlen(w)] = '\0';
		strcpy(neww, w);
		strcat(neww, "\n");
		flagb=0;

		if(galbae == 0){
			if(strcmp(w,"READY1") == 0){
				ready1 = 1;		
			}
			else if(strcmp(w,"READY2") == 0){
				ready2 = 1;
			}

			if(strcmp(w,"READY1")==0 && strcmp(nowplayer,"1") == 0){
				clear();
				move(LINES/2,COLS/2-20);
				printw("You're READY PLAYER 1!!!!!!!!!!!!!!!!!!!");
				mvprintw(LINES/2+10,COLS/2-20,"You're white team. KILL blue!!");
				refresh();

			}else if(strcmp(w,"READY2") == 0 && strcmp(nowplayer, "2") == 0){
				clear();
				move(LINES/2,COLS/2-20);
				printw("You're READY PLAYER 2!!!!!!!!!!!!!!!!!!!");
				mvprintw(LINES/2+10,COLS/2-20,"You're blue team. KILL white!!");
				refresh();
			}
			if(ready1 == 1 && ready2 == 1){

				sleep(2);//////////////////////////////////////sleep
				refresh();
				clear();
				fflush(stdin);
				sleep(1);
				flaga=1;flagb=1;
				galbae=1;
				refresh();
				break;
			}
		}

		if(flaga==1 && flagb==0 && galbae==1){	
			if(ready1 == 1 && ready2 == 1){
				for(int j=1; j<=40; j++){
					if((strcmp(neww, array[j].word)==0)) {
						move(array[j].r,array[j].c);

						if(array[j].color==0){
							array[j].color=1;
							attron(COLOR_PAIR(1));
							addstr(w);
							attroff(COLOR_PAIR(1));
							b_cnt++;
							w_cnt--;
							refresh();
						}

						else if(array[j].color==1){
							array[j].color=0;
							attron(COLOR_PAIR(0));
							addstr(w);
							attroff(COLOR_PAIR(0));
							b_cnt--;
							w_cnt++;
							refresh();
						}

						mvprintw(40,20,"			");
						mvprintw(40,20,"WHITE SHARE: %d",w_cnt);
						mvprintw(40,140,"			");
						mvprintw(40,140,"BLUE SHARE: %d",b_cnt);
						refresh();
					}
				}
				memset(&w, 0, sizeof(w));
				memset(&neww, 0, sizeof(neww));
			}
		}
	}
}

int get_ok_char(){
	int c;
	while((c=getchar())!=EOF&&strchr("12bB",c)==NULL);
	return c;
}

int main(int argc, char* argv[]){
	signal(SIGQUIT, SIG_IGN);

	strcpy(GID, argv[1]);
	strcpy(portnum, argv[2]);
	strcpy(nowplayer, argv[3]);
	show_title();
	return 0;
}
