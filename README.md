# flintnsteel

A dead simple project generator for [Cinder](http://libcinder.org/) projects on Linux. flintnsteel is designed to be a command line interface (cli). For Windows or MacOS please see [TinderBox](https://github.com/cinder/TinderBox).

## Installation

1. Clone this repo
2. Build using CMake

```bash
cd flintnsteel
mkdir build
cd build
cmake ..
make -j4
```
3. Set defaults

```
cp defaults.example defaults
# Edit the resulting file
```
4. (Optional) Add flintnsteel to your path in `~/.profile`.
5. Use it!

## Example Usage

```bash
# change directories to where you want to create the app
flintnsteel MyApp
```

```bash
# change directories to where you want to create the app
flintnsteel -t "myawesometemplate" -p "/path/to/cinder/root" MyApp
```

## Contributing

Feel free to suggest changes and report bugs by [creating an issue](https://github.com/violetcraze/flintnsteel/issues/new). You are also welcome to make pull requests with templates.
