//function resizeEventHandler()
//{
//     var w = window.innerWidth;
//     var h = window.innerHeight;
//     var doc = document.getElementById("central_column");
//     var footer = document.getElementById("footer");
//     var minW = 900;
//     if (w < minW) {
//         doc.style.display = "none";
//     }
//     else {
//         doc.style.display = "inline-block";
//     }
//     var minH = 350;
//     if (h < minH) {
//         footer.style.display = "none";
//     }
//     else {
//         footer.style.display = "block"
//     }
//}

function loadRecentFiles()
{
    "use strict";
    var recentFiles = [];
    var recentFilesFP = [];
    try {
        var mw = mainWindow;
        recentFiles = mw.recentFiles(false);
        recentFilesFP = mw.recentFiles(true);
    }
    catch (e) {
        // Debugging in Web Browser?
        recentFiles = ["file1.kum", "file2.kum", "file3.kum",
            "file4.kum", "file5.kum", "file6.kum",
            "file7.kum", "file8.kum", "file9.kum",
            "file10.kum", "file11.kum", "file12.kum"
        ];
        recentFilesFP = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"];
    }
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
    "use strict";
    var contents = [];
    try {
        var g = gui;
        contents = g.helpList();
    }
    catch (e) {
        // Not running from Kumir?
        contents = ["Справка 1", "Справка 2", "Справка 3", "Справка 4", "Справка 5", "Справка 6", "Справка 7"];
    }
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
    "use strict";
    var contents = [];
    var paths = [];
    try {
        var g = gui;
        contents = gui.coursesList(false);
        paths = gui.coursesList(true);
    }
    catch (e) {
        // Not running from Kumir?
        contents = ["Практикум 1", "Практикум 2", "Практикум 3", "Практикум 4", "Практикум 5", "Практикум 6", "Практикум 7"];
        paths = ["1", "2", "3", "4", "5", "6", "7"];
    }
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
    //resizeEventHandler();
    loadRecentFiles();
    loadHelpContents();
    loadCourseContents();
}

function updateContents()
{
    loadRecentFiles();
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
        TIPS = TIPS.concat(WIN);
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
