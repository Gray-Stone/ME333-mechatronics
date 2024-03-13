#! /usr/bin/python3

# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import matplotlib.pyplot as plt 
from genref import genRef
from statistics import mean 
import time

def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data lengeth = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_int = list(map(int,dat_str.split())) # now the data is a list
        ref.append(dat_int[0])
        data.append(dat_int[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()

def read_plot_matrix_pose(ref_traj):
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    print(f"n of index {n_str}")
    n_int = int(n_str) # turn it into an int
    print('Data lengeth = ' + str(n_int))
    # ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        anlge = float(dat_str)
        # dat_int = list(map(int,dat_str.split())) # now the data is a list
        # ref.append(dat_int[0])
        data.append(anlge)
        data_received = data_received + 1
    meanzip = zip(ref_traj,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = []

    for a in range(len(ref)):
        t.append(a/200)
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.ylabel('value')
    plt.xlabel('index')
    plt.show()



import serial
ser = serial.Serial('/dev/ttyUSB0',230400)
print('Opening port: ')
print(ser.name)

state_map ={
0: "IDLE",
1: "PWM",
2: "ITEST",
3: "HOLD",
4: "TRACK",
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
        print(f"Got raw [{n_str}]")
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


    elif (selection == 'e'):
        n_str = ser.read_until(b'\n')
        print(n_str)

    elif (selection == 'f'):
        value = int(input("\nEnter PWM value for mode f (from -100 to 100)"))
        value = max( min(value, 100), -100)
        print(f"Sending {value}")
        ser.write(f"{value}\n".encode() )
        feedback = ser.read_until(b'\n')
        print(f"Feed back with {feedback}")


    elif (selection=="r"):
        # Get mode
        curr_state = int(ser.read_until(b'\n'));  # get the incremented number back
        print(f"current state: {state_map.get(curr_state,'Invalid_state')} ")

    # Current PI loop
    elif (selection == 'g'):
        pvalue = float(input("\nEnter P gain : "))
        ivalue = float(input("\nEnter I gain : "))
        print(f"Sending P {pvalue} , I {ivalue}")
        ser.write(f"{pvalue} {ivalue}\n".encode())
        feedback = ser.read_until(b'\n')
        print(f"Feed back with {feedback}")

    elif (selection == 'h'):
        n_str = ser.read_until(b'\n')
        # get the incremented number back
        print(f" Gain value returned: {n_str}")
    elif (selection == 'k'):
        read_plot_matrix()

    elif (selection == 'i'):
        pid_string = input("\nEnter pos control PIDs : ") # now the data is a list
        pid_string += "\n"
        pid_string = pid_string.encode()
        print(f"Sending {pid_string}")
        ser.write(pid_string)
        feedback = ser.read_until(b'\n')
        print(f"Feed back with {feedback}")
    elif (selection == 'j'):
        n_str = ser.read_until(b'\n')
        # get the incremented number back
        print(f"gain value returned: {n_str}")
    elif (selection == 'l'):
        target_ang = float(input("\nEnter target angle : "))
        ser.write(f"{target_ang}\n".encode())
        feedback = ser.read_until(b'\n')
        print(f"Feed back with:\n{feedback}")

    elif (selection == 'm'):
        # ref = genRef('step',[0, 0, 0.5,90, 2, 45, 3, 45] )
        ref = genRef('step',[0, 0, 0.5,90, 2.5, 45, 3, 45] )
        # ref = genRef('step',[0,10,0.005,120 , 0.02 , 45])
        print(len(ref))
        t = []

        for a in range(len(ref)):
            t.append(a/200)

        plt.plot(t,ref,'r*-')
        plt.ylabel('value')
        plt.xlabel('index')
        plt.show()
        ser.write(f" {len(ref)} \n".encode())
        count =0 
        time.sleep(0.5)
        for deg in ref:
            time.sleep(0.001)
            encoding = f"{deg}\n".encode()
            print(f"{count}, Sending: {encoding}")
            count+=1
            ser.write(encoding)
        feedback = ser.read_until(b'\n')            
        print(f"Feed back with:\n{feedback}")

    elif (selection == 'n'):
        ref = genRef('cubic',[0, 0, 0.5,90, 1.2, 45, 2, 45])
        print(len(ref))
        t = []

        for a in range(len(ref)):
            t.append(a/200)

        plt.plot(t,ref,'r*-')
        plt.ylabel('value')
        plt.xlabel('index')
        plt.show()
        ser.write(f" {len(ref)} \n".encode())
        count =0 
        time.sleep(0.5)
        for deg in ref:
            time.sleep(0.001)
            encoding = f"{deg}\n".encode()
            print(f"{count}, Sending: {encoding}")
            count+=1
            ser.write(encoding)
            ser.timeout = 10
        feedback = ser.read_until(b'\n')        
        print(f"Feed back with:\n{feedback}")
    
    elif (selection == 'o'):
        read_plot_matrix_pose(ref)



    elif (selection=='p'):
        ser.write("r\n".encode())
        curr_state = int(ser.read_until(b'\n'));  # get the incremented number back
        print(f"current state: {state_map.get(curr_state,'Invalid_state')} ")

    elif (selection == 'q'):

        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()
    elif (selection == "z"):
        print("Whatever from PIC")
        ser.timeout = 0.5
        feedback = ser.read_until(b'\n')            
        print(f"[{feedback}]")

    else:
        print('Invalid Selection ' + selection_endline)
