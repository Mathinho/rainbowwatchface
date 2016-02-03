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
  var url = "http://api.openweathermap.org/data/2.5/weather?appid=ecd3e7093d6e140683678a7e3963e92c&lat=" +
      pos.coords.latitude + "&lon=" + pos.coords.longitude + "&lang=de";

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      console.log(responseText);
      var json = JSON.parse(responseText);
      

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log("Temperatur beträgt " + temperature + "°C");

      // Conditions
      var conditions = json.weather[0].description;      
      console.log("Wetterbedingungen sind " + conditions);
      
      var location = json.name;      
      console.log("Ort ist " + location);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_CONDITIONS": conditions,
        "KEY_LOCATION": location
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Wetter Informationen wurden erfolgreich an Pebble gesendet!");
        },
        function(e) {
          console.log("Es ist ein Fehler beim Senden der Wetter Informationen an Pebble aufgetreten!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Fehler bei der Ortsermittlung!");
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
    console.log("AppMessage empfangen!");
    getWeather();
  }                     
);

