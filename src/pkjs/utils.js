function ordinalSuffixOf(i) {
    var j = i % 10,
        k = i % 100;
    if (j == 1 && k != 11) {
        return i + "st";
    }
    if (j == 2 && k != 12) {
        return i + "nd";
    }
    if (j == 3 && k != 13) {
        return i + "rd";
    }
    return i + "th";
}

const weekdays = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
function dateToScheduleDate(date) {
    return weekdays[date.getDay()] + " " + (date.getMonth() + 1) + "/" + date.getDate()
}

function dateToScheduleTime(date) {
    const mins = date.getMinutes();
    const hours = date.getHours();
    return hours % 12  + ":" + (mins <= 9 ? '0' + mins : mins) + (hours >= 12 ? "pm" : "am")
}

module.exports.ordinalSuffixOf = ordinalSuffixOf;
module.exports.dateToScheduleDate = dateToScheduleDate;
module.exports.dateToScheduleTime = dateToScheduleTime;