#include "cpx/SerialProtocol.h"
#include <stdio.h>

namespace cpx {

void SerialProtocol::begin(light::LightEngine* engine) {
    m_engine = engine;
    m_buffer.reserve(96);
}

void SerialProtocol::update() {
    while (Serial.available() > 0) {
        const char ch = static_cast<char>(Serial.read());
        if (ch == '\r') continue;
        if (ch == '\n') {
            processLine(m_buffer);
            m_buffer = "";
        } else {
            m_buffer += ch;
        }
    }
}

void SerialProtocol::processLine(const String& line) {
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
    if (line == "CLEAR") {
        m_engine->clearManual();
        Serial.println("OK CLEAR");
        return;
    }
    if (line == "TRIGGER IDLE") {
        m_engine->handleEvent(light::LightEvent::Idle);
        Serial.println("OK TRIGGER IDLE");
        return;
    }
    if (line == "TRIGGER FIRE") {
        m_engine->handleEvent(light::LightEvent::Fire);
        Serial.println("OK TRIGGER FIRE");
        return;
    }
    if (line == "TRIGGER RELOAD") {
        m_engine->handleEvent(light::LightEvent::Reload);
        Serial.println("OK TRIGGER RELOAD");
        return;
    }
    if (line == "TRIGGER HIT") {
        m_engine->handleEvent(light::LightEvent::Hit);
        Serial.println("OK TRIGGER HIT");
        return;
    }
    if (line == "TRIGGER CHARGE_START") {
        m_engine->handleEvent(light::LightEvent::ChargeStart);
        Serial.println("OK TRIGGER CHARGE_START");
        return;
    }
    if (line == "TRIGGER CHARGE_STOP") {
        m_engine->handleEvent(light::LightEvent::ChargeStop);
        Serial.println("OK TRIGGER CHARGE_STOP");
        return;
    }

    if (line.startsWith("CURSOR ")) {
        float u = 0.5f;
        float v = 0.5f;
        if (sscanf(line.c_str(), "CURSOR %f %f", &u, &v) == 2) {
            m_engine->setCursor(u, v);
            Serial.println("OK CURSOR");
        } else {
            Serial.println("ERR CURSOR");
        }
        return;
    }

    if (line.startsWith("SET_PIXEL ")) {
        int i = 0, r = 0, g = 0, b = 0;
        if (sscanf(line.c_str(), "SET_PIXEL %d %d %d %d", &i, &r, &g, &b) == 4) {
            m_engine->setManualPixel(static_cast<size_t>(i), light::Rgb{static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)});
            Serial.println("OK SET_PIXEL");
        } else {
            Serial.println("ERR SET_PIXEL");
        }
        return;
    }

    Serial.print("ERR UNKNOWN ");
    Serial.println(line);
}

} // namespace cpx
