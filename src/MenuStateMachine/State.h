#pragma once

class State {
public:
    virtual ~State() = default;
	virtual void init() = 0;
	virtual void update(const float deltaTime) = 0;
    virtual void render() = 0;
};