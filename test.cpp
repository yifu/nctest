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

size_t hl_line_idx = 3, max_lines;

void init_ncurses()
{
    initscr();
    keypad(stdscr, TRUE);
    nonl();
    cbreak();
    halfdelay(1);
    noecho();
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
    max_lines = scandir("./", &namelist, NULL /*filter*/, alphasort);
    if(max_lines == -1)
    {
        perror("scandir:");
        exit(EXIT_FAILURE);
    }
    for(size_t i = 0; i < max_lines; ++i)
    {
        if(i == hl_line_idx)
        {
            attron(A_UNDERLINE | COLOR_PAIR(4));
            printw("%s\n", namelist[i]->d_name);
            attroff(A_UNDERLINE | COLOR_PAIR(4));
        }
        else
        {
            printw("%s\n", namelist[i]->d_name);
        }
        free(namelist[i]);
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
        else if(ch == KEY_UP)
        {
            if(hl_line_idx > 0)
                --hl_line_idx;
        }
        else if(ch == KEY_DOWN)
        {
            if(hl_line_idx < max_lines-1)
                ++hl_line_idx;
        }
        refresh();
    }
}

// g++ test.cpp -o test -lncurses
