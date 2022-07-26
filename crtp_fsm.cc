#ifndef __test_fsm_cc__
#define __test_fsm_cc__

#include <iostream>
#include <variant>

#define FSM_TRANSITION(inst, event, new_state, ...) inst.fsm_transition(event, new_state, ##__VA_ARGS__)

template<typename T>
struct fsm {

    template<typename State>
    State& set_state(State& new_state) {

        if constexpr (!std::is_same<T, State>::value) {
            T& current_state = *static_cast<T*>(this);
            /* leaving the current state */
            current_state.onExit();
            /* going into new state */
            new_state.onEntry();
        }

        return(new_state);
    }

    template<typename Event, typename State, typename... Args> 
    State& fsm_transition(Event evt, State new_state, Args... args) {
        /* do not do state transition if current_state and new_state are same */
        if constexpr (!std::is_same<T, State>::value) {

            T& current_state = *static_cast<T*>(this);
            /* invoke the hook for processing the incoming request */
            current_state.handle_event(evt, args...);

        }
        return(set_state(new_state));
    }
};

struct onDiscover: fsm<onDiscover> {
    typedef enum {evt1, evt2, evt3} evt;
    void onEntry() {
        std::cout << "In onDiscover::onEntry" << std::endl;
    }

    void onExit() {
        std::cout << "In onDiscover::onExit" << std::endl;
    }

    int handle_event(evt event) {
        std::cout << "In onDiscover::handle_event" << std::endl;
        return(0);
    }

    int handle_event(evt event, int arg) {
        std::cout << "In onDiscover::handle_event" << std::endl;
        return(0);
    }
};

struct onOffer : fsm<onOffer> {
    typedef enum {evt1, evt2, evt3} evt;
    void onEntry() {
        std::cout << "In onOffer::onEntry" << std::endl;
    }

    void onExit() {
        std::cout << "In onOffer::onExit" << std::endl;
    }

    int handle_event(evt event) {
        std::cout << "In onOffer::handle_event" << std::endl;
        return(0);
    }

    int handle_event(evt event, int arg) {
        std::cout << "In onOffer::handle_event" << std::endl;
        return(0);
    }
};

using states_t = std::variant<onDiscover, onOffer>;

template<typename... Args>
struct active_instance {

    auto operator()(onDiscover& discover, Args... args) {
        std::cout << "current state is onDiscover " << std::endl;
        onOffer offer;
        states_t st;
        /* feed to fsm now */
        st = FSM_TRANSITION(discover, onDiscover::evt1, offer, args...);
        return(st);
    }
    
    auto operator()(onOffer& offer, Args... args) {
        std::cout << "current state is onOffer " << std::endl;
        onDiscover discover;
        states_t st;
        /* feed to fsm now */
        st = FSM_TRANSITION(offer, onOffer::evt1, discover, args...);
        return(st);
    }

    template<typename State>
    auto operator()(State& defaultSt, Args... args) {
        std::cout << "current state is defaultSt " << std::endl;
        State st;
        return(st);
    }
};



struct fsm_user {
     states_t m_states;

    fsm_user() {
        m_states = FSM_TRANSITION(std::get<onDiscover>(m_states), onDiscover::evt1, std::get<onDiscover>(m_states));
        std::get<onDiscover>(m_states).onEntry();
    }

    int process_request() {
        int x = 12;
        std::cout << "fsm_user::process_request " << std::endl;
        /* Findout which instance is active in std::variant */
        m_states = std::visit(active_instance(), m_states);
        
        return(0);
    }
};

int main() {
    onDiscover discoverSt;
    onOffer offerSt;
    int arg1_val = 20;
    //auto new_state = discoverSt.set_state(offerSt);
    auto new_state = discoverSt.fsm_transition(onDiscover::evt1, offerSt);
    /* start receiving on current state */
    //new_state.handle_event();

    //new_state.fsm_transition(onOffer::evt1, discoverSt);
    FSM_TRANSITION(new_state, onOffer::evt1, discoverSt);
    FSM_TRANSITION(new_state, onOffer::evt1, discoverSt, arg1_val);

    std::cout << "instantiating fsm_user " << std::endl;
    fsm_user fsmUser;
    fsmUser.process_request();
    fsmUser.process_request();
    //offerSt.setState(discoverSt);
}











#endif /*__test_fsm_cc__*/
