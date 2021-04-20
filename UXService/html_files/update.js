var sessionID;

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

function parseUiUpdate(uiString)
{            
    var response = JSON.parse(uiString);
    
    for(var i = 0; i < response.length; ++i) {
        var obj = response[i];
        var div = document.getElementById(obj["name"]);
        if(!div){
            document.getElementById("uis").innerHTML +='<div id="'+obj["name"]+'"</div>';
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

function updateUI()
{
    var xhttp = new XMLHttpRequest();
    var data = new FormData();
    data.append("session",sessionID);
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
                    parseUiUpdate(xhttp.responseText);
                }
            }
        } 
        
    };  
    xhttp.open('POST', updateURI, true);
    xhttp.send(data);
}