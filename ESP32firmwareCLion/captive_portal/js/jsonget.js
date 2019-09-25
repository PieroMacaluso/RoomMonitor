$.get("/data.json", function(data){
    console.log(data);
    document.getElementById("id").value = data.id ? data.id : "";
    document.getElementById("ssid_ap").value = data.ssid_ap ? data.ssid_ap : "";
    document.getElementById("password_ap").value = data.password_ap ? data.password_ap : "";
    document.getElementById("channel").value = data.channel ? data.channel : "";
    document.getElementById("ssid_server").value = data.ssid_server ? data.ssid_server : "";
    document.getElementById("password_server").value = data.password_server ? data.password_server : "";
    document.getElementById("ip_server").value = data.ip_server ? data.ip_server : "";
}, "json");
