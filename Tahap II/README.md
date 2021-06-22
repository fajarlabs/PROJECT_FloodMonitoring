# Project01 - Development Tahap II
The project made a simple flood monitoring system using a buoy sensor, rain-gauge and radio nrf24l01

# Sumber Kode
Didalam folder tahap II ini terdapat folder script diantaranya, <br />
1.) <b>piGateway</b> : Berisi script python yang dijalankan di raspberry gateway <br />
2.) <b>SubscriberMQTT</b> : Berisi script python yg dijalankan disisi server untuk menerima raw data yang dikirim / dibroadcast untuk raspberrypi gateway<br />
3.) <b>TRANSMIT_FLOOD_SENSOR</b> : Berisi script untuk arduino <br />

# FLASH IC ATMEGA328
Menggunakan USB ASP + ZIP Socket <br />
<a href="https://imgbb.com/"><img src="https://i.ibb.co/T4M5ph7/USB-ASP.jpg" alt="USB-ASP" border="0"></a> <br />

Untuk librarynya menggunakan "MiniCore" <br />
Untuk repositorynya ada disini : <b><a href="https://github.com/MCUdude/MiniCore">MiniCore</a></b> <br />

# Raspberry PI<br />
Bukan terminal dan tambahkan library paho.mqtt dengan mengetik <br />
````` pip install paho-mqtt ````` <br />
Untuk scriptnya di override dari script yang sebelumnya menggunakan piGateway yang didalam folder Tahap II <br />
Sebelum menjalankan scriptnya ubah terlebih dahulu account mqtt account disesuaikan dengan server broker yang sudah dibuat sebelumnya. <br />
<a href="https://ibb.co/d7ZqTfV"><img src="https://i.ibb.co/R78mFgs/Edit-Account-MQTT.png" alt="Edit-Account-MQTT" border="0"></a>
<br />
Untuk menjalankannya ketikan pada terminal perintah berikut ini : <br />
````` python piGateway.py ````` <br />

Untuk menjalankan script program untuk subscriber MQTT dan parsing ada didalam folder SubscriberMQTT. Edit account MQTT sesuai server broker yang telah dibuat.<br />
Didalam script subscriber MQTT ini hanya sampai dengan parsing saja, tidak sampai untuk insert ke dalam database. <br />

Untuk menjalankan program ketikan di terminal / command prompt perintah berikut : <br />
````` python subscriberData.py ````` <br />

Untuk format data yang dikirim lewat MQTT berbentuk RAW, dan diparsing didalam script subscriberData.py <br />
<a href="https://ibb.co/tM6VV2m"><img src="https://i.ibb.co/DDF00MV/Format-Code.png" alt="Format-Code" border="0"></a> <br />

Untuk instalasi mosquitto-clients di linux ubuntu seperti berikut ini : <br />
````` sudo apt-get install mosquitto-clients `````

Untuk perintah testing mqtt tanpa script juga bisa dilakukan dengan mengetik perintah : <br />
<a href="https://ibb.co/FBj3fby"><img src="https://i.ibb.co/4j5tDZX/mosquitto-sub.png" alt="mosquitto-sub" border="0"></a><br />
````` mosquitto_sub -h test.mosquitto.org -t "/mytopic/raw_data" -v ````` <br />

Untuk screenshot hasil test seperti berikut ini : <br />
Debug MQTT <br />
<a href="https://ibb.co/vJM7yxg"><img src="https://i.ibb.co/K98M4jR/Debug-MQTT.png" alt="Debug-MQTT" border="0"></a> <br />
Debug di Gateway <br />
<a href="https://ibb.co/WtbXbgG"><img src="https://i.ibb.co/0tgLghm/TRANSMIT-DATA-GTW.png" alt="TRANSMIT-DATA-GTW" border="0"></a> <br />

Untuk IC menggunakan ATMEGA328 dengan keterangan pinout dibawah ini (<b> Warna Biru </b> ) : <br />
<a href="https://ibb.co/nCmdzjr"><img src="https://i.ibb.co/4dtvPpW/68747470733a2f2f692e696d6775722e636f6d2f715849456368542e6a7067.jpg" alt="68747470733a2f2f692e696d6775722e636f6d2f715849456368542e6a7067" border="0"></a><br /><a target='_blank' href='https://id.imgbb.com/'>gambum</a><br /><br />
Pin yang digunakan untuk proyek ini sebagai berikut ini : <br />
Pin untuk NRF (warna biru) <br />
<b>Pin 7 </b> : CE <br />
<b>Pin 8 </b> : CSN <br />
<b>Pin 11</b> : MISO <br />
<b>Pin 12</b> : MOSI <br />
<b>Pin 13</b> : SCK <br />

Pin untuk <i>rain gauge sensor</i> pada <b>Pin 6</b>

Untuk desain PCB menggunakan sprintlayout dan sudah terlampir di dalam folder Tahap II dengan nama file PCB_Interface.lay <br />
Gambar PCB seperti berikut ini: <br />
<a href="https://ibb.co/d2hWskj"><img src="https://i.ibb.co/WyTxNBG/PCB-Interface.jpg" alt="PCB-Interface" border="0"></a><br />

Galeri proyek : <br />
<a href="https://ibb.co/zfkCwdP"><img src="https://i.ibb.co/cTmRdq2/Whats-App-Image-2021-06-20-at-11-57-32-PM-1.jpg" alt="Whats-App-Image-2021-06-20-at-11-57-32-PM-1" border="0"></a><br />
<a href="https://ibb.co/4Vt1FCm"><img src="https://i.ibb.co/nLmPw4g/Whats-App-Image-2021-06-20-at-11-57-32-PM-2.jpg" alt="Whats-App-Image-2021-06-20-at-11-57-32-PM-2" border="0"></a><br />
<a href="https://ibb.co/3zZNfRB"><img src="https://i.ibb.co/GTS2sHc/Whats-App-Image-2021-06-20-at-11-57-32-PM-4.jpg" alt="Whats-App-Image-2021-06-20-at-11-57-32-PM-4" border="0"></a><br />
