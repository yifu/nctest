#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

// g++ -O1 -ggdb test2.cpp -o test2 -lncurses

using namespace std;

bool quit = false;
size_t hl_line_idx = 0, max_lines = 0;

void init_ncurses();
void set_quit_handler();
void print_cur_dir();
void update_hl_line_pos();
void quit_handler(int sig) { quit = true; }
void stop(int errnum);

WINDOW *win1, *win2;

int main()
{
    set_quit_handler();
    init_ncurses();
    int i = 0, j = 0;
    while(not quit)
    {
        // erase();
        // printw("toto titi %d\n", ++i);
        // refresh();
        werase(win1);
        werase(win2);
        box(win1, 0, 0);
        box(win2, 0, 0);
        wprintw(win1, "tata %d\n", ++i);
        wprintw(win2, "toto %d\n", ++j);
        wrefresh(win1);
        wrefresh(win2);
    }
    endwin();
    exit(EXIT_SUCCESS);
}

void set_quit_handler()
{
    struct sigaction action;
    action.sa_handler = quit_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, NULL/*old action*/) == -1)
        stop(errno);
}

void init_ncurses()
{
    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    halfdelay(10/*100ms*/);
    noecho();
    if(has_colors())
    {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
    }
    win1 = newwin(5,20,0,0);
    win2 = newwin(5,20,6,0);
}

void stop(int errnum)
{
    endwin();
    printf("%s\n", strerror(errnum));
    exit(EXIT_FAILURE);
}
