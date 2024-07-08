<div align="center">
  <a href="https://github.com/Maxwell-SS/Capybara-Desktop-Pet">
    <img src="readme/icon.png" alt="Logo" width="80" height="80">
  </a>

  <h1 align="center">Capybara Desktop Pet</h1>

  <p align="center">
    A charming pixel art Capybara companion
  </p>
</div>

## Showcase
<div align="center">
  <img src="readme/gif.gif" alt="Capybara Running">
</div>

## About The Project
A small pixel art capybara that scurries around the bottom of your screen! It isnt interactive and does not block mouse clicks. It is made with C++, Objective-C, and uses OpenGL, GLFW, GLM, and STB. 

**Note:** This application is compatible with macOS only.

## Getting Started

### Prerequisites
Ensure you have `cmake` and `make` installed on your machine.

### Installation

#### Building from Source
To build the application from source:
```sh
git clone https://github.com/Maxwell-SS/Capybara-Desktop-Pet.git
cd Capybara-Desktop-Pet
mkdir build
cd build
cmake ..
make
./Capybara
```
To build a .app bundle use:
```sh
cmake .. -DBUILD_BUNDLE=ON
```
This generates a bundle folder containing the .app.

#### Downloading the DMG
1. Navigate to [releases](https://github.com/Maxwell-SS/Capybara-Desktop-Pet/releases).
2. Download the latest DMG file.
3. Open the DMG and drag the application to your Applications folder.

#### Resolving macOS Security Restrictions
If you encounter a security error stating the application is damaged:
Open a terminal and run these three commands.
```sh
sudo spctl --master-disable
xattr -cr /Applications/Capybara.app
sudo spctl --master-enable
```

## Usage
Upon running the application, a widget will appear in the top right of your screen. This widget allows you to quit the capybara or toggle its visibility across all desktops.

## Credits
Pixel Art by [Rainloaf](https://rainloaf.itch.io/capybara-sprite-sheet)

## License
Distributed under the MIT License. See `LICENSE` for more information.
