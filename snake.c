#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>

typedef struct{
    int y;
    int x;
} cord;

typedef struct queue  {
   cord yx;
   struct queue * previous;
   struct queue * next;
} queue;
queue *lastAdded, *firstAdded;

void put(cord cord) {
    queue var;
    var.yx = cord;

    var.previous = lastAdded;
    var.next = NULL;
    lastAdded->next = &var;
    
   lastAdded = &var; 
}

queue* get() {
    queue * temp = firstAdded;
    firstAdded->next->previous = NULL;
    firstAdded = firstAdded->next;
    return firstAdded;
}
//---------------------------------------------------------------------------------------------------------
#define fruitc '@'
#define snakec '#'
#define headc 'h'

typedef enum  {
    UP = KEY_UP,
    DOWN = KEY_DOWN,
    RIGHT = KEY_RIGHT,
    LEFT = KEY_LEFT,
    NONE
}Dir;

WINDOW* initwin(int height, int width); 
void startgame(int height, int width);
void draw(WINDOW * win, int height, int width, cord * fruit, int score);
void generatefruit(int height, int width, cord * fruit);
void appendSnake(int height, int width, Dir dir);
cord nextCord(int height, int width, Dir dir);
int getnextdir(Dir previous, WINDOW * win);
int moveSnake(int height, int width, Dir direction);
int collision(cord * fruit);
int validmove();


int main(void) {
    int height = 10, width = 40;
    startgame(height, width);
    /*queue tmp = {{10/2, 10/2}, NULL, NULL};
    firstAdded = lastAdded = &tmp;


    printf("y: %d, x: %d\n",lastAdded->yx.y, lastAdded->yx.x );
    moveSnake(10,10, UP);
    printf("y: %d, x: %d\n",lastAdded->yx.y, lastAdded->yx.x );
    */
}
void startgame(int height, int width) {
    Dir dir;
    cord fruit;
    int score, gameover;

    queue tmp = {{height/2, width/2}, NULL, NULL};
    firstAdded = lastAdded = &tmp;
    WINDOW * win = initwin(height, width); 
    
    generatefruit(height, width, &fruit);
    dir = UP;
    gameover = 0;
    score = 1;

    while (!gameover) {
	//dir = getnextdir(dir, win);
	
        moveSnake(height, width, dir);
	if (collision(&fruit)) {
	    generatefruit(height, width,  &fruit);
	    appendSnake(height, width, dir);
	    ++score;
	}
	//gameover = !validmove();	
	draw(win, height, width, &fruit, score);
	refresh();
    }
    
    delwin(win);
    endwin();
}

WINDOW* initwin(int height, int width) {
    initscr();
    int start_y = LINES / 2 - height / 2;
    int start_x = COLS / 2 - width / 2;
    
    WINDOW* win = newwin(height,width, start_y, start_x);
    noecho();
    keypad(stdscr, true);
    keypad(win, true);
    cbreak();
    timeout(10);
    wtimeout(win, 10);
    curs_set(0);
    nodelay(stdscr , true);
    nodelay(win, true);
    refresh();
    wrefresh(win);
    
    return win;
}

int getnextdir(Dir previous , WINDOW * win) {
    int c = getch(); 
    printw("%d ", c);
    switch (c) {
	case LEFT:
	    if (previous != RIGHT) return LEFT;
	    break;
	case RIGHT:
	    if (previous != LEFT) return RIGHT;
	    break;
	case UP:
	    if (previous != DOWN) return UP;
	    break;
	case DOWN:
	    if (previous != UP) return DOWN;
	    break;

    }
    return previous;
}

void generatefruit(int height, int width, cord * fruit) {
	fruit->y = rand()%height;
	fruit->x = rand()%width;
}

void draw(WINDOW* win, int height, int width, cord * fruit, int score) {
    clear();
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "|Score: %d|", score);
    int counter = 0;
    queue *curr = firstAdded;
    while (curr != lastAdded) {
	mvwprintw(win, curr->yx.y, curr->yx.x, "%c", snakec);
	curr = curr->next;
    }
    
    printw("y: %d, x: %d\n",lastAdded->yx.y, lastAdded->yx.x );
    mvwprintw(win, lastAdded->yx.y, lastAdded->yx.x, "%c", headc); 
    mvwprintw(win, fruit->y, fruit->x, "%c", fruitc);
    refresh();
    wrefresh(win);
    usleep(50000);
}

int moveSnake(int height, int width, Dir direction){
    put(nextCord(height, width, direction));
    get();
    return 0;
}

void appendSnake(int height, int width, Dir dir) {
   put(nextCord(height, width, dir)); 
}

cord nextCord(int height, int width, Dir dir) {
   int ay, ax; 
   ay = ax = 0;
   switch (dir) {
	case UP: ay = -1;
	    if (lastAdded->yx.y + ay <= 0) ay = height - 1;
	break;
	case DOWN: ay = 1;
	    if (lastAdded->yx.y + ay >= height) ay = 1;
	break;
	case RIGHT: ax = 1; 
	    if (lastAdded->yx.x + ax>= width) ay = 1;
	break;
	case LEFT: ax = -1;
	    if (lastAdded->yx.x + ax <= 0) ax = width - 1;
   }
  
    cord ncord = {
	lastAdded->yx.y + ay, 
	lastAdded->yx.x + ax
    };
    
    return ncord; 
}

int collision(cord * fruit) {
    return lastAdded->yx.y == fruit->y && lastAdded->yx.x == fruit->x;
}

int validmove() {
    queue *curr = firstAdded;
    while (curr != lastAdded) {
	if (curr->yx.y == lastAdded->yx.y && curr->yx.x == lastAdded->yx.x) return 0;
	curr = curr->next;
    }
    return 1;
}
