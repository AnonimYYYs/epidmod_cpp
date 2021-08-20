# epidmod_cpp
Epidemic spreading model. Work with input and output steams. Cpp implementation for [ws-dss.com](ws-dss.com)

Every person goes through "healthy"->"exposed"->"infectious"->"removed" states way.

__Healthy__ is brand new person tht have no immune nor infection. Can be infected

__Exposed__ is person who just caught virus, but it is not in active form. Still can not infect others, neither cant be infected.

__Infectious__ is person who passed "exposed" state and now can spread virus with contacts. Can infect others.

__Removed__ is person who passed "infectious" state. He have no more virus inside so cant infect others, also he have immunitet or is passed away, so he cannot be infected anymore.

## input describing
Example of input json file can be found [here](../ws_dss/input.json)

Describing for each variable:
*	__PEOPLE_AMOUNT__ is for amount of people that exist in your model. Integer, (0, N], N depends on your memory size.
*	__CONNECTIONS_AMOUNT__ is for initial connections of watts-strogatz algorithm for "constant" graph. Integer, [0, N], N depends on your memory size.
*	__RAND_CONNECTIONS_AMOUNT__ is for random connections that every person have every day. Integer, [0, N], N depends on your memory size.
*	__EXPOSED_PART__ is for initial part of exposed persons. Float, [0,1].
*	__INFECT_PART__ is for initial size of infected persons. Float, [0,1].
*	__WSG_K__ is for chance of edge replacing during watts-strogatz algorithm. Float, [0,1].
*	__TIME__ is for amount of days that should be modelled. Integer, (0, T], T is probably limitted by `sizeof(unsigned int)`, but also depends on memory during output, stream can ran out of memory if too big __TIME__ with too low __PRINT_TICK__ 
*	__CATCH_INFECT__ is a chance to catch infection from one "infectious" person. Float, [0, 1].
*	__WEIBULL_EXPOSED_TO_INFECT_A__ and __WEIBULL_EXPOSED_TO_INFECT_B__ are A and B for weibull distribution, that decide how many days person stays in "exposed" state. Float, (0, inf).
*	__WEIBULL_INFECT_TO_REMOVED_A__ and __WEIBULL_INFECT_TO_REMOVED_B__ are A and B for weibull distribution, that decide how many days person stays in "infectious" state. Float, (0, inf).
*	__PRINT_TICK__ is how often outputted info about states in model. Integer, [0, inf). If 0 - then output only initial and final states data.

## output describing
Example of input json file can be found [here](../ws_dss/output.json)

Describing for each variable:
* __duration__ is time of how long program was working, in seconds. Integer.
* __results__ is array of all data, counted every *__PRINT_TICK__* and during first and last ones. Json array.
  * __healthy__, __exposed__, __infectious__, __removed__ are for amount of corresponding groups inside model by the moment of time. Integer.
  * __time__ is moment of time when this data was taken from model. 0 is for initial data without any calculations. 1 is after first calculations and updating states, e.t.c. Integer.
