#archive 
There should be an UI library with some basic functionality. 

Structure: 
```
Element - An element that can be placed in a container
	Label - A label widget
	Button - A button widget
	Image - An image
	Container - Can contain other elements
		Canvas - An area with a set of elements, placed freely
		ListBox - An area where the elements are placed in a list format
```

The UI can be constructed in c++ and will not require loading / saving. 
The UI is not animated and does not change.

Concepts:
Rect - The position and size
Margin - top, bottom, left, right offsets
Padding - The extra area around an object to consider during calculations
Alignment - The way that you calculate the position of an element relative to the parent
	top bottom left right - 0 - 1 
	0 = attached to the parent rect, value works as an offset, size does not matter
	1 = moves freely, relative to parent rect, size matters
Anchor - The position to consider as "parent" when calculating children offsets


