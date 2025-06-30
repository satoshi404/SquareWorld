// src/bird.h
#ifndef BIRD_H
#define BIRD_H

#include <string>

template <typename T>
struct Vec2 {
    T x, y;
    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}
};

class Bird {
    std::string texture_source;
    Vec2<float> size;
    Vec2<float> pos;
    Vec2<float> velocity; // For movement and jumping

public:
    Bird() : texture_source(""), size{50.0f, 50.0f}, pos{100.0f, 300.0f}, velocity{0.0f, 0.0f} {}

    void SetPosition(float x, float y) {
        pos = Vec2<float>{x, y};
    }

    void SetSize(float w, float h) {
        size = Vec2<float>{w, h};
    }

    void SetVelocity(float vx, float vy) {
        velocity = Vec2<float>{vx, vy};
    }

    void Jump(float force) {
        velocity.y = -force; // Negative for upward jump
    }

    Vec2<float> GetPosition() const { return pos; }
    Vec2<float> GetSize() const { return size; }
    Vec2<float> GetVelocity() const { return velocity; }

    std::string GetTextureSource() const { return texture_source; }
    void SetTextureSource(const std::string& source) { texture_source = source; }
};

#endif