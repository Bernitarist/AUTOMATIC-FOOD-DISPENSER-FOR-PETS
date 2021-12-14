<?php
$dbServername = "localhost";
$dbUsername = "root";
$dbPassword = "";
$dbName = "weight";
// Create connection
$conn = mysqli_connect($dbServername, $dbUsername, $dbPassword, $dbName);

        if ($conn->connect_error) {
            die ("Connection failed: " .$conn->connect_error);
        }

       if (isset($_POST["petWeight"]) && $feedWeight=$_POST['feedWeight']) {

            $petWeight=$_POST['petWeight'];
            $feedWeight=$_POST['feedWeight'];


            $sql="INSERT INTO weightData(petWeight,feedWeight) VALUES('$petWeight','$feedWeight');";
            mysqli_query($conn,$sql);
         }
else { echo"name empty";    }

      ?>
