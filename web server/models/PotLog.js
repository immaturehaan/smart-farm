var mongoose = require('mongoose');
var Schema = mongoose.Schema;

var PotLog = new Schema({
  PotId: {
    type : mongoose.Schema.Types.ObjectId,
    ref : 'Pot'
  },
  TimeStamp: Number,
  CurSoil: Number,
  CurTemp: Number,
});

module.exports = mongoose.model('PotLog', PotLog);