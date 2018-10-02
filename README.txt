//------------------------------------------------------------------------------
//
//     BORDERLANDS:  An interactive granular sampler.  
//          Version 0.04
//------------------------------------------------------------------------------
//  Project website:
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2011  Christopher Carlson
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
Thank you for downloading Borderlands! 
//------------------------------------------------------------------------
// Release Notes:
//------------------------------------------------------------------------
Version 0.04
-Independent volume control for clouds in dB, controlled by b key (with number entry or shift modification as described in usage instructions)
-Full screen/windowed toggle mode - press o key
-Bug fix - selected grain duration was not being applied to newly added grains (due to missing break in a switch statement�)
Version 0.03
-Multichannel support - change MY_CHANNELS in theglobals.h to use > 2 channels, use T key to switch between spatialization modes
-Playback rate LFO frequency control - K key
-Playback rate LFO amount control - L key
Version 0.02
 -Linux compatible version released.
 -Extraneous files removed from the distribution.  
 -Some platforms may have problems displaying the grains - they may appear as squares
  instead of circles. This is due to lack of support for GL_POINT_SMOOTH on the 
  user's graphics card.  A version that uses gluDisc to render the grains is 
  available at the project site.  It is lower quality, but it works�  Squares are nice too...
Version 0.01 
 -First version, produced for Music 256a at Stanford. OSX only.
//------------------------------------------------------------------------
// Installation:
//------------------------------------------------------------------------
Borderlands has only been tested on OSX 10.6.8. 
Please make sure the following libraries are installed on your system.
Libsndfile is used for loading .aiff and .wav files 
http://www.mega-nerd.com/libsndfile/
NOTE: When installing libsndfile, make sure you use the following command. The current documentation for libsndfile is not clear
in this regard.
./configure && make && sudo make install
"sudo" is necessary to give you root privileges for the install.
NOTE: On OSX, I would recommend using Homebrew (http://brew.sh/) to install libsndfile (rather than installing manually as described above). Homebrew makes the process much easier:
To install home-brew, paste this into Terminal and hit enter: 
ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
Once Homebrew is installed, type:
brew install libsndfile
If libsndfile installs correctly, continue installing Borderlands as described below. If the libsndfile install won�t run (or requires admin privileges, try
sudo brew install libsndfile
Once libsndfile has successfully installed, navigate to the Borderlands_v0X/Borderlands directory in Terminal, and type make. 
Note - if you don�t understand what is meant by �navigate to the Borderlands directory,� follow these steps (if you are on a Mac):
-Open Terminal
-Type cd and add a space
-Open the Borderlands_v04 directory in Finder
-Drag the Borderlands folder into the Terminal window next to cd (should now look like cd Users/you/blah/etc/Borderlands_v04/Borderlands). 
-Hit enter to change directories. 
-Now type   make  and press enter. Borderlands should start compiling.
If all goes well, the source should compile. You may see a number of warnings about glut functions being deprecated if you are on Mavericks. Please ignore these unless you get any errors that prevent compiling from completing. If you see errors, please email me at carlsonc AT ccrma DOT stanford DOT edu and I will try to help resolve any issues quickly!
//------------------------------------------------------------------------
// Before running�
//------------------------------------------------------------------------
Put your favorite .wav and .aif files into the loops directory contained in the distribution. 
These will be automatically loaded when you launch Borderlands.
//------------------------------------------------------------------------
// To launch the software�
//------------------------------------------------------------------------
Type ./Borderlands from the source directory in terminal. The screen will be black for 
a bit while your audio files load, and then you will see a title screen with instructions.
//------------------------------------------------------------------------
// Controls
//------------------------------------------------------------------------
The user interaction is primarily mouse and keyboard based. Here are the main controls:
Entry/Exit
------------
ESC	        Quit
? key	        Hide/Show Title Screen
o key		Toggle fullscreen/windowed modes
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
T key		  Switch spatialization modes
L key (+ shift)   Adjust playback rate LFO frequency
K key (+ shift)	  Adjust playback rate LFO amplitude
B key (+ shift)	  Adjust cloud volume in dB
//------------------------------------------------------------------------
// Acknowledgement
//------------------------------------------------------------------------
Libraries included in Borderlands:
Synthesis Toolkit
https://ccrma.stanford.edu/software/stk/
RtAudio is used for real-time audio I/O
http://www.music.mcgill.ca/~gary/rtaudio/
Thanks to Ge Wang and Jorge Herrera for their guidance through the initial stages
of this project.  Thanks to Mike Rotondo his help in my early brainstorming discussions.
Thanks to Tarik Barri for encouraging me to pursue the idea and for suggesting the 
use of the second dimension of the sound rectangles!
