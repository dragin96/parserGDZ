var express = require('express');
var app = express();

var bodyParser = require('body-parser');
app.use(bodyParser.json());

app.listen(1080, function() {
});+

app.post("/report", function(req, res) {
    var data = req.body;
    res.send('{status:ok}');
});