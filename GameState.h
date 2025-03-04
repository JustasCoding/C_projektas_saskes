#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define _WIN32
#define CLEAR_CMD "cls"



#define MAX_BOARD 10
#define NAME_LEN 50
#define FILE_LEN 100
#define MOVE_LEN 10
#define MAX_MOVES 1000

//figuros
#define EMPTY ' '
#define P1 'o'
#define P2 'x'
#define P1_K 'O'
#define P2_K 'X'


typedef struct
{
	char board[MAX_BOARD][MAX_BOARD];
	int size;
	int currentPlayer;
	char p1Name[NAME_LEN];
	char p2Name[NAME_LEN];
	char saveFile[FILE_LEN];
} GameState;


void initializeBoard(GameState *g);
void drawBoard(const GameState *g);
int makeMove(GameState *g, char sc, int sr, char ec, int er);
int isValidMove(const GameState *g, int sr, int sc, int er, int ec);
int hasValidMoves(const GameState *g);
void switchPlayer(GameState *g);
int saveGame(const GameState *g);
int appendMove(const GameState *g, const char *move);
int loadGame(GameState *g, const char *filename);
void clearBuffer(void);
int parseMove(const char *input, char *sc, int *sr, char *ec, int *er);
void addTxtExt(char *filename);


#endif // GAMESTATE_H_INCLUDED
