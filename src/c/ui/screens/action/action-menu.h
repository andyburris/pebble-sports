#include "../../../data/model/models.h"

typedef enum  {
    ACTION_GAME, ACTION_TEAM_1, ACTION_TEAM_2
} MenuAction;

typedef struct {
    Game *game;
    char *team_1_label;
    char *team_2_label;
} ActionMenuLabels;


void game_action_menu_open(Game *game);