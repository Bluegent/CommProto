var sessionID;
var selectedController="Service::Diagnosis";

function generateSessionId()
{
    sessionID = "session-" + Date.now()+ "-"+ Math.floor(Math.random()*25565);
}

function shakeNotif()
{
     var notifAmount = $(".notif_container").length;
     if(notifAmount !=0)
     {
        $("#notif_counter").effect('shake',{direction:'left', distance:3,times:1}, 200);
     }
}

function updateNotifCounter()
{
    var notifAmount = $(".notif_container").length;
    var counter = $("#notif_counter");
    counter.html(notifAmount);
    if(notifAmount !=0)
    {
        counter.show();
    }
    else
    {
        counter.hide();
    }
}

function selectController(id)
{
    selectedController = id;
    forceUpdateUI();
       
    var myOffcanvas = document.getElementById('controllers_bar');
    var bsOffcanvas = bootstrap.Offcanvas.getInstance(myOffcanvas);
    bsOffcanvas.hide();
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

function parseRemovals(removeJSON)
{
    console.log("Parsing removals");
    for(var i = 0; i < removeJSON.length; ++i) 
    {
        console.log(removeJSON[i]);
        var elementId = removeJSON[i];
        var realElement = document.getElementById(elementId);
        if(realElement !=null)
            realElement.remove();
    }
    updateNotifCounter();
}

function parseNotifications(notificationsJSON)
{
    var notifDiv = document.getElementById("notifications");
    var added =0;
    for(var i = 0; i < notificationsJSON.length; ++i) 
    {
        ++added;
        var notifJSON = notificationsJSON[i];
        notifDiv.innerHTML += notifJSON["notification"];
    }
    updateNotifCounter();
    if(added!=0)
    {
        shakeNotif()
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
        
    var removals = response["removals"];  
    if(removals != null)
        parseRemovals(removals);
           
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

