# wav2bmp

wav2bmp makes a spectrum from an audio file in a simple way.

## Building

Firstly, install the dependencies:

```sh
# For Ubuntu/Debian only. Other systems you'll need to install manually.
sh deps.sh
```

It's very straightforward to build it with cmake.

```sh
mkdir -p build && cd build
cmake ..
cmake --build .
```

## Usage

```sh
# Simple use
./wav2bmp -i 1.flac -o 1.png

# Let's rtfm
./wav2bmp --help
```
