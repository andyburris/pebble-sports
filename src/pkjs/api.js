const models = require('./models');
const utils = require('./utils');
const { sports, possession } = require('./models');

require('./models');

// to get team schedule only, url is http://site.api.espn.com/apis/site/v2/sports/:sport/:league/teams/:id/schedule

function getGames(sport, onLoad, onError) {

    if (sport == sports.FAVORITES) { onLoad(sport, []); return;};

    var req = new XMLHttpRequest();
    var endpoint = "http://site.api.espn.com/apis/site/v2/sports";
    switch (sport) {
        case sports.NFL: endpoint += '/football/nfl/scoreboard'; break;
        case sports.MLB: endpoint += '/baseball/mlb/scoreboard'; break;
        case sports.NHL: endpoint += '/hockey/nhl/scoreboard'; break;
        case sports.NBA: endpoint += '/basketball/nba/scoreboard'; break;
        default: break;
    }
    req.open('GET', endpoint);
    req.onload = function (e) {
        if (req.readyState == 4) {
            // 200 - HTTP OK
            if (req.status == 200) {
                const sportsData = JSON.parse(req.responseText);
                const events = sportsData.events;
                const games = events.map(event => parseEvent(sport, event));
                console.log(JSON.stringify(games)); // let's log it to console to see what came from the API
                onLoad(sport, games);
            } else {
                onError(sport)
            }
        } else {
            onError(sport)
        }
    }
    req.onerror = function (e) {
        onError(sport)
    }
    req.send();
}

function parseEvent(sport, event) {
    const competitors = event.competitions[0].competitors;
    const date = new Date(Date.parse(event.date));
    const [details, time] = (function(type) {
        switch (type) {
            case "STATUS_FINAL": return [utils.dateToScheduleDate(date), "Final"];
            case "STATUS_SCHEDULED": return [utils.dateToScheduleDate(date), utils.dateToScheduleTime(date)];
            default: return [gameDetails(sport, event.competitions[0].situation), event.status.type.shortDetail.replace("- ", "")];
        }
    })(event.status.type.name);

    const team1 = competitors[1].team; //ESPN lists home first, we want to list away first so flip
    const team2 = competitors[0].team;

    //if the game hasn't started, the app shouldn't show the scores
    const score1 = event.status.type.name == "STATUS_SCHEDULED" ? "" : competitors[1].score;
    const score2 = event.status.type.name == "STATUS_SCHEDULED" ? "" : competitors[0].score;

    const possession = event.status.type.name != "STATUS_IN_PROGRESS" ? models.possession.NONE : gamePossession(sport, event.competitions[0].situation, team1, team2);

    return new models.Game(
        sport,
        team1.abbreviation,
        score1,
        team2.abbreviation,
        score2,
        possession,
        time,
        details
    );
}

function gameDetails(sport, situation) {
    switch (sport) {
        case sports.NFL: return situation.downDistanceText;
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