#pragma once



/*
There's a lot of ways to use createwindow overlay. All depends on your implementation. Haven't had a problem as a single user on Apex for 2+ month's now.

You could also try:
- hijacking other windows like NVIDIA. https://github.com/iraizo/nvidia-overlay-hijack. You should be careful changing window flags tho.
- CreateWindow without topmost and other giveaway flags.
- kernel drawing (you need a way to safely hook NtGdiDdDDISubmitCommand for this one) https://github.com/BadPlayer555/KernelGDIDraw
- or go internal and find more options.

createwindow and FindWindow are both safe to use to findand create overlay, just keep in mind :
- No MOSTTOP
- no randomclass & window - name like "87drg93asiu5?436?se"
- dont create overlay with size of game(examp.game: 1920x1080 | your overlay = 1900x1070) and just center it.
- set overlay more than one above the game window
*/