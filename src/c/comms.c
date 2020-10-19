#include "pebble.h"
#include "models.h"
#include "comms.h"

typedef enum {
    DataListItem, DataLastListItem, DataNoGames, DataNetworkError
} MessageData;

// Largest expected inbox and outbox message sizes
const uint32_t inbox_size = 256;
const uint32_t outbox_size = 256;

static bool s_js_ready;

static int games_count;
static Sport current_sport = -1;
Game *games;
static GamesSuccessCallback on_games_success;
static GamesErrorCallback on_games_error;

/* static void request_local(Sport sport) {
    switch (sport) {
    case SportNFL:
        printf("getting games for NFL");
        games_count = 2;
        games = malloc(games_count * sizeof(Game));
        games[0] = (Game) {
            .sport = SportNFL, 
            .team1 = "DEN", 
            .score1 = "17", 
            .team2 = "NYJ", 
            .score2 = "10", 
            .possession = Team1, 
            .time = "13:42 4th", 
            .details = "3rd & 8 - DEN 37",
            .summary = "DEN 17 - 10 NYJ",
        };
        games[1] = (Game) {
            .sport = SportNFL, 
            .team1 = "PIT", 
            .score1 = "23", 
            .team2 = "BAL", 
            .score2 = "21", 
            .possession = Team2, 
            .time = "2:01 3rd", 
            .details = "1st & 10 - BAL 25",
            .summary = "PIT 23 - 21 BAL",
        };
        break;
    default:
        printf("getting games for other");
        games_count = 0;
        break;
    }
} */

void request_games(Sport sport, GamesSuccessCallback on_success, GamesErrorCallback on_error) {

    if(games != NULL) {
        on_success(sport, games_count, games);
    }

    //request_local(sport);

    printf("getting games for %s", sport_get_name(sport));
    //on_success(sport, games_count, games);

    DictionaryIterator *out_iter;
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    if(result == APP_MSG_OK) { // Construct the message
        // Add an item to ask for sport
        Tuplet tuple = TupletInteger(MESSAGE_KEY_LOAD_GAMES, sport);
        dict_write_tuplet(out_iter, &tuple);

        // Send this message
        result = app_message_outbox_send();

        if(result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
            on_error(result);
        } else {
            current_sport = sport;
            on_games_success = on_success;
            on_games_error = on_error;
        }
        
    } else {
        // The outbox cannot be used right now
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
        on_error(result);
    }
}

void clear_games() {
    current_sport = -1;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "clearing current games");
    for (int i = 0; i < games_count; i++)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "freeing game %i", i);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%i].sport = %i", i, games[i].sport);
        Game game = games[i];
        APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%d].team1 = %s", i, game.team1);
        free(game.team1);
        free(game.score1);
        free(game.team2);
        free(game.score2);
        free(game.time);
        free(game.summary);
        free(game.details);
    } 
    
    //free(games);
    games = NULL;
    on_games_success = NULL;
    on_games_error = NULL;
    games_count = 0;
}

static char *memorize_dict_string(const DictionaryIterator *dict, uint32_t key) {
    Tuple *tuple = dict_find(dict, key);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "tuple = %s", tuple->value->cstring);
    int len = strlen(tuple->value->cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "tuple length = %d", len);
    char *str = malloc(len + 1);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "after malloc");
    strcpy(str, tuple->value->cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "str = %s", str);
    return str;
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "recieved js message");
    // A new message has been successfully received
    Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_READY);

    // A list of games has been recieved
    Tuple *games_tuple = dict_find(iter, MESSAGE_KEY_SEND_GAME);

    if(ready_tuple) {
        // PebbleKit JS is ready! Safe to send messages
        APP_LOG(APP_LOG_LEVEL_DEBUG, "JS Ready");
        s_js_ready = true;
    }

    if(games_tuple) {

        MessageData data = games_tuple->value->int8;

        APP_LOG(APP_LOG_LEVEL_DEBUG, "recieved games_tuple");
        Tuple *sport_tuple = dict_find(iter, MESSAGE_KEY_SEND_GAME_SPORT);
        int sport = sport_tuple->value->int8;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "sport_tuple = %d", sport);

        // if phone is sending games from a sport the user stopped viewing, discard them 
        if (sport != current_sport){ return; }

        // Handle edge cases. If there was an error connecting to the API or the API returns no games for a sport, use the corresponding callbacks
        if (data == DataNoGames) {
            on_games_error(NoGames);
            return;
        }
        if (data == DataNetworkError) {
            on_games_error(NetworkError);
            return;
        }
    

        char *team_1 = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_1);
        char *team_2 = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_2);
        char *score_1 = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_SCORE_1);
        char *score_2 = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_SCORE_2);

        Tuple *possession_tuple = dict_find(iter, MESSAGE_KEY_SEND_GAME_POSSESSION);
        int possession = possession_tuple->value->int8;

        char *time = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TIME);
        char *details = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_DETAILS);

        char *summary = malloc(strlen(team_1) + strlen(score_1) + strlen(team_2) + strlen(score_2) + 6);
        strcpy(summary, team_1);
        strcat(summary, " ");
        strcat(summary, score_1);
        strcat(summary, " - ");
        strcat(summary, score_2);
        strcat(summary, " ");
        strcat(summary, team_2);

        games = realloc(games, (games_count + 1) * sizeof(Game));
        //games[games_count] = malloc(sizeof(Game));
        APP_LOG(APP_LOG_LEVEL_DEBUG, "after game malloc");

        games[games_count] = (Game) {
            .sport = sport,
            .team1 = team_1,
            .team2 = team_2,
            .score1 = score_1,
            .score2 = score_2,
            .possession = possession,
            .time = time,
            .details = details,
            .summary = summary
        };

        APP_LOG(APP_LOG_LEVEL_DEBUG, "after game creation");

        Game game = games[games_count];
        APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%d].team1 = %s", games_count, game.team1);
        games_count++;

        if (data == DataLastListItem) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "last game, running on_success");
            on_games_success(current_sport, games_count, games);
        }
    }


}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context) {
  // The message just sent has been successfully delivered

}

static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  // The message just sent failed to be delivered
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message send failed. Reason: %d", (int)reason);
}

void setup_comms() {
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_open(inbox_size, outbox_size);
}

void destroy_comms() {
    app_message_deregister_callbacks();
}