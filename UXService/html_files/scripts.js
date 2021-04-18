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
        window[nameOfFunction](conn,controlId,elementId,optionStr);
    }
}


function postToggle(toggle, connection, id)
{
    var http = new XMLHttpRequest();
    var data = new FormData();
    data.append('connection',connection);
    data.append('controlType','toggle');
    data.append('controlId',id);
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
    http.open('POST', 'action', true);
    http.send(data);
}

var updateURI = "update";
function forceUpdateUI()
{
    updateURI = "update-force";
    updateUI();
    updateURI = "update";
}    

var notificationURI = "notification";
function forceNotifications()
{
    notificationURI = "notification-force";
    getNotifications();
    notificationURI = "notification";
}  

function postNotification(connection,id, elemId,optionStr)
{
    document.getElementById(elemId).remove();
    var http = new XMLHttpRequest();
    var data = new FormData();
    data.append('connection',connection);
    data.append('controlType','notification');
    data.append('controlId',id);
    data.append('option',optionStr);
    http.open('POST', 'action', true);
    http.send(data);
}

function getNotifications()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() 
    {
        if (this.readyState == 4)
        {
            if(this.status == 200)
            {
                if(xhttp.responseText == '<null>')
                {
                } 
                else 
                {
                   console.log('updating notifications');
                   document.getElementById('notifications').innerHTML = xhttp.responseText;
                }
            }
        } 
        
    };
    xhttp.open('POST', notificationURI, true);
    xhttp.send();
}

function updateUI()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() 
    {
        if (this.readyState == 4)
        {
            if(this.status == 200)
            {
                if(xhttp.responseText == '<null>')
                {
                } 
                else 
                {
                    console.log('updating UI');
                    
                    var response = JSON.parse(xhttp.responseText);
                    for(var i = 0; i < response.length; i++) {
                        var obj = response[i];
                        var div = document.getElementById(obj["name"]);
                        if(!div){
                            document.getElementById("uis").innerHTML +='<div id="'+obj["name"]+'"</div>';
                            div = document.getElementById(obj["name"]);
                        }
                        
                        div.innerHTML = obj["ui"];
                    }                   
                }
            }
        } 
        
    };  
    xhttp.open('POST', updateURI, true);
    xhttp.send();
}

function startUpdating()
{
    forceUpdateUI();
    forceNotifications();

    var updateUiId = setInterval(function() {updateUI();}, 1000);

    var notificationsId = setInterval(function() {getNotifications();}, 1000);
}

startUpdating();

