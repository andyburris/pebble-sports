#include <stdbool.h>
#include "pebble.h"
#include "../../../data/model/models.h"
#include "../../../data/comms/favorites/favorites-handler.h"
#include "../../../data/comms/games/games-handler.h"
#include "action-menu.h"
#include "result-window.h"

static ActionMenu *s_action_menu;
static ActionMenuLevel *s_level;
static ActionMenuConfig config;
static ActionMenuLabels s_labels;
static Game *s_game;
static Window *s_result_window;
static MenuAction current_action;

static MenuAction refresh_game = ACTION_REFRESH_GAME;
static MenuAction team_1 = ACTION_TEAM_1;
static MenuAction team_2 = ACTION_TEAM_2;

static void favorite_change_callback(int team_id, FavoriteChangeResult result) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "favorite changed callback, result = %d", result);
    if(s_result_window) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "destroying window");
        window_destroy(s_result_window);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "window destroyed");
    }
    s_result_window = result_window_create_favorite(s_game, current_action, result);
    action_menu_set_result_window(s_action_menu, s_result_window);
    action_menu_unfreeze(s_action_menu);
    action_menu_close(s_action_menu, true);
}

static void game_update_callback(GameUpdateResult state) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "game update callback, result = %d", state);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "game update callback, game pointer = %p", s_game);
    if (state == GameUpdated) {
        action_menu_unfreeze(s_action_menu);
        action_menu_close(s_action_menu, true);
    } else {
        s_result_window = result_window_create_refresh(s_game, state);
        action_menu_set_result_window(s_action_menu, s_result_window);
        action_menu_unfreeze(s_action_menu);
        action_menu_close(s_action_menu, true);
    }
    
}

static void action_click_callback(ActionMenu *menu, const ActionMenuItem *performed_action, void *context) {
    action_menu_freeze(s_action_menu);
    MenuAction action = *(MenuAction *)action_menu_item_get_action_data(performed_action);
    current_action = action;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "action click callback, result = %d", action);
    switch (action) {
        case ACTION_REFRESH_GAME: 
            update_game(s_game, game_update_callback);
            break;
        case ACTION_TEAM_1: 
            handle_request_change_favorite(s_game, s_game->team1.id, favorite_change_callback);
            break;
        case ACTION_TEAM_2: 
            handle_request_change_favorite(s_game, s_game->team2.id, favorite_change_callback);
            break;
        default:
            action_menu_unfreeze(s_action_menu);
            break;
    }
}

static void action_menu_close_callback (ActionMenu *menu, const ActionMenuItem *performed_action, void *context) {
    ActionMenuLabels *labels = (ActionMenuLabels *) context;
    free(labels->team_1_label);
    free(labels->team_2_label);
}

static char *create_label(char *team_name, bool is_favorite) {
    char *before = "Add ";
    char *after = " to Favorites";
    if(is_favorite) {
        before = "Remove ";
        after = " from Favorites";
    }
    char *label = malloc(strlen(team_name) + strlen(before) + strlen(after) + 1);
    strcpy(label, before);
    strcat(label, team_name);
    strcat(label, after);
    return label;
}

void game_action_menu_open(Game *game) {

    s_level = action_menu_level_create(3);
    s_game = game;

    printf("creating label for team 1");
    printf("creating label for team 1, name = %s", s_game->team1.name);
    char *team_1_label = create_label(s_game->team1.name, s_game->team1.favorite);
    char *team_2_label = create_label(s_game->team2.name, s_game->team2.favorite);
    
    s_labels = (ActionMenuLabels) {
        .game = s_game,
        .team_1_label = team_1_label,
        .team_2_label = team_2_label
    };

    ActionMenuItem *game_item = action_menu_level_add_action(s_level, "Refresh", action_click_callback, &refresh_game);
    ActionMenuItem *team_1_item = action_menu_level_add_action(s_level, s_labels.team_1_label, action_click_callback, &team_1);
    ActionMenuItem *team_2_item = action_menu_level_add_action(s_level, s_labels.team_2_label, action_click_callback, &team_2);

    MenuAction action = *(MenuAction *)action_menu_item_get_action_data(team_1_item);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "action click callback, result = %d", action);

    config = (ActionMenuConfig) {
        .root_level = s_level,
        .colors = {
            .background = GColorDukeBlue,
            .foreground = GColorWhite
        },
        .context = &s_labels,
        .will_close = action_menu_close_callback
    };

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "team_1_label = %s", ((ActionMenuLabels *)config.context)->team_1_label);
    s_action_menu = action_menu_open(&config);
}