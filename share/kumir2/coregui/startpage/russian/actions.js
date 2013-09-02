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
    var contents = gui.coursesList();
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
        var onClick = 'gui.showCoursesWindow(' + i + ')';
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
