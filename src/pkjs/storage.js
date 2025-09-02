function storedFavorites() { 
    const storedOrNull = localStorage.getItem('favorites');
    if (storedOrNull == null) {
        return [];
    } else {
        return JSON.parse(localStorage.getItem('favorites')); 
    }
}
function updateFavorite(favoriteTeam) {
    var currentFavorites = storedFavorites();
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