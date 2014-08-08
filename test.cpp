#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

// g++ test.cpp -o test -lncurses

using namespace std;

bool quit = false;
size_t hl_line_idx = 0, max_lines = 0;
struct dirent **namelist = 0;

void init_ncurses();
void set_quit_handler();
void print_cur_dir();
void update_hl_line_pos();
void quit_handler(int sig) { quit = true; }
void stop(int errnum);
void free_name_list();

int main()
{
    set_quit_handler();
    init_ncurses();
    while(not quit)
    {
        erase();
        print_cur_dir();
        update_hl_line_pos();
        refresh();
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
}

void print_cur_dir()
{
    char *cwd = get_current_dir_name();
    printw("%s\n", cwd);
    max_lines = scandir(cwd, &namelist, NULL /*filter*/, alphasort);
    if(max_lines == -1)
        stop(errno);
    for(size_t i = 0; i < max_lines; ++i)
    {
        if(i == hl_line_idx)
        {
            attron(COLOR_PAIR(1));
            printw("%s\n", namelist[i]->d_name);
            attroff(COLOR_PAIR(1));
        }
        else
        {
            printw("%s\n", namelist[i]->d_name);
        }
    }
    free(cwd);
    free_name_list();
}

void update_hl_line_pos()
{
    int ch = getch();
    if(ch == KEY_UP)
    {
        if(hl_line_idx > 0)
            --hl_line_idx;
    }
    else if(ch == KEY_DOWN)
    {
        if(hl_line_idx < max_lines-1)
            ++hl_line_idx;
    }
}

void stop(int errnum)
{
    endwin();
    printf("%s\n", strerror(errnum));
    exit(EXIT_FAILURE);
}

void free_name_list()
{
    for(size_t i = 0; i < max_lines; ++i)
        free(namelist[i]);
    free(namelist);
}
