/**
 * Add gobals here
 */

const wifi_creds = document.getElementById("WiFiConnect");
const simsim_co = document.getElementById("Simsim_co");
const network_config = document.getElementById("Network");
const addsensor = document.getElementById("AddSensor");


/**
 * Initialize functions here.
 */
$(document).ready(function(){
    $("#save_network_config").on("click", function () {
        save_network_Configurations();
    });
    getConfig();
});   


/**
 * Save config helper function.
 */
 function save_network_Configurations() {
    // Get the configuration values
    ObyektName = $("#obyekt_name").val();
    HududName = $("#hudud_name").val();
    TumanName = $("#tuman_name").val();
    Chastota = $("#chastota_value").val();
    Apn = $("#apn_name").val();

    $.ajax({
        url: '/saveNetworkConfig.json',
        dataType: 'json',
        method: 'POST',
        cache: false,
        headers: {
            'obyekt-name': ObyektName,
            'hudud-name': HududName,
            'tuman-name': TumanName,
            'chastota-value': Chastota,
            'apn-name': Apn,
        },
        data: { 'timestamp': Date.now() }
    });

}

/**
 * Gets configuration values
 */
function getConfig()
{
    $.getJSON('/getConfig.json', function(data)
    {
        document.getElementById("obyekt_name").value = data["obyekt"];
        document.getElementById("hudud_name").value = data["hudud"];
        document.getElementById("tuman_name").value = data["tuman"];
        document.getElementById("chastota_value").value = data["chastota"];
        document.getElementById("apn_name").value = data["apn"];
    });
}