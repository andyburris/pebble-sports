var models = require('./models');

localStorage.setItem('favorites', JSON.stringify([]));
var favorites = JSON.parse(localStorage.getItem('favorites'));

module.exports = {
    favorites: favorites
}