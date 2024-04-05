import serial.tools.list_ports
import serial
ports = serial.tools.list_ports.comports()
ports_list = [str(port) for port in ports]

print("Available COM ports:")
for port in ports_list:
    print(port)
selected_port = input('Select Port: COM')


port_var = None
for port in ports_list:
    if port.startswith(f'COM{selected_port}'):
        port_var = f'COM{selected_port}'
        break


if port_var is None:
    print("Invalid port selection. Exiting program.")
    exit()

serial_inst = serial.Serial()
serial_inst.baudrate = 2400
serial_inst.port = port_var
serial_inst.open()

try:
    while True:
        
        data_to_send = input('Enter data to send to Arduino: ')
        serial_inst.write(data_to_send.encode('utf-8'))
        data_received = serial_inst.read(1000)
        message_received1 = data_received.decode('utf-8').strip()
        print(f"Received from Arduino: {message_received1}")
        timetaken_receivedd = serial_inst.readline().decode('utf-8').strip()
        print(f"time taken from Arduino: {timetaken_receivedd}")

except KeyboardInterrupt:
    print("\nProgram interrupted.")

finally:
    
    serial_inst.close()
    print("Serial port closed.")
