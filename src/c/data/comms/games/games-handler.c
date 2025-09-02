#include "pebble.h"
#include "../comms.h"
#include "games-handler.h"
#include "../../model/models.h"

static int current_request = -1;
static int games_count;
static Game **games;

static GameListSuccessCallback on_games_success;
static GameListErrorCallback on_games_error;

static GameUpdateCallback on_game_update;

 static void request_local(Sport sport) {
    switch (sport) {
    case SportNFL:
        printf("getting games for NFL");
        DictionaryIterator *game1;
        AppMessageResult result1 = app_message_outbox_begin(&game1);
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_REQUEST_ID, current_request));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_LIST, GamesListLastItem));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_ID, 123));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_SPORT, SportNFL));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_TEAM_1_ID, 1));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_TEAM_1_NAME, "DEN"));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_TEAM_1_SCORE, "17"));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_TEAM_1_RECORD, "5-11"));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_TEAM_1_FAVORITE, true));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_TEAM_2_ID, 2));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_TEAM_2_NAME, "NYJ"));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_TEAM_2_SCORE, "10"));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_TEAM_2_RECORD, "2-14"));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_TEAM_2_FAVORITE, false));
        dict_write_tuplet(game1, &TupletInteger(MESSAGE_KEY_SEND_GAME_POSSESSION, Team1));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_TIME, "13:42 4th"));
        dict_write_tuplet(game1, &TupletCString(MESSAGE_KEY_SEND_GAME_DETAILS, "3rd & 8 - DEN 37"));
        handle_games_recieved(game1);
        break;
    default:
        printf("getting games for other");
        DictionaryIterator *game;
        AppMessageResult result = app_message_outbox_begin(&game);
        dict_write_tuplet(game, &TupletInteger(MESSAGE_KEY_REQUEST_ID, current_request));
        dict_write_tuplet(game, &TupletInteger(MESSAGE_KEY_SEND_GAME_LIST, GamesListNoGames));
        handle_games_recieved(game);
        break;
    }
} 

void handle_request_games(Sport sport, GameListSuccessCallback on_success, GameListErrorCallback on_error) {

    if(games != NULL) {
        on_success(games_count, games);
    }


    // generate a random id for the request so that if two requests happen at once, only the latest is loaded into memory
    int request_id = rand();

    printf("getting games for %s", sport_get_name(sport));
/*     current_request = request_id;
    on_games_success = on_success;
    on_games_error = on_error;
    request_local(sport); */

    DictionaryIterator *out_iter;
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    if(result == APP_MSG_OK) { // Construct the message
        // Add an item to ask for the sport
        Tuplet update_game_tuple = TupletInteger(MESSAGE_KEY_LOAD_GAMES, sport);
        dict_write_tuplet(out_iter, &update_game_tuple);

        // Add an item with the request id
        Tuplet request_id_tuple = TupletInteger(MESSAGE_KEY_REQUEST_ID, request_id);
        dict_write_tuplet(out_iter, &request_id_tuple);

        // Send this message
        result = app_message_outbox_send();

        if(result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
            on_error(ConnectionError);
        } else {
            current_request = request_id;
            on_games_success = on_success;
            on_games_error = on_error;
        }
        
    } else {
        // The outbox cannot be used right now
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
        on_error(ConnectionError);
    }
}

void update_game(Game *game, GameUpdateCallback on_update) {
    // generate a random id for the request so that if two requests happen at once, only the latest is loaded into memory
    int request_id = rand();

    DictionaryIterator *out_iter;
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating game %s - %s, id = %d", game->team1.name, game->team2.name, game->id);

    if(result == APP_MSG_OK) { // Construct the message
        // Add an item to ask for the sport
        Tuplet update_game_id_tuple = TupletInteger(MESSAGE_KEY_UPDATE_GAME_ID, game->id);
        Tuplet update_game_sport_tuple = TupletInteger(MESSAGE_KEY_UPDATE_GAME_SPORT, game->sport);
        dict_write_tuplet(out_iter, &update_game_id_tuple);
        dict_write_tuplet(out_iter, &update_game_sport_tuple);

        // Add an item with the request id
        Tuplet request_id_tuple = TupletInteger(MESSAGE_KEY_REQUEST_ID, request_id);
        dict_write_tuplet(out_iter, &request_id_tuple);

        // Send this message
        result = app_message_outbox_send();

        if(result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
            on_update(GameUpdateNetworkError);
        } else {
            current_request = request_id;
            on_game_update = on_update;
        }
        
    } else {
        // The outbox cannot be used right now
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
        on_update(GameUpdateNetworkError);
    }
}

static void clear_game(Game *game) {
    free(game->team1.name);
    free(game->team1.score);
    free(game->team1.record);
    free(game->team2.name);
    free(game->team2.score);
    free(game->team2.record);
    free(game->time);
    free(game->summary);
    free(game->details);
    free(game);
}

void handle_clear_games() {
    current_request = -1;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "clearing %d current games", games_count);
    for (int i = 0; i < games_count; i++)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "freeing game %i", i);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%i].sport = %i", i, games[i]->sport);
        Game *game = games[i];
        APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%d].team1.name = %s", i, game->team1.name);
        clear_game(game);
    } 
    
    free(games);
    games = NULL;
    on_games_success = NULL;
    on_games_error = NULL;
    games_count = 0;
}

static char *memorize_dict_string(const DictionaryIterator *dict, uint32_t key) {
    Tuple *tuple = dict_find(dict, key);
    int len = strlen(tuple->value->cstring);
    char *str = malloc(len + 1);
    strcpy(str, tuple->value->cstring);
    return str;
}

// From an AppMessage DictionaryIterator, set values in an already malloc-ed Game pointer
void game_set(Game *game, DictionaryIterator *iter) {
    int game_id = dict_find(iter, MESSAGE_KEY_SEND_GAME_ID)->value->int32;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "recieved games_tuple");
    int sport = dict_find(iter, MESSAGE_KEY_SEND_GAME_SPORT)->value->int8;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "sport_tuple = %d", sport);


    char *team_1_name = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_NAME);
    char *team_2_name = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_NAME);

    int team_1_id = dict_find(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_ID)->value->int8;
    int team_2_id = dict_find(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_ID)->value->int8;

    char *team_1_score = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_SCORE);
    char *team_2_score = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_SCORE);
    
    char *team_1_record = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_RECORD);
    char *team_2_record = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_RECORD);

    bool team_1_favorite = dict_find(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_FAVORITE)->value->int8;
    bool team_2_favorite = dict_find(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_FAVORITE)->value->int8;


    int possession = dict_find(iter, MESSAGE_KEY_SEND_GAME_POSSESSION)->value->int8;

    char *time = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TIME);
    char *details = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_DETAILS);

    char *summary = malloc(strlen(team_1_name) + strlen(team_1_score) + strlen(team_2_name) + strlen(team_2_score) + 6);
    strcpy(summary, team_1_name);
    strcat(summary, " ");
    strcat(summary, team_1_score);
    strcat(summary, " - ");
    strcat(summary, team_2_score);
    strcat(summary, " ");
    strcat(summary, team_2_name);

    game->id = game_id;
    game->sport = sport;
    game->team1 = (Team) {
        .name = team_1_name,
        .score = team_1_score,
        .id = team_1_id,
        .favorite = team_1_favorite,
        .record = team_1_record,
    };
    game->team2 = (Team) {
        .name = team_2_name,
        .score = team_2_score,
        .id = team_2_id,
        .favorite = team_2_favorite,
        .record = team_2_record,
    };
    game->possession = possession;
    game->time = time;
    game->details = details;
    game->summary = summary;
}

void handle_games_recieved(DictionaryIterator *iter) {
    int request_id = dict_find(iter, MESSAGE_KEY_REQUEST_ID)->value->int32;

    // if phone is sending games from a request the user stopped viewing, discard them 
    if (request_id != current_request){ 
        return;
    }

    GamesListState data = dict_find(iter, MESSAGE_KEY_SEND_GAME_LIST)->value->int8;

    // Handle edge cases. If there was an error connecting to the API or the API returns no games for a sport, use the corresponding error callback
    if (data == GamesListNoGames) {
        on_games_error(NoGames);
        return;
    }
    if (data == GamesListNetworkError) {
        on_games_error(NetworkError);
        return;
    }

    games = realloc(games, (games_count + 1) * sizeof(Game*));
    games[games_count] = malloc(sizeof(Game));

    game_set(games[games_count], iter);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "after game creation");
    APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%d]->team1.name = %s", games_count, games[games_count]->team1.name);
    games_count++;

    if (data == GamesListLastItem) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "last game, running on_success");
        on_games_success(games_count, games);
    }
}

void handle_game_update_recieved(DictionaryIterator *iter) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "handling game update");

    GameUpdateResult data = dict_find(iter, MESSAGE_KEY_SEND_GAME_UPDATE)->value->int8;

    if (data == GameUpdateNetworkError) {
        on_game_update(GameUpdateNetworkError);
        return;
    }

    int game_id = dict_find(iter, MESSAGE_KEY_SEND_GAME_ID)->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "handling game update, id = %d", game_id);


    for (int i = 0; i < games_count; i++) {
        if (games[i]->id == game_id) {
            clear_game(games[i]);
            games[i] = malloc(sizeof(Game));
            game_set(games[i], iter);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "handling game update, calling callback");
            on_game_update(GameUpdated);
        }
    }
}