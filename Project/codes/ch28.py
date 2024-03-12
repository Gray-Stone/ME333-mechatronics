#! /usr/bin/python3

# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
ser = serial.Serial('/dev/ttyUSB0',230400)
print('Opening port: ')
print(ser.name)

state_map ={
0:"IDLE", 
1:"PWM", 
2:"ITEST", 
3:"HOLD", 
4:"TRACK", 
}

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\td: Dummy Command \tq: Quit') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead

    if (selection == 'c'):
        n_str = ser.read_until(b'\n');  # get the incremented number back
        count = int(n_str) # turn it into an int
        print(f"The motor is at {count} count")
    elif (selection == 'b'):
        n_str = ser.read_until(b'\n');
        count = float(n_str) # turn it into an int
        print(f"Current {count} mA")
    elif (selection == 'd'):
        n_str = ser.read_until(b'\n');  # get the incremented number back
        count = float(n_str) # turn it into an int
        print(f"The motor is at {count} degree")
    elif (selection=="r"):
        # Get mode 
        curr_state = int(ser.read_until(b'\n'));  # get the incremented number back
        print(f"current state: {state_map.get(curr_state,'Invalid_state')} ")

    elif (selection == 'e'):
        n_str = ser.read_until(b'\n');  # get the incremented number back
        print(n_str)
    elif (selection == 'q'):
        
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    else:
        print('Invalid Selection ' + selection_endline)



