var sessionID;
var selectedController="main";

function generateSessionId()
{
    sessionID = "session-" + Date.now()+ "-"+ Math.floor(Math.random()*25565);
}

var updateURI = "update";
function forceUpdateUI()
{
    generateSessionId();
    updateURI = "update-force";
    updateUI();
    updateURI = "update";
}    


function parseControllers(controllersJSON)
{
    for(var i = 0; i < controllersJSON.length; ++i) 
    {
        var obj = controllersJSON[i];
        var div = document.getElementById(obj["name"]);
        if(!div)
        {
            document.getElementById("controller-display").innerHTML +='<div class="d-flex flex-column" id="'+obj["name"]+'"</div>';
            div = document.getElementById(obj["name"]);
        }
        var updates = obj["updates"];
        
        for(var j =0; j < updates.length;++j)
        {
            var update = updates[j];
            var control = document.getElementById(update["element"]);      
            if(!control)
            {
                div.innerHTML+='<div id="'+update["element"]+'"</div>';
                control = document.getElementById(update["element"]);
            }
            control.innerHTML = update["controlString"];            
        }
    }
}

function parseNotifications(notificationsJSON)
{
    var notifDiv = document.getElementById("notifications");
    for(var i = 0; i < notificationsJSON.length; ++i) 
    {
        var notifJSON = notificationsJSON[i];
        notifDiv.innerHTML += notifJSON["notification"];
    }
}

function parseUiUpdate(uiString)
{            
    var response = JSON.parse(uiString);
    
    var controllers = response["controllers"];
    
    var ui = response["controllers"];
    if(ui != null)
        parseControllers(ui);
        
    var notifications = response["notifications"];  
    if(notifications != null)
        parseNotifications(notifications);
           
}

function updateUI()
{
    var xhttp = new XMLHttpRequest();
    var data = new FormData();
    data.append("session",sessionID);
    data.append("selected",selectedController);
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
                    parseUiUpdate(xhttp.responseText);
                }
            }
        } 
        
    };  
    xhttp.open('POST', updateURI, true);
    xhttp.send(data);
}