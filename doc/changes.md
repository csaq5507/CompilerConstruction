# Changes made to the Template

This file lists and justifies all changes made to the [mCc Template](https://github.com/W4RH4WK/mCc) not directly related to any of the assignments.

##Change #1

Changed expressions like e.g.

    (a<b || b==10) 

to

    ((a<b) || (b==10))
    
since our the parser does not handle the higher binding of the different operations.

- - -
