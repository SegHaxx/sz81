		    saverom - save ROM from ZX81.

		Public domain by Russell Marks, 2001.


Description
-----------

saverom saves a ZX81's ROM in the usual ZX81 tape-saving format.
Normally you can't save anything other than Basic programs from a
ZX81; saverom gets around this by only using the save-a-byte-to-tape
routine, and doing the rest itself. The saved ROM should be readable
by anything which can read ZX81 tapes (e.g. zx81get) and which doesn't
check that the actual data looks like a Basic program. The ZX81
filename given to the saved file is `b', mainly because this helps
keep the program short. :-)

Note that saverom does *not* require a 16k ZX81 - it will work
perfectly well with just the 1k, and in fact I recommend running it
that way to avoid any possible problems with RAM pack wobble.


Warning
-------

Before running saverom, you should obviously check that your tape
reader can cope with reading the ZX81's output (if it's connected
directly), or a tape saved from it (if not).

Note that you may *have* to go via tape (or some other kind of
recording) if your machine's sound device isn't getting the signal
from the ZX81 to a high enough level. (Indeed, I had to do this myself
when testing the program.) A `noisy' signal generally isn't too much
of a problem with zx81get, but the signal does still have to be at a
decent level.


Getting the program to the ZX81
-------------------------------

With that out of the way, the usual way to get saverom across to the
ZX81 is to type it in (an alternative approach is described later on).
The machine code is designed with this in mind, so it's relatively
short and can be entered as `Basic' directly, without having to enter
hex or anything.

The program goes like this:

  10 REM code here
  20 RAND USR 16514

The "code here" bit is listed below, with separate tokens/chars
separated by spaces for clarity (but *don't* type any spaces unless it
actually *says* "space"!). It's listed on multiple lines, but should
be typed on a single line 10 (after the REM):

	LN 7 ? 5 0 > ( RETURN F

	invJ invO invP TAB invM

	RND I ( LN 2 graphs7 5

	space space Y RUN ? <=

	RETURN 3 SQR LN 2 graphs7

	7 FAST D D D LPRINT ABS

	invO RND : space INKEY$

	( RETURN TAN / USR / STOP

A few things need explaining about that:

- 0 is zero, O is capital o. It's very, very important not to confuse
  the two. :-)

- `invA' means an inverse A. To type this, do shift-9 (for GRAPH
  mode), then `A', then shift-9 again. (Better to stay in graph mode
  for the three in a row, though. :-))

- `graphs7' is similar - shift-9, shift-7, shift-9.

- RETURN and RUN need to be entered in K mode. To do this, type THEN
  (shift-3) followed by the keyword, then go back and delete THEN,
  then move the cursor forward again. So for RETURN for example, it's
  shift-3, Y, shift-5, shift-0, shift-8.

- don't forget, `<=' is a single token (shift-r).

You might want to see how that looks on a ZX81 once it's typed in
correctly, to check you've got it right, so there's LLIST output here
in listing.png.


Alternative transfer method
---------------------------

Skip this if you decided to type in the program. :-)

If you'd rather not type in the program, and have a Linux box with a
suitable sound device, you could try using my `zx81send' (see
zx81send.c in ../tetris-src.tar.gz) to send the program (a copy of
which is here as saverom.p, in case you want to do it this way). But I
won't say any more about this, as it's probably easier to keep the
transfer one-way.


Running the program
-------------------

Once you've got the program in place, and everything's ready to go,
run it. There should be a few seconds of silence before the data
starts being saved, and you'll see the usual tape-saving pattern on
the screen. The saving process should take less than 4 minutes. If
things go wrong on the receiving end (for example), you can press
space (break) to abort, and re-run the program to try again.

If all goes well, you should end up with an 8192-byte `.p' file,
probably called `b.p'. This isn't really a .p of course, so rename it
to `zx81.rom'. The usual ROM gives the following output from `md5sum
zx81.rom':

db398d4e4e93a6d4dee3bfe146918219  zx81.rom

So if you get that output, you know you've successfully transferred
the usual ZX81 ROM. If not, try a second transfer and see if the files
match - I believe early ZX81s had slightly different ROMs, so it could
still be ok. (But if the two copies don't match, something's clearly
gone wrong.)

BTW, after saving the ROM, saverom leaves the ZX81 in FAST mode. This
shouldn't be a problem, but I just thought I'd mention it. :-)


Source
------

FWIW, assembly source for the machine code is here as saverom.z.


-Rus.
