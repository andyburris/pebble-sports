const models = require('./models');
const storage = require('./storage');
const utils = require('./utils');
const { sports, possession } = require('./models');

require('./models');

// to get team schedule only, url is http://site.api.espn.com/apis/site/v2/sports/:sport/:league/teams/:id/schedule
// to get more info on specific game, url is http://site.api.espn.com/apis/site/v2/sports/:sport/:league/summary?event=:id

function getGames(sport, onLoad, onError) {
    if (sport == sports.FAVORITES) { 
        getFavoriteGames(storage.favorites(), onLoad, onError);
    } else {
        getGamesForSport(sport, onLoad, onError)
    }

}

function getFavoriteGames(favorites, onLoad, onError) {
    console.log("getting favorite games");
    const sportGroups = utils.groupBy(favorites, favoriteItem => favoriteItem.sport);
    console.log("sportGroups = ", JSON.stringify(sportGroups));
    const favoriteSports = Object.keys(sportGroups).map(key => parseInt(key));
    console.log("favoriteSports = ", JSON.stringify(favoriteSports));
    var favoriteGames = [];
    var loadedSports = [];
    Object.values(sportGroups).forEach((sportGroup) => {
        const sport = sportGroup[0].sport
        const teamIDs = sportGroup.map(favoriteItem => favoriteItem.teamID)
        getGamesForSport(
            sport,
            (games) => {
                console.log("loaded sport = ", sport, ", games = ", JSON.stringify(games));
                console.log("teamIDs = ", JSON.stringify(teamIDs), "includes 23 = ", teamIDs.includes("23"));
                const filtered = games.filter(game => teamIDs.includes(game.team1.id) || teamIDs.includes(game.team2.id) );
                console.log("filtered = ", JSON.stringify(filtered));
                favoriteGames.push(...filtered);
                loadedSports.push(sport);
                console.log("favoriteGames = ", JSON.stringify(favoriteGames), ", loadedSports = ", JSON.stringify(loadedSports), ", favoriteSports = ", JSON.stringify(favoriteSports));
                if (favoriteSports.every(sport => loadedSports.includes(sport))) {
                    console.log("loaded all sports");
                    onLoad(favoriteGames);
                }
            },
            onError // for now, an error in any of the requests should just send none of the games
        )
    })
}

function getEndpointForSport(sport) {
    var endpoint = "http://site.api.espn.com/apis/site/v2/sports";
    switch (sport) {
        case sports.NFL: endpoint += '/football/nfl'; break;
        case sports.MLB: endpoint += '/baseball/mlb'; break;
        case sports.NHL: endpoint += '/hockey/nhl'; break;
        case sports.NBA: endpoint += '/basketball/nba'; break;
        default: break;
    }
    return endpoint;
}

function getGamesForSport(sport, onLoad, onError) {
    var req = new XMLHttpRequest();
    const endpoint = getEndpointForSport(sport) + "/scoreboard"
    req.open('GET', endpoint);
    req.onload = function (e) {
        if (req.readyState == 4) {
            // 200 - HTTP OK
            if (req.status == 200) {
                const sportsData = JSON.parse(req.responseText);
                const events = sportsData.events;
                const games = events.map(event => parseEvent(sport, event));
                console.log(JSON.stringify(games));
                onLoad(games);
                return;
            }
        }
        // doesn't run if onLoad is called due to the return statement
        onError();
    }
    req.onerror = function (e) {
        onError()
    }
    req.send();
}

function getGame(id, sport, onLoad, onError) {
    console.log("getting game ", id, " for sport = ", sport);
    var req = new XMLHttpRequest();
    const endpoint = getEndpointForSport(sport) + "/summary?event=" + id;
    console.log("endpoint = ", endpoint);
    req.open('GET', endpoint);
    req.onload = function (e) {
        console.log("onload");
        if (req.readyState == 4) {
            // 200 - HTTP OK
            if (req.status == 200) {
                console.log("HTTP OK");
                const sportsData = JSON.parse(req.responseText);
                console.log("sportsData = ", sportsData);
                const game = parseEvent(sport, sportsData.header);
                console.log(JSON.stringify(game));
                onLoad(game);
                return;
            }
        }
        // doesn't run if onLoad is called due to the return statement
        onError();
    }
    req.onerror = function (e) {
        onError()
    }
    req.send();
}

function parseEvent(sport, event) {
    const competitors = event.competitions[0].competitors;
    const date = new Date(Date.parse(event.competitions[0].date));
    const status = event.competitions[0].status
    const [details, time] = (function(type) {
        switch (type) {
            case "STATUS_FINAL": return [utils.dateToScheduleDate(date), "Final"];
            case "STATUS_SCHEDULED": return [utils.dateToScheduleDate(date), utils.dateToScheduleTime(date)];
            default: return [gameDetails(sport, event.competitions[0].situation), status.type.shortDetail.replace("- ", "")];
        }
    })(status.type.name);

    const id = event.id

    const competitor1 = competitors[1]; //ESPN lists home first, we want to list away first so flip
    const competitor2 = competitors[0];
    const team1 = competitor1.team;
    const team2 = competitor2.team;

    //if the game hasn't started, the app shouldn't show the scores
    const score1 = status.type.name == "STATUS_SCHEDULED" ? "" : competitor1.score;
    const score2 = status.type.name == "STATUS_SCHEDULED" ? "" : competitor2.score;

    const possession = status.type.name != "STATUS_IN_PROGRESS" ? models.possession.NONE : gamePossession(sport, event.competitions[0].situation, team1, team2);

    return new models.Game(
        id,
        sport,
        new models.Team(team1.abbreviation, team1.id, (competitor1.records || competitor1.record)[0].summary), //single-game and multi-game response name the records/record field differently, so accept both
        score1,
        new models.Team(team2.abbreviation, team2.id, (competitor2.records || competitor2.record)[0].summary),
        score2,
        possession,
        time,
        details
    );
}

function gameDetails(sport, situation) {
    switch (sport) {
        case sports.NFL: return situation.downDistanceText || "";
        case sports.MLB: return situation.balls + "-" + situation.strikes + ", " + situation.outs + " outs"; 
        default: return "";
    }
}

function gamePossession(sport, situation, team1, team2) {
    switch (sport) {
        case sports.NFL: return possessionByTeam(situation.possession, team1, team2);
        case sports.MLB: return situation.batter == undefined ? possessionByTeam(situation.dueUp[0].athlete.team.id, team1, team2) : possessionByTeam(situation.batter.athlete.team.id, team1, team2);
        default: return models.possession.NONE;
    }
}

function possessionByTeam(possessionID, team1, team2) {
    return (team1.id == possessionID) ? models.possession.TEAM1 : (team2.id == possessionID) ? models.possession.TEAM2 : models.possession.NONE;
}

module.exports.getGames = getGames;
module.exports.getGame = getGame;