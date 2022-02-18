#pragma once

int easy_shutdown (
    const char* message = nullptr,
    int socket_descriptor = -1,
    bool should_shutdow = false
);

int easy_shutdown (int socket_descriptor);