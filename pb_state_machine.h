#pragma once

#include <unordered_map>

class state_machine
{
public:
	state_machine() = delete;
	virtual ~state_machine() = 0;

	virtual void process() = 0;

protected:
	enum state;
	enum event;

	state machines_state;

	typedef void (*state_func)();
	typedef event (*check_func)();
	typedef std::pair<state, event> state_event_pair;

	std::unordered_map<state, check_func> event_getter;
	std::unordered_map<state_event_pair, state_func> transitions;

	virtual void initialize_machine() = 0;

	/*
		Ich brauch noch eine Funktion, die das Event bekommt
		Für jeden State eine Check funktion, die in Map gespeichert ist, diese gibt das event zurück

		Maybeeee States als eigene Klasse -> NEIN
	*/
};

class bugger_ki : public state_machine {

};

