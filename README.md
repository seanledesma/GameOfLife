Let's talk about what we need to do to get the grid system to work.

I have a window, which has it's coordinates according to pixels. For example, pixel 0 on X axis would be on the far left,
the far right pixel is equal to screen width. I need to translate these window pixel coordinates to grid coordinates that
are independent of the window, since I want to move around the grid and zoom in/out with my mouse.

To paint the grid, it's a bit tricky since the space between the lines I'll draw will change depending on zoom, which
is not actually too hard since I'll keep the cell size * zoom, and keep the lines seperated by that number.

So first I'll try to get the grid printed using cell width, then try to implement zoom, then pan. I'll need a helper funciton
that can convert between window pixel coordinates and virtual grid coordinates.