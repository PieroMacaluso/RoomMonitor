$.getJSON("/data.json", function(json) {
    console.log(json); // this will show the info it in firebug console
    var str = JSON.stringify(json, null, 2);
    str = str.replace(/,/g, "<br>");
    str = str.replace("{", "");
    str = str.replace("}", "");
    document.getElementById("json").appendChild(document.createElement('pre')).innerHTML = str;
});