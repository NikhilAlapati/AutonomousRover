from time import sleep
import PySimpleGUI as sg
import keyboard
import main as bluetooth

uart = bluetooth.getConnection()


def moveForward():
    uart.write(b"w")
    sleep(0.1)


def moveBack():
    uart.write(b"s")
    sleep(0.1)


def moveLeft():
    uart.write(b"a")
    sleep(0.1)


def moveRight():
    uart.write(b"d")
    sleep(0.1)


def stop():
    uart.write(b"b")
    sleep(0.1)


def manualToggle():
    uart.write(b"m")
    sleep(0.1)


keyboard.add_hotkey("w", moveForward)
keyboard.add_hotkey("s", moveBack)
keyboard.add_hotkey("a", moveLeft)
keyboard.add_hotkey("d", moveRight)
keyboard.add_hotkey("m", manualToggle)
keyboard.add_hotkey("space", stop)
sg.theme('Dark')
layout = [
    [sg.Text("Remote controlled bluetooth controller")],
    [sg.Button("Toggle Headlights")],
    [sg.Button("Reconnect")]
]
window = sg.Window("Demo", layout)

while True:
    event, values = window.read()
    if event == sg.WIN_CLOSED:
        break
    if event == "Toggle Headlights":
        uart.write(b"h")
    if event == "Reconnect":
        uart = bluetooth.getConnection()

window.close();
