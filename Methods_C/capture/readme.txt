The file which contains the algorithm is process.c
-------------------------------------------------------------------------------------

The algorithm itself is option 5 in process_function()

It does two things:

Alorithm 1 : Used to detect out-of-bed event

Detects activity and considers this to be the foreground.
To detect activity it breaks teh image is 5X5 blocks.
Each block with enough chnaged pixels is considered an active block.
The bounding rectangle of all active blocks is the active region.
The top line of the active region is assumed to be the perosns height.
The centre of activity is also detected.

Algorithm 2 : Used to detect if teh medication cup was moved

While "placed" is 0, the region of cup placement is sampled and stored.
While "placed" is 1, we do nothing.
While "placed" is 2, we compare the region of cup placement to latest stored.

-------------------------------------------------------------------------------------

The function process() calls process_function with option 5, to test the algorithms.
It uses the "index" to count from start of activity. 
It uses printf statements on the console.
When using the algorithm in the RADIO system:
- Replace printf with calls to system level notifications
- Create triggers for "placed" and for "index", e.g:
  . Index is reset to 0 when a motion sensor detects movement on the bed
  . Placed is moved from 0 to 1 and then to 2 when teh cup is placed on the table

-------------------------------------------------------------------------------------
