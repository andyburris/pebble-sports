var models = require('./models');
var storage = require('./storage');
var demodata = require('./demodata');
var comms = require('./comms');
var api = require('./api');

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

    if (dict["LOAD_GAMES"]) {
        console.log("LOAD_GAMES", dict["LOAD_GAMES"]);
    } else {
        console.log("not load games?");
    }

    switch(true) {
        case ("LOAD_GAMES" in dict):
            console.log("LOAD_GAMES", dict["LOAD_GAMES"]);
            api.getGames(dict["LOAD_GAMES"], loadGames, loadGames);
            break;
    }
});

function loadGames(sport, games) {
    console.log("loading games for sport = ", sport)
/*     var games = [];
    switch (sport) {
        case models.sports.FAVORITES:
            games = demodata.demoGames.filter(g => storage.favorites.some(f => g.isFavorite(f)))
            break;
        default:
            games = demodata.demoGames.filter(g => g.sport == sport)
            break;
    } */
    console.log("games for ", sport, " = ", JSON.stringify(games))
    comms.sendGames(games, 0);
}