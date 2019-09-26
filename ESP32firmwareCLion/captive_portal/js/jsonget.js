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
    const id_regex = new RegExp(/^[1-9][0-9]*$/);
    const ssid_regex = new RegExp(/^[A-Za-z0-9]{1,32}$/);
    const channel_regex = new RegExp(/^[0-9]$|^1[0-4]$/);
    const ip_regex = new RegExp(/^(?=.*[^\.]$)((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.?){4}$/);
    const password_regex = new RegExp(/^(?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,63}$/);
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

    // Decommentato controllo password server poichè non la complessità non deve essere imposta dal client
    // if (!password_regex.test(form["password_server"].value)) {
    //     document.getElementById("errors").innerHTML += "<li>La password deve contenere almeno una lettera maiuscola, una lettera minuscola e un numero</li>";
    //     status = false;
    // }
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