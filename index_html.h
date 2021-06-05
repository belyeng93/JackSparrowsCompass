const char index_html[] PROGMEM = 
R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Bussola</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>

	html {font-family: Arial; display: inline-block; text-align: center;background-color: #e6e6e6;}
	body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}

    p{font-size: 14px;color: #888;margin-bottom: 10px;}
    
    h1 {color: #444444;margin: 50px auto 30px;} 
	h2 {font-size: 3.0rem;}
    h3 {color: #444444;margin-bottom: 50px;}

    .button 
    {
        background-color: #1abc9c;
        border: none;
        color: white;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 5px auto 10px; 
        cursor: pointer;
        border-radius: 4px;
        -ms-transform: translateX(-50%);
        transform: translateX(10%);
    }
    .button:active {background-color: #16a085;}

    
    .dot {height: 25px; width: 25px; background-color: #bbb; border-radius: 50%; display: inline-block; }
    .name{font-size: 3.0rem;}
    .target
    {
        text-align: left;
        display:inline-block;
    }
    .title
    {
        text-align:center;
        font-size: 24px;
        color: #444444;
        font-weight: bold;
    } 
    .container 
    {
        width:100%;
        margin-bottom: 100px;
    }
    .circle-content 
    {
        width:25%;
        text-align:center;
        float:left;
    }
    .circle 
    {
        display:inline-block;
        width:60%;
        padding-bottom:20%;
        border-radius:60%;
        border:15px solid #333;
    }
    .circle-content p 
    {
        font-size:16px;
        color:rgb(0, 0, 0);
    }
    .footer {
        position: fixed;
        left: 0;
        bottom: 0;
        width: 100%;
        /* background-color: red; */
        color: black;
        text-align: center;
        font-size: 10px;
        color: #888;
    }
  </style>
</head>
<body>
    <h1>Bussola</h1>
    <h3 style="margin-bottom: 0;">Inserisci Latitudine e Longitudine</h3>
    <p>Esempio: lat: 43.025567 lon:12.438006</p> 
    <form action="/get"> 
        <div>Lat: <input type="number" step="0.0000001" name="lat"></div>
        <div>Lon: <input type="number" step="0.0000001" name="lon"></div>
        <input class="button" type="submit" value="Invio">
    </form>
    <hr>
    <h1 style="margin-top: 0;">Target Attuale</h1>
    <div style="text-align: center;">
        <div class="target">
            <span>Latitudine target inserita:</span>
            <span id="lat"></span></br>

            <span>Longitudine target inserita:</span>
            <span id="lon"></span></br>

            <span>Orientamento attuale:</span>
            <span id="heading"></span>
            <span>&#176;</span></br>

            <span>Distanza Obiettivo:</span>
            <span id="distance"></span>
            <span>&#91;m&#93;</span></br>
        </div> 
    </div>
    <hr>
    <h1 style="margin-top: 0;">Status</h1>
    <div class="container">
        <div class="circle-content">
            <div class="dot"  id="servo"></div>
            <div>Servo</div>
            <div id="servo_status"></div>
        </div>
        <div class="circle-content">
            <div class="dot"  id="compass"></div>
            <div>Compass</div>
            <div id="comp_status"></div>
        </div>
        <div class="circle-content">
            <div class="dot"  id="gps"></div>
            <div>GPS</div>
            <div id="gps_status"></div>

        </div>
        <div class="circle-content">
            <div class="dot"  id="system"></div>
            <div>System</div>
            <div id="sys_status"></div>

        </div>
    </div>

    
    <div class="footer">
        Developed by: <b>EmSolutions</b>
    </div>
    <script>
        setInterval(function() 
        {
            getCompassStatus();
            getServoStatus();
            getGPSStatus();
            getSysStatus();
            target();
            actualpose();
        }, 200); 
        
        function getCompassStatus() 
        {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() 
            {
                if (this.readyState == 4 && this.status == 200) 
                {
                    if(this.responseText == "calibrating")
                    {
                        document.getElementById("compass").style.backgroundColor = "#e9b200";
                        document.getElementById("comp_status").innerHTML = "Calibrating";
                    }
                    else
                    {
                        if(this.responseText == "ok")
                        {
                            document.getElementById("compass").style.backgroundColor = "#92ec00";
                            document.getElementById("comp_status").innerHTML = "Ok";

                        }
                        else
                        {
                            if(this.responseText == "err")
                            {
                                document.getElementById("compass").style.backgroundColor = "#FF0000";
                                document.getElementById("comp_status").innerHTML = "Error";
                            }
                            else
                            {
                                document.getElementById("compass").style.backgroundColor = "#bbb";
                                document.getElementById("comp_status").innerHTML = "Offline";
                            }
                        }
                        

                    }
                }
            };
            xhttp.open("GET", "compass_status", true);
            xhttp.send();
        }

        function getServoStatus() 
        {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() 
            {
                if (this.readyState == 4 && this.status == 200) 
                {
                    if(this.responseText == "ok")
                    {
                        document.getElementById("servo").style.backgroundColor = "#92ec00";
                        document.getElementById("servo_status").innerHTML = "Ok";
                    }
                    else
                    {
                        if(this.responseText == "err")
                        {
                            document.getElementById("servo").style.backgroundColor = "#FF0000";
                            document.getElementById("servo_status").innerHTML = "Error";
                        }
                        else
                        {
                            document.getElementById("servo").style.backgroundColor = "#bbb";
                            document.getElementById("servo_status").innerHTML = "Offline";
                        }
                    }
                }
            };
            xhttp.open("GET", "servo_status", true);
            xhttp.send();
        }

        function getGPSStatus() 
        {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() 
            {
                if (this.readyState == 4 && this.status == 200) 
                {
                    if(this.responseText == "ok")
                    {
                        document.getElementById("gps").style.backgroundColor = "#92ec00";
                        document.getElementById("gps_status").innerHTML = "Ok";
                    }
                    else
                    {
                        if(this.responseText == "err")
                        {
                            document.getElementById("gps").style.backgroundColor = "#FF0000";
                            document.getElementById("gps_status").innerHTML = "Error";
                        }
                        else
                        {
                            document.getElementById("gps").style.backgroundColor = "#bbb";
                            document.getElementById("gps_status").innerHTML = "Offline";
                        }
                    }
                }
            };
            xhttp.open("GET", "gps_status", true);
            xhttp.send();
        }

        function getSysStatus() 
        {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() 
            {
                if (this.readyState == 4 && this.status == 200) 
                {
                    if(this.responseText == "ok")
                    {
                        document.getElementById("system").style.backgroundColor = "#92ec00";
                        document.getElementById("sys_status").innerHTML = "Ok";
                    }
                    else
                    {
                        if(this.responseText == "err")
                        {
                            document.getElementById("system").style.backgroundColor = "#FF0000";
                            document.getElementById("sys_status").innerHTML = "Error";
                        }
                        else
                        {
                            document.getElementById("system").style.backgroundColor = "#bbb";
                            document.getElementById("sys_status").innerHTML = "Offline";
                        }
                    }
                }
            };
            xhttp.open("GET", "sys_status", true);
            xhttp.send();
        }

        function target() 
        {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() 
            {
                if (this.readyState == 4 && this.status == 200) 
                {
                    var res = this.responseText.split(";");
                    // document.getElementById("distance").innerHTML = res[0];
                    document.getElementById("lat").innerHTML = res[0];
                    document.getElementById("lon").innerHTML = res[1];

                }
            };
            xhttp.open("GET", "target", true);
            xhttp.send();
        }

        function actualpose() 
        {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() 
            {
                if (this.readyState == 4 && this.status == 200) 
                {
                    var res = this.responseText.split(";");
                    document.getElementById("heading").innerHTML = res[0];
                    document.getElementById("distance").innerHTML = res[1];
                }
            };
            xhttp.open("GET", "actualpose", true);
            xhttp.send();
        }

    </script>

</body>
</html>
)rawliteral";
