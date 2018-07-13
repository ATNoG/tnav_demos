function setupHighcharts() {
  Highcharts.setOptions({global: {useUTC: false}});
  var cbme = Highcharts.chart('chartBME280', {
    chart: {type: 'spline'},
    title: {text: 'Temperature, Humidity & Pressure (BME 280)'},
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

  var gaugeOptions = {
    chart: {type: 'solidgauge'},
    title: {text: 'Light (BH1750)'},
    pane: {
      center: ['50%', '85%'],
      size: '140%',
      startAngle: -90,
      endAngle: 90,
      background: {
        backgroundColor: (Highcharts.theme && Highcharts.theme.background2) || '#EEE',
        innerRadius: '60%',
        outerRadius: '100%',
        shape: 'arc'
      }
    },
    tooltip: {enabled: false},
    // the value axis
    yAxis: {
      stops: [
        [0.1, '#3D9970'],  // olive
        [0.25, '#2ECC40'], // green
        [0.5, '#FFDC00'],  // yellow
        [0.75, '#FF851B'], // orange
        [0.9, '#FF4136']   // red
      ],
      lineWidth: 0,
      minorTickInterval: null,
      tickPixelInterval: 400,
      tickWidth: 0,
      title: {
        y: -70
      },
      labels: {
        y: 16
      }
    },

    plotOptions: {
      solidgauge: {
        dataLabels: {
          y: 5,
          borderWidth: 0,
          useHTML: true
        }
      }
    }
  };

  var cbh = Highcharts.chart('chartBH1750', Highcharts.merge(gaugeOptions, {
    yAxis: {
      min: 0,
      max: 100,
      title: {text: ''}},
      credits: {enabled: false},
      series: [{
        name: 'light',
        data: [0],
        dataLabels: {
          format: '<div style="text-align:center"><span style="font-size:25px;color:' +
            ((Highcharts.theme && Highcharts.theme.contrastTextColor) || 'black') + '">{y}</span><br/>' +
            '<span style="font-size:12px;color:silver">%</span></div>'
        },
        tooltip: {
          valueSuffix: 'degree C'
        }
      }]
  }));

  return [cbme, cbh];
}

function setupWS(charts, ws) {
  var cbme = charts[0], cbh = charts[1];
  var ws = new WebSocket(ws);
  ws.onopen = function()
  {
    console.log("Subscribe topics: temperature, humidity, pressure and light.");
    ws.send("{\"type\":\"sub\",\"topic\":\"temperature\"}");
    ws.send("{\"type\":\"sub\",\"topic\":\"humidity\"}");
    ws.send("{\"type\":\"sub\",\"topic\":\"pressure\"}");
    ws.send("{\"type\":\"sub\",\"topic\":\"light\"}");
  };

  ws.onmessage = function (evt)
  {
    var received_msg = evt.data;
    //console.log(received_msg);
    var json = JSON.parse(received_msg);
    var ts = json.ts*1000;
    switch(json.topic) {
      case "temperature":
        cbme.series[0].addPoint([ts, json.value], true, true);
        break;
      case "humidity":
        cbme.series[1].addPoint([ts, json.value], true, true);
        break;
      case "pressure":
        cbme.series[2].addPoint([ts, json.value], true, true);
        break;
      case "light":
        cbh.series[0].points[0].update(json.value);
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
