#ifndef MODELS_H_
#define MODELS_H_

typedef enum {
    DataListItem, DataLastListItem, DataNoGames, DataNetworkError
} MessageData;

typedef enum {
    ConnectionError, NetworkError, NoGames
} AppError;

typedef enum {
    Favorites, SportNFL, SportMLB, SportNHL, SportNBA
} Sport;

typedef enum {
    Team1, Team2, None,
} Possession;

typedef struct Team {
    char *name;
    int id;
} Team;

typedef struct Game
{
    Sport sport;
    char* team1;
    char* score1;
    char* team2;
    char* score2;
    Possession possession;
    char* time;
    char* details;
    char* summary;
} Game;

#endif

int sport_get_icon_res_small(Sport sport);
int sport_get_icon_res_large(Sport sport);
char* sport_get_name(Sport sport);