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
    var status = true;
    const id_regex = new RegExp(/^[0-9][0-9]*$/);
    const ssid_regex = new RegExp(/^[A-Za-z0-9]{1,32}$/);
    const channel_regex = new RegExp(/^[0-9]$|^1[0-4]$/);
    const ip_regex = new RegExp(/^(?=.*[^\.]$)((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.?){4}$/);
    const password_regex = new RegExp(/^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,63}$/);
    const port_regex = new RegExp(/^()([1-9]|[1-5]?[0-9]{2,4}|6[1-4][0-9]{3}|65[1-4][0-9]{2}|655[1-2][0-9]|6553[1-5])$/);
    // TODO: Controllare validazione e esportare nel Server (Schedina)
    document.getElementById("errors").innerHTML = "";
    // Codice di prova
    if (!id_regex.test(form["id"].value)) {
        document.getElementById("errors").innerHTML += "<li>Campo ID deve essere un numero intero</li>";
        status = false;
    }
    if (!ssid_regex.test(form["ssid_ap"].value)) {
        document.getElementById("errors").innerHTML += "<li>Inserire un Campo SSID AP valido di minimo 1 e massimo 32 caratteri alfanumerici</li>";
        status = false;
    }
    if (!ssid_regex.test(form["ssid_server"].value)) {
        document.getElementById("errors").innerHTML += "<li>Inserire un Campo SSID Server valido di minimo 1 e massimo 32 caratteri alfanumerici</li>";
        status = false;
    }
    if (!password_regex.test(form["password_ap"].value)) {
        document.getElementById("errors").innerHTML += "<li>La password deve contenere almeno una lettera maiuscola, una lettera minuscola e un numero</li>";
        status = false;
    }
    if (!channel_regex.test(form["channel"].value)) {
        document.getElementById("errors").innerHTML += "<li>Canale selezionato non valido</li>";
        status = false;
    }
    if (!port_regex.test(form["port_server"].value)) {
        document.getElementById("errors").innerHTML += "<li>Porta non valida</li>";
        status = false;
    }
    if (!ip_regex.test(form["ip_server"].value)) {
        document.getElementById("errors").innerHTML += "<li>Inserire un indirizzo IP valido</li>";
        status = false;
    }
    return status;
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
    document.getElementById("submit").textContent = "Loading...";
    const data = formToJSON(form.elements);
    const jj = JSON.stringify(data, null, "  ");
    console.log(jj);
    console.log("handle");
    var xhr = new XMLHttpRequest();
    xhr.open(form.method, form.action, true);
    xhr.setRequestHeader('Content-Type', 'application/json; charset=UTF-8');
    xhr.addEventListener("load", transferComplete);
    xhr.addEventListener("error", transferFailed);
    // send the collected data as JSON
    xhr.send(JSON.stringify(data));
};

function transferComplete(evt) {
    // $('#success').modal(options);
    document.getElementById("submit").textContent = "Setup Completed!";
    document.getElementById("submit").style.backgroundColor = "#0d881b";
    document.getElementById("submit").style.borderColor = "#0d881b";
}

function transferFailed(evt) {
    // $('#unsuccess').modal(options);
    document.getElementById("submit").textContent = "Setup Failed";
    document.getElementById("submit").style.backgroundColor = "#880d0d";
    document.getElementById("submit").style.borderColor = "#880d0d";
}

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
    document.getElementById("port_server").value = data.port_server ? data.port_server : "";
}, "json");
