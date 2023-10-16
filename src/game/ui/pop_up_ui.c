#include "pop_up_ui.h"
#include <ncurses.h>
#include <string.h>
#include "utils.h"
#include "engine_types.h"
#include "game.h"
#include "main_menu.h"
#include <stdlib.h>
#include "components.h"
#include "draw.h"
#include "about_menu.h"

/*

* a104439 - Rita Camacho

* Menu pop up de anúncio de novos itens.

*/
int new_items_pop_up(char text[], int width, int screenYMax, int screenXMax){
// texto a ser exibido na tela
// largura da janela pop up
// dimensoes maximas da tela

    int key = 0, option = 0;
    int necessaryLines = count_newlines(text) + 1 + 4 + 1; // calculo das linhas necessarias tendo em conta o numero de quebras de linha
    // 1 para a linha do texto, 4 para as linhas de borda da janela, 1 para a linha do botao OK 

    int posY = (screenYMax - necessaryLines) / 2;
    int posX = (screenXMax - width) / 2;
// posicoes da janela pop up para o CENTRO da tela 




    WINDOW *newItemsWindow = newwin(necessaryLines, width, posY, posX); // cria janela
    box(newItemsWindow, 0, 0); // cria borda ao redor da janela
    refresh(); // atualiza a tela
    wrefresh(newItemsWindow); // exibe janela
    keypad(newItemsWindow, true); // ativa entrada de teclas do teclado


mvwprintw(newItemsWindow, 1, 2, "%s", text); // texto impresso na posicao (1,2)
    
    if(LEVEL % 5 == 0){
        mvwprintw(newItemsWindow, 2, 2, "New bomb unlocked!");
    }
// se o inteiro for divisivel por 5, é exibida a mensagem em (2,2)




while(key != 10){ // 10 representa a tecla ENTER

    if(!option){
            wattron(newItemsWindow, A_BOLD); 
            mvwprintw(newItemsWindow, necessaryLines - 2, width - 5, "OK!");
            wattroff(newItemsWindow, A_BOLD);
        }

key = wgetch(newItemsWindow); // aguarda a entrada do usuario e captura a tecla pressionada 


    switch (key)
        {
            case KEY_UP:

                if(option){
                    option--;
                } else {
                    option++;
                }
                break;



            case KEY_DOWN:

                if(option){
                    option--;
                } else {
                    option++;
                }
                break;
            default:
                break;



        }  // loop para que usuario possa navegar pelas opcoes da janela pop up com as telcas up e down 


}



    wclear(newItemsWindow); // limpa a janela pop up
    wrefresh(newItemsWindow); // atualiza a tela
    delwin(newItemsWindow); // destrui a janela pop up 


return !option;
}

/*

* a104439 - Rita Camacho

* Menu pop up de anúncio de vencedor.

*/
int winner_pop_up(char text[], int width, int screenYMax, int screenXMax, Terminal *terminal){

    int key = 0, option = 0;
    int necessaryLines = count_newlines(text) + 1 + 4 + 1; // numero de linhas necessarias para exibir o pop up 
    int posY = (screenYMax - necessaryLines) / 2; // centralizar na tela 
    int posX = (screenXMax - width) / 2; // centralizar na tela 

    WINDOW *winnerWindow = newwin(necessaryLines, width, posY, posX); // janela winnerWindow que recebe a janela criada no terminal a partir de nwewin
    box(winnerWindow, 0, 0); // borda ao redor da janela 
    refresh(); // atualiza a pagina // toda a tela padrao
    wrefresh(winnerWindow); // atualiza apenas a janela winnerWindow 
    keypad(winnerWindow, true); // ativa a entrada de teclado 

    mvwprintw(winnerWindow, 1, 2, "%s", text); // imrpime o texto 

    while(key != 10){
        
        wattron(winnerWindow, A_BOLD);
        mvwprintw(winnerWindow, necessaryLines - 2, width - 23, "YAY! START A NEW GAME");
        wattroff(winnerWindow, A_BOLD);

        key = wgetch(winnerWindow);

        switch (key)
        {
            case KEY_UP:
                if(option){
                    option--;
                } else {
                    option++;
                }
                break;
            case KEY_DOWN:
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

    wclear(winnerWindow);
    wrefresh(winnerWindow);
    delwin(winnerWindow);
    endwin();

    clear();
    refresh();
    
    int selection = main_menu(terminal);

    while(selection == 0 || selection == 1 || selection == 3){
        switch (selection){
        case 0:
            {
                Vector2D inputPos = {terminal->xMax / 2 - 15, terminal->yMax / 2};
                char *playerName = malloc(sizeof(char) * 16);
                text_input_box(inputPos, 30, 15, "Insert your name:", playerName);
                clear();
                refresh();
                game(terminal, playerName);
                clear();
            }
            break;
        case 1:
            {   
                WINDOW * aboutWindow = newwin(terminal->yMax, terminal->xMax, 0, 0);
                box(aboutWindow, 0, 0); 
                clear();
                refresh();
                show_about_menu(terminal, aboutWindow);
                wrefresh(aboutWindow);
                delwin(aboutWindow);
                clear();
                refresh();
            }
            break;
        default:
            break; 
        }
        selection = main_menu(terminal);
    }
    endwin();
    exit(0);
    return -1;
    
    return !option;
}

/*

* a104439 - Rita Camacho

* Menu pop up de game over.

*/
int game_over_pop_up(char text[], int width, int screenYMax, int screenXMax, Terminal *terminal){

    int key = 0, option = 0;
    int necessaryLines = count_newlines(text) + 1 + 4 + 1;
    int posY = (screenYMax - necessaryLines) / 2;
    int posX = (screenXMax - width) / 2;

    WINDOW *gameOverWindow = newwin(necessaryLines, width, posY, posX);
    box(gameOverWindow, 0, 0);
    refresh();
    wrefresh(gameOverWindow);
    keypad(gameOverWindow, true); 

    mvwprintw(gameOverWindow, 1, 2, "%s", text);

    while(key != 10){
        
        wattron(gameOverWindow, A_BOLD);
        mvwprintw(gameOverWindow, necessaryLines - 2, width - 12, "TRY AGAIN!");
        wattroff(gameOverWindow, A_BOLD);

        key = wgetch(gameOverWindow); // aguarda a entrada do usuario que retorna o codigo ascii da tecla pressionada 

        switch (key)
        {
            case KEY_UP:
                if(option){
                    option--;
                } else {
                    option++;
                }
                break;
            case KEY_DOWN:
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

    wclear(gameOverWindow); // limpa a janela do pop up 
    wrefresh(gameOverWindow); // atualiza a tela
    delwin(gameOverWindow); // exclui a janela 
    endwin(); // finaliza o modo de terminal 

    clear(); // limpa a tela
    refresh(); // atualiza a tela 

    int selection = main_menu(terminal); // main_menu para exibir no terminal 

while(selection == 0 || selection == 1 || selection == 3){


    switch (selection){
        case 0:
            {
                Vector2D inputPos = {terminal->xMax / 2 - 15, terminal->yMax / 2};
                char *playerName = malloc(sizeof(char) * 16);
                text_input_box(inputPos, 30, 15, "Insert your name:", playerName);
                clear();
                refresh();
                game(terminal, playerName); // inicia um novo jogo 
                clear();
            }
            break;
        case 1:
            {   
                WINDOW * aboutWindow = newwin(terminal->yMax, terminal->xMax, 0, 0);
                box(aboutWindow, 0, 0); 
                clear();
                refresh();
                show_about_menu(terminal, aboutWindow);
                wrefresh(aboutWindow);
                delwin(aboutWindow);
                clear();
                refresh();
            }
            break;


        default:
            break; 
        }

    selection = main_menu(terminal); // exibe no terminal 

    }
    endwin(); // finaliza o modo terminal
    exit(0); // sai do programa 

    return !option;// retorna 0 se option for diferente de 0 
                // retorna 1 se option for igual a 0 
                // usado para saber se o jogador deseja jogar novamente ou nao 
}

/*

* a104356 - João Lobo

* Janela de ajuda com informação e controlos do jogo.

*/
void help_pop_up(Terminal *terminal){
    int height = 25;
    int width = 90;
    int posX = (terminal->xMax - width) / 2;
    int posY = (terminal->yMax - height) / 2;

    WINDOW * helpWindow = newwin(height, width, posY, posX);
    box(helpWindow, 0, 0);
    refresh();
    wrefresh(helpWindow);
    keypad(helpWindow, true);

    int page = 0;

    int key = 0;
    while(key != 10){
        if(!page){
            wclear(helpWindow);
            box(helpWindow, 0, 0);
            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 1, 2, "ROGUE POINTERS' HELP GUIDE");
            wattroff(helpWindow, A_BOLD);

            mvwprintw(helpWindow, 3, 2, "The goal of the game is to reach the deepest level possible, staying alive");
            mvwprintw(helpWindow, 4, 2, "and collecting the largest amount of items you can!");

            Vector2D playerPos = {55 / 2, 4};
            draw_custom_pixel_to_window(playerPos, "<>", 35, 4, terminal, helpWindow);

            mvwprintw(helpWindow, 6, 2, "Portals allow you to move up and down levels.");

            Vector2D portalPos = {48 / 2, 6};
            draw_custom_pixel_to_window(portalPos, "[]", 84, 83, terminal, helpWindow);

            mvwprintw(helpWindow, 8, 2, "Surprises await you in locked rooms! Find the key, open the chest, ");
            mvwprintw(helpWindow, 9, 2, "and collect items that you can see in your inventory.");

            Vector2D keyPos = {56 / 2, 9};
            draw_custom_pixel_to_window(keyPos, "-o", 82, 70, terminal, helpWindow);

            mvwprintw(helpWindow, 11, 2, "Watch out for water! You can only survive a dive if you stay close to the edges");
            mvwprintw(helpWindow, 12, 2, "losing oxygen and risking your life when you swim in deep water!");

            Vector2D waterPos = {68 / 2, 12};
            draw_custom_pixel_to_window(waterPos, "~ ", choose_color(51, 2, waterPos)-1, choose_color(50, 3, waterPos), terminal, helpWindow);

            mvwprintw(helpWindow, 14, 2, "Don't step on lava! You can only walk on it if you have rocks in your inventory.");

            Vector2D lavaPos = {84 / 2, 14};
            draw_custom_pixel_to_window(lavaPos, "  ", 0, choose_color(44, 5, lavaPos), terminal, helpWindow);

            mvwprintw(helpWindow, 16, 2, "Beware of zombies! It's best to have the greatest weapon or run for your life!");
            mvwprintw(helpWindow, 17, 2, "You may encounter zombies that only fight up close, and others that prefer");
            mvwprintw(helpWindow, 18, 2, "long-distance battles.");

            Vector2D zombieDumbPos = {26 / 2, 18};
            Vector2D zombieIntelligentPos = {30 / 2, 18};
            draw_custom_pixel_to_window(zombieDumbPos, "><", 35, 0, terminal, helpWindow);
            draw_custom_pixel_to_window(zombieIntelligentPos, "><", 35, 59, terminal, helpWindow);

            mvwprintw(helpWindow, 20, 2, "Discover special items and features to enhance your experience while playing");
            mvwprintw(helpWindow, 21, 2, "Rogue Pointers!");

            mvwprintw(helpWindow, 23, 2, "Use the arrow keys to change pages [<][>]");
            mvwprintw(helpWindow, 23, 66, "Press [ENTER] to leave");
        } else {
            wclear(helpWindow);
            box(helpWindow, 0, 0);
            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 1, 2, "ROGUE POINTERS' CONTROL SCHEME");
            wattroff(helpWindow, A_BOLD);

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 3, 2, "MOVING:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 3, 10, "Numpad [1] [2] [3] [4] [6] [7] [8] [9] and arrow keys [<] [/\\] [\\/] [>]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 5, 2, "GO UP IN PORTAL:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 5, 19, "[U]");

            mvwprintw(helpWindow, 5, 23, "|");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 5, 25, "GO DOWN IN PORTAL:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 5, 44, "[D]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 7, 2, "OPEN CHESTS:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 7, 15, "[O]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 9, 2, "ITEM INTERACTIONS (MELEE ATTACK, PLACING BOMBS & LIGHT BEACONS):");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 9, 67, "[ENTER]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 11, 2, "PAUSE GAME:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 11, 14, "[SPACE]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 13, 2, "CLICK BUTTONS:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 13, 17, "[MOUSE LEFT CLICK]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 15, 2, "PLAYER PATHFINDING:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 15, 22, "[MOUSE LEFT CLICK] or [M] arrow keys [<] [/\\] [\\/] [>] then [M]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 17, 2, "SHOOT BULLETS:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 17, 17, "[MOUSE RIGHT CLICK]");

            wattron(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 19, 2, "DEVELOPER CONSOLE:");
            wattroff(helpWindow, A_BOLD);
            mvwprintw(helpWindow, 19, 21, "[+]");

            mvwprintw(helpWindow, 23, 2, "Use the arrow keys to change pages [<][>]");
            mvwprintw(helpWindow, 23, 66, "Press [ENTER] to leave");
        }

        key = wgetch(helpWindow);
        switch (key)
        {
            case KEY_RIGHT:
            case KEY_LEFT:
                page = !page;
                break;
            default:
                break;
        }
    }

    wclear(helpWindow);
    wrefresh(helpWindow);
    delwin(helpWindow);
    endwin();

    clear();
    refresh();
}

/*

* a104439 - Rita Camacho

* Menu pop up de pausa.

*/
int pause_pop_up(char text[], int width, int screenYMax, int screenXMax, Terminal *terminal){

    int key = 0, option = 0;
    int necessaryLines = count_newlines(text) + 1 + 4 + 1;
    int posY = (screenYMax - necessaryLines) / 2;
    int posX = (screenXMax - width) / 2;

    WINDOW * pauseWindow = newwin(necessaryLines, width, posY, posX);
    box(pauseWindow, 0, 0);
    refresh();
    wrefresh(pauseWindow);
    keypad(pauseWindow, true);

    mvwprintw(pauseWindow, 1, 2, "%s", text);

    while(key != 10){
        if(!option){
            wattron(pauseWindow, A_BOLD);
            mvwprintw(pauseWindow, necessaryLines - 2, 2, "CONTINUE");
            wattroff(pauseWindow, A_BOLD);
            mvwprintw(pauseWindow, necessaryLines - 2, width - 9, "RESTART");
        } else {
            wattron(pauseWindow, A_BOLD);
            mvwprintw(pauseWindow, necessaryLines - 2, width - 9, "RESTART");
            wattroff(pauseWindow, A_BOLD);
            mvwprintw(pauseWindow, necessaryLines - 2, 2, "CONTINUE");
        }

        key = wgetch(pauseWindow);
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
            case 10:
                if(option){
                    wclear(pauseWindow);
                    wrefresh(pauseWindow);
                    delwin(pauseWindow);
                    endwin();

                    clear();
                    refresh();
                    int selection = main_menu(terminal);

                    while(selection == 0 || selection == 1 || selection == 3){
                        switch (selection){
                        case 0:
                            {
                                Vector2D inputPos = {terminal->xMax / 2 - 15, terminal->yMax / 2};
                                char *playerName = malloc(sizeof(char) * 16);
                                text_input_box(inputPos, 30, 15, "Insert your name:", playerName);
                                clear();
                                refresh();
                                game(terminal, playerName);
                                clear();
                            }
                            break;
                        case 1:
                            {   
                                WINDOW * aboutWindow = newwin(terminal->yMax, terminal->xMax, 0, 0);
                                box(aboutWindow, 0, 0); 
                                clear();
                                refresh();
                                show_about_menu(terminal, aboutWindow);
                                wrefresh(aboutWindow);
                                delwin(aboutWindow);
                                clear();
                                refresh();
                            }
                            break;
                        default:
                            break; 
                        }
                        selection = main_menu(terminal);
                    }
                    endwin();
                    exit(0);
                    return -1;
                }
                break;
            default:
                break;
        }
    }

    wclear(pauseWindow);
    wrefresh(pauseWindow);
    delwin(pauseWindow);
    endwin();

    return !option;
}