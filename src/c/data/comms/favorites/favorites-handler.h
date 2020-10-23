typedef enum {
    FavoriteRemoved, FavoriteAdded, FavoriteChangeFailed
} FavoriteChangeResult;

//! Function signature for the callback on a request to change a favorite
//! @param result The result of the request
typedef void (*FavoriteChangeCallback)(int teamID, FavoriteChangeResult result); 

void handle_request_change_favorite(Game *favoriting_game, int team_id, FavoriteChangeCallback callback);
void handle_favorite_change_result(DictionaryIterator *iter);