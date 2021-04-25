function isAlphaNumeric(string)
{
    return string.match(/^[a-zA-Z0-9]+$/);
}

function checkUserName()
{
    return isAlphaNumeric($("#username").val()) && $("#username").val().length >= 4;
}

function checkPassword()
{
    return isAlphaNumeric($("#password").val()) && $("#password").val().length >= 8; 
}

function submitForm(evt)
{
    evt.preventDefault();
    var hasUsername = checkUserName();
    var pwGood = checkPassword();
    if(hasUsername && pwGood)
    {
        var data = new FormData();
        data.append("username",$("#username").val());
        data.append("password",$("#password").val());
        $.ajax({
          url: '/login',
          data: data,
          processData: false,
          contentType: false,
          type: 'POST',
          success: function(response){               
                console.log('saving token ="'+response+'"');
                saveToken(response);
                //window.location.replace("/hub.html");
          },
          error: function(response){
                showModal("Wrong username or password, try again.");
          }
        });
    }
    else
    {
        showModal("Invalid input. Pleace check your username and password and make sure they respect the requirements.");
        return false;
    }
}



$(document).ready(function()
{
    var token = getToken();
    console.log('checking token response= "'+token+'"');
    if(token != '')
    {   
        $("#login_form_container").hide();
        var data = new FormData();
        data.append('token',token);
        $.ajax({
          url: '/login',
          data: data,
          processData: false,
          contentType: false,
          type: 'POST',
          success: function(response){               
                console.log("valid token response!");
                window.location.replace("/hub.html");
          },
          error: function(response){
                console.log("invalid token, oops");
                $("#login_form_container").show();
          }
        });
    }
    $("#signup_form").submit(submitForm);
});