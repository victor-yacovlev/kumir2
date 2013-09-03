function resizeEventHandler()
{    
    var w = window.innerWidth;
    var h = window.innerHeight;
    var doc = document.getElementById("central_column");
    var footer = document.getElementById("footer");
    var minW = 900;
    if (w < minW) {
        doc.style.display = "none";
    }
    else {
        doc.style.display = "inline-block";
    }
    var minH = 350;
    if (h < minH) {
        footer.style.display = "none";
    }
    else {
        footer.style.display = "block"
    }
}

function loadRecentFiles()
{
    var recentFiles = mainWindow.recentFiles(false);
    var recentFilesFP = mainWindow.recentFiles(true);
    var block = document.getElementById("recent_list");
    block.innerHTML = "";    
    for (var i=0; i<recentFiles.length; i++) {
        var clazz = "";
        if (i == 0) {
            clazz = "first";
        }
        if (i == recentFiles.length - 1) {
            clazz += " last";
        }
        var onClick = 'mainWindow.loadRecentFile("' + recentFilesFP[i] + '")';
        var text = recentFiles[i];
        var line = "<button class='" + clazz + "' onclick='" + onClick + "'>" + text + "</button>\n";
        block.innerHTML += line;
    }
}

function loadHelpContents()
{
    var contents = gui.helpList();
    var block = document.getElementById("help_list");
    block.innerHTML = "";
    for (var i=0; i<contents.length; i++) {
        var clazz = "";
        if (i == 0) {
            clazz = "first";
        }
        if (i == contents.length - 1) {
            clazz += " last";
        }
        var onClick = 'gui.showHelpWindow(' + i + ')';
        var text = contents[i];
        var line = "<button class='" + clazz + "' onclick='" + onClick + "'>" + text + "</button>\n";
        block.innerHTML += line;
    }
}

function loadCourseContents()
{
    var contents = gui.coursesList(false);
    var paths = gui.coursesList(true);
    var block = document.getElementById("courses_list");
    block.innerHTML = "";
    for (var i=0; i<contents.length; i++) {
        var clazz = "";
        if (i == 0) {
            clazz = "first";
        }
        if (i == contents.length - 1) {
            clazz += " last";
        }
        var onClick = 'gui.showCoursesWindow("' + paths[i] + '")';
        var text = contents[i];
        var line = "<button class='" + clazz + "' onclick='" + onClick + "'>" + text + "</button>\n";
        block.innerHTML += line;
    }
}

function init()
{
    resizeEventHandler();
    loadRecentFiles();
    loadHelpContents();
    loadCourseContents();
}

function getRandomInt (min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

function tipOfTheDay()
{
    var ws = gui.wsName();
    var TIPS = ALL;
    if (ws == "x11") {
        TIPS = TIPS.concat(X11);
    }
    else if (ws == "win32") {
        TIPS = TIPS.concat(WIN32);
    }
    else if (ws == "mac") {
        TIPS = TIPS.concat(MAC);
    }
    if (TIPS.length > 0) {
        var index = getRandomInt(0, TIPS.length-1);
        return TIPS[index];
    }
    else {
        return "";
    }    
}
