#include "pebble.h"
#include "../comms.h"
#include "../../model/models.h"

static int current_request = -1;
static int games_count;
static Game **games;

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

void handle_request_games(Sport sport, GamesSuccessCallback on_success, GamesErrorCallback on_error) {

    if(games != NULL) {
        on_success(games_count, games);
    }

    //request_local(sport);

    // generate a random id for the request so that if two requests happen at once, only the latest is loaded into memory
    int request_id = rand();

    printf("getting games for %s", sport_get_name(sport));

    DictionaryIterator *out_iter;
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    if(result == APP_MSG_OK) { // Construct the message
        // Add an item to ask for the sport
        Tuplet load_games_tuple = TupletInteger(MESSAGE_KEY_LOAD_GAMES, sport);
        dict_write_tuplet(out_iter, &load_games_tuple);

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

static void clear_game(Game *game) {
    free(game->team1.name);
    free(game->team1.score);
    free(game->team2.name);
    free(game->team2.score);
    free(game->time);
    free(game->summary);
    free(game->details);
    free(game);
}

void handle_clear_games() {
    current_request = -1;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "clearing current games");
    for (int i = 0; i < games_count; i++)
    {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "freeing game %i", i);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%i].sport = %i", i, games[i]->sport);
        Game *game = games[i];
        APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%d].team1.name = %s", i, game->team1.name);
        clear_game(game);
    } 
    
    //free(games);
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

void handle_games_recieved(DictionaryIterator *iter) {
    int request_id = dict_find(iter, MESSAGE_KEY_REQUEST_ID)->value->int32;

    // if phone is sending games from a request the user stopped viewing, discard them 
    if (request_id != current_request){ return; }

    MessageData data = dict_find(iter, MESSAGE_KEY_SEND_GAME)->value->int8;

    // Handle edge cases. If there was an error connecting to the API or the API returns no games for a sport, use the corresponding error callback
    if (data == DataNoGames) {
        on_games_error(NoGames);
        return;
    }
    if (data == DataNetworkError) {
        on_games_error(NetworkError);
        return;
    }

    int game_id = dict_find(iter, MESSAGE_KEY_SEND_GAME_ID)->value->int8;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "recieved games_tuple");
    int sport = dict_find(iter, MESSAGE_KEY_SEND_GAME_SPORT)->value->int8;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "sport_tuple = %d", sport);


    char *team_1_name = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_NAME);
    char *team_2_name = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_NAME);

    int team_1_id = dict_find(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_ID)->value->int8;
    int team_2_id = dict_find(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_ID)->value->int8;

    char *team_1_score = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_1_SCORE);
    char *team_2_score = memorize_dict_string(iter, MESSAGE_KEY_SEND_GAME_TEAM_2_SCORE);

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

    games = realloc(games, (games_count + 1) * sizeof(Game));
    //games[games_count] = malloc(sizeof(Game));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "after game malloc");

    games[games_count] = malloc(sizeof(Game));
    Game *game = games[games_count];

    game->id = game_id;
    game->sport = sport;
    game->team1 = (Team) {
        .name = team_1_name,
        .score = team_1_score,
        .id = team_1_id,
        .favorite = team_1_favorite
    };
    game->team2 = (Team) {
        .name = team_2_name,
        .score = team_2_score,
        .id = team_2_id,
        .favorite = team_2_favorite
    };
    game->possession = possession;
    game->time = time;
    game->details = details;
    game->summary = summary;


    APP_LOG(APP_LOG_LEVEL_DEBUG, "after game creation");

    APP_LOG(APP_LOG_LEVEL_DEBUG, "games[%d].team1.name = %s", games_count, game->team1.name);
    games_count++;

    if (data == DataLastListItem) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "last game, running on_success");
        on_games_success(games_count, games);
    }
}