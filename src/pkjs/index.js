var models = require('./models');
var storage = require('./storage');
var comms = require('./comms');
var api = require('./api');
var Clay = require('pebble-clay');
var clayConfig = require('./config');

var clay = new Clay(clayConfig);

Pebble.addEventListener("ready", function(e) {
        console.log("Hello world! - Sent from your javascript application.");
        Pebble.sendAppMessage({'READY': 1});
    }
);

// Get AppMessage events
Pebble.addEventListener('appmessage', function(e) {
    // Get the dictionary from the message
    var dict = e.payload;
  
    console.log('Got message: ' + JSON.stringify(dict));

    // every appmessage from this watch app should come with an associated request id
    if (!"REQUEST_ID" in dict) { console.error("No request id!"); return;}
    const requestID = dict["REQUEST_ID"];

    switch(true) {
        case ("LOAD_GAMES" in dict):
            const sport = dict["LOAD_GAMES"];
            console.log("LOAD_GAMES, sport = ", sport);
            api.getGames(
                sport, 
                function(games) {
                    comms.sendGameList(requestID, games);
                },
                function() {
                    comms.sendGameListError(requestID);
                }
            );
            break;

        case ("UPDATE_GAME_ID" in dict):
            const game_id = dict["UPDATE_GAME_ID"]
            const game_sport = dict["UPDATE_GAME_SPORT"]
            console.log("Updating game id = ", game_id, ", sport = ", game_sport)
            api.getGame(
                game_id.toString(), game_sport,
                function(game) {
                    comms.sendGameUpdate(requestID, game);
                },
                function() {
                    comms.sendGameUpdateError(requestID);
                }
                )
            break;

        case ("ADD_FAVORITE_SPORT" in dict):
            const favoriteSport = dict["ADD_FAVORITE_SPORT"];
            const favoriteTeamID = dict["ADD_FAVORITE_TEAM_ID"];
            const favoriteTeam = new models.FavoriteTeam(favoriteSport, favoriteTeamID.toString());
            const added = storage.updateFavorite(favoriteTeam);
            comms.sendFavoritesResult(requestID, added);
    }
});