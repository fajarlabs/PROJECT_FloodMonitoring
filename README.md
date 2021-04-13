# Project01
The project made a simple flood monitoring system using a buoy sensor and radio nrf24l01

# Payload<br />
  <h2>Format Transmition</h2>
  <b>serial_number</b> : Serial number device<br />
  <b>data</b> : Data value sensor<br />
  <b>req</b> : Request client to receiver<br />
  <b>batt</b> : Battery level<br /><br />
  
# Diagram
<img src="https://i.ibb.co/5j3hp6h/signal.png" alt="signal" border="0"/>

# Serial<br />
<h2>Receive data from Serial Data</h2>

`````
<<DATA1002~0~0~5.44>>
`````
<b>1002</b> : Serial number <br />
<b>0</b> : data <br />
<b>0</b> : request / command <br />
<b>5.44</b> : battery level <br />

<h2>Receive Data Format</h2>

`````<<CMD[node]~[command]~[data]>>`````<br />
`````<<CMD02~0~0>>````` <-------- perintah untuk testping serial untuk menyalakan buzzer<br />
Apabila untuk transmit ke node lain perintah yang diubah adalah nodenya, contonya seperti ini : <br />
`````<<CMD02~1~1>>````` <-------- perintah untuk mengirimkan pesan ke node02 melalu NRF radio<br />

Contoh perintah untuk mematikan untuk menghidupkan relay panel
`````<<CMD02~1~1>>````` <----- ON <br />
`````<<CMD02~1~2>>````` <----- OFF<br />

<a href="https://ibb.co/PWbLk96"><img src="https://i.ibb.co/7pTfhR1/SERIAL.png" alt="SERIAL" border="0"></a>

# Sketch Node Sensor
<img src="https://i.ibb.co/GCDMrsv/SKETCH-bb.png" alt="SKETCH-bb" border="0" />

# Sketch Node Panel Controller
<img src="https://i.ibb.co/P5kdKCw/SKETCH-PANEL-bb.png" alt="SKETCH-PANEL-bb" border="0">

# Box Panel Controller
<img src="https://i.ibb.co/mzH4Y9H/BOX-PANEL.jpg" />
<img src="https://i.ibb.co/tzzwFxp/BOX-PANEL-FLASH.jpg" />
<img src="https://i.ibb.co/HXfv78d/BOX-PANEL2.jpg" />

# Float Sensor
<img src="https://i.ibb.co/J73hCQb/SENSOR-FLOAT.jpg" />
<img src="https://i.ibb.co/Tvc8hM1/SENSOR-FLOAT2.jpg" />
<img src="https://i.ibb.co/Tvc8hM1/SENSOR-FLOAT2.jpg" />
<img src="https://i.ibb.co/P68yMzF/SENSOR-FLOAT4.jpg" />
<img src="https://i.ibb.co/pXg9CGh/SENSOR-FLOAT5.jpg" />

# Sensor solar 
Untuk cek ketersediaan bbm, bukan volume karena bukan tipe analog karena harganya perCM cukup mahal dan perlu kalibrasi dengan panjang, lebar, tinggi<br />
<a href="https://ibb.co/xgt35PT"><img src="https://i.ibb.co/7pdby8m/Whats-App-Image-2021-04-11-at-10-44-52-PM.jpg" alt="Whats-App-Image-2021-04-11-at-10-44-52-PM" border="0"></a>
<a href="https://ibb.co/Pr9xSyZ"><img src="https://i.ibb.co/ZfYdZr6/Whats-App-Image-2021-04-11-at-10-45-48-PM.jpg" alt="Whats-App-Image-2021-04-11-at-10-45-48-PM" border="0"></a>
<a href="https://ibb.co/q9xj9Hf"><img src="https://i.ibb.co/K7mh7tk/Whats-App-Image-2021-04-11-at-10-45-41-PM.jpg" alt="Whats-App-Image-2021-04-11-at-10-45-41-PM" border="0"></a>

# Core control RS232 
Retrieve dan command dari sini <br />
<a href="https://ibb.co/fx5VGBT"><img src="https://i.ibb.co/XCgfkNG/Whats-App-Image-2021-04-11-at-10-38-35-PM.jpg" alt="Whats-App-Image-2021-04-11-at-10-38-35-PM" border="0"></a>

<a href="https://ibb.co/R9BkKkW"><img src="https://i.ibb.co/VtQcGc8/Whats-App-Image-2021-04-12-at-12-04-18-AM.jpg" alt="Whats-App-Image-2021-04-12-at-12-04-18-AM" border="0"></a>
