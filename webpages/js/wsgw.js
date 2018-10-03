function setup(ws) {
  setupWS(ws);
}

function requestStatus(ws) {
  console.log("Request status...")
  ws.send("{\"type\":\"status\"}");
  setTimeout(function() {requestStatus(ws)}, 3000);
}

function setupWS(ws) {
  var ws = new WebSocket(ws);
  ws.onopen = function()
  {
    console.log("Connection open.");
    requestStatus(ws);
  };

  ws.onmessage = function (evt)
  {
    var received_msg = evt.data;
    console.log(received_msg);
    var topics = JSON.parse(received_msg).topics;

    var newtbody = document.createElement('tbody');

    // Add Number of Topics
    var ntopics = document.createElement("tr");
    var nt1 = document.createElement("td");
    var nt2 = document.createElement("td");
    nt1.appendChild(document.createTextNode("Number of topics:"));
    nt2.appendChild(document.createTextNode(topics.length));
    ntopics.appendChild(nt1);
    ntopics.appendChild(nt2);
    newtbody.appendChild(ntopics);

    var trow = document.createElement("tr");
    var tcol = document.createElement("td");
    tcol.appendChild(document.createTextNode("Topics:"));
    trow.appendChild(tcol);
    newtbody.appendChild(trow);
    console.log(topics)

      // Add Topics and Connections
      for(i in topics)
      {
        console.log(topics[i])
          var row1 = document.createElement("tr");
        var c11 = document.createElement("td");
        var c12 = document.createElement("td");
        c11.appendChild(document.createTextNode(topics[i].topic));
        c12.appendChild(document.createTextNode(topics[i].queue));
        row1.appendChild(c11);
        row1.appendChild(c12);
        newtbody.appendChild(row1);

        var row2 = document.createElement("tr");
        var c21 = document.createElement("td");
        c21.appendChild(document.createTextNode("Connections:"));
        row2.appendChild(c21);
        var connections = topics[i].connections;
        for(j in connections) {
          var cl = document.createElement("td");
          cl.appendChild(document.createTextNode(connections[j]));
          row2.appendChild(cl);
        }
        newtbody.appendChild(row2);
      }

    // Replace tbody
    var oldtbody = document.getElementsByTagName("tbody").item(0);
    oldtbody.parentNode.replaceChild(newtbody, oldtbody);
  };

  ws.onclose = function()
  {
    console.log("Connection is closed...");
  };

  return ws;
}
