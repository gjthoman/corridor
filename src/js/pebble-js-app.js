Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://pebble-config.herokuapp.com/config?title=Quick%20Config&fields=email_Username,password_Passwd';

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var configData = JSON.parse(decodeURIComponent(e.response));
}
                        
// Send to the watchapp via AppMessage
var dict = {
  'AppKeyBackgroundColor': configData.background_color,
  'AppKeySecondTick': configData.second_ticks
};

// Send to the watchapp
Pebble.sendAppMessage(dict, function() {
  console.log('Config data sent successfully!');
}, function(e) {
  console.log('Error sending config data!');
});