function invoke(event)
{
    let nameOfFunction = event.target.getAttribute('name');
    let conn = event.target.getAttribute('connectionId');
    let controlId = event.target.getAttribute('controlId');
    console.log(nameOfFunction);
    console.log(conn);
    console.log(controlId);
    if(nameOfFunction == "postNotification")
    {
        let elementId = event.target.getAttribute('elementId');
        let optionStr = event.target.getAttribute('optionStr');
        let actionStr = event.target.getAttribute('actionId');
        window[nameOfFunction](conn,controlId,elementId,optionStr,actionStr);
    }
}


function postToggle(toggle, connection, id)
{
    var http = new XMLHttpRequest();
    var data = new FormData();
    data.append('connection',connection);
    data.append('controlType','toggle');
    data.append('controlId',id);
    data.append("session",sessionID);
    http.open('POST', 'action', true);
    http.send(data);
}

function postButton(connection,button)
{
    var http = new XMLHttpRequest();
    var data = new FormData();
    data.append('connection',connection);
    data.append('controlType','button');
    data.append('controlId',button);
    data.append("session",sessionID);
    http.open('POST', 'action', true);
    http.send(data);
} 

function postNotification(connection,id,elemId,optionStr,actionStr)
{
    document.getElementById(elemId).remove();
    var http = new XMLHttpRequest();
    var data = new FormData();
    data.append('connection',connection);
    data.append('controlType','notification');
    data.append('controlId',id);
    data.append('option',optionStr);
    data.append('actionId',actionStr);
    data.append("session",sessionID);
    http.open('POST', 'action', true);
    http.send(data);
}

function setSliderValue(sliderId, value)
{
    document.getElementById(sliderId).innerHTML = value;
}    
    
function postSlider(connection,id,value)
{
    var http = new XMLHttpRequest();
    var data = new FormData();
    data.append('connection',connection);
    data.append('controlType','slider');
    data.append('controlId',id);
    data.append('value',value);
    data.append("session",sessionID);
    http.open('POST', 'action', true);
    http.send(data);
}

function startUpdating()
{
    forceUpdateUI();

    var updateUiId = setInterval(function() {updateUI();}, 1000);
}

startUpdating();

