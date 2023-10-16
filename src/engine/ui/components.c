#include "components.h"
#include <ncurses.h>
#include <string.h>
#include "utils.h"
#include "engine_types.h"
#include "game.h"


// Elementos de UI genéricos, podem ser utilizados por si mesmos ou combinados com outros.


int menu_select(int options, char *texts[], int width, int y, int x){
    int selection = 0, key = 0;
    int lines  = options + 4;

    WINDOW * selectWindow = newwin(lines, width, y, x);
    box(selectWindow, 0, 0);
    refresh();
    wrefresh(selectWindow);
    keypad(selectWindow, true);

    while (key != 10)
    {
        for(int i = 0; i < options; i++){
            if(i == selection){
                wattron(selectWindow, A_BOLD);
                mvwprintw(selectWindow, i+2, 4, "[x] %s", texts[i]);
                wattroff(selectWindow, A_BOLD);
            } else {
                mvwprintw(selectWindow, i+2, 4, "[ ] %s", texts[i]);
            }
            wrefresh(selectWindow);
        }

        move(0, 0);

        key = wgetch(selectWindow);
        switch (key)
        {
            case KEY_UP:
                if(selection > 0){
                    selection--;
                } else {
                    selection = options - 1;
                }
                break;
            case KEY_DOWN:
                if(selection < options - 1){
                    selection++;
                } else {
                    selection = 0;
                }
                break;
            default:
                break;
        }
    }

    wclear(selectWindow);
    wrefresh(selectWindow);
    delwin(selectWindow);
    return selection;
}


int modal_confim(char text[], int width, int screenYMax, int screenXMax){
    int key = 0, option = 0;
    int necessaryLines = count_newlines(text) + 1 + 4;
    int posY = (screenYMax - necessaryLines) / 2;
    int posX = (screenXMax - width) / 2;

    WINDOW * modalWindow = newwin(necessaryLines, width, posY, posX);
    box(modalWindow, 0, 0);
    refresh();
    wrefresh(modalWindow);
    keypad(modalWindow, true);

    mvwprintw(modalWindow, 1, 2, "%s", text);


    while(key != 10){
        if(!option){
            wattron(modalWindow, A_BOLD);
            mvwprintw(modalWindow, necessaryLines - 2, 2, "YES");
            wattroff(modalWindow, A_BOLD);
            mvwprintw(modalWindow, necessaryLines - 2, width - 4, "NO");
        } else {
            wattron(modalWindow, A_BOLD);
            mvwprintw(modalWindow, necessaryLines - 2, width - 4, "NO");
            wattroff(modalWindow, A_BOLD);
            mvwprintw(modalWindow, necessaryLines - 2, 2, "YES");
        }

        key = wgetch(modalWindow);
        switch (key)
        {
            case KEY_RIGHT:
                if(option){
                    option--;
                } else {
                    option++;
                }
                break;
            case KEY_LEFT:
                if(option){
                    option--;
                } else {
                    option++;
                }
                break;
            default:
                break;
        }
    }

    wclear(modalWindow);
    wrefresh(modalWindow);
    delwin(modalWindow);
    return !option;
}


void button(short color_gradient[4], char *text, Vector2D pos){
    refresh();
    int textSize = strlen(text);

    attron(COLOR_PAIR(color_gradient[0]+8 ));
    attron(A_BOLD);
    mvprintw(pos.y, pos.x + 3, "%s", text);
    attroff(A_BOLD);
    attroff(COLOR_PAIR(color_gradient[0]+8));

    attron(COLOR_PAIR(color_gradient[1]+8));
    mvaddch(pos.y, pos.x+2, ' ');
    mvaddch(pos.y, pos.x + 3 + textSize, ' ');
    attroff(COLOR_PAIR(color_gradient[1]+8));

    attron(COLOR_PAIR(color_gradient[2]+8));
    mvaddch(pos.y, pos.x+1, ' ');
    mvaddch(pos.y, pos.x + 4 + textSize, ' ');
    attroff(COLOR_PAIR(color_gradient[2]+8));

    attron(COLOR_PAIR(color_gradient[3]+8));
    mvaddch(pos.y, pos.x, ' ');
    mvaddch(pos.y, pos.x + 5 + textSize, ' ');
    attroff(COLOR_PAIR(color_gradient[3]+8));
}



void progress_bar(int value, int max, int width, short primaryColor, short secondaryColor, char *text, Vector2D pos){
    int posX = (width / 2) - (strlen(text) / 2);
    int textEnd = posX+strlen(text);

    int index = 0;
    for(int i = 0; i < pos.x + width; i++){
        if(i < (value * width / max)){
            attron(COLOR_PAIR(primaryColor+8));
            attron(A_BOLD);
            if(i >= posX && i < textEnd){
                mvaddch(pos.y, pos.x + i, text[index]);
                index++;
            } else {
                mvaddch(pos.y, pos.x + i, ' ');
            }
            attroff(A_BOLD);
            attroff(COLOR_PAIR(primaryColor+8));
        } else {
            attron(COLOR_PAIR(secondaryColor+8));
            attron(A_BOLD);
            if(i >= posX && i < textEnd){
                mvaddch(pos.y, pos.x + i, text[index]);
                index++;
            } else {
                mvaddch(pos.y, pos.x + i, ' ');
            }
            attroff(A_BOLD);
            attroff(COLOR_PAIR(secondaryColor+8));
        }
    }
}


void text_input_box(Vector2D pos, int width, int inputSize, char *text, char *input){
    WINDOW * inputWindow = newwin(5, width, pos.y, pos.x);
    box(inputWindow, 0, 0);
    refresh();
    wrefresh(inputWindow);

    mvwprintw(inputWindow, 1, 2, "%s", text);
    mvwaddch(inputWindow, 3, 1, '>');

    int cursor = 0;
    int ch;

    wmove(inputWindow, 3, 3);
    curs_set(1);

    while ((ch = wgetch(inputWindow)) != '\n' && cursor < inputSize - 1) {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (cursor > 0) {
                cursor--;
                input[cursor] = ' ';
                mvwprintw(inputWindow, 3, 3, "%s", input);
                input[cursor] = '\0';
                wmove(inputWindow, 3, 3 + cursor);
                wrefresh(inputWindow);
            }
        } else {
            input[cursor] = ch;
            input[cursor + 1] = '\0';
            mvwprintw(inputWindow, 3, 3, "%s", input);
            cursor++;
            wmove(inputWindow, 3, 3 + cursor);
            wrefresh(inputWindow);
        }
    }

    curs_set(0);

    wclear(inputWindow);
    wrefresh(inputWindow);
    delwin(inputWindow);
}
