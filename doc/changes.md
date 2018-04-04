# Changes made to the Template

This file lists and justifies all changes made to the [mCc Template](https://github.com/W4RH4WK/mCc) not directly related to any of the assignments.

- - -
Added Braces to the declaration of the if_stmt:

```
 if_stmt: = "if" , "(" , expression , ")" , "{", statement, "}" , 
                  [ "else" , "{", statement, "}" ] 
```