
var darkMode = true;

function toLightMode()
{
    $(".bg-gray" ).addClass("bg-light-gray").removeClass("bg-gray");    
    $(".bg-dark" ).addClass("bg-light").removeClass("bg-dark");   
    $(".text-light").addClass("text-dark").removeClass("text-light");
    $(".btn-secondary").addClass("btn-primary fakePrimary").removeClass("btn-secondary");
}

function toDarkMode()
{
    $(".bg-light" ).addClass("bg-dark").removeClass("bg-light");    
    $(".bg-light-gray" ).addClass("bg-gray").removeClass("bg-light-gray");   
    $(".text-dark").addClass("text-light").removeClass("text-dark");
    $(".fakePrimary").removeClass("btn-primary fakePrimary").addClass("btn-secondary");
}

function updateMode()
{
    if(darkMode)
    {
        toDarkMode();
    }
    else
    {
        toLightMode();
    }
}

function toggleMode(box)
{
    var mode = box.checked;
    console.log(mode);
    darkMode = mode;
    updateMode();
}