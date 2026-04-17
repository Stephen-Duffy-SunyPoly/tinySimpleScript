# tiny simple script
A terrible scripting language for the [TeenyAT virtual architecture](https://github.com/miniat-amos/TeenyAT)

If you are looking for a real programing language / compiler to use with this architecture, use this: https://github.com/Benricheson101/teeny

built-in functions:  
LCD  

| function          | description                              |
|-------------------|------------------------------------------|
| update()          | updates the screen                       |
| rect(x1,y1,x2,y2) | draws a rectangle to the screen          |
| fill(c)           | sets the fill color                      |
| stroke(c)         | sets the stroke color                    |
| setFill(b)        | set if there should be fill              |
| setStroke(b)      | set if there hsould be stroke            |
| clear()           | draws a rectablge the size of the screen |
| point(x,y)        | draws a single pixel                     |
| print(v)          | prints a value to the terminal           |
| line(x1,y1,x2,y2) | draw a line between the points           |
If you do not set stroke and fill initially, they will be whatever stroke and fill are when update is called the first time

comments come after //

operations:

| Opperation   | Description             |
|--------------|-------------------------|
| var += val   | add val to var          |
| var -= val   | subtract val from var   |
| var *= val   | multiply val and var    |
| var /= val   | divide val by var       |
| var %= val   | mod val by var          |
| var &= val   | bitwise and val and var |
| var \|= val  | bitwise or val and var  |
| var ^= val   | bitwise xor val and var |
