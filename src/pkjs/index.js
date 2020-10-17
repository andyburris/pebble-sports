var models = require('./models');
var storage = require('./storage');
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
            const sport = dict["LOAD_GAMES"];
            console.log("LOAD_GAMES, sport = ", sport);
            api.getGames(sport, comms.sendGames, comms.sendError);
            break;
    }
});