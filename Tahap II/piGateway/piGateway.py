import serial
import struct
from time import sleep
import psycopg2
import time
import threading
import paho.mqtt.client as mqtt

def main():
	global CLIENT, BASE_TOPIC

	while True:
		# read serial
		serial_data = SER.readline()
		data = receiveParse(serial_data)

		# didalam ini adalah proses parsing atau ke database
		itemParse(data, serial_data)

		if CLIENT != None :
			print("Send RAW data OK")
			# raw data dikirim ke server broker yang nanti data raw ini di parsing
			CLIENT.publish(BASE_TOPIC+"/raw_data", serial_data.strip().decode('utf_8'))

		# ini adalah perintah untuk mengirim pesan dari serial dan di broadcast
		# ke transmitter nrf atau dari socket ke radio nrf
		# send command "02"->Node, "1"->command, "1"-> data
		# sendCommand("02","1","2")
		sleep(1)

def connectMQTT():
	global CLIENT, MQTT_HOST, MQTT_PORT, MQTT_TIMEOUT

	CLIENT = mqtt.Client()
	CLIENT.on_connect = on_connect
	CLIENT.on_message = on_message
	CLIENT.connect(MQTT_HOST, MQTT_PORT, MQTT_TIMEOUT)
	CLIENT.loop_forever()

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
	global BASE_TOPIC

	print("Connected with result code "+str(rc))
	# get raw mirror data 
	client.subscribe(BASE_TOPIC+"/raw_data")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

def thread_command():
	while 1 :
		# get all command
		data = getAllCommand()
		for i in data :
			# send command via serial
			sendCommand(i[1], i[2], i[3])
			# remove command after send via serial
			if removeCmdData(i[0]) :
				print("Delete last command with id : %s",(i[0]))

'''
Fungsi dibawah ini untuk mendeteksi apakah sensor banjir mengirim data terbaru
setelah menyalakan GENSET, jika lebih dari 2 Menit maka matikan GENSET secara otomatis
'''
def thread_timeout():
	while 1 :
		try :
			if '1001' in TIMEDICTS :
				elapsed_time = time.time() - TIMEDICTS["1001"] 
				if elapsed_time > 12 :
					if TIMEDICTS["1001"] > 0 :
						print("Generator is shutting-down..")
						sendCommand("02","1","2") # relay-off
					TIMEDICTS["1001"] = 0  # set to None supaya tidak mengirim off-signal mematikan GENSET
		except Exception as e :
			print(e)
		sleep(1) 
'''
Fungsi dibawah ini untuk kirim pesan ke serial RS232 ke gateway
'''
def sendCommand(node = '00', req = '0', data = '0'): 
	global SER

	is_send = False
	try :
		# formatting command
		query = "<<CMD%s~%s~%s>>\r\n" % (node, req, data)
		SER.write(query.encode())
		is_send = True
	except Exception as e :
		print(e)
	return is_send

'''
Fungsi dibawah ini adalah alur programnya, yaitu ketika BBM Solar habis
maka akan menyalakan alarm di Radio Gateway (Buzzer) bahwa bbm dalam keadaan kurang / habis
dan otomatis relay akan terputus, silahkan diubah sesuai kebutuhan
'''
def workflow(serial_number, data):
	global TIMEDICTS

	# periksa serial number dari sensor banjir
	# 1001 adalah node dari sensor banjir
	if serial_number == "1001" :
		splitData = data.split('$')
		floatSensor = splitData[0]

		# jika data == 1 terjadi banjir
		if floatSensor == '1':
			print("Generator is starting-up...")
			sendCommand("02","1","1") # on relay
			TIMEDICTS[serial_number] = time.time()
		
			'''
			if checkFuelExist('1002') : # check data controller panel
				print("Fuel is exist and preparing to starting-up")
				# perintahkan node 02 untuk menyalakan relay
				sendCommand("02","1","1") #on relay
				# dapatkan update-time dari tiap2 serial number
				TIMEDICTS[serial_number] = time.time()
			else :
				print("Fuel is empty")
				sendCommand() # alarm buzzer on radio gateway
				sendCommand("02","1","2") # putuskan relay starter
			'''

# parse item series
def itemParse(receive_data, signal_message):
	try :
		dataList = receive_data.split("~")
		serial_number = dataList[0]
		req = dataList[1]
		data = dataList[2]
		
		# box panel control
		if serial_number == '1002':
			splitItemList = data.split('$')
			# format <<DATA[serial_number]~[request]~[[fuel_sensor_status]$[module_voltage]$[ac_voltage]]
			# fuel_sensor_status = splitItemList[0] 
			module_voltage = splitItemList[1]
			insertData(serial_number, data, req, module_voltage, signal_message.decode('utf_8'))

		# sensor module
		if serial_number == '1001':
			splitItemList = data.split('$')
			# floatSensor = splitItemList[0]
			module_voltage = splitItemList[1]
			insertData(serial_number, data, req, module_voltage, signal_message.decode('utf_8'))
			# cycles workflow
			workflow(serial_number, data) # alur program

		# save into database
		# do...
		# send via MQTT
		# do...
	except Exception as e :
		pass

# receive message from serial
def receiveParse(serial_data):
	global ALLOW_PREVIX

	is_allow = False
	result = ""
	try :
		serial_data = serial_data.strip().decode('utf_8')
		for prev in ALLOW_PREVIX :
			if(prev in serial_data):
				is_allow = True
				serial_data = serial_data.replace(prev,"")
		serial_data = serial_data.replace(">>","")
		if is_allow :
			result = serial_data
		else :
			serial_data = ""
			result = ""
	except Exception as e :
		print(e)

	return result

def insertData(serial_number, data, req, batt, signal_message):
	global CONNECTION

	try:
		cursor = CONNECTION.cursor()

		sql_insert = """ INSERT INTO nodes (serial_number, data, req, battery, signal_message) VALUES (%s, %s, %s, %s, %s)"""
		cursor.execute(sql_insert, (serial_number, data, req, batt, signal_message))

		CONNECTION.commit()
		count = cursor.rowcount
		#print(count, "Record inserted successfully into nodes table")

	except (Exception, psycopg2.Error) as error:
		print("Failed to insert record into nodes table", error)
'''
Jika SOLAR BBM masih ada maka nilai 0, jika 1 maka BBM Solar habis / kurang
'''
def checkFuelExist(serial_number):
	global CONNECTION

	result = False
	try :
		cursor = CONNECTION.cursor()
		sqlite_select_query = """ SELECT data FROM nodes WHERE serial_number = %s ORDER BY ctime DESC limit 1"""
		cursor.execute(sqlite_select_query, (serial_number,))
		records = cursor.fetchone()
		isFuelExist = int(records[0])
		if isFuelExist < 1:
			result = True
	except Exception as e :
		print(e)

	return result

def getAllCommand():
	global CONNECTION

	result = []
	try :
		cursor = CONNECTION.cursor()
		sqlite_select_query = """ SELECT id, node, req, data FROM nodes_cmd """
		cursor.execute(sqlite_select_query)
		result = list(cursor.fetchall())
		sleep(1)

	except Exception as e :
		print(e)

	return result

def removeCmdData(id):
	global CONNECTION

	result = False
	try:
		cursor = CONNECTION.cursor()
		sql_insert = """ DELETE FROM nodes_cmd WHERE id = %s"""
		cursor.execute(sql_insert, (id,))
		CONNECTION.commit()
		result = True
	except (Exception, psycopg2.Error) as error:
		print("Failed to insert record into nodes table", error)

	return result

if __name__ == "__main__":

	# previx serial
	ALLOW_PREVIX = ["<<DATA"]
	TIMEDICTS = {}

	# =============== SERIAL CONNECTION ====================
	SER = None 
	PORT = '/dev/ttyUSB0'
	# PC
	# PORT = 'COM3'
	BAUD_RATE = 9600
	# ======================================================

	# ================ DB ACCOUNT ==========================
	CONNECTION = None
	USERDB = "postgres"
	PASSDB = "postgres"
	HOSTDB = "127.0.0.1"
	PORTDB = "5432"
	DBNAME = "postgres"
	# ======================================================

	# ================ MQTT ACCOUNT ========================
	CLIENT = None
	MQTT_HOST = "test.mosquitto.org"
	MQTT_PORT = 1883
	MQTT_TIMEOUT = 60
	# ======================================================
	# ================ TOPIC ===============================
	BASE_TOPIC = "/mytopic"
	# ======================================================

	try :
		CONNECTION = psycopg2.connect(user=USERDB,password=PASSDB,host=HOSTDB,port=PORTDB,database=DBNAME)
		SER = serial.Serial(PORT, BAUD_RATE, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
		
		# thread time start
		tTimeoutAlarm = threading.Thread(target=thread_timeout, args=())
		tTimeoutAlarm.start()
		
		# if connection fail do not run main program
		tCommand = threading.Thread(target=thread_command, args=())
		tCommand.start()

		# start MQTT broadcast
		tMQTT = threading.Thread(target=connectMQTT, args=())
		tMQTT.start()

		main()
	except Exception as e :
		print(e)
