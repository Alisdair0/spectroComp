# Spectrogram Component
## Created by Alisdair Chauvin
*May 5, 2026*

This is my first go at creating an audiovisualizer that can be used to analyze audio and be used to enhance a projects visual appearance. 
I created spectroComp using the JUCE Framework and a JUCE tutorial on how to visual audio. 
After, I enhanced the color scheme and added sliders to adjust the visualization.

*Usage*
I coded this plugin using C++, so there is an accommodating CMakeLists.txt file that builds this plugin. It is
currently setup to build an AU, VST3, and Standalone version. The standalone is the one I used to troubleshoot, the AU
can be used in Logic specifically, and the VST3 version can be used in any other DAW.

To use, set the input selection to a microphone that can hear adequate sound, and adjust sliders to your liking. To use for internal audio, download
a virtual playback engine (https://existential.audio/blackhole/) and adjust inputs and outputs to route computer audio into the plugin.
Feel free to download and use as you'd like. Enjoy ;)


<img width="799" height="426" alt="Screenshot 2026-05-05 at 4 48 33 PM" src="https://github.com/user-attachments/assets/120290c8-23ec-45bb-9117-297060da3eb9" />


