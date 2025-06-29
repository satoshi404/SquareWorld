
#pragma once
#include <glm/glm.hpp>
#include <string>

class Spotlight {
private:
    std::string name;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec4 color;
    float cutoff;   // Inner cutoff angle (degrees)
    float intensity;

public:
    Spotlight(const std::string& name);
    std::string getName() const { return name; }
    glm::vec3 getPosition() const { return position; }
    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getDirection() const { return direction; }
    void setDirection(const glm::vec3& dir) { direction = dir; }
    glm::vec4 getColor() const { return color; }
    void setColor(const glm::vec4& col) { color = col; }
    float getCutoff() const { return cutoff; }
    void setCutoff(float c) { cutoff = c; }
    float getIntensity() const { return intensity; }
    void setIntensity(float i) { intensity = i; }
};
