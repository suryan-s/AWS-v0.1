<!DOCTYPE html>
<html><body>
<?php


$servername = "localhost";

// REPLACE with your Database name with id
$dbname = "XXXXXXXXXXXXXXX";
// REPLACE with Database user with id
$username = "XXXXXXXXXXXXXXX";
// REPLACE with Database user password
$password = "XXXXXXXXXXXXXXX";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id,reading_time, sensor, location, Temperature, Humidity, Pressure,UV_index,Light_LUX  FROM SensorData ORDER BY id DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td>
        <td>Timestamp</td> 
        <td>Sensor</td> 
        <td>Location</td> 
        <td>Temperature</td> 
        <td>Humidity</td>
        <td>Pressure</td> 
        <td>UV_index</td> 
        <td>Light_LUX</td>          
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_reading_time = $row["reading_time"];
        $row_sensor = $row["sensor"];
        $row_location = $row["location"];
        $row_Temperature = $row["Temperature"];
        $row_Humidity = $row["Humidity"]; 
        $row_Pressure = $row["Pressure"]; 
        $row_UV_index = $row["UV_index"]; 
        $row_Light_LUX = $row["Light_LUX"]; 
        
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));

        //for users in india,
        ////$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 5 hours + 30 minutes"));
      
        echo '<tr> 
                <td>' . $row_id . '</td>
                <td>' . $row_reading_time . '</td>  
                <td>' . $row_sensor . '</td> 
                <td>' . $row_location . '</td> 
                <td>' . $row_Temperature . '</td> 
                <td>' . $row_Humidity . '</td>
                <td>' . $row_Pressure . '</td> 
                <td>' . $row_UV_index . '</td> 
                <td>' . $row_Light_LUX . '</td> 
                
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>