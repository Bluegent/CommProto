
function setCheckmark(condition, name)
{
    if (condition) {
        $(name).removeClass();
        $(name).addClass("text-success fa fa-check");

    } else { 
        $(name).removeClass();
        $(name).addClass("text-danger fa fa-times");
    }
}


function pwMatch()
{
    return $("#password").val() == $("#password_repeat").val();
}


function isAlphaNumeric(string)
{
    return string.match(/^[a-zA-Z0-9]+$/);
}

function checkPwMatch()
{
    if($("#password_repeat").val() == "")
        return;
    setCheckmark(pwMatch(),"#pw_repeat_check");
}


function checkUserName()
{
    return isAlphaNumeric($("#username").val()) && $("#username").val().length >= 4;
}

function checkPassword()
{
    return isAlphaNumeric($("#password").val()) && $("#password").val().length >= 8; 
}


function checkUname(evt)
{
    setCheckmark(checkUserName(),"#uname_check");
}

function checkPw(evt)
{
    var condition = checkPassword();
    setCheckmark(condition,"#pw_check");
}

function closeModal()
{
    $('#invalid_input').modal('hide');
}

function showModal(message)
{
    $('#invalid_input_text').html(message);
    $('#invalid_input').modal('show');
}


function submitForm(evt)
{
    evt.preventDefault();
    var match = pwMatch();
    var hasUsername = checkUserName();
    var pwGood = checkPassword();
    if(match && hasUsername && pwGood)
    {
        var data = new FormData();
        data.append("username",$("#username").val());
        data.append("password",$("#password").val());
        data.append("serial",$("#serial").val());
        $.ajax({
          url: '/login',
          data: data,
          processData: false,
          contentType: false,
          type: 'POST',
          success: function(response){
                
                console.log(response);
                var d = new Date();
                var expDays = 10;
                d.setTime(d.getTime() + (expDays*24*60*60*1000));
                var expires = "expires="+ d.toUTCString();
                document.cookie =  "token=" + response + ";" + expires + ";path=/";
                
                window.location.replace("/hub.html");
          },
          error: function(response){
                showModal("Serial code unrecognized.");
          }
        });
    }
    else
    {
        showModal("Invalid input. Pleace check your username and password and make sure they respect the requirements.");
        return false;
    }
}

$(document).ready(function(){
    $("#password, #password_repeat").keyup(checkPwMatch);
    $("#password").keyup(checkPw);
    $("#username").on('input',checkUname);
    $("#signup_form").submit(submitForm);
});