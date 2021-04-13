import serial
import struct
from time import sleep
import psycopg2

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
		sendCommand("02","1","2")
		sleep(1)

def sendCommand(node, req, data):
	is_send = False
	try :
		# formatting command
		query = "<<CMD%s~%s~%s>>\r\n" % (node, req, data)
		SER.write(query.encode())
		is_send = True
	except Exception as e :
		print("ERROR")
		print(e)
	return is_send

# parse item series
def itemParse(receive_data, signal_message):
	try :
		dataList = receive_data.split("~")
		serial_number = dataList[0]
		data = dataList[1]
		req = dataList[2]
		batt = dataList[3]
		insertData(serial_number, data, req, batt, signal_message.decode('utf_8'))

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


if __name__ == "__main__":
	try :
		CONNECTION = psycopg2.connect(user=USERDB,password=PASSDB,host=HOSTDB,port=PORTDB,database=DBNAME)
		SER = serial.Serial(PORT, BAUD_RATE, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
	except Exception as e :
		print(e)
	main()