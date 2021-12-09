import string
from time import sleep
import PySimpleGUI as sg
import keyboard
import main as bluetooth
import sys
import glob
import serial

messagesReceived = 0
messagesSent = 0


def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


ports_ = serial_ports()[0]
print(ports_)
ser = serial.Serial(ports_, 9600)
uart = bluetooth.getConnection()


def move_forward():
    uart.write(b"2")
    sleep(0.1)


def move_back():
    uart.write(b"5")
    sleep(0.1)


def move_left():
    uart.write(b"4")
    sleep(0.1)


def move_right():
    uart.write(b"6")
    sleep(0.1)


def stop():
    uart.write(b"B")
    sleep(0.1)


def manual_toggle():
    uart.write(b"A")
    sleep(0.1)


def activate_horn():
    uart.write(b"*")
    sleep(0.1)


keyboard.add_hotkey("w", move_forward)
keyboard.add_hotkey("s", move_back)
keyboard.add_hotkey("a", move_left)
keyboard.add_hotkey("d", move_right)
keyboard.add_hotkey("m", manual_toggle)
keyboard.add_hotkey("h", activate_horn)
keyboard.add_hotkey("space", stop)
# sg.theme('Dark')
# layout = [
#     [sg.Text("Remote controlled bluetooth controller")]
# ]
# window = sg.Window("Demo", layout)

while True:
    var = ""
    message = uart.read(100)
    if message:
        print(message.decode('utf-8'))
        messagesReceived += 1
        print("%s%s" % ("Total Messages Received:", messagesReceived))
    if ser.in_waiting > 0:
        var = ser.read()
        print(var)
        uart.write(var)
        messagesSent += 1
        print("%s%s" % ("Total Messages Sent:", messagesReceived))
        del var
    # event, values = window.read()
    # if event == sg.WIN_CLOSED:
    #     break

ser.close()
# window.close();
