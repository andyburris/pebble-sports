/*!
 * Group items from an array together by some criteria or value.
 * (c) 2019 Tom Bremmer (https://tbremer.com/) and Chris Ferdinandi (https://gomakethings.com), MIT License,
 * @param  {Array}           arr      The array to group items from
 * @param  {String|Function} criteria The criteria to group by
 * @return {Object}                   The grouped object
 */
var groupBy = function (arr, criteria) {
	return arr.reduce(function (obj, item) {

		// Check if the criteria is a function to run on the item or a property of it
		const key = typeof criteria === 'function' ? criteria(item) : item[criteria];

		// If the key doesn't exist yet, create it
		if (!obj.hasOwnProperty(key)) {
			obj[key] = [];
		}

		// Push the value to the object
		obj[key].push(item);

		// Return the object to the next item in the loop
		return obj;

	}, {});
};



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
    const formattedHours = ((hours - 1) % 12) + 1; // convert 0 to 12
    // console.log("converting date to schedule time, date = ", date, ", hours = ", formattedHours)
    return formattedHours + ":" + (mins <= 9 ? '0' + mins : mins) + (hours >= 12 ? "pm" : "am")
}

module.exports.groupBy = groupBy;
module.exports.ordinalSuffixOf = ordinalSuffixOf;
module.exports.dateToScheduleDate = dateToScheduleDate;
module.exports.dateToScheduleTime = dateToScheduleTime;