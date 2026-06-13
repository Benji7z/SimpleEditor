#include <stdio.h>

#define TB_IMPL
#include "termbox2.h"

#define MAX_CHARS 65536

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return 1;
    }

    const char *filename = argv[1];

    char content[MAX_CHARS] = {0};
    int len = 0;

    //cursor position as an index into content[]
    int cursor = 0;

    // scroll offset: which screen-row is at the top of the viewport
    int scroll_offset = 0;

    // Load existing file content, if it exists
    FILE *f = fopen(filename, "r");
    if (f != NULL)
    {
        int c;
        while ((c = fgetc(f)) != EOF && len < MAX_CHARS - 1)
        {
            content[len] = (char)c;
            len++;
        }
        fclose(f);
    }

    cursor = len; // start at end of file

    tb_init();

    struct tb_event event;

    while (1)
    {
        tb_clear();

        // first pass: figure out where the cursor lands (in full, unscrolled coordinates)
        int x = 0, y = 0;
        int cursorx = 0, cursory = 0;

        for (int i = 0; i < len; i++)
        {
            if (i == cursor)
            {
                cursorx = x;
                cursory = y;
            }

            if (content[i] == '\n')
            {
                x = 0;
                y++;
            }
            else
            {
                x++;
                if (x >= tb_width())
                {
                    x = 0;
                    y++;
                }
            }
        }

        if (cursor == len)
        {
            cursorx = x;
            cursory = y;
        }

        // adjust scroll_offset so the cursor stays within the viewport
        if (cursory < scroll_offset)
        {
            scroll_offset = cursory;
        }
        if (cursory >= scroll_offset + tb_height())
        {
            scroll_offset = cursory - tb_height() + 1;
        }
        if (scroll_offset < 0)
        {
            scroll_offset = 0;
        }

        // second pass: redraw, shifting everything up by scroll_offset
        x = 0;
        y = 0;

        for (int i = 0; i < len; i++)
        {
            if (content[i] == '\n')
            {
                x = 0;
                y++;
            }
            else
            {
                int sy = y - scroll_offset;
                if (sy >= 0 && sy < tb_height())
                {
                    tb_printf(x, sy, TB_WHITE, 0, "%c", content[i]);
                }
                x++;
                if (x >= tb_width())
                {
                    x = 0;
                    y++;
                }
            }
        }

        // draw cursor block (with whatever character is under it)
        char under = (cursor < len) ? content[cursor] : ' ';
        if (under == '\n') under = ' ';

        int screen_cursory = cursory - scroll_offset;
        if (screen_cursory >= 0 && screen_cursory < tb_height())
        {
            tb_set_cell(cursorx, screen_cursory, under, TB_BLACK, TB_WHITE);
        }

        tb_present();

        tb_poll_event(&event);

        if (event.type == TB_EVENT_KEY)
        {
            if (event.key == TB_KEY_ENTER)
            {
                for (int i = len; i > cursor; i--)
                    content[i] = content[i - 1];
                content[cursor] = '\n';
                len++;
                cursor++;
            }

            else if (event.key == TB_KEY_BACKSPACE || event.key == TB_KEY_BACKSPACE2)
            {
                if (cursor > 0)
                {
                    for (int i = cursor - 1; i < len - 1; i++)
                        content[i] = content[i + 1];
                    len--;
                    cursor--;
                }
            }

            else if (event.key == TB_KEY_DELETE)
            {
                if (cursor < len)
                {
                    for (int i = cursor; i < len - 1; i++)
                        content[i] = content[i + 1];
                    len--;
                }
            }

            else if (event.key == TB_KEY_TAB)
            {
                for (int t = 0; t < 4; t++)
                {
                    if (len >= MAX_CHARS - 1) break;
                    for (int i = len; i > cursor; i--)
                        content[i] = content[i - 1];
                    content[cursor] = ' ';
                    len++;
                    cursor++;
                }
            }

            else if (event.key == TB_KEY_ARROW_LEFT)
            {
                if (cursor > 0) cursor--;
            }

            else if (event.key == TB_KEY_ARROW_RIGHT)
            {
                if (cursor < len) cursor++;
            }

            else if (event.key == TB_KEY_ARROW_UP)
            {
                int line_start = cursor;
                while (line_start > 0 && content[line_start - 1] != '\n')
                    line_start--;

                if (line_start > 0)
                {
                    int col = cursor - line_start;

                    int prev_line_end = line_start - 1;
                    int prev_line_start = prev_line_end;
                    while (prev_line_start > 0 && content[prev_line_start - 1] != '\n')
                        prev_line_start--;

                    int prev_line_len = prev_line_end - prev_line_start;

                    if (col > prev_line_len) col = prev_line_len;

                    cursor = prev_line_start + col;
                }
            }

            else if (event.key == TB_KEY_ARROW_DOWN)
            {
                int line_start = cursor;
                while (line_start > 0 && content[line_start - 1] != '\n')
                    line_start--;

                int col = cursor - line_start;

                int line_end = cursor;
                while (line_end < len && content[line_end] != '\n')
                    line_end++;

                if (line_end < len)
                {
                    int next_line_start = line_end + 1;
                    int next_line_end = next_line_start;
                    while (next_line_end < len && content[next_line_end] != '\n')
                        next_line_end++;

                    int next_line_len = next_line_end - next_line_start;

                    if (col > next_line_len) col = next_line_len;

                    cursor = next_line_start + col;
                }
            }

            else if (event.key == TB_KEY_ESC)
            {
                tb_clear();
                for (int i = 0; i <= tb_width(); i++)
                {
                    tb_printf(i, tb_height() - 1, 0, TB_WHITE, " ");
                }
                for (int i = 0; i <= tb_width(); i++)
                {
                    tb_printf(i, tb_height() - 3, 0, TB_WHITE, " ");
                }
                tb_printf(0, tb_height() - 2, TB_WHITE, 0, "EXIT (Y/N) ? ");
                tb_present();

                struct tb_event confirm;
                tb_poll_event(&confirm);

                if (confirm.ch == 'y' || confirm.ch == 'Y')
                {
                    break;
                }
            }
            else if (event.ch != 0 && len < MAX_CHARS - 1)
            {
                for (int i = len; i > cursor; i--)
                    content[i] = content[i - 1];
                content[cursor] = (char)event.ch;
                len++;
                cursor++;
            }
        }
    }

    tb_shutdown();

    f = fopen(filename, "w");
    if (f != NULL)
    {
        fwrite(content, 1, len, f);
        fclose(f);
    }

    return 0;
}