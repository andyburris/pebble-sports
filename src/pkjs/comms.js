function sendGames(games, index) {
    console.log("sending item ", index);
    var game = games[index]
    var dict = {
        'SEND_GAME': index == games.length - 1 ? 1 : 0,
        'SEND_GAME_SPORT': game.sport,
        'SEND_GAME_TEAM_1': game.team1,
        'SEND_GAME_TEAM_2': game.team2,
        'SEND_GAME_SCORE_1': game.score1,
        'SEND_GAME_SCORE_2': game.score2,
        'SEND_GAME_POSSESSION': game.possession,
        'SEND_GAME_TIME': game.time,
        'SEND_GAME_DETAILS': game.details,
    }
    Pebble.sendAppMessage(dict, function() {
        console.log("message success");
        // Use success callback to increment index
        index++;
    
        if(index < games.length) {
          // Send next item
          sendGames(games, index);
        } else {
          console.log('Last item sent!');
        }
      }, function() {
        console.log('Item transmission failed at index: ' + index);
      });
}

module.exports.sendGames = sendGames;