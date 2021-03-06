#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

// g++ -O1 -ggdb test.cpp -o test -lncurses

using namespace std;

bool quit = false;
size_t hl_line_idx = 0; // TODO Item list may change shortly in time
			// (for instance emacs introduces backup files
			// beginning and finishing with # char). As a
			// result the highlighted lines doe not move
			// while it must highlight the same entry
			// name. We must not use an index here, but a
			// pointer to an entry.
size_t max_lines = 0;
struct dirent **namelist = 0;

int keys[100] = {0};
size_t keys_sz = 0;

char debug[200] = "";

void init_ncurses();
void set_quit_handler();
void update_cur_dir_info();
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
	update_cur_dir_info();
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
        init_pair(2, COLOR_RED, COLOR_BLACK);
    }
}

void update_cur_dir_info()
{
    free_name_list();
    char *cwd = get_current_dir_name();
    printw("%s\n", cwd);
    max_lines = scandir(cwd, &namelist, NULL /*filter*/, alphasort);
    if(max_lines == -1)
        stop(errno);
    free(cwd);
}

void print_cur_dir()
{
    // TODO Understand why when there are more items than the number
    // of line on the screen, then the last line collapse all the last
    // items.
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
    // for(size_t i = 0; i < keys_sz; ++i)
    // 	printw("[%02x]", keys[i]);
    // printw("Move cwd to %s.\n", path);

    // TODO it would be great to print that in front of the entry
    // instead of the end of the list.
    attron(A_BOLD | COLOR_PAIR(2));
    printw("%s\n", debug);
    attroff(A_BOLD | COLOR_PAIR(2));
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
    else if(ch == '\r')
    {
	if(max_lines > 0)
	{
	    assert(namelist[hl_line_idx]->d_name);
	    char path[PATH_MAX] = "";
	    strcat(path, "./");
	    strcat(path, namelist[hl_line_idx]->d_name);
	    if(chdir(path) == -1)
	    {
		if(errno == ENOENT)
		    strcpy(debug, "Not an entry.");
		else if(errno == ENOTDIR)
		    strcpy(debug, "Not a dir.");
	    }
	    update_cur_dir_info();
	    hl_line_idx = 0;
	}
    }
    else if(ch != ERR)
    {
	keys[keys_sz++] = ch;
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
