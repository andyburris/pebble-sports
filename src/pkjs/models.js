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

const gamedata = {
    LIST_ITEM: 0,
    LAST_LIST_ITEM: 1,
    NO_GAMES: 2,
    NETWORK_ERROR: 3
}

function Game (sport, team1, score1, team2, score2, possession, time, details) {
    this.sport = sport;
    this.team1 = team1;
    this.score1 = score1;
    this.team2 = team2;
    this.score2 = score2;
    this.possession = possession;
    this.time = time;
    this.details = details;
    this.isFavorite = function(favoriteTeam) {
        return favoriteTeam.sport == sport && (favoriteTeam.team == team1 || favoriteTeam.team == team2);
    }
}

function FavoriteTeam(sport, team) {
    this.sport = sport;
    this.team = team;
}

module.exports = {
    sports: sports,
    possession: possession,
    Game: Game,
    FavoriteTeam: FavoriteTeam
}