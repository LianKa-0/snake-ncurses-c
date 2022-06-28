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
   cord * yx;
   struct queue * previous;
   struct queue * next;
} queue;

typedef struct{
    int len;
    struct queue * firstAdded;
    struct queue * lastAdded;
}snake;

snake * new() {
    snake * new = malloc( sizeof ( snake ) );
    new->firstAdded = NULL;
    new->lastAdded = NULL;
    new->len = 0;
    return new;
}

int isEmpty(snake * snake) {
    return !snake->firstAdded || !snake->lastAdded;
}
void putBack(snake * snake, cord * cord) {
    snake->len++;
    queue * new = malloc( sizeof (queue) );

    new->yx = cord;
    new->next = NULL;
    new->previous = snake->lastAdded;
    
    if (isEmpty(snake)) {
	snake->lastAdded = snake->firstAdded = new;
	return;
    }

    snake->lastAdded->next = new; 
    snake->lastAdded = new;
}
    
void putFront(snake * snake, cord * cord) {
    snake->len++;
    queue * new = malloc( sizeof (queue) );

    new->yx = cord;
    new->next = snake->firstAdded;
    new->previous = NULL;
    
    if (isEmpty(snake)) {
	snake->lastAdded = snake->firstAdded = new;
	return;
    }

    snake->firstAdded->previous = new; 
    snake->firstAdded = new;
}

cord* get(snake * snake) {
    snake->len--;
    if (isEmpty(snake)) return NULL;

    cord * tmp;
    queue * node = snake->firstAdded;
    snake->firstAdded = node->next;
    if (!snake->firstAdded) 
	snake->lastAdded = NULL;
    else
	snake->firstAdded->previous = NULL;
    tmp = node->yx;
    free(node);
    return tmp;
}
void delete(snake * snake) {
    while (snake->len > 0) {
	get(snake);
    }
	free(snake);
}
//---------------------------------------------------------------------------------------------------------
#define fruitc '@'
#define snakec 'o'
#define headc '$' 
#define sleepmic 100000

typedef enum  {
    UP = KEY_UP,
    DOWN = KEY_DOWN,
    RIGHT = KEY_RIGHT,
    LEFT = KEY_LEFT,
    NONE
}Dir;

WINDOW* initwin(int height, int width); 
void startgame(int height, int width);
void draw(WINDOW * win, int height, int width, cord * fruit, snake * snake,  int score);
void generatefruit(int height, int width, cord * fruit);
void appendSnake(int height, int width, Dir dir, snake * snake);
cord* nextCord(int height, int width, Dir dir, snake * snake);
int getnextdir(Dir previous, WINDOW * win);
int moveSnake(int height, int width, Dir direction, snake * snake);
int collision(cord * fruit, snake * snake);
int validmove(snake * snake);


int main(int argc, char * argv[]) {

    int height, width;
    height = atoi(argv[1]);
    width = atoi(argv[2]);
    
    if (height < 15 || width < 15)
	exit(EXIT_FAILURE);
   startgame(height, width);
}
void startgame(int height, int width) {
    Dir dir;
    cord fruit;
    int score, gameover;
    snake * snake;

    WINDOW * win = initwin(height, width); 
    cord * initial = malloc(sizeof(cord));
    initial->y = height/2; 
    initial->x = width/2; 

    generatefruit(height, width, &fruit);
    snake = new();
    putBack(snake, initial);
    dir = UP;
    gameover = 0;
    score = 0;
    
    while (!gameover) {

	dir = getnextdir(dir, win);

	    moveSnake(height, width, dir, snake);
	    if (collision(&fruit, snake)) {
		generatefruit(height, width,  &fruit);
		appendSnake(height, width, dir, snake);
		++score;
	    }

	draw(win, height, width, &fruit, snake,  score);
	gameover = !validmove(snake);	
	refresh();
    }
    
    delwin(win);
    endwin();
    delete(snake);
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
    //printw("%d ", c);
    if (c == KEY_EXIT) exit(0);
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
	fruit->y = rand()%(height-2) + 1;
	fruit->x = rand()%(width-2) + 1;
}

void draw(WINDOW* win, int height, int width, cord * fruit, snake * snake,int score) {
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "|Score: %d|", score);

    queue *curr = snake->firstAdded;
    while (curr != snake->lastAdded) {
	mvwprintw(win, curr->yx->y, curr->yx->x, "%c", snakec);
	curr = curr->next;
    }
    mvwprintw(win, snake->lastAdded->yx->y, snake->lastAdded->yx->x, "%c", headc); 
    mvwprintw(win, fruit->y, fruit->x, "%c", fruitc);
    refresh();
    wrefresh(win);
    usleep(sleepmic);
}

int moveSnake(int height, int width, Dir direction, snake * snake){
    putBack(snake, nextCord(height, width,direction, snake));
    free(get(snake));
    return 0;
}

void appendSnake(int height, int width, Dir dir,snake * snake) {
   putBack(snake,  nextCord(height, width,dir, snake)); 
}

cord * nextCord(int height, int width, Dir dir, snake * snake) {
   int ay, ax; 
   ay = ax = 0;
   switch (dir) {
	case UP: ay = -1;break;
	case DOWN: ay = 1;break;
	case RIGHT: ax = 1; break;
	case LEFT: ax = -1;
   }
    
    cord * ncord = malloc(sizeof(cord));
    ncord->y =  snake->lastAdded->yx->y + ay;
    ncord->x =  snake->lastAdded->yx->x + ax;

    if (ncord->y < 1) ncord->y = height - 2;
    else if (ncord->y > height - 2) ncord->y = height = 1;
    else if (ncord->x > width - 2) ncord->x = 1;
    else if (ncord->x < 1) ncord->x = width - 2;

    return ncord; 
}

int collision(cord * fruit,snake * snake) {
    return snake->lastAdded->yx->y == fruit->y && snake->lastAdded->yx->x == fruit->x;
}

int validmove(snake * snake) {
    queue *curr = snake->firstAdded;
    while (curr != snake->lastAdded) {
	if (curr->yx->y == snake->lastAdded->yx->y && curr->yx->x == snake->lastAdded->yx->x) return 0;
	curr = curr->next;
    }
    return 1;
}
