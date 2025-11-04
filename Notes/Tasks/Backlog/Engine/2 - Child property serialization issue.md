#engine
There is an issue with the child property serialization
The issue occurs when structs that are propertyowners contains other structs that are also propertyowners.
Some properties are being registered multiple times, or to the wrong property owner.
This is probably due to an issue with the constructor order.