/** A main class for DFA an related data, needed for regex
 */
#pragma once
#include "matchresult.h"

namespace regex
{
	
struct Edge
{
	int State; //!< An index, which DFA should go to, after proceeding
	bool Begin; //!< Should we begin a new capture group here
	bool End;   //!< Should we end a new capture group here
	bool Not;   //!< Whether we should NOT go to end state in case of match
	bool Final; //!< Whether this node is final
	char C;     //!< A character, which should be tested
    bool Not2; //!< Should we use second char
    char C2;   //!< A second character
	
	inline Edge() : State(0), Begin(false), End(false), Not(false), Final(false), C(0), Not2(false), C2(0)
	{
		
	}
	/*! Creates new edge
		\param[in] s end state
		\param[in] begin a begin state
		\param[in] end end state
		\param[in] nt a flag for not matching
		\param[in] c character
	 */
	inline Edge(int s, bool begin, bool end, bool nt, char c) :  State(s), Begin(begin), End(end), Not(nt), Final(false), C(c), Not2(false), C2(0)
	{
		
	}
	/*! Makes edge trigger on character
		\param[in] c character
		\return self		
	 */
	inline Edge& match(char c)
	{
		Not = false;
		C = c;
		return *this;
	}
	
	/*! Makes edge trigger on not character
		\param[in] c character
		\return self		
	 */
	inline Edge& notMatch(char c)
	{
		Not = true;
		C = c;
		return *this;
	}
	/*! Forces edge to start capture group
		\return self
	 */
	inline Edge& startCaptureGroup()
	{
		Begin = true;
		return *this;
	}
    /*! Forces edge to end capture group
     */
    inline Edge& endCaptureGroup()
	{
	    End = true;
        return *this;
	}
};


struct DFA
{
	std::vector< std::vector<regex::Edge> > Edges;
	/*! Creates new DFA
	 */
	DFA();
	/*! Tries to execute automata on string, returning result
		\patam[in] s result
	 */ 
	regex::MatchResult match(const std::vector<char>& s);
	/*! Adds new state
		\return self-reference
	 */
	regex::DFA& newState();
	/*! Makes DFA shift to next state, adding new edge
		\return new edge
	 */
	regex::Edge& nextStateOn();
	/*! Makes DFA loop on this state, adding new edge
		\return whether loops
	 */
	regex::Edge& loopOn();
    /*! Compiles a substring for DFA
        \param[in] s string
        \return last string
     */
    regex::Edge& stringShouldContain(const std::string& s);
    /*! Adds final string as ending width c
        \param[in] c character
     */
    void stringMustEndWidth(char c);
    /*! Repeats character zero or infinity
        \param[in] c character
        \return exiting edge
     */
    regex::Edge& zeroOrInfinityNot(char c);
    /*! Repeats character zero or infinity
        \param[in] c repeated and exiting character
        \param[in] c2 a disabled character, which should not occur in string
        \return exiting edge
     */
    regex::Edge& zeroOrInfinityNot(char c, char c2);
};	
	
}
 