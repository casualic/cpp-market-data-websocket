Addition (+) with assignment:
y = y + [4] creates a new list and reassigns y to point to it.
y now points to a different object.
append() method:
y.append(4) modifies the list in place.
y still points to the same object; its contents change.
So:
+ with = → reassignment (new object, y points elsewhere)
append() → in-place mutation (same object, contents change)