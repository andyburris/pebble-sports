#include <stdlib.h>
#include "pebble.h"
#include "../../model/models.h"
#include "favorites-handler.h"


static FavoriteChangeCallback s_callback;
static Game *s_favoriting_game;
static int s_favoriting_team_id = -1;
static int current_request = -1;

void handle_request_change_favorite(Game *favoriting_game, int team_id, FavoriteChangeCallback callback) {
    // generate a random id for the request so that if two requests happen at once, only the latest is loaded into memory
    int request_id = rand();

    DictionaryIterator *out_iter;
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    if(result == APP_MSG_OK) { // Construct the message
        // Add an item to specify the sport
        Tuplet favorite_sport_tuple = TupletInteger(MESSAGE_KEY_ADD_FAVORITE_SPORT, favoriting_game->sport);
        dict_write_tuplet(out_iter, &favorite_sport_tuple);

        // Add an item to specify the id of the team being favorited
        Tuplet favorite_team_id_tuple = TupletInteger(MESSAGE_KEY_ADD_FAVORITE_TEAM_ID, team_id);
        dict_write_tuplet(out_iter, &favorite_team_id_tuple);

        // Add an item with the request id
        Tuplet request_id_tuple = TupletInteger(MESSAGE_KEY_REQUEST_ID, request_id);
        dict_write_tuplet(out_iter, &request_id_tuple);

        // Send this message
        result = app_message_outbox_send();

        if(result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
            callback(team_id, FavoriteChangeFailed);
        } else {
            current_request = request_id;
            s_favoriting_game = favoriting_game;
            s_favoriting_team_id = team_id;
            s_callback = callback;
        }
        
    } else {
        // The outbox cannot be used right now
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
        callback(team_id, FavoriteChangeFailed);
    }
}

void handle_favorite_change_result(DictionaryIterator *iter) {

    int request_id = dict_find(iter, MESSAGE_KEY_REQUEST_ID)->value->int32;

    // if phone is sending favorites from a request the user stopped viewing, discard it 
    if (request_id != current_request){ return; }

    Tuple *favorite_tuple = dict_find(iter, MESSAGE_KEY_CONFIRM_FAVORITE);
    FavoriteChangeResult result = favorite_tuple->value->int8;

    if (result != FavoriteChangeFailed){
        bool is_favorite = result == FavoriteAdded;
        if (s_favoriting_team_id == s_favoriting_game->team1.id) {
            s_favoriting_game->team1.favorite = is_favorite;
        }
        if (s_favoriting_team_id == s_favoriting_game->team2.id) {
            s_favoriting_game->team2.favorite = is_favorite;
        }
    }

    
    s_callback(s_favoriting_team_id, result);

    s_callback = NULL;
    s_favoriting_game = NULL;
    s_favoriting_team_id = -1;
    current_request = -1;
}