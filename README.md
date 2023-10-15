# Chiky_Chiky_Pizza_MkI
Chiky Chiky Pizza MkI is a machine prototype that can sorting object. In this project the objects are pizza, each of them different from eachother in term of slides. For example, this pizza is full and the other is half.

# How?
This is just a prototype so I use color detection along with I2C to connect to other display which is arduino nano. If the pizza is less than 80%, the conveyor will convey it to dispenser, on the other hand if the pizza is 100% the conveyor will convey to packaging process.

# The devices that are used in project
1. Pop - 7
> This is the device use to control the camera called "Huskylens" and send the datas via I2C to Arduino Nano to display current state. The current state is statement that check if the statement is pass or not. For example, if the statement check that pizza is not pass the LED will turn red, if it passed, it will turn to green.
2. Arduino Nano
> This use to show the LED statement to end user.
3. Huskylens
> Do color detection and send those datas to Pop-7 to process it.
4. Sharp Ir sensor 
> Check if the object has already reached destination.
5. Motors
> For driven the conveyor.
