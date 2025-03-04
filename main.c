#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "GameState.h"


int main()
{
    GameState g;
    int choice;
    printf("Welcome to Checkers!\n1) New Game\n2) Load Game\nChoice: ");

    if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2))
    {
        printf("Invalid choice.\n");
        return EXIT_FAILURE;
    }

    clearBuffer();

    if (choice == 1)
    {
        printf("Player 1 name: ");
        scanf("%49s", g.p1Name);
        printf("Player 2 name: ");
        scanf("%49s", g.p2Name);
        printf("Board size (8 or 10): ");
        if (scanf("%d", &g.size) != 1 || (g.size != 8 && g.size != 10))
        {
            printf("Invalid size.\n");
            return EXIT_FAILURE;
        }

        clearBuffer();

        printf("Save filename (Without .txt): ");
        scanf("%99s", g.saveFile);

        clearBuffer();

        addTxtExt(g.saveFile);
        initializeBoard(&g);
        drawBoard(&g);

        if (!saveGame(&g))
        {
            printf("Save failed.\n");
            return EXIT_FAILURE;
        }
    }

    else
    {
        printf("Load filename: ");
        scanf("%99s", g.saveFile);
        clearBuffer();
        addTxtExt(g.saveFile);

        if (!loadGame(&g, g.saveFile))
        {
            printf("Load failed.\n");
            return EXIT_FAILURE;
        }

        drawBoard(&g);
    }

    //zaidimo ciklas
    while (1)
    {
        if (!hasValidMoves(&g))
        {
            if (g.currentPlayer == 1)
            {
                printf("%s has no valid moves. %s wins!\n", g.p1Name, g.p2Name);
            }
            else
            {
                printf("%s has no valid moves. %s wins!\n", g.p2Name, g.p1Name);
            }
            break;
        }

        if (g.currentPlayer == 1)
        {
            printf("%s's turn (%c). Move(f.e. A3-B4) (To exit write 'exit'):", g.p1Name, P1);
        }
        else
        {
            printf("%s's turn (%c). Move(f.e. A3-B4) (To exit write 'exit'):", g.p2Name, P2);
        }

        char input[MOVE_LEN];

        if (scanf("%9s", input) != 1)
        {
            printf("Invalid input.\n");
            clearBuffer();
            continue;
        }

        if (strcasecmp(input, "exit") == 0)
        {
            printf("Goodbye!\n");
            break;
        }

        clearBuffer();

        char sc, ec;
        int sr, er;

        if (!parseMove(input, &sc, &sr, &ec, &er))
        {
            printf("Invalid format. Use C3-D4.\n");
            continue;
        }

        sc = toupper(sc);
        ec = toupper(ec);

        if (sc < 'A' || sc >= 'A' + g.size || ec < 'A' || ec >= 'A' + g.size || sr < 1 || sr > g.size || er < 1 || er > g.size)
        {
            printf("Out of bounds.\n");
            continue;
        }

        if (makeMove(&g, sc, sr, ec, er))
        {
            system(CLEAR_CMD);
            drawBoard(&g);
            if (!appendMove(&g, input))
            {
                printf("Failed to save move.\n");
            }
            switchPlayer(&g);

        }
        else
        {
            printf("Invalid move.\n");
        }
    }
    return 0;
}
