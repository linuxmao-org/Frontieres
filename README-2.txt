//------------------------------------------------------------------------------
// FRONTIERES:  An interactive granular sampler.  
//------------------------------------------------------------------------------
//  Project website:
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2014  Christopher Carlson
// carlsonc@ccrma.stanford.edu
// www.modulationindex.com
//

// These install instructions were last updated on Monday, May 26, 2014

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.



Thank you for downloading Frontieres! 

//------------------------------------------------------------------------
// Installation:
//------------------------------------------------------------------------

Frontieres has been tested on Mavericks. 

Please make sure the following libraries are installed on your system.


RtAudio is used for real-time audio I/O
http://www.music.mcgill.ca/~gary/rtaudio/

Libsndfile is used for loading .aiff and .wav files 
http://www.mega-nerd.com/libsndfile/

Note: On OSX, I would recommend using Homebrew to install libsndfile, as it makes the process much easier:

http://brew.sh/

To install home-brew, paste this into Terminal and hit enter: 

ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"

Once Homebrew is installed, type:

brew install libsndfile

If libsndfile installs correctly, continue installing the necessary libraries to run Frontieres as described below. If the libsndfile install won’t run (or requires admin privileges, try

sudo brew install libsndfile


———————

If you haven't already, download the source from http://ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html, unzip it, 
navigate to the Frontieres directory in Terminal, and type make. 

If all goes well, the source should compile. Note, on Mavericks, there may be a number of warnings due to glut functions being deprecated. If there are no error messages and only warnings, this should be fine. 

If you get error messages, please email me at carlsonc AT ccrma DOT stanford DOT edu and I will try to help resolve any issues quickly!


//------------------------------------------------------------------------
// Before running…
//------------------------------------------------------------------------

Put your favorite .wav and .aif files into the loops directory contained in the distribution. 
These will be automatically loaded when you launch Frontieres.



//------------------------------------------------------------------------
// To launch the software…
//------------------------------------------------------------------------

Type ./Frontieres from the source directory in terminal. The screen will be black for 
a bit while your audio files load, and then you will see a title screen with instructions.



//------------------------------------------------------------------------
// Controls
//------------------------------------------------------------------------

The user interaction is primarily mouse and keyboard based. Here are the main controls:

Entry/Exit
------------
ESC	        Quit
? key	        Hide/Show Title Screen


Rectangles
------------
Left click	Select
Drag	        Move
TAB key	        Cycle selection of overlapping rectangles under mouse
R key + drag	Resize
F key	        Flip orientation


Cloud Addition and Selection
------------
G key (+ shift)	Add (remove) cloud to/from end of collection
Delete key	Remove selected cloud
Left click	Select
Drag	        Move


Cloud Parameters
------------
After selecting a cloud, parameters associated with the granular synthesis can be edited. 
Most parameters involve keyboard interfacing. Select the key corresponding to the mode, 
then press it again to change the value. In some cases, numeric keys are used to enter 
specific values. In parameters associated with grain motion, the mouse is used.

V key (+ shift)	  Add (remove) voices
A key	          Toggle cloud on/off
D key (+ shift)	  Increment (decrement) duration
D key + numbers	  Enter duration value (ms) - press Enter to accept
S key (+ shift)	  Increment (decrement) overlap
S key + numbers	  Enter overlap value - press Enter to accept
Z key (+ shift)	  Increment (decrement) pitch
Z key + numbers	  Enter pitch value - press Enter to accept
W key	          Change window type (HANNING, TRIANGLE, EXPDEC, REXPDEC, SINC, RANDOM)
W key + 
1 through 6	  Jump to specific window type
F key	          Switch grain direction (FORWARD, BACKWARD, RANDOM)
R key	          Enable mouse control of XY extent of grain position randomness
X key	          Enable mouse control of X extent of grain position randomness
Y key	          Enable mouse control of Y extent of grain position randomness




//------------------------------------------------------------------------
// Acknowledgement
//------------------------------------------------------------------------
Thanks to Ge Wang and Jorge Herrera for their guidance through the initial stages
of this project.  Thanks to Mike Rotondo his help in my early brainstorming discussions.
Thanks to Tarik Barri for encouraging me to pursue the idea and for suggesting the 
use of the second dimension of the sound rectangles!
