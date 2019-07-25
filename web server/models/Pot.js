var mongoose = require('mongoose');
var Schema = mongoose.Schema;

var PotSchema = new Schema({
    PotId: String,
    Name: String,
    IsAutoPump: Boolean,
    IsPump: Boolean,
    HighPumpLevel: Number,
    LowPumpLevel: Number,
    ResetWifi: Number
});

module.exports = mongoose.model('Pot', PotSchema);