#include "cpx/SerialProtocol.h"
#include <stdio.h>

namespace cpx {

void SerialProtocol::begin(light::LightEngine* engine,
                           light::PulseBlobEffect* pulse,
                           light::StripeFieldEffect* stripes,
                           light::FlashFieldEffect* flash) {
    m_engine = engine;
    m_pulse = pulse;
    m_stripes = stripes;
    m_flash = flash;
    m_buffer.reserve(96);
}

void SerialProtocol::update(uint32_t nowMs) {
    while (Serial.available() > 0) {
        char ch = static_cast<char>(Serial.read());
        if (ch == '\r') continue;
        if (ch == '\n') {
            processLine(m_buffer, nowMs);
            m_buffer = "";
        } else {
            m_buffer += ch;
        }
    }
}

void SerialProtocol::processLine(const String& line, uint32_t nowMs) {
    if (!m_engine) return;

    if (line == "PING") {
        Serial.println("PONG");
        return;
    }
    if (line == "MODE ANIM") {
        m_engine->setMode(light::EngineMode::Animation);
        Serial.println("OK MODE ANIM");
        return;
    }
    if (line == "MODE MANUAL") {
        m_engine->setMode(light::EngineMode::Manual);
        Serial.println("OK MODE MANUAL");
        return;
    }
    if (line == "PLAY PULSE") {
        m_engine->setAnimationField(m_pulse);
        Serial.println("OK PLAY PULSE");
        return;
    }
    if (line == "PLAY STRIPES") {
        m_engine->setAnimationField(m_stripes);
        Serial.println("OK PLAY STRIPES");
        return;
    }
    if (line == "PLAY FLASH") {
        m_engine->setAnimationField(m_flash);
        m_flash->trigger(nowMs);
        Serial.println("OK PLAY FLASH");
        return;
    }
    if (line == "TRIGGER FLASH") {
        m_flash->trigger(nowMs);
        Serial.println("OK TRIGGER FLASH");
        return;
    }
    if (line == "CLEAR") {
        m_engine->clearManual();
        Serial.println("OK CLEAR");
        return;
    }

    if (line.startsWith("SET_PIXEL ")) {
        int i, r, g, b;
        const int parsed = sscanf(line.c_str(), "SET_PIXEL %d %d %d %d", &i, &r, &g, &b);
        if (parsed == 4 && i >= 0 && i < 16) {
            m_engine->setManualPixel(static_cast<size_t>(i), light::Rgb{static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)});
            Serial.println("OK SET_PIXEL");
        } else {
            Serial.println("ERR SET_PIXEL");
        }
        return;
    }

    if (line.startsWith("PULSE_CENTER ")) {
        float u, v;
        const int parsed = sscanf(line.c_str(), "PULSE_CENTER %f %f", &u, &v);
        if (parsed == 2 && m_pulse) {
            m_pulse->centerU = u;
            m_pulse->centerV = v;
            Serial.println("OK PULSE_CENTER");
        } else {
            Serial.println("ERR PULSE_CENTER");
        }
        return;
    }

    Serial.print("ERR UNKNOWN ");
    Serial.println(line);
}

}
