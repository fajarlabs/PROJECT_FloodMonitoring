import serial
import struct
from time import sleep

PORT = 'COM3'
BAUD_RATE = 9600

ser = serial.Serial(PORT, BAUD_RATE, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)

ALLOW_PREVIX = ["<<DATA"]

def main():
	while True:
		# read serial
		data = receiveParse(ser.readline())

		# didalam ini adalah proses parsing dan data diteruskan ke MQTT atau ke database
		itemParse(data)

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
		ser.write(query.encode())
		is_send = True
	except Exception as e :
		print("ERROR")
		print(e)
	return is_send

# parse item series
def itemParse(receive_data):
	try :
		dataList = receive_data.split("~")
		serial_number = dataList[0]
		data = dataList[1]
		req = dataList[2]
		batt = dataList[3]
		print(serial_number+"~"+data+"~"+req+"~"+batt)

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

if __name__ == "__main__":
    main()