<?php
    header("Content-Type: text/html;charset=utf-8");
    ini_set("display_errors", "On");
    error_reporting(E_ALL);

    // initialize connection parameters, for security must be blank
    $server="";
    $sql="";
    $usr_bd="";
    $pwd_bd="";
    $bd="";

    // receive the parameters either via POST or GET
    if (isset($_POST["sql"]))
    {
        $sql=$_POST["sql"];
    }
    else if (isset($_GET["sql"]))
    {
        $sql=$_POST["sql"];
    }
    if (isset($_POST["usr_bd"]))
    {
        $usr_bd=$_POST["usr_bd"];
    }
    else if (isset($_POST["usr_bd"]))
    {
        $usr_bd=$_POST["usr_bd"];
    }
    if (isset($_POST["pwd_bd"]))
    {
        $pwd_bd=$_POST["pwd_bd"];
    }
    else if (isset($_POST["pwd_bd"]))
    {
        $pwd_bd=$_POST["pwd_bd"];
    }
    if (isset($_POST["bd"]))
    {
        $bd=$_POST["bd"];
    }
    else if (isset($_POST["bd"]))
    {
        $bd=$_POST["bd"];
    }
    if (isset($_POST["server"]))
    {
        $server=$_POST["server"];
    }
    else if (isset($_POST["server"]))
    {
        $server=$_POST["server"];
    }

    $mysqli_main = mysqli_connect($server, $usr_bd, $pwd_bd, $bd); // connect to the database server
    if (mysqli_connect_errno())
    {
        printf("Fallo al conectar: %s<br>", mysqli_connect_error());
        exit();
    }
    mysqli_query($mysqli_main, "set names 'utf8'"); // handle data in utf8

    $query_main = mysqli_query($mysqli_main, $sql);   // do SQL query
    if ($query_main==FALSE) // si algo salió mal
    {
        mysqli_close($mysqli_main);
        echo "error sql"; // return error string
        return;
    }
    
    $insertIntoPos=strpos($sql, " into "); // was an insert into query
    if ($insertIntoPos>0)
    {
        $lastID = mysqli_insert_id($mysqli_main); // get last index
        echo $lastID; // return or print last index
        return; // finish
    }

    $i=0;
    $arreglo=array(); // pass the returned information to an array
    while ($row=mysqli_fetch_array($query_main, MYSQLI_ASSOC))
    {
        $arreglo[$i++]=$row; // rown by row
    }
    mysqli_close($mysqli_main); // do not forget close connection

    if ($i==0)
    {
        echo ""; // empty dataset
    }
    else
    {
        $json=json_encode($arreglo); // parse to JSON format
        echo $json; // return or print this JSON
    }
?>
