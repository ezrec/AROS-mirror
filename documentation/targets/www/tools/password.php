<html>
<head>
    <title>AROS: Password encryption tool</title>
</head>
<body>
    <?php 
        $salt      = substr( md5( time() . getmypid() ), 0, 2 );
        $encrypted = crypt( $_REQUEST["password"], $salt );
    ?>

    <p>Encrypted password: <?php echo $encrypted; ?></p> 
</body>
</html>
