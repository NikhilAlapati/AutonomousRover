from time import sleep
import PySimpleGUI as sg
import keyboard
import main as bluetooth
import sys
import glob
import serial


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


def moveForward():
    uart.write(b"2")
    sleep(0.1)


def moveBack():
    uart.write(b"5")
    sleep(0.1)


def moveLeft():
    uart.write(b"4")
    sleep(0.1)


def moveRight():
    uart.write(b"6")
    sleep(0.1)


def stop():
    uart.write(b"B")
    sleep(0.1)


def manualToggle():
    uart.write(b"A")
    sleep(0.1)


def activateHorn():
    uart.write(b"*")
    sleep(0.1)


keyboard.add_hotkey("w", moveForward)
keyboard.add_hotkey("s", moveBack)
keyboard.add_hotkey("a", moveLeft)
keyboard.add_hotkey("d", moveRight)
keyboard.add_hotkey("m", manualToggle)
keyboard.add_hotkey("h", activateHorn)
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
    if ser.in_waiting > 0:
        var = ser.read()
        print(var)
        uart.write(var)
        del var
    # event, values = window.read()
    # if event == sg.WIN_CLOSED:
    #     break

ser.close()
# window.close();
