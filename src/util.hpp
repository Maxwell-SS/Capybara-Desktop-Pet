#pragma once

#include <GLFW/glfw3.h>

class TimeStep {
public:
    TimeStep() : lastFrame(0.0f), deltaTime(0.0f), startTime(0.0f) {}

    void update() {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    float getSeconds() const { return deltaTime; }
    float getMilliseconds() const { return deltaTime * 1000.0f; }

    void startTimer() { startTime = glfwGetTime(); }
    float stopTimer() { 
        float endTime = glfwGetTime();
        return endTime - startTime; 
    }

private:
    float lastFrame;
    float deltaTime;
    float startTime;
};