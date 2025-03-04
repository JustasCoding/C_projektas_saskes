#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "GameState.h"

//Prideda .txt prie failo pavadinimo, jeigu jo nera
void addTxtExt(char *filename)
{
    size_t len = strlen(filename); // len pavadinimo ilgis
    if(len < 4 || strcmp(filename + len - 4, ".txt") != 0) // jeigu failo pavadinimas trumpesnis uz 4 arba neturi .txt
    {
        if(len + 4 < FILE_LEN) // jeigu failo pavadinimas + 4 yra maziau uz 100
        {
            strcat(filename, ".txt"); // prideda .txt
        }
        else
        {
            filename[FILE_LEN - 5] = '\0'; // Jeigu yra ilgesnis pavadinimas uz 100, atimama 5 pozicijos nuo 100 ir irasoma .txt
            strcat(filename, ".txt");
        }
    }
}

// Isvalo bufferi, jog po nuskaitymo neliktu EOF arba endline ir nuskaitant nepaimtu ko nereik
void clearBuffer()
{
    int c;
    while((c = getchar()) != '\n' && c != EOF); // skaito eilute kol nesusiduria su \n arba EOF
}

// Sugeneruoja pradine lenta
void initializeBoard(GameState *g)
{
    for (int r = 0; r < (*g).size; r++) // pereina per visa lenta ir kiekviena laukeli priskiria tuscia reiksme
    {
        for (int c = 0; c < (*g).size; c++)
        {
            (*g).board[r][c] = EMPTY;
        }
    }

    int rows;// kiek eiluciu bus, kuriose yra saskiu

    if ((*g).size == 8) // jeigu bus irasyta 8, tai bus 3 eilutes, kuriose bus saskes
    {
        rows = 3;
    }
    else
    {
        rows = 4;
    }

    for (int r = 0; r < rows; r++)// VIRSUS antro zaidejo saskiu isdestymas
    {
        for (int c = 0; c < (*g).size; c++)
        {
            if ((r + c) % 2) // jeigu eilutes ir stulpelio indeksu suma yra nelyginis tai priskiria ji P2(ant tamsiu)
            {
                (*g).board[r][c] = P2;
            }
        }
    }

    for (int r = (*g).size - rows; r < (*g).size; r++)// APACIA pirmo zaideju saskiu pozicijos isdestymas. is lentos dydzio atimame eilutes ir gauname, kiek reikia uzpildyti pozicu
    {
        for (int c = 0; c < (*g).size; c++)
        {
            if ((r + c) % 2)
            {
                (*g).board[r][c] = P1;//jeigu eilutes ir stulpelio indeksu suma yra nelyginis tai priskiria ji P1(ant tamsiu)
            }
        }
    }

    (*g).currentPlayer = 1; // kuris zaidejas pradeda partija.
}

// Nupiesia lenta
void drawBoard(const GameState *g)
{
    // RAIDES
    printf("   ");// cia lygiavimui palikta
    for (int c = 0; c < (*g).size; c++)//
    {
        printf(" %c  ", 'A' + c);//pavercia stupelio indeksa i raide
    }
    printf("\n");

    // KONTURAS
    for (int r = 0; r < (*g).size; r++)
    {
        // Konturas
        printf("  +");
        for (int c = 0; c < (*g).size; c++)
        {
            printf("---+");
        }
        printf("\n");

        //NUMERIS
        printf("%2d|", (*g).size - r);//suformatuoja skaicius plociu 2, sulygiuota desineje puseje bus
        for (int c = 0; c < (*g).size; c++)
        {
            char ch; // koks simbolis bus nustato
            if ((r + c) % 2 == 0)// jeigu stulpelio ir eilutes indeksu suma yra lygine, tai jis yra sviesus. ir bus tuscia
            {
                ch = ' ';
            }
            else
            {
                ch = (*g).board[r][c];// priskiria figura
            }

            printf(" %c |", ch);// isprintina simboli ir prideda bruksni
        }

        printf("%d\n", (*g).size - r);//isprintina skaiciu kitoje puseje ir eina i kita eilute
    }

    // Apatinis konturas
    printf("  +");
    for (int c = 0; c < (*g).size; c++)
    {
        printf("---+");
    }
    printf("\n");

    // Apatines raides
    printf("   ");
    for (int c = 0; c < (*g).size; c++)
    {
        printf(" %c  ", 'A' + c);
    }
    printf("\n");
}

// Zaideju seka tikrina
void switchPlayer(GameState *g)
{
    if ((*g).currentPlayer == 1)
    {
        (*g).currentPlayer = 2;
    }
    else
    {
        (*g).currentPlayer = 1;
    }
}

// Isskaido ejimus, ka ivede
int parseMove(const char *input, char *sc, int *sr, char *ec, int *er)
{
	return sscanf(input, "%c%d-%c%d", sc, sr, ec, er) == 4;
}


//Patikrina ar ejimas yra galimas
int isValidMove(const GameState *g, int sr, int sc, int er, int ec)
{
    //Patikrina, ar koordinates nevirsija lentos ribu
    if (sr < 0 || sr >= (*g).size || sc < 0 || sc >= (*g).size ||
        er < 0 || er >= (*g).size || ec < 0 || ec >= (*g).size)
    {
        return 0;
    }

    //Patikrina, ar startini langeli uzima butent to zaidëjo figura
    char p = (*g).board[sr][sc];
    if (((*g).currentPlayer == 1 && (p != P1 && p != P1_K)) ||
        ((*g).currentPlayer == 2 && (p != P2 && p != P2_K)))
    {
        return 0;
    }

    //Tikslinis langelis turi bûti tuscias
    if ((*g).board[er][ec] != EMPTY)
    {
        return 0;
    }


    int dr = er - sr, dc = ec - sc;
    if (abs(dr) != abs(dc))
        return 0;


    //Paprastu figuru judejimo kryptis: P1 tik i virsø, P2 tik zemyn
    if (((*g).currentPlayer == 1 && p == P1 && dr >= 0) ||
        ((*g).currentPlayer == 2 && p == P2 && dr <= 0))
    {
        return 0;
    }


    if (abs(dr) == 1)
        return 1;


    if (abs(dr) == 2)
    {
        int mr = sr + dr / 2, mc = sc + dc / 2; //vidurinis langelis, per kuri ðoka
        char mid = (*g).board[mr][mc];
        //kirtimas galimas, jei ten yra priesininko figura (paprasta arba dama)
        if (((*g).currentPlayer == 1 && (mid == P2 || mid == P2_K)) ||
            ((*g).currentPlayer == 2 && (mid == P1 || mid == P1_K)))
        {
            return 1;
        }
    }

    return 0;
}


//Saskes pajudinimas
int makeMove(GameState *g, char sc, int sr, char ec, int er)
{//Sulygiuoti masyvo indeksus su ivestimi
    int sRow = (*g).size - sr;//start Row=size - startrownumb
    int sCol = sc - 'A';// start Collumn= Start Collumn char - A
    int eRow = (*g).size - er;// end Row= size= endRownumb
    int eCol = ec - 'A';//end Collumn = end CollumnChar - A

    if (!isValidMove(g, sRow, sCol, eRow, eCol))// patikrina ar galimas ejimas
    {
        return 0;
    }

    char p = (*g).board[sRow][sCol]; // pasiima figura, kuri buvo pradiniame laukelyje
    (*g).board[eRow][eCol] = p;// naujas langelis gauna ta figura
    (*g).board[sRow][sCol] = EMPTY;//buves laukelis tampa tuscias

    if (abs(eRow - sRow) == 2)//jeigu suolis yra 2, tai reiskias isvyko kirtimas
    {
        (*g).board[(sRow + eRow) / 2][(sCol + eCol) / 2] = EMPTY;// tada vidurine reiksme yra nukirsta saske ir ta pozicija yra uzpildoma empty
    }

    //VIRTIMAS DAMA
    if (((*g).currentPlayer == 1 && eRow == 0 && p == P1) ||
        ((*g).currentPlayer == 2 && eRow == (*g).size - 1 && p == P2)) // dama tampama kai P1 pasiekia virsutine eile 0, o P2 tampa, kai is lentos dydzio atimamas 1
    {
        if ((*g).currentPlayer == 1)
        {
            (*g).board[eRow][eCol] = P1_K;
            printf("%s is promoted to QUEEN!\n", (*g).p1Name); // kai tampa dama, tai yra parasoma, jog pakelta i dama yra
        }
        else
        {
            (*g).board[eRow][eCol] = P2_K;
            printf("%s is promoted to QUEEN!\n", (*g).p2Name);
        }
    }
    return 1;
}

// Ar zaidejas turi legaliu ejimu
int hasValidMoves(const GameState *g)
{
    for (int r = 0; r < (*g).size; r++)
    {
        for (int c = 0; c < (*g).size; c++)
        {
            char p = (*g).board[r][c];
            int current = (*g).currentPlayer; // surenka visas saskiu pozicijas, kurios priklauso tam zaidejui
            if ((current == 1 && (p == P1 || p == P1_K)) ||
                (current == 2 && (p == P2 || p == P2_K))) // jeigu tame laukelyje yra dabrtinio zaidejo saske tai ieskomi galimi ejimai
            {
                // KAIP SASKE GALI JUDETI
                int dirs[8][2]; // galimu judejimo krypciu vektoriai
                int nd = 0;// galimu krypciu kiekis

                if (p == P1 || p == P1_K) // jeigu pasirinkta P1 saske  yra paprasta arba dama, tai gali judeti tik pirmyn istrizai
                {
                    dirs[nd][0] = -1; dirs[nd++][1] = -1;// paprasta figura pajuda vienu  i virsu(mazinam indeksa) ir vienu i kaire arba i desine
                    dirs[nd][0] = -1; dirs[nd++][1] = 1;
                    if (p == P1_K)// jeigu yra karalius, tai dar pridedam galimybe eiti atgal
                    {
                        dirs[nd][0] = 1; dirs[nd++][1] = -1; // dama gali judeti dar ir vienu zemyn(didinam indeksa) ir i kaire arba i desine
                        dirs[nd][0] = 1; dirs[nd++][1] = 1;
                    }
                }

                if (p == P2 || p == P2_K) // tas pats kaip ir su P1, tik kai saske eina i prieki tai indeksa didinam, o kai tgal tai mazinam
                {
                    dirs[nd][0] = 1;  dirs[nd++][1] = -1;
                    dirs[nd][0] = 1;  dirs[nd++][1] = 1;
                    if (p == P2_K)
                    {
                        dirs[nd][0] = -1; dirs[nd++][1] = -1;
                        dirs[nd][0] = -1; dirs[nd++][1] = 1;
                    }
                }

                //Tikrinamos visos galimos kryptis
                for (int i = 0; i < nd; i++)//eina per krypciu masyva
                {
                    int nr = r + dirs[i][0]; //naujas eilutes indeksas= eilute + krypties poslinkis
                    int nc = c + dirs[i][1];//naujas stulpelio indeksas= stulpelio + krypties poslinkis
                    if (isValidMove(g, r, c, nr, nc))//patikrina ar ejimas yra galimas
                    {
                        return 1;
                    }

                    //Patikriname kirtima
                    if (abs(dirs[i][0]) == 2 && abs(dirs[i][1]) == 2) //jeigu suolis yra 2(kirtimas)
                    {
                        int mr = r + dirs[i][0]/2;//vidurio koordinaciu radimas
                        int mc = c + dirs[i][1]/2;
                        char mid = (*g).board[mr][mc];
                        if ((current == 1 && (mid == P2 || mid == P2_K)) ||
                            (current == 2 && (mid == P1 || mid == P1_K)))//patikrinam ar cia buvo P1 ar P2 saske arba dama
                        {
                            if ((*g).board[nr][nc] == EMPTY)//tikriname ar laukelis i kuri norime nusokti yra tuscias
                            {
                                return 1; //jeigu abi salygos yra tenkinamos, reiskias kirtimas yra galimas
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// // Apraso zaideju informacija i faila(NE EJIMMU)
int saveGame(const GameState *g)
{
    FILE *f = fopen((*g).saveFile, "w");//atidaro faila
    if (!f)
    {
        perror("Save failed");
        return 0;
    }
    fprintf(f, "Player1: %s\nPlayer2: %s\nBoardSize: %d\nMoves:\n",
            (*g).p1Name, (*g).p2Name, (*g).size);
    fclose(f);

    return 1;
}

//Aprasso i faila zaideju ejimus
int appendMove(const GameState *g, const char *move)
{
    FILE *f = fopen((*g).saveFile, "a");//failas atidarytas papildymui
    if (!f)
    {
        perror("Append failed");
        return 0;
    }

    fprintf(f, "%s\n", move);
    fclose(f);

    return 1;
}

// Uzkrauna issaugota zaidima
int loadGame(GameState *g, const char *filename)
{
    FILE *f = fopen(filename, "r");// atidaro faila skaitymui
    if (!f)
    {
        perror("Load failed");
        return 0;
    }

    char line[256], move[MOVE_LEN];
    // Perskaito pirmo zaidejo ejima
    if (fgets(line, sizeof(line), f) && sscanf(line, "Player1: %s", (*g).p1Name) != 1)//tikrina ar eilute atitinka sablona Player1:
    {
        printf("Nepavyko nuskaityti failo.\n");
        fclose(f);
        return 0;
    }

    // Perskaito antro zaidejo ejima
    if (fgets(line, sizeof(line), f) && sscanf(line, "Player2: %s", (*g).p2Name) != 1)
    {
        printf("Nepavyko nuskaityti failo.\n");
        fclose(f);
        return 0;
    }

    // Perskaito lentos dydi
    if (fgets(line, sizeof(line), f) && sscanf(line, "BoardSize: %d", &(*g).size) != 1)
    {
        printf("Nepavyko nuskaityti lentos dydzio.\n");
        fclose(f);
        return 0;
    }
    // Ar yra ejimu
    if (fgets(line, sizeof(line), f) == NULL || strncmp(line, "Moves:", 6) != 0)//jeigu pirmi sesi simboliai nesutampa su Moves: reiskias nera ejimu saraso
    {
        printf("Nero tokiu ejimu.\n");
        fclose(f);
        return 0;
    }

    initializeBoard(g);//Iskviecia funkcija, kuri sukuria lenta ir uzpildo ja pradine busena
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < MAX_MOVES)// skaito failo kiekviena eilute tol kol pasiekia MAX_Moves(1000) arba kol baigsis failas
    {
        if (sscanf(line, "%s", move) != 1)// jeigu nuskaicus negaunama ejimo reprezentacija. tai yra tiesiog skaitoma toliau
        {
            continue;
        }

        char sc, ec;// start column, end collumn
        int sr, er;//start row, end row

        if (!parseMove(move, &sc, &sr, &ec, &er))// Kintamiesiems suteikia reiksmes is uzrasymo A3-B4 nutu sc=A sr = 3 ec=B er=4
        {
            printf("Netinkamas ejimo formatas: %s\n", move);
            fclose(f);
            return 0;
        }

        sc = toupper((unsigned char)sc);//pradine stulpelio raide pavercia didziaja raide
        ec = toupper((unsigned char)ec);//galine stulpelio raide pavercia didziaja raide

        if (sc < 'A' || sc >= 'A' + (*g).size ||
            ec < 'A' || ec >= 'A' + (*g).size ||
            sr < 1 || sr > (*g).size ||
            er < 1 || er > (*g).size)// tikriname ribas, ar raide nera per maza arba per didele.
        {
            printf("Ejimas virsija lentos ribas: %s\n", move);
            fclose(f);
            return 0;
        }

        if (!makeMove(g, sc, sr, ec, er)) // Iskvieciama ejimo funkcija
        {
            printf("Negalimas ejimas: %s\n", move);
            fclose(f);
            return 0;
        }
        switchPlayer(g);
        count++;
    }

    fclose(f);

    // Nustato, kurio zaidejo eile yra dabar eiti
    if (count % 2 == 0)// jiegu nuskaitytu ejimu kiekis yra lyginis skaicius, reiskias pirmo zaidejo eile, jeigu nelyginis tai antro
    {
        (*g).currentPlayer = 1;
    }
    else
    {
        (*g).currentPlayer = 2;
    }

    return 1;
}
