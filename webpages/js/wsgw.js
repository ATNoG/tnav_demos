var ws;

function setup(ws) {
  setupWS(ws);
}

function requestStatus(ws) {
  console.log("Request status...")
  ws.send("{\"type\":\"status\"}");
  setTimeout(function () { requestStatus(ws) }, 3000);
}

function ReleaseAll() {
  console.log("ReleaseAll")
  msg = { type: 'releaseall' }
  ws.send(JSON.stringify(msg))
}

function Shutdown() {
  console.log("Shutdown")
  msg = { type: 'shutdown' }
  ws.send(JSON.stringify(msg))
}

function setupWS(address) {
  ws = new WebSocket(address);
  ws.onopen = function () {
    console.log("Connection open.");
    requestStatus(ws);
  };

  ws.onmessage = function (evt) {
    let received_msg = evt.data;
    console.log(received_msg);
    let topics = JSON.parse(received_msg).topics;

    let table = document.createElement('tbody');

    // Add Number of Topics
    let ntopics = table.insertRow(-1);
    let nt1 = ntopics.insertCell(-1);
    let nt2 = ntopics.insertCell(-1);
    nt1.innerHTML = "Number of topics:";
    nt2.innerHTML = topics.length;

    let trow = table.insertRow(-1);
    let tcol = trow.insertCell(-1);
    tcol.innerHTML = "Topics:";

    console.log(topics)

    // Add Topics and Connections
    for (i in topics) {
      console.log(topics[i])
      var row1 = table.insertRow(-1);
      var c11 = row1.insertCell(-1);
      var c12 = row1.insertCell(-1);
      c11.innerHTML = topics[i].topic;
      c12.innerHTML = topics[i].queue;


      var row2 = table.insertRow(-1);
      var c21 = row2.insertCell(-1);
      c21.innerHTML = "Connections:";
      var connections = topics[i].connections;
      for (j in connections) {
        var cl = row2.insertCell(-1);
        cl.innerHTML = connections[j];
      }
    }

    // Replace tbody
    var oldtbody = document.getElementsByTagName("tbody").item(0);
    oldtbody.parentNode.replaceChild(table, oldtbody);
  };

  ws.onclose = function () {
    console.log("Connection is closed...");
  };

  return ws;
}
