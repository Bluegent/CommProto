function clearCookie()
{
    Cookies.remove("auth_token");
}

function saveToken(response)
{
   
    if(!$("#remember_me").val())
    {
        Cookies.set("auth_token",response);
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
        return "";
    }
    return cookieToken;
}
