function setupHighcharts() {
  Highcharts.setOptions({ global: { useUTC: false } });
  var chart = Highcharts.chart('container', {
    chart: {
      type: 'spline'
    },
    title: {
      text: 'Temperature, Humidity & Pressure (BME 280)'
    },
    xAxis: {
      type: 'datetime',
      tickPixelInterval: 150
    },
    yAxis: [{
      min: 0,
      max: 40,
      labels: {
        format: '{value}°C',
        style: {
          color: '#f15c80'
        }
      },
      title: {
        text: 'Temperature',
        style: {
          color: '#f15c80'
        }
      }
    },
    {
      min: 0,
      max: 100,
      labels: {
        format: '{value}%',
        style: {
          color: '#7cb5ec'
        }
      },
      title: {
        text: 'Humidity',
        style: {
          color: '#7cb5ec'
        }
      },
      opposite: true
    },
    {
      min: 950,
      max: 1100,
      labels: {
        format: '{value}hPa',
        style: {
          color: '#90ed7d'
        }
      },
      title: {
        text: 'Pressure',
        style: {
          color: '#90ed7d'
        }
      },
      opposite: true
    }],
    legend: {
      enabled: false
    },
    exporting: {
      enabled: false
    },
    series: [{
      name: 'Temperature',
      type: 'spline',
      yAxis: 0,
      color: '#f15c80',
      data: (function () {
        var data = [], time = 0, i;
        for (i = 0; i < 10; i += 1) {
          data.push({ x: (time - ((10 - i) * 1000)), y: 15 });
        }
        return data;
      }())
    },
    {
      name: 'Humidity',
      type: 'spline',
      yAxis: 1,
      color: '#7cb5ec',
      data: (function () {
        var data = [], time = 0, i;
        for (i = 0; i < 10; i += 1) {
          data.push({ x: (time - ((10 - i) * 1000)), y: 50 });
        }
        return data;
      }())
    },
    {
      name: 'Pressure',
      type: 'spline',
      yAxis: 2,
      color: '#90ed7d',
      data: (function () {
        var data = [], time = 0, i;
        for (i = 0; i < 10; i += 1) {
          data.push({ x: (time - ((10 - i) * 1000)), y: 1000 });
        }
        return data;
      }())
    }
    ]
  });
  return chart
}

function setupWS(chart, ws) {
  let mqtt = new Paho.MQTT.Client('localhost', 9001, 'clientjs');

  // set callback handlers
  mqtt.onConnectionLost = onConnectionLost;
  mqtt.onMessageArrived = onMessageArrived;

  let options = { timeout: 3, onSuccess: onConnect };
  mqtt.connect(options);

  function onConnect() {
    console.log('Connected...');
    mqtt.subscribe('esp/hum');
    mqtt.subscribe('esp/pre');
    mqtt.subscribe('esp/tem');
  }

  // called when the client loses its connection
  function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
      console.log("onConnectionLost: " + responseObject.errorMessage);
    }
  }

  // called when a message arrives
  function onMessageArrived(message) {
    let today = new Date();
    let time = today.getTime();

    console.log("onMessageArrived(" + message.destinationName + "):" + message.payloadString);
    value = parseFloat(message.payloadString);

    switch (message.destinationName) {
      case "esp/tem":
        chart.series[0].addPoint([time, value], true, true);
        break;
      case "esp/hum":
        chart.series[1].addPoint([time, value], true, true);
        break;
      case "esp/pre":
        chart.series[2].addPoint([time, value], true, true);
        break;
    }


  }
}

function checkHC() {
  if (typeof Highcharts == "undefined") {
    setTimeout(function () { checkHC() }, 1000);
  } else {
    setupWS(setupHighcharts());
  }
}

function setup() {
  checkHC();
}
