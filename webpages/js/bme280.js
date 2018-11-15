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
      min:950,
      max:1100,
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
    series: [{name: 'Temperature',
      type: 'spline',
      yAxis: 0,
      color: '#f15c80',
      data: (function () {
        var data = [], time = 0, i;
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
        var data = [], time = 0, i;
        for (i = 0; i < 10; i += 1) {
          data.push({x: (time - ((10-i) * 1000)),y: 50});
        }
        return data;
      }())
    },
    {name: 'pressure',
      type: 'spline',
      yAxis: 2,
      color: '#90ed7d',
      data: (function () {
        var data = [], time = 0, i;
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
