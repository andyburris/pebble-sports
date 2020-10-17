var models = require('./models');

localStorage.setItem('favorites', JSON.stringify([new models.FavoriteTeam(models.sports.MLB, "WSH"), new models.FavoriteTeam(models.sports.NFL, "PIT")]));
var favorites = JSON.parse(localStorage.getItem('favorites'));

module.exports = {
    favorites: favorites
}