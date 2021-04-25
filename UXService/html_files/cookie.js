
var badToken='';

function saveToken(response)
{
   
    if(!$("#remember_me").val())
    {
        console.log("saving to local variable");
        badToken = response;
        return;
    }
    
    console.log("saving to cookie");
    Cookies.set("auth_token",response,{expires:10});
}

function getCookieToken()
{
    var cookie = Cookies.get('auth_token');
    return cookie;
}

function getToken()
{
    var cookieToken = getCookieToken();
    if(cookieToken == undefined)
    {
        
        console.log("defaulting to local variable");
        return badToken;
    }
    return cookieToken;
}
