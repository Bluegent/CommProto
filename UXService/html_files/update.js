var sessionID;
var selectedController="Service::Diagnosis";

function generateSessionId()
{
    sessionID = "session-" + Date.now()+ "-"+ Math.floor(Math.random()*25565);
}

function selectController(id)
{
    selectedController = id;
    forceUpdateUI();
}

var updateURI = "update";
function forceUpdateUI()
{
    generateSessionId();
    updateURI = "update-force";
    updateUI();
    updateURI = "update";
}    


function parseController(controllerJSON)
{
    var controllerName = controllerJSON["name"];
    var div = document.getElementById(controllerName);
    if(!div)
    {
        document.getElementById("controller-display").innerHTML ='<div class="col" id="'+controllerName+'"</div>';
        div = document.getElementById(controllerName);
    }
    var updates = controllerJSON["updates"];
    
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

function parseNotifications(notificationsJSON)
{
    var notifDiv = document.getElementById("notifications");
    for(var i = 0; i < notificationsJSON.length; ++i) 
    {
        var notifJSON = notificationsJSON[i];
        notifDiv.innerHTML += notifJSON["notification"];
    }
}


function removeNonContained(controllerButton, controllersJSON)
{
    for(var i=0;i<controllersJSON.length;++i)
    {
        
        var buttonName = "btn-" + controllersJSON[i]["name"];
        if( buttonName == controllerButton.id)
        {
            return;
        }
    }
    controllerButton.remove();
}
    
function getCategory(string)
{
    if(string.startsWith('Service'))
    {
        return 'service_btns';
    }
    if(string.startsWith('Endpoint'))
    {
        return 'endpoint_btns';
    }
    return 'endpoint_btns';
}

function parseControllers(controllersJSON)
{
    var controllers = document.getElementsByClassName("ctrl_selector");
    for(var i=0;i<controllers.length;++i)
    {
        removeNonContained(controllers[i],controllersJSON);
    }
    
    for(var i=0;i<controllersJSON.length;++i)
    {
        var name = "btn-"+controllersJSON[i]["name"];        
        var ctrlBtn = document.getElementById(name);
        if(ctrlBtn == null)
        {
            document.getElementById(getCategory(name)).innerHTML += controllersJSON[i]["control_string"];
        }
    }
    
    
}

function parseUiUpdate(uiString)
{            
    var response = JSON.parse(uiString);
    
    var controllers = response["controllers"];
    if(controllers !=null)
        parseControllers(controllers);
    
    var ui = response["controller"];
    if(ui != null)
        parseController(ui);
        
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
                parseUiUpdate(xhttp.responseText);                
            }
        } 
        
    };  
    xhttp.open('POST', updateURI, true);
    xhttp.send(data);
}