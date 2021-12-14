<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="5" >
    <link rel="stylesheet" type="text/css" href="style.css" media="screen"/>

	<title> Weight Data </title>

</head>

<body>
    <h1>PET DATA</h1>
<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "weight";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {

    die("Connection failed: ".$conn->connect_error);
}

$sql = "SELECT * FROM weightData ORDER BY id ASC"; /*select items to display from the vehicleinformation table in the data base*/

echo '<table cellspacing="5" cellpadding="5">
      <tr>
        <th>ID</th>
          <th>date and time</th>
          <th>petWeight</th>
        <th>feedWeight</th>
      </tr>';

if ($result = $conn->query($sql)) {    //we ask if we have data in our database
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];//in my row array i get the data in 'id
        $row_reading_time = $row["reading_time"];
        $row_petWeight = $row["petWeight"];
        $row_feedWeight = $row["feedWeight"];


        echo '<tr>
                <td>' . $row_id . '</td>
                <td>' . $row_reading_time . '</td>
                <td>' . $row_petWeight. '</td>
                <td>' . $row_feedWeight. '</td>

              </tr>';
    }
    $result->free();
}
$conn->close();
?>
</table>
</body>
</html>
	</body>
</html>
