/**
 * Retrieves input data from a form and returns it as a JSON object.
 * @param  {HTMLFormControlsCollection} elements  the form elements
 * @return {Object}                               form data as an object literal
 */
const formToJSON = elements => [].reduce.call(elements, (data, element) => {

    if (element.value) {
        data[element.name] = element.value;
    }
    return data;

}, {});

function validateForm(form) {
    // TODO: Da fare Validazione Dati lato client
    document.getElementById("errors").innerHTML = "";
    // Codice di prova
    var x = form["id"].value;
    if (x === "") {
        document.getElementById("errors").innerHTML += "<li>Inserire campo ID</li>";
        return false;
    }
    return true;
}

/**
 * A handler function to prevent default submission and run our custom script.
 * @param  {Event} event  the submit event triggered by the user
 * @return {void}
 */
const handleFormSubmit = event => {

    // Stop the form from submitting since we’re handling that with AJAX.
    event.preventDefault();

    if (!validateForm(form)) return;
    const data = formToJSON(form.elements);
    const jj = JSON.stringify(data, null, "  ");
    console.log(jj);


    // // TODO: Call our function to get the form data.
    // const data = {};
    //
    // // Demo only: print the form data onscreen as a formatted JSON object.
    // const dataContainer = document.getElementsByClassName('results__display')[0];
    //
    // // Use `JSON.stringify()` to make the output valid, human-readable JSON.
    // dataContainer.textContent = JSON.stringify(data, null, "  ");
    //
    // // ...this is where we’d actually do something with the form data...

    console.log("handle");
    var xhr = new XMLHttpRequest();
    xhr.open(form.method, form.action, true);
    xhr.setRequestHeader('Content-Type', 'application/json; charset=UTF-8');

    // send the collected data as JSON
    xhr.send(JSON.stringify(data));

    xhr.onloadend = function () {
        // done
        console.log("done");
    };
};

const form = document.getElementById('formesp');
form.addEventListener('submit', handleFormSubmit);

$.get("/data.json", function (data) {
    console.log(data);
    document.getElementById("id").value = data.id ? data.id : "";
    document.getElementById("ssid_ap").value = data.ssid_ap ? data.ssid_ap : "";
    document.getElementById("password_ap").value = data.password_ap ? data.password_ap : "";
    document.getElementById("channel").value = data.channel ? data.channel : "";
    document.getElementById("ssid_server").value = data.ssid_server ? data.ssid_server : "";
    document.getElementById("password_server").value = data.password_server ? data.password_server : "";
    document.getElementById("ip_server").value = data.ip_server ? data.ip_server : "";
}, "json");