#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <dirent.h>

using namespace std;

bool quit = false;
void quit_handler(int sig) { quit = true; }
void endwin_funcall() { endwin(); }

void init_ncurses()
{
    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    halfdelay(1);
    echo();
    if(has_colors())
    {
        start_color();
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }
    atexit(endwin_funcall);
}

void set_quit_handler()
{
    struct sigaction action;
    action.sa_handler = quit_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, NULL/*old action*/) == -1)
    {
        perror("sigaction:");
        exit(EXIT_FAILURE);
    }
}

void print_cur_dir()
{
    struct dirent **namelist;
    int n = scandir("./", &namelist, NULL /*filter*/, alphasort);
    if(n == -1)
    {
        perror("scandir:");
        exit(EXIT_FAILURE);
    }
    while(n--)
    {
        assert(printw("%s\n", namelist[n]->d_name) == OK);
        free(namelist[n]);
    }
    free(namelist);
}

int main()
{
    int i = 0;
    set_quit_handler();
    init_ncurses();
    while(not quit)
    {
        erase();
        printw("%d\n", i++);
        print_cur_dir();
        int ch = getch();
        if(ch == ERR)
        {
            printw("NO INPUT\n");
        }
        else if(ch == KEY_LEFT)
        {
            printw("LEFT\n");
        }
        refresh();
    }
}

// g++ test.cpp -o test -lncurses
