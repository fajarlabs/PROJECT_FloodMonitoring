# Project01
The project made a simple flood monitoring system using a buoy sensor and radio nrf24l01
  
# Diagram
<img src="https://i.ibb.co/5j3hp6h/signal.png" alt="signal" border="0"/>

# Raspberry PI<br />
Scan IP pakai "advanced ip Scanner" atau tools scan lainnya untuk mencari IP dari raspberry pi<br />
Lalu silahkan masuk melalui SSH / putty<br />
<b>SSH</b></br>
User : pi<br />
Pass : raspberry<br />
Cara masuk ssh, buka cmd lalu ketikkan ````` ssh pi@xxx.xxx.xxx.xxx ````` lalu masukkan passwordnya<br />
Lokasi script python ada di direktori : ````` /home/pi/Documents ````` <br />
<a href="https://imgbb.com/"><img src="https://i.ibb.co/pxky859/PATH-FILE-SCRIPT-PYTHON.png" alt="PATH-FILE-SCRIPT-PYTHON" border="0"></a><br />
Cara menjalankan script ptyhonnya secara manual seperti berikut ini : <br />````` pi@raspberrypi:~/Documents $ python3 piGateway.py ````` <br />
Untuk autoRUN script agar ketika restart raspberrypi-nya bisa langsung aktif menggunakan PM2<br />
Cara menjalankannya perintahnya berikut ini :  <br />````` pm2 start 0  ````` <br />dan untuk mematikan autoRUNnya bisa menggunakan perintah ini : <br />````` pm2 stop 0  ````` <br />
<a href="https://ibb.co/MpYBygm"><img src="https://i.ibb.co/Qn2D7J4/pm2.png" alt="pm2" border="0"></a>
  <br /><br />
# Serial<br />
<h2>Receive data from Serial Data</h2>

Dibawah ini adalah bentuk format serial data yang diterima dari radio<br />
`````<<DATA[SN]~[REQ]~[[DATA1]$[DATA2]$[DATA3]...>>`````<br />
Format data seperti dibawah ini : <br />
<b>SN</b> : Serial number atau NODE radio <br />
<b>REQ</b> : Adalah mode yang di kirim ke node lain, ini harus di sesuikan dengan coding mode di alatnya <br />
<b>DATA</b> : Berisi multi data yang di pisahkan / delimiter dengan karakter '$' / dolar <br />

<h2>Command Data Format</h2>

`````<<CMD[node]~[req]~[data]>>`````<br />
<b>NODE</b> : Serial number atau NODE radio <br />
<b>REQ</b> : Adalah mode yang di kirim ke node lain, ini harus di sesuikan dengan coding mode di alatnya <br />
<b>DATA</b> : request / command tanpa berisi delimiter '$' dolar hanya data bertipe unsigned-long <br />

<h2>Contoh Perintah</h2>

`````<<CMD02~0~0>>````` <-------- perintah untuk testping serial untuk menyalakan buzzer<br />
Apabila untuk transmit ke node lain perintah yang diubah adalah nodenya, contonya seperti ini : <br />
`````<<CMD02~1~1>>````` <-------- perintah untuk mengirimkan pesan ke node02 melalu NRF radio<br />

Contoh perintah untuk mematikan untuk menghidupkan relay panel<br />
`````<<CMD02~1~1>>````` <----- ON <br />
`````<<CMD02~1~2>>````` <----- OFF<br />

<h2>Check ports</h2><br />
<a href="https://ibb.co/YyH0wQB"><img src="https://i.ibb.co/b2cbDm5/Whats-App-Image-2021-04-14-at-7-31-27-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-31-27-AM" border="0"></a><br />
<a href="https://imgbb.com/"><img src="https://i.ibb.co/P9nCbmL/PORT-LIST.png" alt="PORT-LIST" border="0"></a><br />
Port yang di pakai di raspberrypi gateway : <br />
<b>Port : /dev/ttyUSB0</b> <br />
<b>Baud Rate : 9600</b> <br /><br />

<h2>Records DB (PostgreSQL)</h2>
<a href="https://ibb.co/zH8crwm"><img src="https://i.ibb.co/fG8R27X/data-Record.png" alt="data-Record" border="0"></a><br />
<h2>Commands on table "nodes_cmd"</h2>
<a href="https://imgbb.com/"><img src="https://i.ibb.co/9Vf42ZB/Command-Push.png" alt="Command-Push" border="0"></a><br />
<h2>Log Panel Controller</h2>
<a href="https://ibb.co/3dY326t"><img src="https://i.ibb.co/KhzPd4C/data-Log-Controller.png" alt="data-Log-Controller" border="0"></a>

# Sketch Command Center <br />
<a href="https://ibb.co/sbL2gpL"><img src="https://i.ibb.co/q7bdDLb/COMMAND-CENTER-bb.png" alt="COMMAND-CENTER-bb" border="0"></a><br />
# Sketch Float Sensor <br />
<a href="https://ibb.co/3CtSbP8"><img src="https://i.ibb.co/x7Vzbrc/FLOAT-SENSOR.png" alt="FLOAT-SENSOR" border="0"></a><br />
# Sketch Panel Controller <br />
<a href="https://ibb.co/PCRHHVG"><img src="https://i.ibb.co/9YJ55dH/PANEL-CONTROLLER-bb.png" alt="PANEL-CONTROLLER-bb" border="0"></a><br />

# Box Panel Controller <br />
Lampu indikator ada 3 warna : <br />
<b>Warna Merah</b> : Kontaktor dalam keadaan OFF / kontaktor tidak terhubung <br />
<b>Warna Kuning</b> : Power Supply OK </br>
<b>Warna Hijau</b> : Kontaktor dalam keadaan ON atau generator dalam keadaan starting-up / aktif<br /><br />
<a href="https://ibb.co/4SdvPH1"><img src="https://i.ibb.co/ykSvWr4/Whats-App-Image-2021-04-14-at-7-13-49-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-13-49-AM" border="0"></a>
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

<a href="https://ibb.co/0DzcRdz"><img src="https://i.ibb.co/K08Xp38/Whats-App-Image-2021-04-13-at-5-51-59-PM.jpg" alt="Whats-App-Image-2021-04-13-at-5-51-59-PM" border="0"></a><br />

<a href="https://ibb.co/DgNR2WH"><img src="https://i.ibb.co/QjZKwQ3/Whats-App-Image-2021-04-13-at-5-54-51-PM.jpg" alt="Whats-App-Image-2021-04-13-at-5-54-51-PM" border="0"></a><br />

<a href="https://ibb.co/h1q2V0N"><img src="https://i.ibb.co/M5XP7dH/Whats-App-Image-2021-04-13-at-6-48-35-PM.jpg" alt="Whats-App-Image-2021-04-13-at-6-48-35-PM" border="0"></a><br />

<h2> Cara mencopot pipa untuk setup ketinggian level airnya</h2>
<a href="https://ibb.co/zmyzbyr"><img src="https://i.ibb.co/Jcf92fK/Whats-App-Image-2021-04-14-at-7-35-01-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-35-01-AM" border="0"></a>
<a href="https://ibb.co/1ZxKc63"><img src="https://i.ibb.co/31HWKzw/Whats-App-Image-2021-04-14-at-7-33-16-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-33-16-AM" border="0"></a>
<a href="https://ibb.co/bJRhJFB"><img src="https://i.ibb.co/HdpvdCz/Whats-App-Image-2021-04-14-at-7-28-10-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-28-10-AM" border="0"></a>
<a href="https://ibb.co/Jsb5nLQ"><img src="https://i.ibb.co/rwL4kPs/Whats-App-Image-2021-04-14-at-7-26-18-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-26-18-AM" border="0"></a>
<a href="https://ibb.co/mtGvR3R"><img src="https://i.ibb.co/Bjt4f9f/Whats-App-Image-2021-04-14-at-7-24-02-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-24-02-AM" border="0"></a>

<h2> Cara lepas untuk setup pelampung </h2>
<a href="https://ibb.co/Rpw31Mp"><img src="https://i.ibb.co/qMQNz4M/Whats-App-Image-2021-04-14-at-7-18-26-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-18-26-AM" border="0"></a>
<a href="https://ibb.co/GCVyGZR"><img src="https://i.ibb.co/kqhjzLX/Whats-App-Image-2021-04-14-at-7-18-06-AM.jpg" alt="Whats-App-Image-2021-04-14-at-7-18-06-AM" border="0"></a>
