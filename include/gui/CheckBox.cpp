#include "CheckBox.hpp"
#include "Theme.hpp"

namespace gui
{

CheckBox::CheckBox(bool checked):
    m_box(Box::Input)
{
    int offset = Theme::PADDING + Theme::borderSize;
    float box_size = m_cross.getSize().x + offset * 2;
    m_box.setSize(box_size, box_size);
    m_cross.setPosition(offset, offset);
    check(checked);

    setSize(m_box.getSize());
}


bool CheckBox::isChecked() const
{
    return m_checked;
}


void CheckBox::check(bool checked)
{
    m_checked = checked;
}


void CheckBox::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
    sf::RenderStates rs(states.blendMode, getTransform(), states.texture, states.shader);
    target.draw(m_box, states);
    if (m_checked)
        target.draw(m_cross, rs);
}

// callbacks -------------------------------------------------------------------

void CheckBox::onStateChanged(State state)
{
    m_box.applyState(state);
}


void CheckBox::onMouseReleased(float x, float y)
{
    if (containsPoint(sf::Vector2f(x, y)))
    {
        check(!m_checked);
        triggerCallback();
    }
}


void CheckBox::onKeyPressed(const sf::Event::KeyEvent& key)
{
    if (key.code == sf::Keyboard::Space)
    {
        check(!m_checked);
        triggerCallback();
    }
}

}
