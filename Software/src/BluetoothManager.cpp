#include "BluetoothManager.h"
#include "MusicPlayer.h"
#include "AudioProcessor.h"

// Static variable for callbacks
BluetoothManager* BluetoothManager::instance = nullptr;

// External references
extern AudioProcessor audio_processor;

BluetoothManager::BluetoothManager(const String& device_name) :
    target_device(device_name),
    music_player(nullptr),
    is_connected(false) {
    instance = this; // For static callbacks
}

void BluetoothManager::setMusicPlayer(MusicPlayer* player) {
    music_player = player;
}

bool BluetoothManager::initialize(const String& local_name) {
    if (!music_player) {
        Serial.println("Error: MusicPlayer not set");
        return false;
    }
    
    Serial.println("Initializing Bluetooth A2DP...");
    
    a2dp_source.set_local_name(local_name.c_str());
    a2dp_source.set_data_callback(audioDataCallback);
    a2dp_source.set_on_connection_state_changed(connectionStateCallback);
    a2dp_source.set_avrc_passthru_command_callback(avrcCommandCallback);
    
    a2dp_source.start();
    
    Serial.println("Bluetooth initialized - discoverable mode active");
    return true;
}

bool BluetoothManager::connect() {
    Serial.printf("Connecting to: %s\n", target_device.c_str());
    a2dp_source.start(target_device.c_str());
    a2dp_source.set_connected(true);
    is_connected = true;
    return true;
}

void BluetoothManager::disconnect() {
    Serial.println("Disconnecting...");
    is_connected = false;
    a2dp_source.set_connected(false);
}

bool BluetoothManager::VolumeSet(int vol) {
    a2dp_source.set_volume(vol);
    return true;
}

int32_t BluetoothManager::audioDataCallback(uint8_t* data, int32_t len) {
    if (!data || len <= 0) {
        return 0;
    }

    if (!instance || !instance->music_player) {
        memset(data, 0, len);
        return len;
    }
    
    if (instance->music_player->isBusy()) {
        memset(data, 0, len);
        return len;
    }

    PlayerState state = instance->music_player->getState();
    if (state == PlayerState::STOPPED || state == PlayerState::PAUSED) {
        memset(data, 0, len);
        return len;
    }
    
    int32_t result = audio_processor.readAudioData(data, len);
    
    if (result == 0) {
        Serial.println("Track finished, moving to next...");
        if (instance->music_player) {
            instance->music_player->notifyTrackFinished();
        }
        memset(data, 0, len);
        return len;
    }
    
    return result;
}

void BluetoothManager::connectionStateCallback(esp_a2d_connection_state_t state, void* ptr) {
    if (!instance) return;
    
    Serial.print("A2DP connection state: ");
    
    switch (state) {
        case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
            Serial.println("DISCONNECTED");
            instance->is_connected = false;
            if (instance->music_player) {
                instance->music_player->notifyConnectionStateChanged(false);
            }
            break;
            
        case ESP_A2D_CONNECTION_STATE_CONNECTING:
            Serial.println("CONNECTING");
            break;
            
        case ESP_A2D_CONNECTION_STATE_CONNECTED:
            Serial.println("CONNECTED");
            instance->is_connected = true;
            if (instance->music_player) {
                instance->music_player->notifyConnectionStateChanged(true);
            }
            break;
            
        case ESP_A2D_CONNECTION_STATE_DISCONNECTING:
            Serial.println("DISCONNECTING");
            break;
    }
}

void BluetoothManager::avrcCommandCallback(uint8_t key, bool isReleased) {
    if (!instance || !instance->music_player || !isReleased || instance->music_player->isBusy()) return;
    
    Serial.print("AVRC Command: ");
    
    switch (key) {
        case ESP_AVRC_PT_CMD_PLAY:
            Serial.println("PLAY");
            instance->music_player->executeCommand(PlayerCommand::PLAY);
            break;
            
        case ESP_AVRC_PT_CMD_PAUSE:
            Serial.println("PAUSE");
            instance->music_player->executeCommand(PlayerCommand::PAUSE);
            break;
            
        case ESP_AVRC_PT_CMD_STOP:
            Serial.println("STOP");
            instance->music_player->executeCommand(PlayerCommand::STOP);
            break;
            
        case ESP_AVRC_PT_CMD_FORWARD:
            Serial.println("NEXT");
            instance->music_player->executeCommand(PlayerCommand::NEXT_TRACK);
            break;
            
        case ESP_AVRC_PT_CMD_BACKWARD:
            Serial.println("PREVIOUS");
            instance->music_player->executeCommand(PlayerCommand::PREV_TRACK);
            break;
            
        case ESP_AVRC_PT_CMD_VOL_UP:
            Serial.println("VOLUME UP");
            instance->music_player->executeCommand(PlayerCommand::VOLUME_UP);
            break;
            
        case ESP_AVRC_PT_CMD_VOL_DOWN:
            Serial.println("VOLUME DOWN");
            instance->music_player->executeCommand(PlayerCommand::VOLUME_DOWN);
            break;
            
        default:
            Serial.printf("Unknown: 0x%02X\n", key);
            break;
    }
}
