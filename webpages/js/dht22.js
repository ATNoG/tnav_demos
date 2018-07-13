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
          color: Highcharts.getOptions().colors[5]
        }
      },
      title: {
        text: 'Temperature',
        style: {
          color: Highcharts.getOptions().colors[5]
        }
      }
    },
    { 
      min: 0,
      max: 100,
      labels: {
        format: '{value}%',
        style: {
          color: Highcharts.getOptions().colors[0]
        }
      },
      title: {
        text: 'Humidity',
        style: {
          color: Highcharts.getOptions().colors[0]
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
      color: Highcharts.getOptions().colors[5],
      data: (function () {
        var data = [],time = (new Date()).getTime(), i;
        for (i = 0; i < 10; i += 1) {
          data.push({x: (time - ((10-i) * 1000)),y: 15});
        }
        return data;
      }())
    },
    {name: 'humidity',
      type: 'spline',
      yAxis: 1,
      color: Highcharts.getOptions().colors[0],
      data: (function () {
        var data = [],time = (new Date()).getTime(), i;
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

function setup(ws) {
  setupWS(setupHighcharts(), ws);
}
