A lunar lander kind-of clone. Just without landing anything.

## How to install

First, install the dependencies

`sudo apt-get install build-essential libglew-dev libglfw3-dev portaudio19-dev libsndfile1`

If you had trouble with this try

`sudo apt-get install build-essential libglew-dev libglfw3-dev`

Then download the .zip package from this page. Extract it and cd to the extracted folder. Run the following to play:

    make
    ./ss

If you had trouble with that try

    make mute
    ./ss
