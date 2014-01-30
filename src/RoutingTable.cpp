/* ADHOC OLSR PROJECT
 *
 * File : RoutingTable.cpp
 *
 * Created on: 27 janv. 2014
 *     Author: CHAHAL Karim & NGOMA Alban (ADHOC TEAM)
 *
 * Description : This file is concerning the application (and system) routing table management,
 * 				 with all his methods.
 */


#include "Route.h"



	std::list<Route> routeList; // a list containing all the destinations and information about them.


	RoutingTable::RoutingTable(){

		this->routeList = new std::list(); // initializes an empty list to be completed.

	}// end of constructor

	// METHODS

	 void RoutingTable::addRoute(Route r){ // add a new route in the Routing Table of the application.

		 // SYSCALL NETLINK SOCKET, + writing in kernel routing table
		 std::list<Route>::iterator routeIt =routeList.begin();

		 for(routeIt = Node.begin(); routeIt!= Node.end(); ++routeIt)
		     {
			 	 routeList.push_back(r);
		     }

	}

	void RoutingTable::deleteRoute(Route r){ // delete an existing route from the Routing Table of the application.




	}


	void RoutingTable::updateRouteList(){



	} // applies changes after calling systemTableUpdate routine.


	void RoutingTable::systemTableUpdate(std::list<Route> routeList){ // updating the system Routing Table after each timer delay.



	}


	std::list<Route> getRouteList(){ // returns the private attribute routeList containing all the routes
				return routeList;
			}





