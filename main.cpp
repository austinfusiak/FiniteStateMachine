
#include <iostream> 
#include <map> 
#include <stdexcept> 
#include <assert.h>
#include <boost/function.hpp> 

using namespace std;

class FiniteStateMachine; 

class State {

	public:
		virtual void init( FiniteStateMachine & a_aFiniteStateMachine ) = 0; 
	
		std::string getStateIdentifer() { return m_aStateIdentifier; }

	public:
		std::string m_aStateIdentifier;	
}; 

class StateFullObject {

	public:
		StateFullObject();
		
		StateFullObject( std::string aInitState ) : m_aCurrentState ( aInitState ) { }

		~StateFullObject() {}  

		std::string getCurrentState(){ return m_aCurrentState; } 
			
		void setCurrentState( std::string & a_aState ) { m_aCurrentState = a_aState; }

	private:
		std::string m_aCurrentState; 
};

class FiniteStateMachine {

	public:
		
		typedef boost::function < bool ( StateFullObject & a_aStateFulObject ) > StateTransitionFunc; 
		typedef std::pair < std::string, std::string > StateEventPair;
		typedef std::pair < StateTransitionFunc, std::string > StateFuncPair;
		typedef std::map < StateEventPair, StateFuncPair > StateTransitionCnt; 

	public:
		FiniteStateMachine() {
			std::cout << "Initializing FiniteStateMachine" << std::endl;
		}

		~FiniteStateMachine() {} 
		
		bool processEvent( std::string a_aEvent, StateFullObject & a_aStateFullObject ){
		
			try { 	
				std::cout << "Processing State Transition currentState:[" << a_aStateFullObject.getCurrentState() << "] event:[" << a_aEvent << "]" << std::endl; 	
				
				StateEventPair l_aStateEventPair ( a_aStateFullObject.getCurrentState(), a_aEvent ); 

				StateTransitionCnt::iterator l_aStateFuncIter = m_aStateTransitionCnt.find( l_aStateEventPair ); 
				if ( l_aStateFuncIter == m_aStateTransitionCnt.end() ) {
					throw std::runtime_error("Failed to find state transition");
				} 
		
				StateTransitionFunc l_aStateTransitionFunc = l_aStateFuncIter->second.first; 
				std::string l_aNextState = l_aStateFuncIter->second.second;
				
				bool l_bStateTransitionResult = l_aStateTransitionFunc( a_aStateFullObject ); 
				if ( not l_bStateTransitionResult ) {
					throw std::runtime_error("Failed to transition state succesfully");
				}

				a_aStateFullObject.setCurrentState( l_aNextState );  
				std::cout << "Finished Processing State Transition currentState:[" << a_aStateFullObject.getCurrentState() << "]" << std::endl; 	

			} catch ( const std::exception & ex) {
			       	std::cout << "Unknown Exception Caught Failed to process event, ex:" << ex.what() << std::endl;
				return false; 
			} catch ( const std::string & ex) {
			       	std::cout << "Unknown Exception Caught Failed to process event, ex:" << ex << std::endl; 
				return false; 
			} catch( ... ) {
			       std::cout << "Unknown Exception Caught Failed to process event" << std::endl; 
				return false; 
			}
				
			return true; 
		} 
		
		void registerStateTransition( std::string a_aCurrentState, std::string a_aEvent, StateTransitionFunc & a_aStateTransitionFunc, std::string a_aNextState )
		{
			std::cout << "Registering State transition a_aCurrentState:[" << a_aCurrentState << "] a_aEvent:[" << a_aEvent << "] a_aNextState:[" << a_aNextState << "]" << std::endl; 
			StateEventPair l_aStateEventPair ( a_aCurrentState, a_aEvent ); 
			StateFuncPair l_aStateFuncPair ( a_aStateTransitionFunc, a_aNextState ); 
			m_aStateTransitionCnt [ l_aStateEventPair ] = l_aStateFuncPair; 
		}
		
		void print() {
			std::cout << "Printing State Transition Container" << std::endl;
			std::cout << "State Transition Container size:[" << m_aStateTransitionCnt.size() << "]" << std::endl;
			

			for ( StateTransitionCnt::iterator iter = m_aStateTransitionCnt.begin(); iter != m_aStateTransitionCnt.end(); iter ++ ) { 

				StateEventPair l_aStateEventPair = iter->first;
				StateFuncPair l_aStateFuncPair = iter->second; 

				std::cout << "a_aCurrentState:[" << l_aStateEventPair.first << "] " \
					" a_aEvent:[" << l_aStateEventPair.second << "] " \
					" a_aNextState:[" << l_aStateFuncPair.second << "]" << std::endl; 
			}
		}

	private:
		StateTransitionCnt m_aStateTransitionCnt;

};

class InitialState : public State {


	public:

		InitialState() {
			m_aStateIdentifier = std::string("InitialState");
		} 

		void init( FiniteStateMachine & a_aFiniteStateMachine ) {

			FiniteStateMachine::StateTransitionFunc l_aStateTransitionFunc = InitialState::onEventOneToFinalState;
			a_aFiniteStateMachine.registerStateTransition( m_aStateIdentifier, std::string("EventOne"), l_aStateTransitionFunc, std::string("FinalState") ); 

			l_aStateTransitionFunc = InitialState::onEventTwoToIntermediateState;
			a_aFiniteStateMachine.registerStateTransition( m_aStateIdentifier, std::string("EventTwo"), l_aStateTransitionFunc, std::string("IntermediateState")); 
		}

	private:

		static bool onEventOneToFinalState( StateFullObject & a_aStateFullObject ) {
			return true;
		} 
		
		static bool onEventTwoToIntermediateState( StateFullObject & a_aStateFullObject ) {
			return true;
		} 
}; 

class IntermediateState : public State {
	
	public:

		IntermediateState() {
			m_aStateIdentifier = std::string("IntermediateState"); 
		} 

		void init( FiniteStateMachine & a_aFiniteStateMachine ) {
			FiniteStateMachine::StateTransitionFunc l_aStateTransitionFunc = IntermediateState::onEventThreeToFinalState;
			a_aFiniteStateMachine.registerStateTransition( m_aStateIdentifier, std::string("EventThree"), l_aStateTransitionFunc, std::string("FinalState") ); 
		}

	private:

		static bool onEventThreeToFinalState( StateFullObject & a_aStateFullObject ) {
			return true;
		} 
}; 

class FinalState : public State {

	public:

		FinalState() {
			m_aStateIdentifier = std::string("FinalState"); 
		}

		void init( FiniteStateMachine & a_aFiniteStateMachine ) {
		}

	private:
}; 


class TestStateMachine : public FiniteStateMachine {
	
	public:
		TestStateMachine() {

			std::cout << "Initializing TestStateMachine" << std::endl;

			InitialState l_aInitialState;
			IntermediateState l_aIntermediateState;
			FinalState l_aFinalState;
			
			l_aInitialState.init( *this ); 
			l_aIntermediateState.init( *this ); 
			l_aFinalState.init( *this ); 
		} 

	private:
		typedef std::map< std::string, int> CntA;  
};


int main()
{
	std::cout << "Test Program" << std::endl;

	// Test Statefull Object Creation	
	StateFullObject l_aTestStateFullObject( std::string("InitialState")); 
	assert ( l_aTestStateFullObject.getCurrentState() == std::string("InitialState") ); 


	// Test State Machine Creation	
	TestStateMachine l_aTestStateMachine; 
	l_aTestStateMachine.print(); 


	// Testing Transition from Initial State to Final State
	l_aTestStateMachine.processEvent( std::string("EventOne"), l_aTestStateFullObject );
	assert ( l_aTestStateFullObject.getCurrentState() == std::string("FinalState") ); 

	
	// Testing Transition from Initial State to intermediate state to Final State
	StateFullObject l_aTestStateFullObject2("InitialState"); 

	l_aTestStateMachine.processEvent( std::string("EventTwo"), l_aTestStateFullObject2 );
	assert ( l_aTestStateFullObject2.getCurrentState() == std::string("IntermediateState") ); 
		
	l_aTestStateMachine.processEvent( std::string("EventThree"), l_aTestStateFullObject2 );
	assert ( l_aTestStateFullObject2.getCurrentState() == std::string("FinalState") ); 
	

	std::cout << "Finished Test Program" << std::endl;

	return 0;
}

