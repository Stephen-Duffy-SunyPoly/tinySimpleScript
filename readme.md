# Tiny Simple Script
A terrible scripting language for the [TeenyAT virtual architecture](https://github.com/miniat-amos/TeenyAT)

If you are looking for a real programing language / compiler to use with this architecture, use this: https://github.com/Benricheson101/teeny

## built-in functions:  
#### General (all systems)

| function                   | description                                  |
|----------------------------|----------------------------------------------|
| delay(val)                 | stop the program for val cycles              |
| setPortA(val)              | set the value of GPIO port A                 |
| setPortB(val)              | set the value of GPIO port B                 |
| setPortADirection(dirBits) | set the direction of each pin on GPIO port A |
| setPortBDirection(dirBits) | set the direction of each pin on GPIO port B |
| val = readPortA()          | read the value of GPIO port A                |
| val = readPortB()          | read the value of GPIO port B                |
| val = random()             | get a random positive number                 |
| val = randomBits()         | get a random bit pattern                     |
| val = getCycleCount()      | get the number of cycles since reset         |
| cal = getWallTime()        | get the number of 1/16 seconds since reset   |
| resetCycleCount()          | reset the cycle count                        |
| resetWallTime()            | reset the wall time                          |
| trap()                     | stop executing the program at this funcion   |


#### LCD System 

| function               | description                                 |
|------------------------|---------------------------------------------|
| update()               | updates the screen                          |
| rect(x1,y1,x2,y2)      | draws a rectangle to the screen             |
| fill(c)                | sets the fill color                         |
| stroke(c)              | sets the stroke color                       |
| setFill(b)             | set if there should be fill                 |
| setStroke(b)           | set if there should be stroke               |
| clear()                | draws a rectablge the size of the screen    |
| point(x,y)             | draws a single pixel                        |
| print(v)               | prints a value to the terminal              |
| line(x1,y1,x2,y2)      | draw a line between the points              |
| var = getMouseX()      | get the on screen x position of the mouse   |
| var = getMouseY()      | get the on screen y position of the mouse   |
| var = getMouseButton() | get if the left mouse button is pressed     |
| var = getKeyboard()    | key a key from the keyboard that is pressed |  

If you do not set stroke and fill initially, they will be whatever stroke and fill are when update is called the first time

#### Edison System
TODO

## Language specifics
Programs are typically stored in `.tass` files

Comments come after `//`

**Declare a global variable**
```tass
gavr varname
```

**Declare a local variable**
```tass
lvar varname
```
note: Local vars can only be access within the scope they are declared. This includes scopes under where they are declared. 

**Assign a variable**
```tass
varname = value
```

**Logical conditions**
```tass
value1 <compare op> value2
```

**If statements**
```tass
if (condition) {
    ...
}

if (condition){
    ...
} else {
    ...
}
```
Note that braces must be on the same line as the keywords.

**Loops**
```tass
do {
    ...
} while (condition)
```
For infinite loops use the condition `0 == 0`

**Declaring functions**
```tass
function foo(param1, param2, ... ,paramN) {
    ...
}

function bar(param1, param2, ... ,paramN) {
    ...
    return value
}
```
Functions can have any number of parameters.  
A function can only contain 1 return, and it must be the last statement in the function.

**Calling user functions**
```tass
foo(8,var1) // call foo with the parametrs 8 and var1

myvar = bar(16) // call bar with a parameter of 16 and stire the return value in myvar

bar(myvar) // call bar wit a parameter of myvar and disreguard the return value
```

**Operating on variables**
```tass
myvar += 10 // add 10 to myvar

myvar &= otherVar // and myvar with otherVar and store the result in myvar
```


**operations:**

| Opperation  | Description                 |
|-------------|-----------------------------|
| var += val  | add val to var              |
| var -= val  | subtract val from var       |
| var *= val  | multiply val and var        |
| var /= val  | divide val by var           |
| var %= val  | mod val by var              |
| var &= val  | bitwise and val and var     |
| var \|= val | bitwise or val and var      |
| var ^= val  | bitwise xor val and var     |
| var >> val  | right shift val by var bits |
| var << val  | left shidt val by var bits  |
| var ++      | increase var by 1           |
| var --      | decrease var by 1           |
| var ~=      | negate var                  |

## "compiler" usage:

programs can be compiled using the following command
```shell
tinySimplescript ./program.tass
```

**flags:**  
`--tnasm <exe path>` supply the path of the assembler on your system. When present after compilation has finished, the generated assembly file will be automatically assembled info a valid binary.  
`--edison` Build a program for the edison system. Compiles with functions specific to the edison system and not the LCD system.  
`--nosys` Builds a program for a generic Teeny AT system with no system specific functions available.


## Example programs

Draw a rectangle around the mouse
```tass
gvar top
gvar bottom
gvar left
gvar right
gvar rectColor = 100 //note: functions can not be used to initialize vars inline with their deffiniton

do {
    lvar mouseX
    lvar mouseY
    //get the position of the mouse
    mouseX = getMouseX()
    mouseY = getMouseY()
    fill(100)//set the background color
    stroke(100)
    clear()//clear the screen
    
    rectColor ++ // change the color by 1
    fill(rectColor)//set the color of the rectangle
    stroke(rectColor)
    
    //calculate the corner positions
    left = mouseX
    right = mouseX
    left -= 5
    right += 5
    top = mouseY
    bottom = mouseY
    top -= 5
    bottom += 5
    //draw the rectangle
    rect(left,top,right,bottom)
    update() // update what is shown on the screen
    
} while(0 == 0)
```

You can also check out the varius test programs created during development inside the scripts folder



# Compiling:
Building is handled using cmake:  
should work with most modern compilers

setup:
```shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
Or just open this project in your favorite C++ IDE and let it take care of everything