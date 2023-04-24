# Mini-Radio-Station
FM Transmitter-NS73M

This project is a simple FM Transmitter that is deisgned to be a module for the M5 Stick C Plus. Here is a link to the parts on Digikey https://www.digikey.com/en/mylists/list/XUSR3FV555. However, the NS73M is no longer active, so it's not in the Digikey Cart but if you have one feel free to try this simple design. The xlsx file is just a Excel Sheet with information on each part.

The pdf's are just print out's of the eagle schematic and board. They are simply for reference. The Eagle files are the library or .lbr (which contains all the necessary components for the PCB), the schematic or .sch (which contains the PCB symbols), and the board or .brd (which has the actual pcb layout). This project does have Boards on OSHPark already and it is called Mini Radio Station.

The stl files are the case files that are ready to be 3D printed. If any modifications want to be made the f3d file is the actual fusion 360 drawing. 

Lastly, the arduino code is the ino file. This code is incomplete as the module can only be on one frequency, but you can reprogram it to be on the appropriate frequency. With some improvement a better version would allow the buttons on the M5 Stick C Plus to change frequencies. I would suggest using https://github.com/poelstra/arduino-multi-button. 
