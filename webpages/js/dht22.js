function setupHighcharts() {
  Highcharts.setOptions({global: {useUTC: false}});
  var chart = Highcharts.chart('container', {
    chart: {
      type: 'spline'
    },
    title: {
      text: 'Temperature & Humidity (DHT 22)'
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
    }],
    legend: {
      enabled: false
    },
    exporting: {
      enabled: false
    },
    series: [{name: 'Temperature',
      type: 'spline',
      yAxis: 0,
      color: '#f15c80',
      data: (function () {
        var data = [],time = 0, i;
        for (i = 0; i < 10; i += 1) {
          data.push({x: (time - ((10-i) * 1000)),y: 15});
        }
        return data;
      }())
    },
    {name: 'humidity',
      type: 'spline',
      yAxis: 1,
      color: '#7cb5ec',
      data: (function () {
        var data = [],time = 0, i;
        for (i = 0; i < 10; i += 1) {
          data.push({x: (time - ((10-i) * 1000)),y: 50});
        }
        return data;
      }())
    }
    ]
  });
  return chart
}

function setupWS(chart, ws) {
  var ws = new WebSocket(ws);
  ws.onopen = function()
  {
    console.log("Subscribe topics: temperature and humidity.");
    ws.send("{\"type\":\"sub\",\"topic\":\"temperature\"}");
    ws.send("{\"type\":\"sub\",\"topic\":\"humidity\"}");
  };

  ws.onmessage = function (evt)
  {
    var received_msg = evt.data;
    console.log(received_msg);
    var json = JSON.parse(received_msg);
    var ts = json.ts*1000;
    if(json.topic == "temperature") {
      chart.series[0].addPoint([ts, json.value], true, true);
    } else {
      chart.series[1].addPoint([ts, json.value], true, true);
    }
  };

  ws.onclose = function()
  {
    console.log("Connection is closed...");
  };
}

function checkHC(ws) {
  if(typeof Highcharts == "undefined") {
    setTimeout(function() {checkHC(ws)}, 1000);
  } else {
    setupWS(setupHighcharts(), ws);
  }
}

function setup(ws) {
  checkHC(ws);  
}
