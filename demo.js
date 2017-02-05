var ad = require('./build/Release/npm_demo');


if (process.argv.length < 1) {
    console.warn(usage);
    process.exit(1);
}

var channel = parseInt(process.argv[0], 10);
var count = 0;
var start = 0;
var end = 0;

var iid = setInterval(function() {
  start = new Date().getTime();
  
  ad.read(channel);
  /*	
  ad.read(channel, function(err, adc, volt) {
  	end = new Date().getTime();
    if (err) {
      console.warn('' + err);
    } else {
      var elapsed = end - start;
      console.log("adc: %s, volt: %s%%, time: %dms", adc[7].toFixed(2), volt[7].toFixed(2), end - start);
    }
  });
  */
}, 100); // 2500
