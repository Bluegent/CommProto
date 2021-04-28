$(function() {
$(".list-group-tree").on('click', "[data-toggle=collapse]", function(){
  $(this).toggleClass('in')
	$(this).next(".list-group.collapse").collapse('toggle');
  return false;
})

});

function invoke(event)
{
    let nameOfFunction = event.target.getAttribute('name');
    let conn = event.target.getAttribute('connectionId');
    let controlId = event.target.getAttribute('controlId');
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
    addToken(data);
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
    addToken(data);
    data.append('connection',connection);
    data.append('controlType','button');
    data.append('controlId',button);
    data.append("session",sessionID);
    http.open('POST', 'action', true);
    http.send(data);
} 

function remove(elemId)
{
    
    setInterval(function()
    {
        var notifElem = document.getElementById(elemId);
        if(notifElem!=null)
        {
            notifElem.remove()
        }
    }
    , 1);
    
}

function postNotification(connection,id,elemId,optionStr,actionStr)
{
    var http = new XMLHttpRequest();
    var data = new FormData();
    addToken(data);
    data.append('connection',connection);
    data.append('controlType','notification');
    data.append('controlId',id);
    data.append('option',optionStr);
    data.append('actionId',actionStr);
    data.append("session",sessionID);
    http.open('POST', 'action', true);
    http.send(data);
    remove(elemId);
}

function setSliderValue(sliderId, value)
{
    var num = parseFloat(value,10);
    document.getElementById(sliderId).innerHTML = num.toFixed(1);
}    
    
function postSlider(connection,id,value)
{
    var http = new XMLHttpRequest();
    var data = new FormData();
    addToken(data);
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
    updateNotifCounter();
}




$(document).ready(function(){
    
    $('#connection_lost').modal({
        backdrop: 'static',
        keyboard: false
    })
    
    var token = getToken();
    console.log("hub token = "+token);
    if(token == '')
    {
        window.location.replace("/login.html");
    }
    else
    {
        var data = new FormData();
        data.append('token',token);
        $.ajax({
          url: '/check_token',
          data: data,
          processData: false,
          contentType: false,
          type: 'POST',
          success : function(response)
          {
                startUpdating();
                updateNotifCounter();
          },
          error: function(response){
                window.location.replace("/login.html");
          }
        });
    }
    
    
});
