#include "Button.hpp"
#include "Theme.hpp"

namespace gui
{

Button::Button(const sf::String& string)
{
    m_box.item().setFont(Theme::getFont());
    m_box.item().setCharacterSize(Theme::textSize);
    m_box.setSize(Theme::minWidgetWidth, Theme::getBoxHeight());
    setString(string);
    setSize(m_box.getSize());
}


void Button::setString(const sf::String& string)
{
    m_box.item().setString(string);

    // Recompute widget width
    int fittingWidth = m_box.item().getLocalBounds().width + Theme::PADDING * 2 + Theme::borderSize * 2;
    int width = std::max(fittingWidth, Theme::minWidgetWidth);
    m_box.setSize(width, Theme::getBoxHeight());
    m_box.centerTextHorizontally(m_box.item());
    setSize(m_box.getSize());
}


const sf::String& Button::getString() const
{
    return m_box.item().getString();
}


void Button::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    sf::RenderStates rs(states.blendMode, getTransform(), &Theme::getTexture(), states.shader);
    target.draw(m_box, rs);
}

// Callbacks -------------------------------------------------------------------

void Button::onStateChanged(State state)
{
    m_box.applyState(state);
}


void Button::onMouseMoved(float x, float y)
{
    if (getState() == StatePressed)
    {
        if (containsPoint(sf::Vector2f(x, y)))
            m_box.press();
        else
            m_box.release();
    }
}


void Button::onMousePressed(float x, float y)
{
    m_box.press();
}


void Button::onMouseReleased(float x, float y)
{
    if (containsPoint({x, y}))
    {
        triggerCallback();
    }
}


void Button::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
    {
        triggerCallback();
        m_box.press();
    }
}


void Button::onKeyReleased(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Enter)
    {
        m_box.release();
    }
}

}
