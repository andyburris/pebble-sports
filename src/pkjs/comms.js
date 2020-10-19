const { messagedata } = require('./models');

require('./models');

function sendGames(requestID, games) {
    if (games.length == 0) {
        sendEmpty(requestID);
    } else {
        sendGame(requestID, games, 0);
    }
}

function sendGame(requestID, games, index) {
    console.log("sending item ", index);
    const game = games[index]
    const lastItem = index == games.length - 1;
    const dict = {
        'REQUEST_ID': requestID,
        'SEND_GAME': lastItem ? messagedata.LAST_LIST_ITEM : messagedata.LIST_ITEM,
        'SEND_GAME_SPORT': game.sport,
        'SEND_GAME_TEAM_1': game.team1,
        'SEND_GAME_TEAM_2': game.team2,
        'SEND_GAME_SCORE_1': game.score1,
        'SEND_GAME_SCORE_2': game.score2,
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

function sendEmpty(requestID) {
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

function sendError(requestID) {
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

module.exports.sendGames = sendGames;
module.exports.sendError = sendError;