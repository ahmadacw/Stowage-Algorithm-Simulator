Simulator Project Flow:- 

The main function cleans up the input from the cmd and passes the correct travel, algorithm and output paths to a function called Simulator, Simulator cleans up the travel directories, rejecting any travel that is not valid,
Loads the algorithm so files from the provided directory, rejecting any so file that did not register an algorithm.
Now we have all valid travels and algorithms, Simulator loops through all algorithms and all travels, calculating the needed output, ship plan, ship route, cargo data directory, and passing them along with a shared pointer
To an Algorithm, to a function called travel in the Simulation header, tavel returns a pair of integers, the first number being the number of operations and the second one being the error code of the travel, the number of 
Operations is -1 if the algorithm travel pair is rejected.

Travel runs the simulation for a pair of (travel algorithm), it take as an input the ship plan, ship route, cargo data, output, path and a shared pointer to an algorithm, it calls the algorithm function in sequence, readShipPlan,
ReadShipRoute, from there the function loops through each port calling getInstructionForCargo with the relevant paths.
The travel function also keeps track of the errors that you are supposed to be present in the current travel at each step, if at any step the algortihm returns an error that it is not supposed to return, the algorithm travel pair 
Is rejected.
(details on how the errors are calculated and checked are in the subsequent explanation).

Algorithm Project Flow: -

Each algorithm has a member object of type Ship which is the object that all crane instructions is calculated on, and also an ErrorHandler object that keeps track of the current errors in the current travel.

For each of readShipPlan and readShipRoute, a function is called from the parser which returns an object that can be added to the ship to initialize it, once it is initialized  calcInstructionForCargo can be called,
If the calls happen in any other order the algorithm will return the previouse error reported.

CalcInstructions calls calcstowage, calcstowage then calls getInstructionsTounload and then callInstructionsToload, adding both instruction vectros together and reports back to getInstructionsforCargo, which then
Calls a function to write the instructions file, after which it returns the algorithm ErrorHandler back to the Simulator, it can do so because there is a cast from ErrorHandler to int, to make it compatible with all simulators.

Common Headers:-
Exception, holds a class called Error handler, holds a member object of type int called mask, can be converted to int to be returned by the algorithm.
Parser, hold all functions which read and write from files, for each file read by each function the format of the file is validated and all edge cases are calculated and a proper error code is returned.
Cargo, the Container object.
Port, holds two vectors of containers one for arrivals the other for departures, gets updated with cargo when calcstowage is called while the ship is at that specific port.
Ship, holds a 3D vector of Cargo acting as the ship plan, the route of the ship which is a vector of ports, cargo called empty, cargo called unavailable, the plan is initialized with either empty or unavailable.
Also holds set of Ids for the Ids that are currently on the ship.
Instruction, holds enough fields to represent any instruction, has a defined operator << for easy printing.

Error Handling:-

Simulator:-
the simulator constructs a ship much like my algorithm does, after constructing the ship it checks if the accumulated error codes are the same as the error codes returned from the algorithm, if not the algortihm travel pair
Is rejected, at that point we call getInstructionsForCargo, the simulator reads the instruction file and attempts to apply all instructions to the simulated ship, this is done through a function called evaluateInstructions,
this function calculates all containers that need to be unloaded loaded and rejected, applies all instructions read from the file to the simulated ship, if at any point during evaluation we reach an instruction that cannot be
applied the travel is rejected, if we have gone through all instructions and not all the proper loads, unloads and rejects are not preformed we reject the travel algorithm pair.

Algorithm:-
ship plan and ship route errors are handled by the functions that are called from the parser, all other errors are calculated using helper functions at each step of the algorithm.

Parser:-
while reading each file, each line is validated by passing it to a helper function which updates the ParserErrorhandler global object and returns an indication for which error was detected, the parser then returns the error
either by updating an ErrorHandler that was passed to it by refrence, or by inserting it into the fist sport in a vector that it is returning.

MultiThreading:- 
I created a wraper class that is called Simulator, each Simulator wraps a travel and an algorithm, the objects are created in the SimulatorBackEnd and are injected to the Pool through an enqueque function.
inside the threadPool, each thread pulls a Simulator object from a queue which holds them, initates the algorithm for the thread, it was done this way to avoid any probelsm with sharedPointers.
the thread then continues and calls a member function for the Simulator object called travel which runs the Simulation.
the intended return value from each Simulation object is stored an a member field of the simulator, and is then extracted by the Simulator backEnd.
