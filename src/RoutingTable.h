/* ADHOC OLSR PROJECT
 *
 * File : RoutingTable.h
 *
 * Created on: 27 janv. 2014
 *     Author: CHAHAL Karim & NGOMA Alban (ADHOC TEAM)
 *
 * Description : Header of RoutingTable class.
 */

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_
#include "Route.h"
#


class RoutingTable{

public:
	RoutingTable(); // Initializes the Application Routing Table.
	void addRoute(Route r); // add a new route in the Routing Table of the application
	void deleteRoute(Route r); // delete an existing route from the Routing Table of the application.
	void updaterouteList();
	void systemTableUpdate(std::list<Route> routeList);

	inline std::list<Route> getRouteList(){ // returns the private attribute routeList containing all the routes
			return this->routeList;
		}

private:
	std::list<Route> routeList;

};


#endif /* ROUTINGTABLE_H_ */
