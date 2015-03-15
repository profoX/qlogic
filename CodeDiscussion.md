# Code Discussion #

This page is meant for the developers to discuss the code and talk about different ways to implement certain things.

## Wiring user interface ##

The wiring user interface is completely different from KLogic. Wesley's idea is to implement it like this:

  * <font color='green'>When in <i>wire mode</i> user clicks and holds the left mouse button on an item to draw a wire between that item and another item</font>
  * <font color='green'>When the mouse cursor is over another item, the shortest possible/allowed wire between the two items should be shown</font>
  * <font color='green'>When the connection is invalid, there should be an "invalid" icon over the target item</font>
  * <font color='green'>When the connection is valid, there should be an "input" or "output" icon in front of the target icon, connected to the wire.</font>
  * <font color='green'>When the target is an item which is capable of input AND output (like logic gates) the chosen mode depends on the source item:<br>
<ul><li>Source item is an input item: wire will be an "out" connection from source to target<br>
</li><li>Source item is an output item: wire will be an "in" connection from source to target<br>
</li><li>Source item is an input AND output item: connection type will be decided depending on which side of the target item the mouse cursor is located. Visual feedback (input/output icon) will help the user</font>
</li></ul>  * <font color='red'>When there is no target item and the end of the wire is an empty space, a connecting dot item will be created. Other wires can be connected to this dot item, or the dot item can be replaced by another (real) item.</font>
  * <font color='red'>New wires can be attached anywhere on existing wires; a dot item will be created.</font>
  * <font color='red'>Wires should snap to certain pixel positions.</font>
  * <font color='green'>Items should snap to certain pixel positions (to improve alignment of items).</font>
  * <font color='green'>Every item should have a property which decides on which sides it can send or receive signals.</font>
  * <font color='green'>There should be a small offset between the item and the connected wire, the offset should be a small straight line (like in KLogic)</font>
  * <font color='red'>Location of multiple input wires on the same side of an item should be equally divided among the available space of that side of the item.</font>
  * <font color='green'>Multiple output signals on the same side should start from the same point (center).</font>
  * <font color='green'>Wires should move together with the item they are attached to when the position of an item is changed</font>

Legend:
  * <font color='green'>Implemented</font>
  * <font color='blue'>Work in progress</font>
  * <font color='red'>Not started</font>

**Please post comments about this design approach below this line!**