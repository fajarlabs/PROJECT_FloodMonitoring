import serial
import struct
from time import sleep
import psycopg2
import time
import threading

# raspberry
#PORT = '/dev/ttyUSB0'
# PC
PORT = 'COM3'
BAUD_RATE = 9600

# Serial connection
SER = None 

# previx serial
ALLOW_PREVIX = ["<<DATA"]

# db account
USERDB = "postgres"
PASSDB = "postgres"
HOSTDB = "127.0.0.1"
PORTDB = "5432"
DBNAME = "postgres"
# db connection
CONNECTION = None
TIMEDICTS = {}

start_time = None

def main():
	while True:
		# read serial
		serial_data = SER.readline()
		data = receiveParse(serial_data)

		# didalam ini adalah proses parsing dan data diteruskan ke MQTT atau ke database
		itemParse(data, serial_data)

		# ini adalah perintah untuk mengirim pesan dari serial dan di broadcast
		# ke transmitter nrf atau dari socket ke radio nrf
		# send command "02"->Node, "1"->command, "1"-> data
		# sendCommand("02","1","2")
		sleep(1)


def thread_command():
	while 1 :
		pass

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
	print(data);

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
	result = []
	try :
		cursor = CONNECTION.cursor()
		sqlite_select_query = """ SELECT * FROM nodes_cmd"""
		cursor.execute(sqlite_select_query)
		records = list(cursor.fetchall())

	except Exception as e :
		print(e)

	return result

if __name__ == "__main__":
	try :
		CONNECTION = psycopg2.connect(user=USERDB,password=PASSDB,host=HOSTDB,port=PORTDB,database=DBNAME)
		SER = serial.Serial(PORT, BAUD_RATE, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
		# thread time start
		ttout = threading.Thread(target=thread_timeout, args=())
		ttout.start()
		# if connection fail do not run main program
		main()
	except Exception as e :
		print(e)
