#include "../../../data/model/models.h"

#ifndef SCORE_SCREEN_H_
#define SCORE_SCREEN_H_

typedef struct {
    const char *team1;
    const char *score1;
    const char *team2;
    const char *score2;
    Possession possession;
} ScoreData;

#endif

void show_score_screen (const Game game);
void hide_score_screen (void);