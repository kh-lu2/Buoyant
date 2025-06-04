# Buoyant  
Welcome to **Buoyant**, a compiler for **Buoya**!

## Buoya
Get to know some [Buoyancy](#buoyancy)!
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
Expressions are anything that can be evaluated to a number. They are made of 0 or more mathematical operations: addition(`+`), substraction(`-`), multiplication(`o`) and division(`:`). The first two are of a lesser precedence than the second two. You can use `()`, too. Expressions use numbers as well as variables.
> **Note:** before variables can be used in expressions, they must have been [assigned a value](#simple-statement). 

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
> **Important!** In Buoya, there is a special variable called `&`. If you assign something to this variable, your program will exit with exit code equal to the assigned expression. Buoya exits with 0 by default.

You can join simple statements by putting a `,` between them:\
 `. smpl-stmt1 , smpl-stmt2 .`

Examples:
```
.@@@ ~~.
.%$% ~ (## + 4) : 2, ## ~ ##, % ~~.
.& ~ $ o (4 + #$@).
```
#### If Statement
If statements are, well, if statements. The syntax is similiar to the way other programming languages do it, taking Python as example.

Python:
```python
if binary_expression:
  statements
elif binary_expression:
  statements
else:
  statements
```
Buoya:
```
[ expresssion ]
  statements
{ expression }
  statements
!
  statements
^
```
A notable difference between the two is that Buoya does not support binary expressions. It compares given expression to 0 instead.

To have a valid if statement all you need is `[]` and `^` (it ends the statement). After `[]` you can have 0 or more `{}` and then an optional `!`.
> **Note:** If statements are statements. It means that inside them you can use both simple statements and if statements.

Example:
```
[@]
  .$$ ~ 7 o (2 + ##@).
{@ - 1}
  .$$ ~ 7.
  [$$ - 7]
    .@ ~~, @@ ~ 1.
  !
    .@ ~ 1.
  ^
  .& ~~.
{@ - 2} 
  .%%%% ~ $$ + 2, $$ ~~, @@$ ~ %%%% o 4.
^
```

### Additional information
Buoya ignores all white spaces and does not force you to write code in any particular way. However, to make it as readable as possible it is recommended to follow the examples above.

## Buoyant
Buoyant compiles your Buoya code into assembly (will work on Linux x86-64 machines and wsl) and then automatically creates an executable for you. You can find all the build files in `/build`.

To help you write working code, Buoyant offers a great range of error messages. If it failed to compile your program, it will tell you exactly where and what mistake you made!

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
- 27 -> Invalid scope end (you did not end the scope after else)

Generating:
- 30 -> No variable like that (you used a variable before assigning value to it)

## Buoyancy
Buoyancy is the power of a liquid to make an object float[^1]. The word looks absolutely gorgeous and is one of the very few words whose meaning you can feel based on its pronounciation. I decided it would be a great foundation for naming this project, and that is how Buoya and its compiler, Buoyant, came to life.

> **Fun fact:** Buoyancy is also a person's ability to remain cheerful, even is sad or unpleasant situations[^2].

[^1]: From Longman Dictionary of Contemporary English
[^2]: From Collins English Dictionary
