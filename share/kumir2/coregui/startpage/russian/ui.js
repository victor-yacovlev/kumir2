function setScrollState() {
    "use strict";
    var scrollPos = $(window).scrollTop();
    var documentHeight = $(document).height();
    var boxAlpha = 0.0;
    if (scrollPos) {
        boxAlpha = 0.2 + scrollPos / documentHeight;
    }
    var boxShadowValue = "0 0 10px rgba(0,0,0," + boxAlpha +")";
    $("#header").css("box-shadow", boxShadowValue);
}

function showApplicationVersion() {
    "use strict";
    var versionString = "9.9.99";
    try {
        var g = gui;
        versionString = g.applicationVersionString();
    }
    catch (e) {
        // Not running from Kumir?
    }
    $("#version_part")[0].innerText = versionString;
}