#pragma once // event.hpp
// MIT
// Allosker ------------------------------
// =========+
// Simplicistic container for window related events as well its different types
// ---------------------------------------

#include "utilities/window.hpp"
#include <variant>


class Event
{
public:

	// = Event definitions

	struct Resized
	{
		vec2i size;
	};

	struct MouseButtonPressed
	{
		MouseButtons scancode;
		KeyboardModes mode;
	};

	struct MouseButtonReleased
	{
		MouseButtons scancode;
		KeyboardModes mode;
	};

	struct MouseMoved
	{
		vec2f pos;
	};

	struct MouseWheelScrolled
	{
		vec2f delta;
	};

	struct KeyPressed
	{
		Keys scancode;
		KeyboardModes mode;
	};

	struct KeyReleased
	{
		Keys scancode;
		KeyboardModes mode;
	};

	
	// = Predicates

	template<typename EventSubType>
	bool is()
	{
		static_assert(isEventSubtype<EventSubType>, "EventSubType must be a subtype of the class Event");
		if constexpr (isEventSubtype<EventSubType>)
			return std::holds_alternative<EventSubType>(m_data);
	}

	template<typename EventSubType>
	const EventSubType* get_if()
	{
		static_assert(isEventSubtype<EventSubType>, "EventSubType must be a subtype of the class Event");
		if constexpr (isEventSubtype<EventSubType>)
			return std::get_if<EventSubType>(&m_data);
	}


	// = Initilisation

	template<typename EventSubType>
	Event(const EventSubType& type)
		: m_data{ std::in_place_type<EventSubType>, type }
	{
	}
	

private:

	std::variant <
		Resized,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseWheelScrolled,
		KeyPressed,
		KeyReleased
	> m_data;


	template <typename T, typename... Ts>
	[[nodiscard]] static constexpr bool isInVariant(const std::variant<Ts...>*)
	{
		return std::disjunction_v<std::is_same<T, Ts>...>;
	}

	template <typename T>
	static constexpr bool isEventSubtype = isInVariant<T>(decltype (&m_data)(nullptr));
};