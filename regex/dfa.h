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
	
	inline Edge() : State(0), Begin(false), End(false), Not(false), C(0)
	{
		
	}
	/*! Creates new edge
		\param[in] s end state
		\param[in] begin a begin state
		\param[in] end end state
		\param[in] nt a flag for not matching
		\param[in] c character
	 */
	inline Edge(int s, bool begin, bool end, bool nt, char c) :  State(s), Begin(begin), End(end), Not(nt), Final(false), C(c)
	{
		
	}
};


struct DFA
{
	std::vector< std::vector<Edge> > Edges;
	/*! Creates new DFA
	 */
	DFA();
	/*! Tries to execute automata on string, returning result
		\patam[in] s result
	 */ 
	regex::MatchResult match(const std::string& s);
};	
	
}
 