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