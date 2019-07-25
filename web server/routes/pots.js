var express = require('express');
var router = express.Router();
var pot = require("../controllers/PotController.js");

// restrict index for logged in user only
router.get('/', pot.home);

router.post('/', pot.doUpdate);

router.post('/add', pot.add);

router.get('/info', pot.info);

router.post('/update', pot.doUpdate);
router.post('/wupdate', pot.doWebUpdate);

// route to register page
router.get('/register', pot.register);

// route for register action
router.post('/register', pot.doUpdate);

module.exports = router;
