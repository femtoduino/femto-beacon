eagle-pcb-helpers
=================

A collection of JS utilities to help with Eagle PCB grunt work.

## Panelizer helper
A utility to help you panelize stuff outside the board size limits.

![Image of the Panelizer helper](panelizer_helper.png)

### Requires

 - Node http-server `npm install -g http-server`

### Usage
Use a terminal, navigate to this folder, and run `http-server ./`, then open up [https://localhost:8080/panelizer_helper.html](https://localhost:8080/panelizer_helper.html) in your favorite browser.

### About
Due to the artificially (and arbitrarily) imposed board size limitations of EAGLE PCB licensing, it becomes impossible to panelize large amounts of small boards.
This tool allows one to copy a column of board outlines, components, and text, and create the corresponding XML with updated part names (such as "IC1...IC5" to "IC6...IC10", etc), while applying a shift value to the X coordinates.

You still need to manually open the .brd file, and locate the last column of `<element>` nodes, but you can simply copy and paste that XML into this helper, set an X offset value, and process the text to generate the next column.

EAGLE PCB doesn't have any problem copying vias and traces outside the artificial boundry limits, so you really only need this tool for dimensions and components.