## ESP32 Bluetooth MP3 Player

This project is a **proof of concept** and an **experiment** to build a Bluetooth A2DP source device using an **ESP32**. It reads MP3 files from a microSD card and streams the audio wirelessly to Bluetooth headphones or speakers.

### Features

  * **A2DP Source:** The ESP32 acts as an A2DP source, not a sink, meaning it streams audio *to* a device like Bluetooth headphones.
  * **MicroSD Card:** Reads MP3 audio files from a microSD card, supporting a simple file system navigation.
  * **Playlist Management:** Automatically scans the SD card for `.mp3` files and creates an alphabetical playlist.
  * **Serial Control:** Provides a basic command-line interface via the serial monitor to control playback (play, pause, next, previous) and manage the playlist.
  * **AVRC Support:** Responds to playback control commands (play, pause, next, previous) sent from the connected Bluetooth device.

-----

### Project Structure & Dependencies

This project is built upon the fantastic libraries by **[Phil Schatzmann](https://github.com/pschatzmann)**. A huge thank you for these tools, which make this project possible\!

  * **[`arduino-audio-tools`](https://github.com/pschatzmann/arduino-audio-tools)**: Handles audio processing and decoding.
  * **[`ESP32-A2DP`](https://github.com/pschatzmann/ESP32-A2DP)**: Manages the Bluetooth A2DP source functionality.
  * **[`arduino-libhelix`](https://github.com/pschatzmann/arduino-libhelix)**: Provides the MP3 decoding engine (based on the Helix MP3 decoder).

### Hardware Requirements

  * **ESP32** (I used ESP32-WROOM-32D, but any ESP32 board should work)
  * **MicroSD card reader module**
  * **MicroSD card** with MP3 files
  * **Connecting wires**

Connect your SD card reader to the ESP32. The code is configured for the CS pin to be on **GPIO5**. You may need to adjust this constant (`SD_CS_PIN`) in the code to match your wiring.

The default SPI pins on the ESP32 are:
* **MISO**: GPIO19
* **MOSI**: GPIO23
* **SCK**: GPIO18
* **SS (Chip Select)**: GPIO5

-----

### Getting Started

#### 1\. Software Setup

To get started, you'll need to use PlatformIO.

  * **Install PlatformIO**: If you haven't already, follow the [official PlatformIO installation guide](https://platformio.org/install).

  * **Clone the Repository**: Clone this project's repository to your local machine.
    ```bash
    git clone https://github.com/GiacoBot/ESP32MP3
    cd ESP32MP3
    ```

  * **Initialize Submodules**: This project uses several libraries as Git submodules. Run the following command to download them:

    ```bash
    git submodule update --init --recursive
    ```

#### 2\. Hardware and Code Configuration

  * **Connect Hardware**: Wire your SD card reader to the ESP32. The code is configured for the **CS pin to be on GPIO5**. If you've used a different pin, you'll need to update the `SD_CS_PIN` constant in the code to match your wiring.

  * **Target Device Name**: By default, the code is set to connect to a device named "Lenovo LP40". You can change this to your desired Bluetooth device's name by modifying the `TARGET_DEVICE_NAME` constant in the code.
    ```cpp
    const char* TARGET_DEVICE_NAME = "YOUR DEVICE NAME HERE";
    ```

  * **Prepare Your MP3 Files**: The audio library requires MP3 files to have a sample rate of **44100 Hz**. You can use a tool like `ffmpeg` to convert your files. Here's a command for converting a single file:

    ```bash
    ffmpeg -i input.mp3 -ar 44100 output.mp3
    ```

    Once converted, place the MP3 files on your microSD card and insert it into the reader.

#### 3\. Uploading the Code

  * **Open Project in VS Code**: Launch VS Code and open the project folder.
> [!IMPORTANT]  
> The PlatformIO project is located within the `Software` folder. When you open this project in VS Code, make sure you open the `Software` directory, not the root of the repository, to ensure PlatformIO can find all the necessary files.
  * **Upload**: Use the PlatformIO toolbar in VS Code to build and upload the sketch to your ESP32 board.

-----

### Usage

1.  **Connect ESP32**: Power on your ESP32 board.

2.  **Open Serial Monitor**: Open the Serial Monitor in VS Code (it should already have the baud rate set to 115200). The ESP32 will automatically scan the SD card and build the playlist.

3.  **Control Playback**: Once connected, you can use the commands listed below in the Serial Monitor. The project also supports playback commands sent directly from your connected device (e.g., play/pause buttons on your headphones).

      * `c`: Connect to the device specified by `TARGET_DEVICE_NAME`.
      * `d`: Disconnect from the current Bluetooth device.
      * `p`: Pause or resume playback.
      * `n`: Play the next track in the playlist.
      * `b`: Play the previous track.
      * `l`: List all tracks on the playlist.
      * `1-9`: Play a specific track number (e.g., typing `3` will play the third song).
      * `r`: Rescan the SD card to update the playlist.
      * `s`: Show the current playback status.
      * `h`: Display the help message.

-----

This project serves as a great starting point for anyone looking to experiment with ESP32 audio streaming and Bluetooth functionality. Feel free to fork it, modify it, and expand on its features\!