var mongoose = require("mongoose");
var passport = require("passport");
var User = require("../models/User");
var Pot = require("../models/Pot");
var PotLog = require("../models/PotLog");
var PotDB = require("../global");
var dateFormat = require('dateformat');

var potController = {};

// Restrict access to root page
potController.home = function(req, res) {
  if (req.user) {
    Pot.findById(req.user.potList, (err, pot) => {
      if (pot) {
        PotLog.findOne({PotId: pot._id}, {}, { sort: { 'TimeStamp' : -1 } }, function(err, potLog) {
          potLog.UpdateAt = dateFormat(new Date(potLog.TimeStamp * 1000), "dd, mmm, yyyy hh:MM:ss");
          return res.render('pot/index', { user : req.user, Pot: pot, PotLog: potLog });
        });
      }
      else {
        return res.render('pot/index', { user : req.user, Pot: {'no': true}, PotLog: {'no': 'no'} });
      }
    });
  } else {
    return res.redirect('/');
  }
};

potController.add = function(req, res) {
  console.log(req.body);
  var query = { PotId: req.body.potId },
  update = { 
      Name: req.body.potName,
      PotId: req.body.potId 
  },
  options = { upsert: true, new: true, setDefaultsOnInsert: true };
  // Find the document
  Pot.findOneAndUpdate(query, update, options, function(err, pot) {
      if (err) throw err;
      // console.log(res);

      User.findByIdAndUpdate(req.user._id, 
        { potList: pot._id },
        function(err, user) {
          if (err) throw err;
        }
        );
        // return redirect('/pots');
        return res.json({
          Pot: pot
        });
      }
  );
};

potController.info = function(req, res) {
  if (req.query.iot || (req.query.web && req.user && req.query.id)) {
    Pot.findOne({PotId: req.query.id}, (err, pot) => {
      if (err) {
        throw(err);
      }
      if (req.query.web && pot._id) {
        PotLog.findOne({PotId: pot._id}, {}, { sort: { 'TimeStamp' : -1 } }, function(err, potLog) {
          return res.json({Pot: pot, PotLog: potLog});
        });
      } else {
        return res.json(pot);
      }
    });
  } else {
    return res.json(
      { 
        'NoDevice': true,
      }
    );
  }
  // console.log(PotDB.GLOBALPOT[req.query["id"]]);
}
// Go to registration page
potController.register = function(req, res) {
  // res.render('register');
};

// Post registration
potController.doUpdate = function(req, res) {
  var query = { PotId: req.body.ID },
    update = { 
      // ResetWifi: req.body.ResetWifi
      IsPump: req.body.IsPump
    },
    options = { upsert: true, new: true, setDefaultsOnInsert: true };
    Pot.findOne(query, (err, pot) => {
      if(pot) {
        Pot.findOneAndUpdate(query, update, function(err, cpot) {
          if (err) throw err;
          var potLog = new PotLog({
            PotId: cpot._id,
            TimeStamp: req.body.UpdateAt,
            CurSoil: req.body.CurSoil,
            CurTemp: req.body.CurTemp,
          }, { strict: false });
    
          potLog.save((err) => {
            if (err) throw err;
          });
        });
      } else {
        var nPot = new Pot ({
          PotId: req.body.ID,
          Name: req.body.DvName,
          IsAutoPump: req.body.IsAutoPump,
          IsPump: req.body.IsPump,
          HighPumpLevel: req.body.HighPumpLevel,
          LowPumpLevel: req.body.LowPumpLevel,
        });
        nPot.save((err) => {
          if (err) throw err;
        })
      }
    })
  return res.json({ username: 'Flavio' , 'autoPump': 1});
};

potController.doWebUpdate = function(req, res) {
  var update = {},
    options = { upsert: true, new: true, setDefaultsOnInsert: true };

    if ('ResetWifi' in req.body) {
      update.ResetWifi = req.body.ResetWifi;
    }
    if ('IsAutoPump' in req.body) {
      update.IsAutoPump = req.body.IsAutoPump;
    }
    if ('IsPump' in req.body) {
      update.IsPump = req.body.IsPump;
    }
    if ('HighPumpLevel' in req.body) {
      update.HighPumpLevel = req.body.HighPumpLevel;
    }
    if ('LowPumpLevel' in req.body) {
      update.LowPumpLevel = req.body.LowPumpLevel;
    }

    if ('Remove' in req.body) {
      User.findByIdAndUpdate(req.user._id, 
        { potList: null },
        function(err, user) {
          if (err) throw err;
          return res.json({'IsRemoved': true});
        }
      );
      return;
    }
    console.log(update);
    // Find the document
    Pot.findByIdAndUpdate(req.body.id, update, options, function(err, pot) {
      if (err) throw err;
      console.log(pot);
    });
  return res.json({ username: 'Flavio' , 'autoPump': 1});
};

module.exports = potController;
