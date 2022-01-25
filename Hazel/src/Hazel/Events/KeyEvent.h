#pragma once

#include "Event.h"

namespace Hazel {

  class HazelExports KeyEvent : public Event
  {
  public:
    inline int GetKeyCode() const { return m_KeyCode; }
    inline int GetKeyGetScanCode() const { return m_KeyScanCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
  protected:
    KeyEvent(int keycode, int scancode = 0)
      : m_KeyCode(keycode), m_KeyScanCode(scancode) {}

    int m_KeyCode, m_KeyScanCode;
  };

  class HazelExports KeyPressedEvent : public KeyEvent
  {
  public:
    KeyPressedEvent(int keycode, int scancode, int repeatCount)
      : KeyEvent(keycode, scancode), m_RepeatCount(repeatCount) {}

    inline int GetRepeatCount() const { return m_RepeatCount; }

    std::string ToString() const override
    {
      std::stringstream ss;
      ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
      return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
  private:
    int m_RepeatCount;
  };

  class HazelExports KeyReleasedEvent : public KeyEvent
  {
  public:
    KeyReleasedEvent(int keycode, int scancode)
      : KeyEvent(keycode, scancode) {}

    std::string ToString() const override
    {
      std::stringstream ss;
      ss << "KeyReleasedEvent: " << m_KeyCode;
      return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
  };
  
  class HazelExports KeyTypedEvent : public KeyEvent
  {
  public:
    KeyTypedEvent(int keycode)
      : KeyEvent(keycode) {}

    std::string ToString() const override
    {
      std::stringstream ss;
      ss << "KeyTypedEvent: " << m_KeyCode;
      return ss.str();
    }

    EVENT_CLASS_TYPE(KeyTyped)
  };
}