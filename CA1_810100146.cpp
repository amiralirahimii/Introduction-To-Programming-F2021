#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#pragma warning(disable:4996)

//colors
#define RED 12
#define BLUE 3
#define GREEN 10
#define YELLOW 14
#define GRAY 8
#define PINK 13
#define WHITE 15
#define WAIT_TIME_MILI_SEC 100
//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
// general
#define BOARD_SIZE 40
#define INITIAL_SNAKE_LENGTH 3
#define MINIMUM_SNAKE_LENGTH 2
#define MAX_LEN_SNAKES 30
#define NUMBER_OF_MOUSES 20
//board_characters
#define EMPTY '0'
#define MOUSE 'm'
#define PLAYER1_SNAKE_HEAD '1'
#define PLAYER2_SNAKE_HEAD '2'
#define PLAYER1_SNAKE_BODY 'a'
#define PLAYER2_SNAKE_BODY 'b'
//Bernard, Poison and golden star
#define BERNARD_CLOCK 'c' //on the board character
#define GOLDEN_STAR '*' //on the board character
#define POISON 'x' //on the board character
#define NUMBER_OF_POISONS 5
#define NUMBER_OF_GOLDEN_STARS 3
#define BERNARD_CLOCK_APPEARANCE_CHANCE_PERCENT 20
#define BERNARD_CLOCK_APPEARANCE_CHECK_PERIOD_MILI_SEC 2000
#define BERNARD_CLOCK_FROZEN_TIME_MILI_SEC 4000

CONSOLE_FONT_INFOEX former_cfi;
CONSOLE_CURSOR_INFO former_info;
COORD former_screen_size;

void reset_console() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleDisplayMode(consoleHandle, CONSOLE_WINDOWED_MODE, &former_screen_size);
    SetCurrentConsoleFontEx(consoleHandle, FALSE, &former_cfi);
    SetConsoleCursorInfo(consoleHandle, &former_info);
}

void hidecursor(HANDLE consoleHandle)
{
    GetConsoleCursorInfo(consoleHandle, &former_info);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void set_console_font_and_font_size(HANDLE consoleHandle) {
    former_cfi.cbSize = sizeof(former_cfi);
    GetCurrentConsoleFontEx(consoleHandle, FALSE, &former_cfi);
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 20;
    cfi.dwFontSize.Y = 20;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Courier");
    SetCurrentConsoleFontEx(consoleHandle, FALSE, &cfi);
}

void set_full_screen_mode(HANDLE consoleHandle) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    former_screen_size.X = csbi.dwSize.X; former_screen_size.Y = csbi.dwSize.Y;
    COORD coord;
    SetConsoleDisplayMode(consoleHandle, CONSOLE_FULLSCREEN_MODE, &coord);
}

void init_screen()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    set_full_screen_mode(consoleHandle);
    hidecursor(consoleHandle);
    set_console_font_and_font_size(consoleHandle);

}

void wait_and_get_direction(int* player1_snake_direction, int* player2_snake_direction) {
    DWORD64 start_time, check_time;
    start_time = GetTickCount64();
    check_time = start_time + WAIT_TIME_MILI_SEC; //GetTickCount returns time in miliseconds
    char key = 0;
    char player1_key_hit = 0;
    char player2_key_hit = 0;

    while (check_time > GetTickCount64()) {
        if (_kbhit()) {
            key = _getch();
            if (key == 0)
                key = _getch();
            if (key == 'w' || key == 'a' || key == 's' || key == 'd')
                player1_key_hit = key;
            if (key == 'i' || key == 'j' || key == 'k' || key == 'l')
                player2_key_hit = key;
        }
    }

    switch (player1_key_hit) {
    case 'w': if (*player1_snake_direction != DOWN) *player1_snake_direction = UP; break;
    case 'a': if (*player1_snake_direction != RIGHT) *player1_snake_direction = LEFT; break;
    case 's': if (*player1_snake_direction != UP) *player1_snake_direction = DOWN; break;
    case 'd': if (*player1_snake_direction != LEFT) *player1_snake_direction = RIGHT; break;
    default: break;
    }

    switch (player2_key_hit) {
    case 'i': if (*player2_snake_direction != DOWN) *player2_snake_direction = UP; break;
    case 'j': if (*player2_snake_direction != RIGHT) *player2_snake_direction = LEFT; break;
    case 'k': if (*player2_snake_direction != UP) *player2_snake_direction = DOWN; break;
    case 'l': if (*player2_snake_direction != LEFT) *player2_snake_direction = RIGHT; break;
    default: break;
    }
}

void draw_point(char point_content) {
    switch (point_content) {
    case PLAYER1_SNAKE_HEAD: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED); printf("@"); break;
    case PLAYER2_SNAKE_HEAD: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);  printf("@"); break;
    case PLAYER1_SNAKE_BODY: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);  printf("o"); break;
    case PLAYER2_SNAKE_BODY: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BLUE);  printf("o"); break;
    case MOUSE: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GRAY); printf("m"); break;
    case GOLDEN_STAR: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW); printf("*"); break;
    case POISON: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GREEN); printf("x"); break;
    case BERNARD_CLOCK: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PINK); printf("c"); break;
    default: printf(" ");
    }
}

void draw_horizonatal_walls() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
    for (int i = 0; i < BOARD_SIZE + 2; ++i)
        printf("-");
    printf("\n");
}

void draw_board(char board_content[BOARD_SIZE][BOARD_SIZE]) {
    system("cls");
    draw_horizonatal_walls();
    for (int i = 0; i < BOARD_SIZE; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
        printf("|"); // vertical wall 
        for (int j = 0; j < BOARD_SIZE; j++)
            draw_point(board_content[i][j]);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
        printf("|\n"); // vertical wall
    }
    draw_horizonatal_walls();
}

void first_initialize(int player1_snake_body[MAX_LEN_SNAKES + 1][2], int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) { //initalizing snake1
        player1_snake_body[i][0] = BOARD_SIZE - INITIAL_SNAKE_LENGTH + i;
        player1_snake_body[i][1] = 0;
    }
    
    for (int i = 1; i < INITIAL_SNAKE_LENGTH; i++) { //putting snake1 in board content
        board_content[player1_snake_body[i][0]][player1_snake_body[i][1]] = PLAYER1_SNAKE_BODY;
    }
    board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] = PLAYER1_SNAKE_HEAD; //putting snake1 head seperatly in board_content

    for (int i = 0; i < INITIAL_SNAKE_LENGTH; i++) {
        player2_snake_body[i][0] = INITIAL_SNAKE_LENGTH-i-1;
        player2_snake_body[i][1] = BOARD_SIZE-1;
    }

    for (int i = 1; i < INITIAL_SNAKE_LENGTH; i++) { //putting snake2 in board content
        board_content[player2_snake_body[i][0]][player2_snake_body[i][1]] = PLAYER2_SNAKE_BODY;
    }
    board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] = PLAYER2_SNAKE_HEAD; //putting snake2 head seperatly in board_content
}


void put_mouses(char board_content[BOARD_SIZE][BOARD_SIZE]) {
    srand(time(NULL));
    for (int i = 0; i < NUMBER_OF_MOUSES; i++) {
        int x, y;
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board_content[x][y] != NULL) {
            i--;
            continue;
        }
        else {
            board_content[x][y] = MOUSE;
        }
    }
}

void replace_mouse(char board_content[BOARD_SIZE][BOARD_SIZE]) {
    srand(time(NULL));
    while (true) {
        int x, y;
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board_content[x][y] != NULL)
            continue;
        else {
            board_content[x][y] = MOUSE;
            break;
        }
    }
}

void put_golden_star(char board_content[BOARD_SIZE][BOARD_SIZE]) {
    srand(time(NULL));
    for (int i = 0; i < NUMBER_OF_GOLDEN_STARS; i++) {
        int x, y;
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board_content[x][y] != NULL) {
            i--;
            continue;
        }
        else {
            board_content[x][y] = GOLDEN_STAR;
        }
    }
}

void replace_golden_star(char board_content[BOARD_SIZE][BOARD_SIZE]) {
    srand(time(NULL));
    while (true) {
        int x, y;
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board_content[x][y] != NULL)
            continue;
        else {
            board_content[x][y] = GOLDEN_STAR;
            break;
        }
    }
}

void put_poison(char board_content[BOARD_SIZE][BOARD_SIZE]) {
    srand(time(NULL));
    for (int i = 0; i < NUMBER_OF_POISONS; i++) {
        int x, y;
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board_content[x][y] != NULL) {
            i--;
            continue;
        }
        else {
            board_content[x][y] = POISON;
        }
    }
}

void replace_poison(char board_content[BOARD_SIZE][BOARD_SIZE]) {
    srand(time(NULL));
    while (true) {
        int x, y;
        x = rand() % BOARD_SIZE;
        y = rand() % BOARD_SIZE;
        if (board_content[x][y] != NULL)
            continue;
        else {
            board_content[x][y] = POISON;
            break;
        }
    }
}

void put_bernard_clock(int *check_bernard_clock,char board_content[BOARD_SIZE][BOARD_SIZE], clock_t *refrence_clock_spawn) {
    clock_t now = clock();
    if (*check_bernard_clock == 1) {//when the bernard clock disappeard we should start measuring time
        *refrence_clock_spawn = now;
    }
    else if (now - *refrence_clock_spawn >= BERNARD_CLOCK_APPEARANCE_CHECK_PERIOD_MILI_SEC && *check_bernard_clock == 0) {//added this to make sure it goes to this if only when check is 0
        int check;//to check if we were lucky to have bernard clock or not
        srand(time(NULL));
        check = rand() % 100;
        if (check < BERNARD_CLOCK_APPEARANCE_CHANCE_PERCENT) {
            *check_bernard_clock = 1;
            while (true) {
                int x, y;
                x = rand() % BOARD_SIZE;
                y = rand() % BOARD_SIZE;
                if (board_content[x][y] != NULL)
                    continue;
                else {
                    board_content[x][y] = BERNARD_CLOCK;
                    break;
                }
            }
        }
        *refrence_clock_spawn = now;
    }
}

void change_player1_snake_body(int player1_snake_length, int player1_snake_body[MAX_LEN_SNAKES + 1][2], int player1_snake_direction) {
    //this function change the snake body based on the direction

    for (int i = player1_snake_length - 1; i >= 0; i--) {
        player1_snake_body[i + 1][0] = player1_snake_body[i][0];
        player1_snake_body[i + 1][1] = player1_snake_body[i][1];
    }

    switch (player1_snake_direction) {
    case UP:
        if (player1_snake_body[0][0] == 0) {
            player1_snake_body[0][0] = BOARD_SIZE - 1;
        }
        else
            player1_snake_body[0][0]--;
        break;
    case DOWN:
        if (player1_snake_body[0][0] == BOARD_SIZE - 1) {
            player1_snake_body[0][0] = 0;
        }
        else
            player1_snake_body[0][0]++;
        break;
    case RIGHT:
        if (player1_snake_body[0][1] == BOARD_SIZE - 1) {
            player1_snake_body[0][1] = 0;
        }
        else
            player1_snake_body[0][1]++;
        break;
    case LEFT:
        if (player1_snake_body[0][1] == 0) {
            player1_snake_body[0][1] = BOARD_SIZE - 1;
        }
        else
            player1_snake_body[0][1]--;
        break;
    }
}

void change_player2_snake_body(int player2_snake_length, int player2_snake_body[MAX_LEN_SNAKES + 1][2], int player2_snake_direction) {
    //this function change the snake body based on the direction

    for (int i = player2_snake_length - 1; i >= 0; i--) {
        player2_snake_body[i + 1][0] = player2_snake_body[i][0];
        player2_snake_body[i + 1][1] = player2_snake_body[i][1];
    }

    switch (player2_snake_direction) {
    case UP:
        if (player2_snake_body[0][0] == 0) {
            player2_snake_body[0][0] = BOARD_SIZE - 1;
        }
        else
            player2_snake_body[0][0]--;
        break;
    case DOWN:
        if (player2_snake_body[0][0] == BOARD_SIZE - 1) {
            player2_snake_body[0][0] = 0;
        }
        else
            player2_snake_body[0][0]++;
        break;
    case RIGHT:
        if (player2_snake_body[0][1] == BOARD_SIZE - 1) {
            player2_snake_body[0][1] = 0;
        }
        else
            player2_snake_body[0][1]++;
        break;
    case LEFT:
        if (player2_snake_body[0][1] == 0) {
            player2_snake_body[0][1] = BOARD_SIZE - 1;
        }
        else
            player2_snake_body[0][1]--;
        break;
    }
}

void check_player1_eat_mouse(int* player1_snake_length, int player1_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    if (board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] == MOUSE) {//check if player 1 eat mouse
        if (*player1_snake_length < MAX_LEN_SNAKES) {
            (*player1_snake_length)++;
            replace_mouse(board_content);
        }
        else {
            board_content[player1_snake_body[*player1_snake_length][0]][player1_snake_body[*player1_snake_length][1]] = EMPTY;
            replace_mouse(board_content);
        }
    }
}

void check_player2_eat_mouse(int* player2_snake_length, int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == MOUSE) {//check if player 2 eat mouse
        if (*player2_snake_length < MAX_LEN_SNAKES) {
            (*player2_snake_length)++;
            replace_mouse(board_content);
        }
        else {
            board_content[player2_snake_body[*player2_snake_length][0]][player2_snake_body[*player2_snake_length][1]] = EMPTY;
            replace_mouse(board_content);
        }
    }
}

int check_player1_eat_golden_star(int* player2_snake_length, int player1_snake_body[MAX_LEN_SNAKES + 1][2], int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    if (board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] == GOLDEN_STAR) {//return 1 in case of player 2 loss and return 0 in case of continue
        (*player2_snake_length)--;
        replace_golden_star(board_content);
        board_content[player2_snake_body[*player2_snake_length+1][0]][player2_snake_body[*player2_snake_length+1][1]] = EMPTY;
    }
    if (*player2_snake_length < MINIMUM_SNAKE_LENGTH)
        return 1;
    else
        return 0;
}

int check_player2_eat_golden_star(int* player1_snake_length, int player1_snake_body[MAX_LEN_SNAKES + 1][2], int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == GOLDEN_STAR) {//return 2 in case of player 1 loss and return 0 in case of continue
        (*player1_snake_length)--;
        replace_golden_star(board_content);
        board_content[player1_snake_body[*player1_snake_length+1][0]][player1_snake_body[*player1_snake_length+1][1]] = EMPTY;
    }
    if (*player1_snake_length < MINIMUM_SNAKE_LENGTH)
        return 2;
    else
        return 0;
}

int check_player1_eat_poison(int* player1_snake_length, int player1_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    if (board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] == POISON) {//return 2 in case of player 1 loss and return 0 in case of continue
        (*player1_snake_length)--;
        replace_poison(board_content);
        board_content[player1_snake_body[*player1_snake_length + 1][0]][player1_snake_body[*player1_snake_length + 1][1]] = EMPTY;
    }
    if (*player1_snake_length < MINIMUM_SNAKE_LENGTH)
        return 2;
    else
        return 0;
}

int check_player2_eat_poison(int* player2_snake_length, int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == POISON) {//return 1 in case of player 2 loss and return 0 in case of continue
        (*player2_snake_length)--;
        replace_poison(board_content);
        board_content[player2_snake_body[*player2_snake_length + 1][0]][player2_snake_body[*player2_snake_length + 1][1]] = EMPTY;
    }
    if (*player2_snake_length < MINIMUM_SNAKE_LENGTH)
        return 1;
    else
        return 0;
}

void check_player1_eat_bernard_clock(int *check_bernard_clock,int player1_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE],clock_t *refrence_player_freeze, int* check_player2_freeze) {
    if (board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] == BERNARD_CLOCK) {
        *check_bernard_clock = 0;
        *refrence_player_freeze = clock();
        *check_player2_freeze = 1;//when player 1 eat the clock,player 2 should get freezed
    }
}

void check_player1_freeze_past(clock_t *refrence_player_freeze, int* check_player1_freeze) {
    clock_t now = clock();
    if (now - *refrence_player_freeze >= BERNARD_CLOCK_FROZEN_TIME_MILI_SEC) {
        *check_player1_freeze = 0;
    }
}

void check_player2_eat_bernard_clock(int* check_bernard_clock,int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE],clock_t* refrence_player_freeze , int* check_player1_freeze) {
    if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == BERNARD_CLOCK) {
        *check_bernard_clock = 0;
        *refrence_player_freeze = clock();
        *check_player1_freeze = 1;//when player 2 eat the clock,player 1 should get freezed
    }
}

void check_player2_freeze_past(clock_t* refrence_player_freeze, int* check_player2_freeze) {
    clock_t now = clock();
    if (now - *refrence_player_freeze >= BERNARD_CLOCK_FROZEN_TIME_MILI_SEC) {
        *check_player2_freeze = 0;
    }
}

void move_snakes(int* player1_snake_length, int* player2_snake_length, int player1_snake_body[MAX_LEN_SNAKES + 1][2], int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE]) {
    //clearing the last part of snakes and placing the body behind head(didnt use "for" to lease the process)
    board_content[player1_snake_body[*player1_snake_length][0]][player1_snake_body[*player1_snake_length][1]] = EMPTY;
    board_content[player2_snake_body[*player2_snake_length][0]][player2_snake_body[*player2_snake_length][1]] = EMPTY;
    board_content[player1_snake_body[1][0]][player1_snake_body[1][1]] = PLAYER1_SNAKE_BODY;
    board_content[player2_snake_body[1][0]][player2_snake_body[1][1]] = PLAYER2_SNAKE_BODY;
}

int change_board_content(int* player1_snake_length, int* player2_snake_length, int player1_snake_body[MAX_LEN_SNAKES + 1][2], int player2_snake_body[MAX_LEN_SNAKES + 1][2], char board_content[BOARD_SIZE][BOARD_SIZE],int*check_bernard_clock,clock_t *refrence_player_freeze,int * check_player1_freeze ,int * check_player2_freeze) {
    //use arguemnets to initialize the new board content and returns 1 or 2 to shows winner and 3 to show draw and 0 for continue
    if (player1_snake_body[0][0] == player2_snake_body[0][0] && player1_snake_body[0][1] == player2_snake_body[0][1]) {
        if (*player1_snake_length > *player2_snake_length) { //check if the heads hit each other
            return 1;
        }
        else if (*player1_snake_length == *player2_snake_length) {
            return 3;
        }
        else if (*player1_snake_length < *player2_snake_length)
            return 2;
    }

    int check_player_1_win;//this variable is for checking if the players lose on eating poison or golden star
    int check_player_2_win;//this variable is for checking if the players lose on eating poison or golden star
    check_player1_eat_mouse(player1_snake_length,player1_snake_body,board_content);
    check_player2_eat_mouse(player2_snake_length, player2_snake_body, board_content);
    check_player_1_win = check_player1_eat_golden_star(player2_snake_length, player1_snake_body, player2_snake_body, board_content);
    check_player_2_win = check_player2_eat_golden_star(player1_snake_length, player1_snake_body, player2_snake_body, board_content);
    check_player_2_win = check_player1_eat_poison(player1_snake_length, player1_snake_body, board_content);
    check_player_1_win = check_player2_eat_poison(player2_snake_length, player2_snake_body, board_content);
    check_player1_eat_bernard_clock(check_bernard_clock,player1_snake_body, board_content, refrence_player_freeze ,check_player2_freeze);
    check_player2_eat_bernard_clock(check_bernard_clock,player2_snake_body, board_content, refrence_player_freeze ,check_player1_freeze);
    if (check_player_1_win == 1 && check_player_2_win == 2)
        return 3;
    else if (check_player_1_win == 1)
        return 1;
    else if (check_player_2_win == 2)
        return 2;

    move_snakes(player1_snake_length, player2_snake_length, player1_snake_body, player2_snake_body, board_content);

    if (board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] == PLAYER2_SNAKE_BODY) { //check if player 1 hit the enemy body
        if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == PLAYER1_SNAKE_BODY) {
            if (*player1_snake_length > *player2_snake_length) {
                return 1;
            }
            else if (*player1_snake_length == *player2_snake_length) {
                return 3;
            }
            else if (*player1_snake_length < *player2_snake_length)
                return 2;
        }
        else
            return 2;
    }
    else if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == PLAYER1_SNAKE_BODY) {//check if player 2 hit enemy body
        return 1;
    }

    else if (board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] == PLAYER1_SNAKE_BODY) {//check if player 1 hit his body
        if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == PLAYER2_SNAKE_BODY) {
            if (*player1_snake_length > *player2_snake_length) {
                return 1;
            }
            else if (*player1_snake_length == *player2_snake_length) {
                return 3;
            }
            else if (*player1_snake_length < *player2_snake_length)
                return 2;
        }
        else
            return 2;
    }
    else if (board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] == PLAYER2_SNAKE_BODY) {//check if player 2 hit his body
        return 1;
    }
    //here when we have checked everything we update the snake heads positions on the board content
    board_content[player1_snake_body[0][0]][player1_snake_body[0][1]] = PLAYER1_SNAKE_HEAD;
    board_content[player2_snake_body[0][0]][player2_snake_body[0][1]] = PLAYER2_SNAKE_HEAD;
    return 0;
}


// prototypes
void init_screen();
void reset_console();
void wait_and_get_direction(int* player1_snake_direction, int* player2_snake_direction);
void draw_board(char board_content[BOARD_SIZE][BOARD_SIZE]);

int main() {
    //First defining the variables that are needed during the game
    char board_content[BOARD_SIZE][BOARD_SIZE] = { 0 };
    int player1_snake_direction=UP;
    int player2_snake_direction=DOWN;
    int player1_snake_body[MAX_LEN_SNAKES + 1][2] = { 0 };
    int player2_snake_body[MAX_LEN_SNAKES + 1][2] = { 0 };
    int player1_snake_length = INITIAL_SNAKE_LENGTH;
    int player2_snake_length = INITIAL_SNAKE_LENGTH;
    first_initialize(player1_snake_body, player2_snake_body, board_content);
    put_mouses(board_content);
    put_golden_star(board_content);
    put_poison(board_content);
    int check_who_won = 0;//to check if the change_board_content resturn 1 or 2 or 3
    int check_bernard_clock = 0; //to check if bernard clock exists on board or not
    int check_player1_freeze=0; //to check if players are frozen or not
    int check_player2_freeze = 0;//to check if players are frozen or not
    clock_t refrence_clock_spawn = clock();
    clock_t refrence_player_freeze;

    init_screen(); //call this to make window adjustments
    while (TRUE) {
        draw_board(board_content);
        wait_and_get_direction(&player1_snake_direction, &player2_snake_direction);
        put_bernard_clock(&check_bernard_clock, board_content, &refrence_clock_spawn);//the func first check that is there any bernard clock or not
        if (check_player1_freeze == 1) {
            check_player1_freeze_past(&refrence_player_freeze,&check_player1_freeze);
        }
        if (check_player1_freeze == 0) {
            change_player1_snake_body(player1_snake_length, player1_snake_body, player1_snake_direction);
        }
        if (check_player2_freeze == 1) {
            check_player2_freeze_past(&refrence_player_freeze, &check_player2_freeze);
        }
        if (check_player2_freeze == 0) {
            change_player2_snake_body(player2_snake_length, player2_snake_body, player2_snake_direction);
        }
        check_who_won = change_board_content(&player1_snake_length, &player2_snake_length, player1_snake_body, player2_snake_body, board_content,&check_bernard_clock,&refrence_player_freeze,&check_player1_freeze, &check_player2_freeze);
        if (check_who_won != 0) {
            break;
        }
    }
    //The Game is Finished, Showing the result.
    reset_console();
    if (check_who_won == 1) {
        printf("Player Red(1) has won the game");
    }
    else if (check_who_won == 2) {
        printf("Player Blue(2) has won the game");
    }
    else if (check_who_won == 3) {
        printf("Draw");
    }
    getchar();
    return 0;
}