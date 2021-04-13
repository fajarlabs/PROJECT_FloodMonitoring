'''
Untuk melihat daftar port yang nantinya nama port tersebut 
dipakai di konfigurasi pada file piGateway.php atau di serial
'''

import serial.tools.list_ports as port_list
ports = list(port_list.comports())
for p in ports:
    print (p)