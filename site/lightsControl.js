var connection = new WebSocket('ws://' + location.host + ':80/ws', ['arduino']);

connection.binaryType = "arraybuffer";

connection.onopen = function () {
  connection.send('Connect ' + new Date());
};
connection.onerror = function (error) {
  console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
  console.log('Server: ', e.data);
};
connection.onclose = function () {
  console.log('WebSocket connection closed');
};

// CONFIG

var FIRST_BYTES = {"mode":128, 
  "brightness":129,
  "rf_twinkle":130,
  "rf_bandsize":131,
  "xt_twinkle":132,
  "xt_starsize":133,
  "xt_ornamentspacing":134,
  "hb_bpm": 135,
  "xt_triggerShow":12};

function fetchConfigVariables() {
  // Remove existing config
  var existingConfig = document.getElementById("configVariables");
  if(existingConfig !== null) {
      document.body.removeChild(existingConfig);
  }

  // Create new script element and load a script into it
  js = document.createElement("script");
  js.onload = updateUIFromConfig;
  js.src = "getConfig";
  js.id = "configVariables";
  document.body.appendChild(js);
}

function updateUIFromConfig() {
  document.getElementById('modeSelect').selectedIndex = xmasconfig.mode;
  document.getElementById('brightnessValue').value = xmasconfig.brightness;

  document.getElementById('rf_TwinkleCB').checked = xmasconfig.rf_twinkle > 0;
  document.getElementById('rf_bandSize').value = xmasconfig.rf_bandsize;

  document.getElementsByName('xt_TwinkleCB').forEach(function (ele, idx) {
    ele.checked = xmasconfig.xt_twinkle > 0;
  });
  document.getElementsByName('xt_starSize').forEach(function (ele, idx) {
    ele.value = xmasconfig.xt_starsize;
  });
  document.getElementsByName('xt_ornamentSpacing').forEach(function (ele, idx) {
    ele.value = xmasconfig.xt_ornamentspacing;
  });
  document.getElementById('hb_bpm').value = xmasconfig.hb_bpm;

  viewMode(xmasconfig.mode);
}

function loadConfig() {
  sendingEnabled = false;
  try{
    fetchConfigVariables();
  } finally {
    sendingEnabled = true;
  }

}

// MESSAGE SENDING

var sendingEnabled = true; // Can be disabled during refresh operations.

function sendPayload(payload) {
  if (sendingEnabled) {
    console.log(payload);
    connection.send(payload);
  }
}

function sendMessage(firstByte, secondByte) {
  var payload = new Int8Array(2);
  payload[0]=firstByte;
  payload[1]=secondByte;
  console.log(payload);
  connection.send(payload);
}

// VIEW CONTROL
var currentModeView = 0;

function viewMode(newMode) {
  document.getElementById('mode_'+currentModeView).style.display = "none";
  document.getElementById('mode_'+newMode).style.display = "block";
  currentModeView = newMode;
}