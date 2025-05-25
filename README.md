# Buoyant  
Welcome to **Buoyant**, a compiler for **Buoya**!

## Buoya
Get to know some Buoyancy!
### Number
Buoya only supports integers in range (0, 255). Examples:
```
4
52
0
```
### Variable
Variables can hold numbers. Their names have to consist only of these characters: `@#$%`. Variable examples:
```
#
$$$$
%#$@$##@
```
### Expression
Expressions are anything that can be evaluated to a number. They are made of 0 or more mathematical operations: addition(`+`), substraction(`-`), multiplication(`o`) and division(`:`). The first two are of a lesser precedence the second two. You can use `()`, too. Expressions use numbers as well as variables.
> **Note**: before variables can be used in expressions, they must have been [assigned a value](#simple-statement). 

Examples:
```
23
$#
@@ - (53 + 2) o 2 : $##
```
### Statement
A program in Buoya is basically a list of statements. There are two types of statements, simple statements and if statements.
#### Simple statement
Simple statements are responsible for assigning values to variables. The basic syntax for a simple statement is:
`. variable ~ expression .`
If you want to assign zero, you can write:
`. variable ~~ .`
> **Important!** In Buoya, there is a special variable called `&`. If you assign something to this variable, your program will exit with exit code equal to the assigned expression.

You can join simple statements by putting a `,` between them:\
 `. smpl-stmt1 , smpl-stmt2 .`

Examples:
```
.@@@ ~~.
.%$% ~ (## + 4) : 2, ## ~ ##, % ~~.
.& ~ $ o (4 + #$@).
```
#### If Statement
[info about if statement]

## Buoyant
Buoyant compiles you Buoya code into assembly (will work on Linux x86-64 machines and wsl) and then automatically creates an executable for you. You can find all the build files in `/build`.

To help you write working code, Buoyant offers a great range of error messages. If it failed to compile your program, it will give you tell you exactly where and what mistake you made!

All supported exit codes can be found below.\
General:
- 0 -> Everything ok
- 1 -> Wrong usage
- 2 -> Wrong file extention
- 3 -> Ifstream failed (probably something wrong with your file)

Lexing:
- 10 -> Buoya does not support that (invalid characters)

Parsing:
- 20 -> Expected statement begin
- 21 -> Expected statement end
- 22 -> Expected ..... end (you did not close a bracket)
- 23 -> Expected scope end
- 24 -> Invalid term in an expression
- 25 -> Invalid statement
- 26 -> Invalid ....... statement (you messed up a simple statement)

Generating:
- 30 -> No variable like that (you used a variable before assigning value to it)