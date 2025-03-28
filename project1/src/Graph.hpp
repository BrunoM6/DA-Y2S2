#ifndef PROJDA1_GRAPH_HPP
#define PROJDA1_GRAPH_HPP

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <set>


#define INF std::numeric_limits<double>::max()

class Pipe;


/// @brief Class for Locations of a water supply network (Graph class).
/// @details Each Location object represents a vertex in the Graph. These vertexes could be either a Reservoir, a PumpingStation, or a City - which are its subclasses.
/// @see Reservoir
/// @see PumpingStation
/// @see City
class Location {
protected:
    /// @brief Code to find the associated Location object.
    std::string code;
    /// @brief Unique identifier of each Location object.
    /// @details Two objects of the same subclass can't have the same ID, but two objects from different subclasses can.
    int id;

    /// @brief All the outgoing edges (Pipe objects) of the Vertex object.
    std::vector<Pipe*> adj;
    /// @brief All the incoming edges (Pipe objects) of the Vertex object.
    std::vector<Pipe*> incoming;

    /// @brief Auxiliary field used in DFS and BFS algorithms.
    /// @details Used to, during a DFS or BFS traversal, know if a vertex has been iterated through (visited) already.
    bool visited = false;
    /// @brief Auxiliary field used in other Graph algorithms.
    /// @details Used to know if a vertex is current being processed during a traversal. (used by the isDAG algorithm)
    bool processing = false;
    /// @brief Auxiliary field used in other Graph algorithms.
    /// @details Represents the order in which vertex are visited during a traversal. (used by the topsort algorithm)
    unsigned int indegree;

    /// @brief Auxiliary field used to store the edge (Pipe object) we took to reach this vertex (Location object) during a graph traversal.
    /// @details Used to backtrack the augmenting paths found using BFS, during the edmondsKarp algorithm.
    Pipe* path = nullptr;


    /**
     * @brief Removes an edge (Pipe object) passed as an argument.
     * @details Iterates through the incoming member variable of the vertex that called this function, looking for the edge (Pipe object) to remove; Also assures the deletion of its pointer.
     * @param edge Pointer to the edge (Pipe object) to be removed from the incoming member variable.
     * @note Time complexity: O(n), where n is the size of the incoming member variable.
     */
    void deleteEdge(Pipe *edge);

public:
    /**
     * @brief Parametrized constructor of the Location class.
     * @details Initializes the member variables with their respective parameters. The adj and incoming member variables are initialized using the Location::addEdge() function. The other member variables are only used for specific algorithms, so they are initialized as needed.
     * @param id Integer value corresponding to the id member variable
     * @param code Const reference to a string corresponding to the code member variable
     * @note Time complexity: O(1).
     */
    Location(int id, const std::string &code);
    /**
     * @brief Virtual destructor for the Location class.
     * @details When an object of a derived class is deleted through a pointer to the Location class, this virtual destructor ensures that the proper derived class destructor is called.
     * @note The destructor is marked as virtual to enable dynamic polymorphism.
     */
    virtual ~Location() = default;


    /**
     * @brief Getter for the code of the location the Location object refers to.
     * @returns String with the location code.
     */
    std::string getCode() const;
    /**
     * @brief Getter for the unique identifier of the location the Location object refers to.
     * @returns String with the id.
     */
    int getId() const;
    /**
     * @brief Getter for the vector of pointers to outgoing edges (Pipe objects).
     * @returns Vector of pointers to Pipe objects.
     */
    std::vector<Pipe*> getAdj() const;
    /**
     * @brief Getter for the vector of pointers to incoming edges (Pipe objects).
     * @returns Vector of pointers to the Pipe objects.
     */
    std::vector<Pipe*> getIncoming() const;
    /**
    * @brief Getter for the visited state of the Location object.
    * @returns Boolean with the visited state.
    */
    bool isVisited() const;
    /**
     * @brief Getter for the processing state of the Location object.
     * @returns Boolean with the processing state.
     */
    bool isProcessing() const;
    /**
     * @brief Getter for the number of incoming edges (Pipe objects) of the Location object.
     * @returns Integer with the indegree amount.
     */
    unsigned int getIndegree() const;
    /**
     * @brief Getter for the pointer to the Pipe object that was used to reach the Location object during a graph traversal.
     * @returns Pointer to the Pipe object.
     */
    Pipe* getPath() const;

    /**
     * @brief Sets the value of the code member variable.
     * @param code String containing the new location code.
     */
    void setInfo(std::string code);
    /**
     * @brief Sets the value of the visited member variable.
     * @param visited Boolean value containing the new visited state.
     */
    void setVisited(bool visited);
    /**
     * @brief Sets the value of the processing member variable.
     * @param processing Boolean value containing the new processing state.
     */
    void setProcessing(bool processing);
    /**
     * @brief Sets the value of the indegree member variable.
     * @param indegree Boolean value containing the new number of incoming edges.
     */
    void setIndegree(unsigned int indegree);
    /**
     * @brief Sets the value of the indegree member variable.
     * @param path Pointer to the Pipe object that corresponds to the new edge used to reach the Location object.
     */
    void setPath(Pipe *path);


    /**
     * @brief Adds an outgoing edge (Pipe object) to the adj member variable.
     * @details Also updates the indegree and incoming member variables.
     * @param dest Pointer to the Location object the edge (Pipe object) connects to.
     * @param w Weight associated with the edge (Pipe object).
     * @return Pointer to the created Pipe object.
     * @note Time complexity: O(1)
     */
    Pipe* addEdge(Location *dest, double w);
    /**
     * @brief Auxiliary function to remove an outgoing edge - that goes from the vertex that calls this function to a vertex with content in.
     * @param in Content of the edge's destination vertex.
     * @return true if successful; <br>
     *         false if such edge does not exist;
     * @note Time complexity: O(n+p), where n is the size of the adj member variable and p is the complexity of deleteEdge().
     * @see deleteEdge()
     */
    bool removeEdge(const std::string &in);
    /**
     * @brief Auxiliary function to remove all outgoing edges (Pipe object) from the adj member variable.
     * @details Calls deleteEdge() function for each edges (Pipe object) in adj to also remove it from the incoming member variable.
     * @details Iterates through the adj member variable, clearing it.
     * @note Time complexity: O(n*p), where n is the size of the adj member variable and p is the complexity of deleteEdge().
     * @see deleteEdge()
     */
    void removeOutgoingEdges();
};

/// @brief Type of Location in a water supply network (Graph class).
/// @details Each (Water) Reservoir object is represented as a vertex in the graph. These vertexes provide water to the entire network and have a maximum delivery capacity of X m3/sec - they are the "start nodes" of this project's graph (they don't have any incoming edges).
/// @see Location
class Reservoir : public Location {
private:
    /// @brief The reservoir's full name.
    std::string name;
    /// @brief The municipality where the reservoir is located.
    std::string municipality;
    /// @brief Maximum amount of water that can be delivered by each reservoir (in m^3/sec).
    double maximumDelivery;

public:
    /**
     * @brief Parametrized constructor of the Reservoir sub-class.
     * @details Initializes the member variables with their respective parameters.
     * @param name, municipality, id, code, maximumDelivery Parameters containing all the information to initialize the corresponding member variables.
     * @note Time complexity: O(1).
     */
    Reservoir(const std::string &name, const std::string &municipality, int id, const std::string& code, double maximumDelivery);

    /**
     * @brief Getter for the name of the reservoir the Location object refers to.
     * @returns String with the name of the water reservoir.
     */
    const std::string& getName() const;
    /**
     * @brief Getter for the reservoir's municipality.
     * @returns String with the name of the municipality where the water reservoir is in.
     */
    const std::string& getMunicipality() const;
    /**
     * @brief Getter for the reservoir's maximum delivery capacity (in m^3/sec).
     * @returns Floating-point value with maximum amount of water that can be drawn from a reservoir.
     */
    double getMaximumDelivery() const;
};

/// @brief Type of Location in a water supply network (Graph class).
/// @details Each PumpingStation object is represented as a vertex in the graph. These vertexes direct or redirect flow from other PumpingStations or water Reservoirs to the final delivery sites.
/// @see Location
class PumpingStation : public Location {
public:
    /**
     * @brief Parametrized constructor of the PumpingStation sub-class.
     * @details Initializes the member variables with their respective parameters.
     * @param id, code Parameters containing all the information to initialize the corresponding member variables.
     * @note Time complexity: O(1).
     */
    PumpingStation(int id, const std::string &code);
};

/**
 * @brief Struct to store information about City objects
 * @details Used as a return value for some of this tool's main functions, to store only the necessary information about City objects to be printed by the menus.
 * @see City
 */
struct CityInfo {
    /// @brief The city's name (name member variable)
    std::string cityName;
    /// @brief The city's code (Location's code member variable)
    std::string cityCode;
    /// @brief The amount of water needed by the city (demand member variable)
    double demand;
    /// @brief The maximum amount of water that is delivered to the city, given the initial/unchanged Graph layout (totalFlow member variable)
    double totalFlow;
    /**
     * @brief The maximum amount of water that is delivered to the city, given a changed version of the Graph
     * @details Not always initialized (only when removing parts of the Graph - Reservoirs/PumpingStations/Pipes)
     * @see Graph::removeReservoirCompare()
     * @see Graph::removeStationCompare()
     * @see Graph::removePipeCompare()
     */
    double affectedTotalFlow;
};

/// @brief Type of Location in a water supply network (Graph class).
/// @details Each City object is represented as a vertex in the graph. These vertexes are the delivery sites - they directly connect the water supply network to the final consumer - they are the "end nodes" of this project's graph (they don't have any outgoing edges).
/// @see Location
class City : public Location {
private:
    /// @brief The city's full name.
    std::string name;
    /// @brief Amount of water needed to be delivered to each delivery site (city).
    double demand;
    /// @brief Total population of the delivery site.
    int population;
    /**
     * @brief Maximum amount of water that is able to be delivered to each city, considering the current Graph layout.
     * @details NOT to be confused with the Graph's MaxFlow (sum of totalFlow of all cities). <br>
     *          To be initialized upon executing edmondsKarp().
     */
    double totalFlow;

public:
    /**
     * @brief Parametrized constructor of the City sub-class.
     * @details Initializes the member variables with their respective parameters.
     * @param name, id, code, demand, population Parameters containing all the information to initialize the corresponding member variables.
     * @note Time complexity: O(1).
     */
    City(const std::string &name, int id, const std::string &code, double demand, int population);

    /**
     * @brief Getter for the name of the city.
     * @returns String with the name of the delivery site (city).
     */
    std::string getName() const;
    /**
     * @brief Getter for the city's need of delivery (in m^3/sec).
     * @returns Floating-point value with amount of water that a city needs to be supplied with.
     */
    double getDemand() const;
    /**
     * @brief Getter for the city's maximum flow.
     * @returns Floating-point value with maximum amount of water that can be delivered to a city, considering the current Graph layout.
     */
    double getTotalFlow() const;

    /**
     * @brief Setter for the city's maximum flow.
     * @param totalFlow Floating-point value with new maximum flow.
     */
    void setTotalFlow(double totalFlow);
};



/// @brief Class for edges of a graph (Graph class).
/// @details Each Pipe object represents a pipeline through the combination of: its source Location (code in Location object), its destination Location (dest member variable).
/// @see Location
class Pipe {
protected:
    /// @brief Pointer to the destination Location object (Reservoir, PumpingStation or City) of a Pipe object.
    Location* dest;
    /// @brief Edge weight. In this case, it represents the maximum water capacity of a Pipe object.
    double weight;

    /// @brief Auxiliary field used when working with bidirectional edges.
    /// @details Stores a pointer to source Location of a Pipe object - Used for backtracking augmenting paths in edmondsKarp(), for example.
    Location* orig;
    /// @brief Auxiliary field used when working with bidirectional edges.
    /// @details Stores a pointer the corresponding edge (Pipe object) that connects the same pair of vertexes, but in the opposite direction.
    Pipe* reverse = nullptr;

    /// @brief Current flow in the Pipe object.
    /// @details For flow-related problems - used together with the weight member variable, where they represent the current flow and the maximum amount of flow, respectively.
    double flow;
    /// @brief Current state of the Pipe object.
    /// @details For flow-related problems - indicates whether the Pipe object is activated or disabled - disabling it effectively removes the edge.
    bool activated;

public:
    /**
     * @brief Parametrized constructor of the Pipe class.
     * @details Initializes the member variables with their respective parameters. The other member variables are only used for specific algorithms, so they are initialized as needed.
     * @param orig Pointer to the source Location object.
     * @param dest Pointer to the destination Location object.
     * @param w Floating-point value that corresponds to the weight of the edge.
     * @note Time complexity: O(1).
     */
    Pipe(Location *orig, Location *dest, double w);

    /**
     * @brief Getter for the dest member variable.
     * @returns Pointer to the destination Location object.
     */
    Location* getDest() const;
    /**
     * @brief Getter for the edge's weight.
     * @returns Floating-point value with the edge weight.
     */
    double getWeight() const;
    /**
     * @brief Getter for the orig member variable.
     * @returns Pointer to the source Location object.
     */
    Location* getOrig() const;
    /**
     * @brief Getter for the reverse member variable.
     * @returns Pointer to the Pipe object (edge) that goes in the opposite direction between the same two vertexes.
     */
    Pipe* getReverse() const;
    /**
     * @brief Getter for the flow member variable.
     * @returns Floating-point value with the edge's current flow.
     */
    double getFlow() const;
    /**
     * @brief Getter for the activated member variable.
     * @returns Boolean value that indicates the state of the edge.
     */
    bool isActivated() const;


    /**
     * @brief Sets the value of the reverse member variable.
     * @param reverse Pointer to the new opposite Pipe object.
     */
    void setReverse(Pipe *reverse);
    /**
     * @brief Sets the value of the flow member variable.
     * @param flow Floating-point value that corresponds to the new flow value going through the Pipe object.
     */
    void setFlow(double flow);
    /**
     * @brief Sets the value of the activated member variable.
     * @param activated Boolean value that that indicates the new state of the Pipe object.
     */
    void setActivated(bool activated);
};



/// @brief Class to represent a graph.
/// @see Location
/// @see Pipe
class Graph {
private:
    /// @brief Member variable that stores all the vertices of a Graph object.
    std::vector<Location*> vertexSet;
    /// @brief Hash table data structure that stores all Reservoir objects associated with their code for faster access.
    std::unordered_map<std::string, Reservoir*> mapReservoirs;
    /// @brief Hash table data structure that stores all PumpingStation objects associated with their code for faster access.
    std::unordered_map<std::string, PumpingStation*> mapStations;
    /// @brief Hash table data structure that stores all City objects associated with their code for faster access.
    std::unordered_map<std::string, City*> mapCities;
    /// @brief Hash table data structure that stores all Pipe objects associated with the concatenation of the source and destination vertex's codes for faster access.
    std::unordered_map<std::string, Pipe*> mapPipes;


    /**
     * @brief Finds the index of the vertex with a given content.
     * @param in Code (content) of the vertex.
     * @note Time complexity: O(n), where n is the number of vertices in the vertexSet member variable.
     */
    int findVertexIdx(const std::string &in) const;


    /**
     * @brief Auxiliary function to add a Super-Source (Reservoir object which has code R_0) vertex and a Super-Sink (City object which has code C_0) vertex, and their respective edges to a Graph.
     * @details In a flow-related problem: <br>
     *          Super-Source connects itself through an edge - Pipe object - to every "starting node" which are the Reservoir objects; <br>
     *          Every "end node" - City objects - connects itself through an edge to a Super-Sink; <br>
     *          The weight of the edges from the Super-Source to each Reservoir object is the maximum delivery capacity of each Reservoir; <br>
     *          The weight of the edges from each City object to the Super-Sink is the demand of each City;
     *          These special vertexes are created by function to be used by the edmondsKarp() algorithm;
     * @param toRemove Code (content) of the Reservoir object to be "removed" (flow from Super-Source to Reservoir set to 0).
     * @note Time complexity: O((n+p)*q), where: <br>
     *       - n is the number of Reservoir objects in the Graph; <br>
     *       - p is the number of City objects in the Graph; <br>
     *       - q is the complexity of the addEdge() function;
     * @see edmondsKarp()
     * @see getCitiesFlow()
     * @see verifyCityNeeds()
     */
    void addR0C0(const std::string &toRemove = "");


    /**
     * @brief Auxiliary function implementing the Edmonds-Karp (EK) algorithm - for getCitiesFlow().
     * @details It finds the MaxFlow in a flow network represented by a Graph. It iteratively finds augmenting paths from the source to the target vertex, and augments the flow along these paths until no more augmenting paths exist.
     * @param source Code (content) of the source Location object (vertex)
     * @param target Code (content) of the target Location object (vertex)
     * @note Time complexity: O(V*E^2), where V is the number of vertices in the Graph and E is the number of edges.
     * @see findAugmentingPath()
     * @see findMinResidualAlongPath()
     * @see augmentFlowAlongPath()
     */
    void edmondsKarp(const std::string &source, const std::string &target);
    /**
     * @brief Auxiliary function to find an augmenting path using Breadth-First Search (BFS) - for edmondsKarp().
     * @details During the BFS traversal, for each outgoing and incoming edge, calls testAndVisit() to test their validity
     * @param s Source Location object (vertex).
     * @param t Target Location object (vertex).
     * @return true if an augmenting path to the target is found, false otherwise.
     * @note Time complexity: O(V+E), where V is the number of vertices in the Graph and E is the number of edges in the Graph.
     * @see testAndVisit()
     */
    bool findAugmentingPath(Location *s, Location *t);
    /**
     * @brief Auxiliary function to test the given vertex 'w' and visit it if conditions are met - for findAugmentingPath().
     * @details If the vertex wasn't visited yet, the edge has residual capacity and it is activated, then that vertex can be part of the current augmenting path.
     * @param q Reference to BFS queue (from findAugmentingPath() function), so that w can be enqueued, if valid.
     * @param e Pointer to the Pipe object being traversed, to check its residual capacity and its state (activated or deactivated).
     * @param w Pointer to the neighboring Location object (vertex) during the BFS, to check if it's already been traversed through.
     * @param residual Residual capacity of e: <br>
     *        - For outgoing edges, it is the available capacity for additional flow (so it is the difference between the edge's maximum capacity and current flow); <br>
     *        - For incoming edges, it is the flow that is able to be decreased (so it is the edge's flow);
     * @note Time complexity: O(1).
     * @see findAugmentingPath()
     */
    void testAndVisit(std::queue<Location*> &q, Pipe *e, Location *w, double residual);
    /**
     * @brief Auxiliary function to find the bottleneck - minimum residual capacity along the augmenting path.
     * @details Traverses the augmenting path to find the minimum residual capacity.
     * @param s Source Location object (vertex).
     * @param t Target Location object (vertex).
     * @return Floating-point value representing the bottleneck of that augmenting path.
     * @note Time complexity: O(n), where n is the number of edges in the augmenting path (at most E-1), where E is the number of edges in the Graph.
     */
    double findMinResidualAlongPath(Location *s, Location *t);
    /**
     * @brief Auxiliary function to augment flow along the augmenting path with the given flow value.
     * @details Traverses the augmenting path and updates the flow values accordingly, increasing the flow along forward edges (from the direction of s to t) and decreasing the flow along residual edges (from the direction of t to s).
     * @param s Source Location object (vertex).
     * @param t Target Location object (vertex).
     * @param f Floating-point value representing the minimum residual capacity (bottleneck) in the augmenting path being analyzed.
     * @note Time complexity: O(n), where n is the number of edges in the augmenting path (at most E), where E is the number of edges in the Graph.
     */
    void augmentFlowAlongPath(Location *s, Location *t, double f);

public:
    /**
     * @brief Default constructor of the Graph class.
     * @details Initializes the member variables with empty placeholders.
     * @note Time complexity: O(1).
     */
    Graph();
    /**
     * @brief Copy constructor of the Graph class.
     * @details Initializes the member variables with another Graph object's member variables.
     * @note Time complexity: O(1).
     */
    Graph(const Graph &g);


    /**
     * @brief Reservoirs-file reader function
     * @details Initializes part of the vertexSet member variable and the respective unordered_map with the pointers to the Reservoir objects, by reading them from a file.
     * @param file String containing the path to the .csv file.
     * @return true if the file was parsed correctly; <br>
     *         false, if the file wasn't opened correctly;
     * @note Time complexity: O(n*p), where n is the number lines in the flights.csv file and p is the complexity of unordered_map::find() method, that depends on the number of collisions, which a good hash function should minimize.
     * @see addVertexReservoir()
     */
    bool read_file_reservoir(const std::string &file);
    /**
     * @brief Stations-file reader function
     * @details Initializes part of the vertexSet member variable and the respective unordered_map with the pointers to the PumpingStation objects, by reading them from a file.
     * @param file String containing the path to the .csv file.
     * @return true if the file was parsed correctly; <br>
     *         false, if the file wasn't opened correctly;
     * @note Time complexity: O(n*p), where n is the number lines in the flights.csv file and p is the complexity of unordered_map::find() method.
     * @see addVertexStation()
     */
    bool read_file_stations(const std::string &file);
    /**
     * @brief Cities-file reader function
     * @details Initializes part of the vertexSet member variable and the respective unordered_map with the pointers to the City objects, by reading them from a file.
     * @param file String containing the path to the .csv file.
     * @return true if the file was parsed correctly; <br>
     *         false, if the file wasn't opened correctly;
     * @note Time complexity: O(n*p), where n is the number lines in the flights.csv file and p is the complexity of unordered_map::find() method.
     * @see addVertexStation()
     */
    bool read_file_cities(const std::string &file);
    /**
     * @brief Pipes-file reader function
     * @details Initializes the respective map, and the adj member variable of each Location object in the vertexSet with pointers to Pipe objects, by reading them from a file.
     * @param file String containing the path to the .csv file.
     * @return true if the file was parsed correctly; <br>
     *         false, if the file wasn't opened correctly;
     * @note Time complexity: O(n*p), where: <br>
     *       - n is the number lines in the flights.csv file; <br>
     *       - p is the complexity of either the addEdge() or the addBidirectionalEdge() functions, depending on if the edge is unidirectional or bidirectional;
     * @see addEdge()
     * @see addBidirectionalEdge()
     */
    bool read_file_pipes(const std::string &file);


    /**
     * @brief Getter for the total number of vertices in the vertexSet member variable.
     * @returns Integer with the number of vertices in the Graph.
     */
    int getNumVertex() const;
    /**
     * @brief Getter for the vertexSet member variable.
     * @returns Vector with all the vertices in the graph.
     */
    std::vector<Location*> getVertexSet() const;

    /**
     * @brief Read-only overloaded getter for the mapReservoirs member variable.
     * @returns Copy of the mapReservoirs data structure.
     */
    std::unordered_map<std::string, Reservoir*> getMapReservoirs() const;
    /**
     * @brief Read-write overloaded getter for the mapReservoirs member variable.
     * @returns Modifiable reference to the mapReservoirs data structure.
     */
    std::unordered_map<std::string, Reservoir*>& getMapReservoirs();

    /**
     * @brief Read-only overloaded getter for the mapStations member variable.
     * @returns Copy of the mapStations data structure.
     */
    std::unordered_map<std::string, PumpingStation*> getMapStations() const;
    /**
     * @brief Read-write overloaded getter for the mapStations member variable.
     * @returns Modifiable reference to the mapStations data structure.
     */
    std::unordered_map<std::string, PumpingStation*>& getMapStations();

    /**
     * @brief Read-only overloaded getter for the mapCities member variable.
     * @returns Copy of the mapCities data structure.
     */
    std::unordered_map<std::string, City*> getMapCities() const;
    /**
     * @brief Read-write overloaded getter for the mapCities member variable.
     * @returns Modifiable reference to the mapCities data structure.
     */
    std::unordered_map<std::string, City*>& getMapCities();

    /**
     * @brief Read-only overloaded getter for the mapPipes member variable.
     * @returns Copy of the mapPipes data structure.
     */
    std::unordered_map<std::string, Pipe*> getMapPipes() const;
    /**
     * @brief Read-write overloaded getter for the mapPipes member variable.
     * @returns Modifiable reference to the mapPipes data structure.
     */
    std::unordered_map<std::string, Pipe*>& getMapPipes();


    /**
     * @brief Auxiliary function to find a vertex with a given content.
     * @param in Code (content) of the vertex.
     * @return Pointer to the vertex if found, and nullptr otherwise.
     * @note Time complexity: O(n), where n is the number of vertices in the vertexSet member variable.
     */
    Location* findVertex(const std::string &in) const;


    /**
     * @brief Adds a vertex (of subclass Reservoir) with a given content to the vertexSet and mapReservoirs member variables.
     * @param name, municipality, id, code, maximumDelivery All the information a Reservoir object should have.
     * @return true if successful; <br>
     *         false if a vertex with that content already exists;
     * @note Time complexity: O(1).
     * @see findVertex()
     */
    bool addVertexReservoir(const std::string &name, const std::string &municipality, int id, const std::string &code, double maximumDelivery);
    /**
     * @brief Adds a vertex (of subclass Station) with a given content to the vertexSet and mapStation member variables.
     * @param id, code All the information a PumpingStation object should have.
     * @return true if successful; <br>
     *         false if a vertex with that content already exists;
     * @note Time complexity: O(1).
     * @see findVertex()
     */
    bool addVertexStation(int id, const std::string& code);
    /**
     * @brief Adds a vertex (of subclass City) with a given content to the vertexSet and mapCities member variables.
     * @param name, id, code, demand, population All the information a City object should have.
     * @return true if successful; <br>
     *         false if a vertex with that content already exists;
     * @note Time complexity: O(1).
     * @see findVertex()
     */
    bool addVertexCity(const std::string &name, int id, const std::string &code, double demand, int population);


    /**
     * @brief Removes a vertex with a given content from the graph, and all outgoing and incoming edges.
     * @details Once it finds the vertex with content in, it calls removeOutgoingEdges() to remove all outgoing Pipe objects, and removeEdge() for each vertex in the vertexSet to remove all incoming Pipe objects to that vertex.
     * @param in Content of the vertex to be removed.
     * @return true if successful; <br>
     *         false if such vertex does not exist;
     * @note Time complexity: O(n + (p + n*q)), where: <br>
     *       - n is the size of the vertexSet member variable; <br>
     *       - p is the complexity of the removeOutgoingEdges() function if the vertex is found, otherwise the function isn't called; <br>
     *       - q is the complexity of the removeEdge() function if the vertex is found, otherwise the function isn't called;
     * @see Location::removeOutgoingEdges()
     * @see removeEdge()
     */
    bool removeVertex(const std::string &in);

    /**
     * @details Adds an edge to the graph, given the contents of the source and destination vertices, and the edge weight.
     * @param source Code of the source Location.
     * @param dest Code of the destination Location.
     * @param w Max capacity of the new Pipe object.
     * @return true if successful; <br>
     *         false if the source or destination vertexes don't exist;
     * @note Time complexity: same as findVertex() -> O(n), where n is the number of vertices in the vertexSet member variable.
     * @see findVertex()
     * @see Location::addEdge()
     */
    bool addEdge(const std::string &source, const std::string &dest, double w);
    /**
     * @brief Removes an edge from the graph, identified by the source and destination contents.
     * @param source Source vertex's contents.
     * @param dest Destination vertex's contents.
     * @return true if successful;
     *         false if such edge does not exist;
     * @note Time complexity: O(n+p), where n is the complexity of findVertex() and p is the complexity of Location::removeEdge().
     * @see findVertex()
     * @see Location::removeEdge()
     */
    bool removeEdge(const std::string &source, const std::string &dest);
    /**
     * @details Adds a bidirectional edge to the Graph (two Pipe objects, one in each direction), given the contents of the source and destination vertices and the edge weight.
     * @param source Code of the source Location.
     * @param dest Code of the destination Location.
     * @param w Max capacity of the new Pipe object.
     * @return true if successful; <br>
     *         false if the source or destination vertexes don't exist;
     * @note Time complexity: same as findVertex() -> O(n), where n is the number of vertices in the vertexSet member variable.
     * @see findVertex()
     * @see Location::addEdge()
     * @see Pipe::setReverse()
     */
    bool addBidirectionalEdge(const std::string &source, const std::string &dest, double w);



    /**
     * @brief Function that implements the 1st part of exercise [T2.1] - Assignment of each city with its corresponding maximum flow value.
     * @details Creates a copy of the original graph, to add a Super-Source and Super-Sink vertex temporarily (addR0C0()). Runs edmondsKarp to find the MaxFlow of the graph. Prepares the output according to the input parameters.
     * @param allCities true - function should return the maximum flow values that reaches each city. <br>
     *                  false - function should return the maximum flow value that reaches city whose code is oneCity.
     * @param oneCity Empty string if true is passed as the allCities argument. <br>
     *                Code of the city we wish to know the maximum flow of, if false is passed as the allCities argument.
     * @param removeR Code (content) of the Reservoir object to be "removed" (flow from Super-Source to Reservoir set to 0) by the addR0C0() function.
     * @return Vector of CityInfo structs that contain all information about each city.
     * @note Time complexity: O(n + p + q*(V+E)), where: <br>
     *       - n is the complexity of addR0C0(); <br>
     *       - p is the complexity of edmondsKarp(); <br>
     *       - q is the number of City objects (in the mapCities member variable); <br>
     *       - V is the number of vertices in the Graph and E is the number of edges in the Graph;
     * @see CityInfo
     * @see addR0C0()
     * @see edmondsKarp()
     * @see writeFileMaxFlow() for outputting the Max-Flow to a file.
     */
    std::vector<CityInfo> getCitiesFlow(bool allCities, const std::string &oneCity = "", const std::string &removeR = "");
    /**
     * @brief Function that implements the 2nd part of exercise [T2.1] - File output
     * @details Writes to a file (edits or creates it) the maximum flow that reaches each city - received from calling getCitiesFlow().
     * @return true if the information was outputted correctly to the file; <br>
     *         false if the file wasn't opened correctly;
     * @note Time complexity: O(n+p), where n is the complexity of getCitiesFlow() and p is the number of pairs returned by it.
     * @see getCitiesFlow()
     */
    bool writeFileMaxFlow();

    /**
     * @brief Function that implements the features of exercise [T2.2].
     * @details Given the current Graph's layout, checks if the total flow through each City is lower than its demand.
     * @return Vector of CityInfo structs that contain all information about the cities whose totalFlow doesn't meet the demand.
     * @note Time complexity: O(n+p), where n is the complexity of getCitiesFlow() and p is the number of City objects in the Graph.
     * @see CityInfo
     * @see getCitiesFlow()
     */
    std::vector<CityInfo> verifyCityNeeds();

    /**
     * @brief Function that implements the 1st feature of exercise [T3.1].
     * @details Given a Reservoir object code, calls getCitiesFlow() on a Graph copy with that Reservoir "removed" (flow from Super-Source set to 0) and stores the new totalFlow of each city.
     * @param code Code (content) of the Reservoir object.
     * @return Vector with CityInfo structs for each city affected by the removal of the Reservoir.
     * @note Time complexity: O(n+p), where n is the complexity of getCitiesFlow() and p is the number of City objects in the Graph.
     * @see CityInfo
     * @see getCitiesFlow()
     */
    std::vector<CityInfo> removeReservoirCompare(const std::string &code);

    /**
     * @brief Function that implements the feature of exercise [T3.2].
     * @details For each PumpingStation in the Graph, calls the removeStationCompare() function to obtain the cities affected by its removal.
     * @return Unordered Map that associates the code of each PumpingStation removed with a vector with CityInfo structs for each city affected by said removal.
     * @note Time complexity: O(n*p), where n is the number of PumpingStation objects in the Graph and p is the complexity of removeStationCompare().
     * @see CityInfo
     * @see removeStationCompare()
     */
    std::unordered_map<std::string, std::vector<CityInfo>> removeStationsCompare();
    /**
     * @brief Auxiliary function for the implementation of the 1st feature of exercise [T3.2].
     * @details Given a PumpingStation object code, calls getCitiesFlow() on a Graph copy with that PumpingStation removed and stores the new totalFlow of each city.
     * @param code Code (content) of the PumpingStation object.
     * @return Vector with CityInfo structs for each city affected by the removal of the PumpingStation.
     * @note Time complexity: O(n+p), where n is the complexity of getCitiesFlow() and p is the number of City objects in the Graph.
     * @see CityInfo
     * @see getCitiesFlow()
     */
    std::vector<CityInfo> removeStationCompare(const std::string &code);

    /**
     * @brief Function that implements the feature of exercise [T3.3].
     * @details For each Pipe in the Graph, calls the removePipeCompare() function to obtain the cities affected by its removal.
     * @return Unordered Map that associates the code of each Pipe removed (activated member variable set to false) with a vector with CityInfo structs for each city affected by said removal.
     * @note Time complexity: O(n*p), where n is the number of Pipe objects in the Graph and p is the complexity of removePipeCompare().
     * @see CityInfo
     * @see removePipeCompare()
     */
    std::unordered_map<std::string, std::vector<CityInfo>> removeAllPipesCompare();
    /**
     * @brief Function that implements an extra feature of exercise [T3.3].
     * @details For each Pipe in the input (found by their source and destination codes - in each pair of the set), calls getCitiesFlow() with those Pipes removed (activated member variable set to false) and stores the new totalFlow of each city.
     * @param pipelines Set (to avoid duplicate Pipeline objects) of pairs with the source  and destination Location codes of each pipeline.
     * @return Vector with CityInfo structs for each city affected by the removal of the multiple Pipe objects.
     * @note Time complexity: O(n+p+q), where: <br>
     *       - n is the complexity of getCitiesFlow(); <br>
     *       - p is the number of City objects in the Graph; <br>
     *       - q is the number of Pipelines to be removed;
     * @see CityInfo
     * @see getCitiesFlow()
     */
    std::vector<CityInfo> removeMultiplePipesCompare(const std::set<std::pair<std::string, std::string>> &pipelines);
    /**
     * @brief Auxiliary function for the implementation of the feature of exercise [T3.3].
     * @details Given the codes of the source and destination, calls getCitiesFlow() on a Graph copy with that Pipe removed (activated member variable set to false) and stores the new totalFlow of each city.
     * @param sourceCode Code (content) of the Pipe's source (Location object).
     * @param destCode Code (content) of the Pipe's destination (Location object).
     * @return Vector with CityInfo structs for each city affected by the removal of the Pipe.
     * @note Time complexity: O(n+p), where n is the complexity of getCitiesFlow() and p is the number of City objects in the Graph.
     * @see CityInfo
     * @see getCitiesFlow()
     */
    std::vector<CityInfo> removePipeCompare(const std::string &sourceCode, const std::string &destCode);

};


#endif /* PROJDA1_GRAPH_HPP */