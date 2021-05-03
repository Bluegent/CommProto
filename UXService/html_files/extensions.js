

function makeKnobs()
{
    $(".rotary-container").each(function()
    {
        makeKnob($(this));
    }
    
    );
}

$(document).ready(function(){
    addObserver(makeKnobs);
});

function makeKnob(id)
{
    var knob = id.find('.dial');
    
    var converted = knob[0].hasAttribute('conv');
    if(converted)
    {
        return;
    }
    knob.attr('conv',true);
    
    var min = knob.attr('min');
    var max = knob.attr('max');
    var step = knob.attr('step');
    var fgColor = knob.attr('fgColor');
    var disable = knob.attr('disable') == "true";
    var con_name = knob.attr('connection_name');
    var con_id = knob.attr('control_id');
    console.log(knob);
    
    //console.log(min+" "+max+" "+step+" "+con_name+" "+con_id+" "+disable);
    knob.knob({
        'min':min,
        'max':max,
        'step':step,
        'width':100,
        'height':100,
        'fgColor':fgColor,
        'bgColor':'#aaaaaa',
        'angleOffset':-135,
        'angleArc':275,
        'readOnly':disable,
        'release' : function (v) {postRotary(con_name,con_id,v);}
        });
}

function setRotaryValue(rotaryId, value)
{
    var num = parseFloat(value,10);
    document.getElementById(rotaryId).innerHTML = num.toFixed(1);
}

function postRotary(connection,id,value)
{
    var http = new XMLHttpRequest();
    var data = new FormData();
    addToken(data);
    data.append('connection',connection);
    data.append('controlType','rotary');
    data.append('extension','rotary');
    data.append('controlId',id);
    data.append('value',value);
    data.append("session",sessionID);
    http.open('POST', 'action', true);
    http.send(data);
}