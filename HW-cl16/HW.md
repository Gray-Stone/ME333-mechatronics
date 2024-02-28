Chapter 25 #7

Chapter 26 #4 part a only


# CH 25 

## Q7 

> You are choosing a motor for the last joint of a new direct-drive robot arm design. 
(A direct-drive robot does not use gearheads on the motors, creating high speeds with low
friction.) Since it is the last joint of the robot, and it has to be carried by all the other
joints, you want it to be as light as possible. From the line of motors you are considering
from your favorite motor manufacturer, you know that the mass increases with the motor’s
power rating. Therefore you are looking for the lowest power motor that works for your
specifications. Your specifications are that the motor should have a stall torque of at least
0.1 Nm, should be able to rotate at least 5 revolutions per second when providing
0.01 Nm, and the motor should be able to operate continuously while providing 0.02 Nm.
Which motor do you choose from Table 25.1? Give a justification for your answer.

We have 4 motors, with poser 3, 10 , 20 ,90

stall torque 0.1Nm: all motors but motor 3 could satisfy this requirement.

With the 0.02Nm normal work load requirement, motor 3 is eliminated as its max continuous torque is only 0.002Nm

Using the torque-speed relationship, we could have this equation

$$
\tau = \tau_{stall} *(1- {\omega\over\omega_{max}})
$$

We simply need to plug in the values to find out how much torque each motor can do at 300 rpm

| motor |10|20|90|
|-|-|-|-|
| $\omega_{max}$ (rpm) | 4980| 9660| 7180|
| $\tau_{max}$ (mNm) | 131 | 225 | 929|
| $\tau$ (mNm) at 300 rpm | 123 | 218 | 890 |

From the table we can tell that all three motor: 10, 20, 90 meets our need. Since we want the smallest motor, motor 10 is enough for us.


# ch 26

## 4
>You are working for a startup robotics company designing a small differential-drive mobile robot, and your job is to choose the motors and gearing. A diff-drive robot has two wheels, each driven directly by its own motor, as well as a caster wheel or two for balance. Your design specifications say that the robot should be capable of continuously climbing a 20◦ slope at 20 cm/s. To simplify the problem, assume that the mass of the whole robot, including motor amplifiers, motors, and gearing, will be 2 kg, regardless of the motors and gearing you choose. Further assume that the robot must overcome a viscous damping force of (10 Ns/m)×v when it moves forward at a constant velocity v, regardless of the slope. The radius of the wheels has already been chosen to be 4 cm, and you can assume they never slip. If you need to make other assumptions to complete the problem, clearly state them. You will choose among the 15 V motors in Table 25.1, as well as gearheads with G = 1, 10, 20, 50, or 100. Assume the gearing efficiency η for G = 1 is 100%, and for the others, 75%. (Do not combine gearheads! You get to use only one.)

### a 
> Provide a list of all combinations of motor and gearhead that satisfy the specifications, and explain your reasoning. (There are 20 possible combinations: four motors and five gearheads.) “Satisfy the specifications” means that the motor and gearhead can provide at least what is required by the specifications. Remember that each motor only needs to provide half of the total force needed, since there are two wheels.

requirement:
* 20 cm/s on 20 deg slop
* 2 kg overall load

To overcome gravity, the wheel need to provide a constant forward force of 
$$
sin(20) * 2kg * 9.8 = 6.703 N
$$

* wheel radius of 4cm

wheel circumference is 25.14 cm
This result in a rotation velocity of 
$$
{{20 cm/s}\over{ 25.14 cm/rev }} = 0.7955 rev/s
$$

or 47.97 rpm

* overcome friction of 10 Ns/m * V 

at speed of 20cm/s, the force to overcome the damping force is 
$$
10Ns/m * 0.2m/s = 2N 
$$

* each wheel provide half of the force.

Total forward force is $2N+6.703N=8.703N$ then distributed to each wheel, each wheel 
need to provide 4.3515N of linear force. 
With 4cm wheel radius. The torque needed is $4.3515 * 0.04 = 0.17406Nm$

With the help of python code, I can find all the torque each motor and gearhead combination can provide at required speed. 

See `MotorTorqueFinder.py` for the code

| motor name | gear ratio | output_torque| meets requirement? | 
| --- |--- |---| --- |  
| motor 3 | 1 | 0.009964201492537314 | False |
| motor 3 | 10 | 0.07231511194029852 | False |
| motor 3 | 20 | 0.13926044776119406 | False |
| motor 3 | 50 | 0.3078777985074627 | True |
| motor 3 | 100 | 0.48151119402985076 | True |
| motor 10 | 1 | 0.12973813855421687 | False |
| motor 10 | 10 | 0.8878603915662651 | True |
| motor 10 | 20 | 1.5864415662650604 | True |
| motor 10 | 50 | 2.5465097891566266 | True |
| motor 10 | 100 | 0.36103915662650576 | True |
| motor 20 | 1 | 0.22388268633540373 | True |
| motor 20 | 10 | 1.6037014751552796 | True |
| motor 20 | 20 | 3.039805900621118 | True |
| motor 20 | 50 | 6.3425368788819885 | True |
| motor 20 | 100 | 8.49514751552795 | True |
| motor 90 | 1 | 0.9227932966573817 | True |
| motor 90 | 10 | 6.501997249303622 | True |
| motor 90 | 20 | 12.072988997214487 | True |
| motor 90 | 50 | 23.19993123259053 | True |
| motor 90 | 100 | 23.12472493036212 | True |

It is obvious in the output, only 4 combinations can't meet the requirement, and other 16 combinations work.