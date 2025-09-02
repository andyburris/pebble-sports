const models = require('./models');
const storage = require('./storage');

/******************* Game List Comms *******************/

function sendGameList(requestID, games) {
    if (games.length == 0) {
        sendEmptyGameList(requestID);
    } else {
        console.log("sending games = ", JSON.stringify(games));
        sendGameListItem(requestID, games, 0);
    }
}

function sendGameListItem(requestID, games, index) {
    const game = games[index]

    const favorites = storage.storedFavorites();
    const team1Favorite = favorites.some(favoriteTeam => game.sport == favoriteTeam.sport && game.team1.id == favoriteTeam.teamID)
    const team2Favorite = favorites.some(favoriteTeam => game.sport == favoriteTeam.sport && game.team2.id == favoriteTeam.teamID)

    const isLastItem = index == games.length - 1;
    const dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME_LIST': isLastItem ? models.gameslistdata.LAST_LIST_ITEM : models.gameslistdata.LIST_ITEM,
        'SEND_GAME_ID': parseInt(game.id),
        'SEND_GAME_SPORT': game.sport,
        'SEND_GAME_TEAM_1_NAME': game.team1.name,
        'SEND_GAME_TEAM_2_NAME': game.team2.name,
        'SEND_GAME_TEAM_1_ID': parseInt(game.team1.id),
        'SEND_GAME_TEAM_2_ID': parseInt(game.team2.id),
        'SEND_GAME_TEAM_1_SCORE': game.score1,
        'SEND_GAME_TEAM_2_SCORE': game.score2,
        'SEND_GAME_TEAM_1_RECORD': game.team1.record,
        'SEND_GAME_TEAM_2_RECORD': game.team2.record,
        'SEND_GAME_TEAM_1_FAVORITE': team1Favorite,
        'SEND_GAME_TEAM_2_FAVORITE': team2Favorite,
        'SEND_GAME_POSSESSION': game.possession,
        'SEND_GAME_TIME': game.time,
        'SEND_GAME_DETAILS': game.details,
    }

    console.log("sending item ", index);

    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
        // Use success callback to increment index
        index++;

        if (index < games.length) {
            // Send next item
            sendGameListItem(requestID, games, index);
        } else {
            console.log('Last item sent!');
        }
    }, function () {
        console.log('Item transmission failed at index: ' + index);
    });
}

function sendEmptyGameList(requestID) {
    var dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME_LIST': models.gameslistdata.NO_GAMES
    }
    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
    }, function () {
        console.log('Item transmission failed');
    });
}

function sendGameListError(requestID) {
    console.log("ERROR GETTING GAME LIST")
    var dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME_LIST': models.gameslistdata.NETWORK_ERROR
    }
    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
    }, function () {
        console.log('Item transmission failed');
    });
}

/******************* Game Update Comms *******************/

function sendGameUpdate(requestID, game) {
    console.log("sending update");

    const favorites = storage.storedFavorites();
    const team1Favorite = favorites.some(favoriteTeam => game.sport == favoriteTeam.sport && game.team1.id == favoriteTeam.teamID)
    const team2Favorite = favorites.some(favoriteTeam => game.sport == favoriteTeam.sport && game.team2.id == favoriteTeam.teamID)

    const dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME_UPDATE': models.updategamedata.UPDATE_GAME,
        'SEND_GAME_ID': parseInt(game.id),
        'SEND_GAME_SPORT': game.sport,
        'SEND_GAME_TEAM_1_NAME': game.team1.name,
        'SEND_GAME_TEAM_2_NAME': game.team2.name,
        'SEND_GAME_TEAM_1_ID': parseInt(game.team1.id),
        'SEND_GAME_TEAM_2_ID': parseInt(game.team2.id),
        'SEND_GAME_TEAM_1_SCORE': game.score1,
        'SEND_GAME_TEAM_2_SCORE': game.score2,
        'SEND_GAME_TEAM_1_RECORD': game.team1.record,
        'SEND_GAME_TEAM_2_RECORD': game.team2.record,
        'SEND_GAME_TEAM_1_FAVORITE': team1Favorite,
        'SEND_GAME_TEAM_2_FAVORITE': team2Favorite,
        'SEND_GAME_POSSESSION': game.possession,
        'SEND_GAME_TIME': game.time,
        'SEND_GAME_DETAILS': game.details,
    }
    Pebble.sendAppMessage(dict, function () {
        console.log("message success");
    }, function () {
        console.log('Item transmission failed at index: ' + index);
    });
}

function sendGameUpdateError(requestID) {
    console.log("sending update error");
    var dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME_UPDATE': models.updategamedata.NETWORK_ERROR
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

module.exports.sendGameList = sendGameList;
module.exports.sendGameListError = sendGameListError;
module.exports.sendGameUpdate = sendGameUpdate;
module.exports.sendGameUpdateError = sendGameUpdateError;
module.exports.sendFavoritesResult = sendFavoritesResult;