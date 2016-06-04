Pebble.addEventListener('showConfiguration', function() {
  var back = window.localStorage.getItem('BACKGROUND_COLOR') ? window.localStorage.getItem('BACKGROUND_COLOR') : 0;
  var time = window.localStorage.getItem('TIME_MIDGROUND_COLOR') ? window.localStorage.getItem('TIME_MIDGROUND_COLOR') : 5592405;
  var date = window.localStorage.getItem('DATE_MIDGROUND_COLOR') ? window.localStorage.getItem('DATE_MIDGROUND_COLOR') : 5592405;
  var fore = window.localStorage.getItem('MAIN_COLOR') ? window.localStorage.getItem('MAIN_COLOR') : 16777215;
  var seconds = window.localStorage.getItem('SECONDS') ? window.localStorage.getItem('SECONDS') : 0;

  var url = 'http://gjthoman.github.io/corridor/config.html?background_color=' + back + '&time_midground_color=' + time + '&date_midground_color=' + date + '&foreground_color=' + fore + '&seconds=' + seconds;
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences

  var configData = JSON.parse(decodeURIComponent(e.response));
  // Send to the watchapp via AppMessage
  var dict = {
    'AppKeyBackgroundColor': configData.background_color,
    'AppKeyTimeMidgroundColor': configData.time_midground_color,
    'AppKeyDateMidgroundColor': configData.date_midground_color,
    'AppKeyMainColor': configData.foreground_color,
    'AppKeySeconds': configData.seconds
  };
  
  window.localStorage.setItem('BACKGROUND_COLOR', configData.background_color);
  window.localStorage.setItem('TIME_MIDGROUND_COLOR', configData.time_midground_color);
  window.localStorage.setItem('DATE_MIDGROUND_COLOR', configData.date_midground_color);
  window.localStorage.setItem('MAIN_COLOR', configData.foreground_color);
  window.localStorage.setItem('SECONDS', configData.seconds);
  
  // Send to the watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Config data sent successfully!');
  }, function(e) {
    console.log(JSON.stringify(e));
    console.log('Error sending config data!');
  });
});