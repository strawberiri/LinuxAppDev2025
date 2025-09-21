#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#define DX 7
#define DY 3

void show_lines(int start_line, int end_line, char **lines, int win_width, WINDOW *win)
{
    for (int i = start_line; i < end_line; i++)
    {
        // cut lines that are too long
        if (strlen(lines[i]) > win_width)
        {
            lines[i][win_width] = '\0';
        }

        wprintw(win, "%s", lines[i]);
    }
    wrefresh(win);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "No file passed as argument");
        return 1;
    }

    char *filename = argv[1];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open file: ");
    }

    size_t max_lines = 100, line_ctr = 0;
    char **lines = malloc(sizeof(char *) * max_lines);
    char buf[1024];

    while (fgets(buf, sizeof(buf), fp))
    {
        char *line = malloc(strlen(buf) + 1);
        strcpy(line, buf);

        if (line_ctr >= max_lines)
        {
            max_lines *= 2;
            char **bigger_lines = realloc(lines, sizeof(char *) * max_lines);
            lines = bigger_lines;
        }

        lines[line_ctr++] = line;
    }

    fclose(fp);

    initscr();
    noecho();
    setlocale(LC_ALL, "");
    curs_set(0);

    WINDOW *frame, *win;

    // frame with the filename
    frame = newwin(LINES - 2 * DY, COLS - 2 * DX, DY, DX);
    box(frame, 0, 0);
    mvwaddstr(frame, 0, (int)((COLS - 2 * DX - strlen(filename)) / 2), filename);
    wrefresh(frame);

    int win_height = LINES - 2 * DY - 2;
    int win_width = COLS - 2 * DX - 2;
    win = newwin(win_height, win_width, DY + 1, DX + 1);
    keypad(win, TRUE);
    scrollok(win, FALSE);

    int start_line = 0;
    int end_line = line_ctr > win_height ? win_height : line_ctr;
    show_lines(start_line, end_line, lines, win_width, win);

    int c;
    while ((c = wgetch(win)) != 27)
    {
        if (c == ' ' && start_line < line_ctr - win_height)
        {
            werase(win);
            show_lines(++start_line, ++end_line, lines, win_width, win);
        }
    }

    delwin(win);
    delwin(frame);
    endwin();

    return 0;
}
