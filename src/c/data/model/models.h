#include <stdbool.h>

#ifndef MODELS_H_
#define MODELS_H_

typedef enum {
    GamesListItem, GamesListLastItem, GamesListNoGames, GamesListNetworkError
} GamesListState;

typedef enum {
    GameUpdated, GameUpdateNetworkError
} GameUpdateResult;

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
    char *score;
    char *record;
    int id;
    bool favorite;
} Team;

typedef struct Game
{
    int id;
    Sport sport;
    Team team1;
    Team team2;
    Possession possession;
    char* time;
    char* details;
    char* summary;
} Game;

#endif

int sport_get_icon_res_small(Sport sport);
int sport_get_icon_res_large(Sport sport);
char* sport_get_name(Sport sport);