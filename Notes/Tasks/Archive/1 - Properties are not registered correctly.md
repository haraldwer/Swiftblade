Issue: 
Order of property registration is wrong

Solution: 
Property map has to be shared between instances of the same type
Call Reg() from constructor
Reg will only be run once for each type
Reg will store instance adress before calling constructor
Reg will manually call constructor 
Templatization guarantees unique property map for each type

Also, store memory offsets instead of pointers