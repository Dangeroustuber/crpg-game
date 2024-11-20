#pragma once

#include "pch.h"

#pragma warning(disable: 4201)

struct Event {
    enum class Key : uint16_t {
        Unknown = 0,     
        W,A,S,D
    };

    enum class MouseButton : uint8_t {
        Left,
        Middle,
        Right
    };

    enum class Type : uint8_t {
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        KeyDown,
        KeyUp
    };

    Type type;

    union {
        struct { // mouse move
            int32_t mouseMoveX;
            int32_t mouseMoveY;
        };

        struct { // mouse press 
            MouseButton mouseButton;
            int32_t mousePressX;   
            int32_t mousePressY;   
        };

        struct {
            Key keyCode;
        };
    };

};