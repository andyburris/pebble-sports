#include "pebble.h"
#include "./models.h"
#include "action-menu.h"

static ActionMenu *s_action_menu;
static ActionMenuLevel *s_level;
static ActionMenuConfig config;
static ActionMenuLabels s_labels;
static Game s_game;

typedef enum  {
    GAME, TEAM_1, TEAM_2
} MenuAction;

static void action_click_callback(ActionMenu *menu, const ActionMenuItem *performed_action, void *context) {
    MenuAction action = *(MenuAction *)action_menu_item_get_action_data(performed_action);
    ActionMenuLabels *labels = (ActionMenuLabels *) context;
    Game *game = labels->game;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "clicked on action, game.sport = %d", game->sport);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "clicked on action, game.team1 = %s", game->team1);
}

static void action_menu_close_callback (ActionMenu *menu, const ActionMenuItem *performed_action, void *context) {
    ActionMenuLabels *labels = (ActionMenuLabels *) context;
    free(labels->team_1_label);
    free(labels->team_2_label);
}

static char *create_label(char *team_name) {
    char *label = malloc(strlen(team_name) + 17);
    strcpy(label, "Add ");
    strcat(label, team_name);
    strcat(label, " to Timeline");
    return label;
}

void game_action_menu_open(Game game) {
    s_level = action_menu_level_create(3);
    s_game = game;

    char *team_1_label = create_label(s_game.team1);
    char *team_2_label = create_label(s_game.team2);
    
    s_labels = (ActionMenuLabels) {
        .game = &s_game,
        .team_1_label = team_1_label,
        .team_2_label = team_2_label
    };

    MenuAction add_game = GAME;
    MenuAction team_1 = TEAM_1;
    MenuAction team_2 = TEAM_2;
    ActionMenuItem *game_item = action_menu_level_add_action(s_level, "Add Game to Timeline", action_click_callback, &add_game);
    ActionMenuItem *team_1_item = action_menu_level_add_action(s_level, s_labels.team_1_label, action_click_callback, &team_1);
    ActionMenuItem *team_2_item = action_menu_level_add_action(s_level, s_labels.team_2_label, action_click_callback, &team_2);

    config = (ActionMenuConfig) {
        .root_level = s_level,
        .colors = {
            .background = GColorElectricUltramarine,
            .foreground = GColorWhite
        },
        .context = &s_labels,
        .will_close = action_menu_close_callback
    };

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "team_1_label = %s", ((ActionMenuLabels *)config.context)->team_1_label);
    s_action_menu = action_menu_open(&config);
}