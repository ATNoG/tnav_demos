function setupHighcharts() {
  Highcharts.setOptions({global: {useUTC: false}});
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
    },
    {
      min:950,
      max:1100,
      labels: {
        format: '{value}hPa',
        style: {
          color: Highcharts.getOptions().colors[2]
        }
      },
      title: {
        text: 'Pressure',
        style: {
          color: Highcharts.getOptions().colors[2]
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
    },
    {name: 'pressure',
      type: 'spline',
      yAxis: 2,
      color: Highcharts.getOptions().colors[2],
      data: (function () {
        var data = [],time = (new Date()).getTime(), i;
        for (i = 0; i < 10; i += 1) {
          data.push({x: (time - ((10-i) * 1000)),y: 1000});
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
    console.log("Subscribe topics: temperature, humidity and pressure.");
    ws.send("{\"type\":\"sub\",\"topic\":\"temperature\"}");
    ws.send("{\"type\":\"sub\",\"topic\":\"humidity\"}");
    ws.send("{\"type\":\"sub\",\"topic\":\"pressure\"}");
  };

  ws.onmessage = function (evt)
  {
    var received_msg = evt.data;
    console.log(received_msg);
    var json = JSON.parse(received_msg);
    var ts = json.ts*1000;
    switch(json.topic) {
      case "temperature":
        chart.series[0].addPoint([ts, json.value], true, true);
        break;
      case "humidity":
        chart.series[1].addPoint([ts, json.value], true, true);
        break;
      case "pressure":
        chart.series[2].addPoint([ts, json.value], true, true);
        break;
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
