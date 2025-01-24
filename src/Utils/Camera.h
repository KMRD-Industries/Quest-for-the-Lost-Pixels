#pragma once
#include "SFML/Graphics/View.hpp"

class Camera
{
public:
    sf::View view;
    sf::Vector2f position;
    float zoomLevel;

    Camera() = default;

    Camera(const sf::Vector2f& initialPosition, const sf::Vector2f& size) : position(initialPosition), zoomLevel(1.0f)
    {
        view.setSize(size);
        view.setCenter(position);
    }

    void setPosition(const sf::Vector2f& newPosition)
    {
        position = newPosition;
        view.setCenter(position);
    }

    void setSize(const sf::Vector2u& newSize)
    {
        view.setSize(static_cast<sf::Vector2f>(newSize));
        view.setCenter(position);
    }

    void zoom(const float& factor)
    {
        zoomLevel *= factor;
        view.setSize(view.getSize() * zoomLevel);
    }

    sf::View getView() const { return view; }
};
