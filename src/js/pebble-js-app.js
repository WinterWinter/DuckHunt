Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    Pebble.openURL("http://winterwinter.github.io/DuckHunt/");
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var tempScale = JSON.parse(decodeURIComponent(e.response));
    console.log("Temperature Scale " + JSON.stringify(tempScale.scale));
    
    
    var dictionary = {
      "KEY_SCALE" : tempScale.scale,
       };

    //Send to Pebble, persist there
    Pebble.sendAppMessage(dictionary,
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);


function iconFromWeatherId(weatherId) {
  if (weatherId < 300) {
    //Storm
    return 4;
  } else if (weatherId < 600){
    //Snow
    return 2;
  } else if (weatherId < 700) {
    //Rain
    return 3;
  } else if (weatherId > 800) {
    //Cloud
    return 1;
  } else {
    //Sun
    return 0;
  }
}


var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(text) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(text);

      // Temperature in Kelvin requires adjustment
      //var lat = pos.coords.latitude;
      //var long = pos.coords.longitude;
      var temperature = Math.round(json.main.temp);
      var icon = iconFromWeatherId(json.weather[0].id);
      //var icon_id = json.weather[0].id;
      
      console.log("Temperature is " + temperature);
      console.log(icon);
      //console.log("Icon ID is " + icon_id);
      //console.log("Latitude is " + lat);
      //console.log("Longitude is " + long);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_ICON":icon
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);