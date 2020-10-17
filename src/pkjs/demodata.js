var models = require('./models');

const demoGames = [
    new models.Game(
        models.sports.MLB,
        "WSH",
        "4",
        "HOU",
        "2",
        models.possession.TEAM2,
        "Top 2nd",
        "3-2, 2 out"
    ),
    new models.Game(
        models.sports.MLB,
        "LA",
        "0",
        "SF",
        "1",
        models.possession.NONE,
        "Final",
        ""
    ),
    new models.Game(
        models.sports.NFL,
        "DEN",
        "17",
        "NYJ",
        "10",
        models.possession.TEAM1,
        "13:42 4th",
        "3rd & 8 - DEN 37"
    ),
    new models.Game(
        models.sports.NFL,
        "PIT",
        "23",
        "BAL",
        "21",
        models.possession.TEAM2,
        "2:01 3rd",
        "1st & 10 - BAL 25"
    )
]

module.exports.demoGames = demoGames;