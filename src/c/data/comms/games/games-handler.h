#include "../../model/models.h"
#include "../comms.h"

#ifndef GAMES_HANDLER_H_

//! Function signature for the callback on a successful request for games
//! @param reason The reason for the error
typedef void (*GameListErrorCallback)(AppError error);


//! Function signature for the callback on a successful request for games
//! @param sport The sport that was requested
//! @param games The array of games returned for the sport
typedef void (*GameListSuccessCallback)(int game_count, Game **games);

//! Function signature for the callback on a game update, success or failure.
//! Only returns the result of the update since any actual data update should be propagated through pointers.
//! @param state The result of the update
typedef void (*GameUpdateCallback)(GameUpdateResult state);

#endif


#define GAMES_HANDLER_H_

void handle_request_games(Sport sport, GameListSuccessCallback on_success, GameListErrorCallback on_error);
void handle_clear_games();
void handle_games_recieved(DictionaryIterator *iter);
void update_game(Game *game, GameUpdateCallback on_update);
void handle_game_update_recieved(DictionaryIterator *iter);