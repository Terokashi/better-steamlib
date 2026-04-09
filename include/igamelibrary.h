#pragma once

#include <vector>

#include "game.h"

class IGameLibrary
{
public:
    virtual ~IGameLibrary() = default;
    virtual std::vector<Game> loadGames() = 0;
    virtual void refresh() = 0;
};

