#include "spotlight.h"

Spotlight::Spotlight(const std::string& name)
    : name(name),
      position(0.0f, 0.0f, 0.0f),
      direction(0.0f, 0.0f, -1.0f),
      color(1.0f, 1.0f, 1.0f, 1.0f),
      cutoff(12.5f),
      intensity(1.0f) {}
