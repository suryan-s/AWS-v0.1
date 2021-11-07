<?php


$servername = "localhost";

// REPLACE with your Database name
$dbname = "XXXXX";
// REPLACE with Database user
$username = "XXXXXXX";
// REPLACE with Database user password
$password = "XXXXXXX";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "XXXXXXXX";

$api_key= $sensor = $location = $Temperature = $Humidity = $Pressure = $UV_index = $Light_LUX ="";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $sensor = test_input($_POST["sensor"]);
        $location = test_input($_POST["location"]);
        $Temperature = test_input($_POST["Temperature"]);
        $Humidity = test_input($_POST["Humidity"]);
        $Pressure = test_input($_POST["Pressure"]);
        $UV_index = test_input($_POST["UV_index"]);
        $Light_LUX = test_input($_POST["Light_LUX"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO SensorData (sensor, location, Temperature, Humidity, Pressure,UV_index,LIght_LUX)
        VALUES ('" . $sensor . "', '" . $location . "', '" . $Temperature . "', '" . $Humidity . "', '" . $Pressure . "', '" . $UV_index . "', '" . $Light_LUX . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>