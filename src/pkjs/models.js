const sports = {
    FAVORITES: 0,
    NFL: 1,
    MLB: 2,
    NHL: 3,
    NBA: 4
}

const possession = {
    TEAM1: 0,
    TEAM2: 1,
    NONE: 2
}

const messagedata = {
    LIST_ITEM: 0,
    LAST_LIST_ITEM: 1,
    NO_GAMES: 2,
    NETWORK_ERROR: 3
}

function Team(name, id, record) {
    this.name = name;
    this.id = id;
    this.record = record;
}

function Game (id, sport, team1, score1, team2, score2, possession, time, details) {
    this.id = id;
    this.sport = sport;
    this.team1 = team1;
    this.score1 = score1;
    this.team2 = team2;
    this.score2 = score2;
    this.possession = possession;
    this.time = time;
    this.details = details;
    this.isFavorite = function(favoriteTeam) {
        return favoriteTeam.sport == sport && (favoriteTeam.teamID == team1.id || favoriteTeam.teamID == team2.id);
    }
}

function FavoriteTeam(sport, teamID) {
    this.sport = sport;
    this.teamID = teamID;
}

module.exports = {
    sports: sports,
    possession: possession,
    messagedata: messagedata,
    Team: Team,
    Game: Game,
    FavoriteTeam: FavoriteTeam
}