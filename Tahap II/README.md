# Project01
The project made a simple flood monitoring system using a buoy sensor and rain-gauage & radio nrf24l01

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
# Serial<br />
<h2>Receive data from Serial Data</h2>

Dibawah ini adalah bentuk format serial data yang diterima dari radio<br />
`````<<DATA[SN]~[REQ]~[[DATA1]$[DATA2]$[DATA3]...>>`````<br />
Format data seperti dibawah ini : <br />
<b>SN</b> : Serial number atau NODE radio <br />
<b>REQ</b> : Adalah mode yang di kirim ke node lain, ini harus di sesuikan dengan coding mode di alatnya <br />
<b>DATA</b> : Berisi multi data yang di pisahkan / delimiter dengan karakter '$' / dolar <br />
<a href="https://imgbb.com/"><img src="https://i.ibb.co/pJrTyk9/Result-All-Data.png" alt="Result-All-Data" border="0"></a>
