const { messagedata, FavoriteTeam } = require('./models');
const storage = require('./storage');

require('./models');

/******************* Games Comms *******************/

function sendGames(requestID, games) {
    if (games.length == 0) {
        sendGamesEmpty(requestID);
    } else {
        console.log("sending games = ", JSON.stringify(games));
        sendGame(requestID, games, 0);
    }
}

function sendGame(requestID, games, index) {
    console.log("sending item ", index);
    const game = games[index]

    const favorites = storage.favorites();
    const team1Favorite = favorites.some(favoriteTeam => game.sport == favoriteTeam.sport && game.team1.id == favoriteTeam.teamID)
    const team2Favorite = favorites.some(favoriteTeam => game.sport == favoriteTeam.sport && game.team2.id == favoriteTeam.teamID)

    const isLastItem = index == games.length - 1;
    const dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME': isLastItem ? messagedata.LAST_LIST_ITEM : messagedata.LIST_ITEM,
        'SEND_GAME_ID': parseInt(game.id),
        'SEND_GAME_SPORT': game.sport,
        'SEND_GAME_TEAM_1_NAME': game.team1.name,
        'SEND_GAME_TEAM_2_NAME': game.team2.name,
        'SEND_GAME_TEAM_1_ID': parseInt(game.team1.id),
        'SEND_GAME_TEAM_2_ID': parseInt(game.team2.id),
        'SEND_GAME_TEAM_1_SCORE': game.score1,
        'SEND_GAME_TEAM_2_SCORE': game.score2,
        'SEND_GAME_TEAM_1_FAVORITE': team1Favorite,
        'SEND_GAME_TEAM_2_FAVORITE': team2Favorite,
        'SEND_GAME_POSSESSION': game.possession,
        'SEND_GAME_TIME': game.time,
        'SEND_GAME_DETAILS': game.details,
    }
    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
        // Use success callback to increment index
        index++;

        if (index < games.length) {
            // Send next item
            sendGame(requestID, games, index);
        } else {
            console.log('Last item sent!');
        }
    }, function () {
        console.log('Item transmission failed at index: ' + index);
    });
}

function sendGamesEmpty(requestID) {
    var dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME': messagedata.NO_GAMES
    }
    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
    }, function () {
        console.log('Item transmission failed');
    });
}

function sendGamesError(requestID) {
    var dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME': messagedata.NETWORK_ERROR
    }
    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
    }, function () {
        console.log('Item transmission failed');
    });
}

/******************* Favorites Comms *******************/
function sendFavoritesResult(requestID, result) {
    var dict = {
        'REQUEST_ID': requestID,
        'CONFIRM_FAVORITE': result
    }
    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
    }, function () {
        console.log('Item transmission failed');
    });
}

module.exports.sendGames = sendGames;
module.exports.sendGamesError = sendGamesError;
module.exports.sendFavoritesResult = sendFavoritesResult;