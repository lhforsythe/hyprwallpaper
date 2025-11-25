# hyperWallpaper -- a GUI for hyprpaper written in C++ and GTK that is directly compatible with immutable setups
### NOTE: This project is very similar to WayPaper, but differs in that it is less dependent on external depedencies (pun-intended) as these are difficult to maintain on an immutable distro (which I run). The project also aims to be lighter and more efficient, as it makes use of C++ rather than Python. Thus, once I get the project to a more complete state, I will package it as a flatpak.

<img width="960" height="1046" alt="image" src="https://github.com/user-attachments/assets/5b02f622-ec01-492a-b6db-224554d29c72" />

### Feature Checklist:
* Automatic creation of a wallpaper directory ✅
* List all images in wallpaper directory without manual input ✅
* Connection to hyprpaper to allow wallpaper change ✅
* Multi-monitor support (not yet, currently just changes the wallpaper on all monitors)
* Multi-directory support (with categorization of wallpaper types)
* Directory refresh
* Proper settings page (with configs like changing the default wallpaper directory)
* Possibly implement a way to access, browse, and apply wallpapers from online sites.
* drag-drop import

### Required Dependencies:
Nothing except hyprpaper, which is usually already bundled with defualt hyprland installs.

### Installation:
Will upload a proper way to install, but for right now it can be compiled using CMake and the gtkmm4 development package. Flatpak and/or appimage is a definite to-do.

### First launch:
After launching, a directory called "Wallpapers" will be placed within $HOME/Photos (if there isn't already one). Place all wallpapers within this directory.
