#include "AudioProcessor.h"

AudioProcessor::AudioProcessor() : decoder(&current_file, &mp3) {
    // Decoder is initialized with references to the file and mp3 objects.
}

bool AudioProcessor::openFile(const String& filepath) {
    if (current_file) {
        current_file.close();
    }
    
    current_file = SD.open(filepath);
    if (!current_file) {
        Serial.println("Failed to open file: " + filepath);
        return false;
    }
    
    decoder.transformationReader().resizeResultQueue(1024 * 8);
    if (!decoder.begin()) {
        Serial.println("Decoder begin() failed");
        current_file.close();
        return false;
    }
    
    Serial.printf("Opened file: %s\n", filepath.c_str());
    return true;
}

void AudioProcessor::closeFile() {
    if (current_file) {
        current_file.close();
    }
    decoder.end();
}

int32_t AudioProcessor::readAudioData(uint8_t* buffer, int32_t len) {
    if (!current_file || !current_file.available()) {
        return 0; // Signal end of track
    }
    
    int32_t bytes_read = decoder.readBytes(buffer, len);
    
    // If we didn't get the full buffer, fill the rest with silence
    if (bytes_read < len) {
        memset(buffer + bytes_read, 0, len - bytes_read);
    }
    
    return len; // Always return the requested length for A2DP
}