const { FavoriteTeam, sports } = require('./models');
var models = require('./models');

function storedFavorites() { return JSON.parse(localStorage.getItem('favorites')); }
function updateFavorite(favoriteTeam) {
    var currentFavorites = favorites();
    if (currentFavorites.some(ft => ft.sport == favoriteTeam.sport && ft.teamID == favoriteTeam.teamID)) {
        //remove the updating favoriteTeam
        currentFavorites = currentFavorites.filter(ft => ft.sport != favoriteTeam.sport || ft.teamID != favoriteTeam.teamID);
        localStorage.setItem('favorites', JSON.stringify(currentFavorites));
        return false;
    } else {
        currentFavorites.push(favoriteTeam);
        localStorage.setItem('favorites', JSON.stringify(currentFavorites));
        return true;
    }

}

module.exports = {
    storedFavorites: storedFavorites,
    updateFavorite: updateFavorite
}