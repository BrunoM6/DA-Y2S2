#ifndef PROJDA2_GRAPH_HPP
#define PROJDA2_GRAPH_HPP

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <set>


#define INF std::numeric_limits<double>::max()


/// @brief Class for Vertexes
/// @details Each Vertex object represents a vertex in the Graph.
class Vertex {
protected:
    /// @brief Unique identifier of the location the Vertex object refers to.
    int id;
    /// @brief Latitude of the location the Vertex object refers to.
    double latitude = 0.0;
    /// @brief Longitude of the location the Vertex object refers to.
    double longitude = 0.0;

    /// @brief Map of all the outgoing edges of the Vertex object, with the dest id as key and the weight of the edge as value.
    std::unordered_map<int, double> adj;

    /// @brief Auxiliary field used in DFS and BFS algorithms.
    /// @details Used to, during a DFS or BFS traversal, know if a vertex has been iterated through (visited) already.
    bool visited = false;

public:
    /**
     * @brief Parametrized constructor of the Vertex class without coordinates.
     * @details Initializes the member variables with their respective parameters. The adj and incoming member variables are initialized using the Vertex::addEdge() function.
     * @param id Integer value corresponding to the id member variable.
     * @note Time complexity: O(1).
     */
    explicit Vertex(int id);

    /**
     * @brief Parametrized constructor of the Vertex class.
     * @details Initializes the member variables with their respective parameters. The adj and incoming member variables are initialized using the Vertex::addEdge() function.
     * @param id Integer value corresponding to the id member variable.
     * @param latitude Integer value corresponding to the latitude member variable.
     * @param longitude Integer value corresponding to the longitude member variable.
     * @note Time complexity: O(1).
     */
    Vertex(int id, double latitude, double longitude);

    /**
     * @brief Getter for the unique identifier of the location the Vertex object refers to.
     * @returns String with the id.
     */
    int getId() const;

    /**
     * @brief Getter for the value of the latitude member variable.
     * @returns Double value that refers to the latitude.
     */
    double getLatitude() const;

    /**
     * @brief Getter for the value of the longitude member variable.
     * @returns Double value that refers to the longitude.
     */
    double getLongitude() const;

    /**
     * @brief Getter for the map of pointers to outgoing edges (Edge objects).
     * @returns Map with key as id of dest and value as the weight.
     */
    std::unordered_map<int, double> getAdj() const;

    /**
     * @brief Getter for the visited state of the Vertex object.
     * @returns Boolean with the visited state.
     */
    bool isVisited() const;


    /**
     * @brief Sets the value of the latitude member variable.
     * @param lat Double value that refers to vertex latitude.
     */
    void setLatitude(double lat);

    /**
     * @brief Sets the value of the longitude member variable.
     * @param lon Double value that refers to the vertex longitude.
     */
    void setLongitude(double lon);

    /**
     * @brief Sets the value of the visited member variable.
     * @param visited Boolean value containing the new visited state.
     */
    void setVisited(bool visited);


    /**
     * @brief Adds an edge to a certain vertex.
     * @details If edge already exists it will replace the value instead.
     * @param dest Id of the destination vertex.
     * @param weight Weight of the destination vertex.
     */
     void addEdge(int dest, double weight);
     /**
      * @brief Removes edge from a certain vertex.
      */
     void removeEdge(int dest);
};

/// @brief Class to represent a graph.
/// @see Vertex
class Graph {
private:
    /// @brief Member variable that stores all the vertices of a Graph object.
    std::unordered_map<int, Vertex*> vertexMap;


    /**
     * @brief Calculates total weights of a path represented by a vector.
     * @param path A vector with the ids, in order, of the path.
     * @return A double that represents the total weight of the path.
     */
    double pathDistance(std::vector<int> path) const;
    /**
     * @brief Calculates the distance of two valid vertexes using their latitude and longitude values.
     * @param v1 First vertex.
     * @param v2 Second vertex.
     * @return Double values representative of the distance.
     * @note Time complexity: O(1), simple calculation.
     */
    static double getHaversine(Vertex *v1, Vertex *v2) ;
    /**
     * @brief Converts a coordinate from degrees to radians.
     * @param coord Coordinate in degrees to be converted to radians.
     * @return Double value containing the coordinate in radians.
     * @note Time complexity: O(1).
     */
    static double convertToRadians(double coord);

    /**
     * @brief Function that computes all Hamiltonian paths starting at an arbitrary source vertex.
     * @param source Id of the source vertex in which to start and end each path.
     * @return A vector of vectors, each element of this vector is a path represented by a vector of integers which are the ids of the traversed vertices.
     * @note Time complexity: O(n!), where: <br>
     *       - n is the number of vertices; <br>
     */
    std::vector<std::vector<int>> computeHamiltonianPaths(int source) const;
    /**
     * @brief Recursive auxiliary function to compute all paths.
     * @param result Address of result, will store paths as we reach the end of the recursive call.
     * @param path Vector with current path state being explored.
     * @param remaining Stores information about which nodes have been already visited in the current path.
     * @note Time complexity: O(n!), where: <br>
     *       - n is the number of vertices; <br>
     */
    void hamiltonianRecursive(std::vector<std::vector<int>> &result, std::vector<int> path, std::vector<bool> remaining) const;


    /**
     * @brief Backtracking recursive auxiliary function.
     * @note Time complexity: O(n^2*2^n), where: <br>
     *      - n is the number of vertices; <br>
     */
    void recursiveBacktracking(std::vector<int> &path, std::vector<int> &best_path) const;

    /**
     * @brief Uses Prim's Algorithm to get MST for Graph.
     * @return A vector of vectors representing resulting MST.
     * @note Time complexity: O((V + E) log V), where: <br>
     *      -  V is the number of vertices <br>
     *      -  E is the number of edges <br>
     */
    std::vector<std::vector<int>> prims() const;
    /**
     * @brief Performs a DFS traversal of a tree.
     * @details Traverses the tree in a pre-order way.
     * @param tree
     * @param res Vector to store the result of the traversal.r
     * @param current Node currently being visited.
     * @param visited Vector to keep track of visited nodes.
     */
    void preorderDFS(const std::vector<std::vector<int>>& tree, std::vector<int>& res, int current, std::vector<bool>& visited) const;


public:
    /**
     * @brief Default constructor of the Graph class.
     * @details Initializes the member variables with empty placeholders.
     * @note Time complexity: O(1).
     */
    Graph();

    /**
     * @brief Destructor of the Graph class.
     * @details Deletes all the vertices in the vertexSet member variable.
     * @note Time complexity: O(n), where n is the number of vertices in the vertexSet member variable.
     */
    ~Graph();

    /**
     * @brief Edge reader function
     * @details Initializes part of the vertexMap member variable and the respective vertices and edges by reading them from a file.
     * @param file String containing the path to the .csv file.
     * @param firstLine Boolean that informs if there is a first line or not. Used because of the medium size graphs.
     * @return true if the file was parsed correctly; <br>
     *         false, if the file wasn't opened correctly;
     * @note Time complexity: O(n*p), where n is the number lines in the file and p is the complexity of unordered_map::find() method, that depends on the number of collisions, which a good hash function should minimize.
     * @see addVertexReservoir()
     */
    bool read_edges(const std::string &file, bool firstLine = true);

    /**
     * @brief Node reader function
     * @details Initializes vertices from a graph.
     * @param file String containing the path to the .csv file.
     * @return true if the file was parsed correctly; <br>
     *         false, if the file wasn't opened correctly;
     * @note Time complexity: O(n*p), where n is the number lines in the flights.csv file and p is the complexity of unordered_map::find() method, that depends on the number of collisions, which a good hash function should minimize.
     * @see addVertexReservoir()
     */
    bool read_nodes(const std::string &file);


    /**
     * @brief Getter for the total number of vertices in the vertexSet member variable.
     * @returns Integer with the number of vertices in the Graph.
     */
    int getNumVertex() const;

    /**
     * @brief Getter for the vertexSet member variable.
     * @returns Vector with all the vertices in the graph.
     */
    std::unordered_map<int, Vertex*> getVertexMap() const;


    /**
     * @brief Auxiliary function to find a vertex with a given content.
     * @param id of the vertex.
     * @return Pointer to the vertex if found, and nullptr otherwise.
     * @note Time complexity: O(n), where n is the number of vertices in the vertexSet member variable.
     */
    Vertex* findVertex(const int &id) const;


    /**
     * @brief Adds all edges previously removed to the graph.
     * @param v Desired vertex to add edges pointing to.
     * @param weights Map with key as id and value as weight, used to store weight info.
     * @see Vertex::addEdge()
     */
     void addAllEdgesTo(Vertex *v, std::unordered_map<int, double> weights) const;

     /**
     * @brief Removes all edge to desired vertex.
     * @param v Desired vertex to isolate.
     * @returns Unordered map with key as id and value as weight, used to store weight info for latter adding.
     * @see Vertex::addEdge()
      */
      std::unordered_map<int,double> removeAllEdgesTo(Vertex *v) const;


    /**
     * @brief Uses brute-force approach to resolve TSP.
     * @param src Id of source vertex.
     * @return Vector with the resulting path.
     * @note Time complexity: O(n!), since we use computeHamiltonianPaths, where: <br>
     *      - n is the number of vertices; <br>
     */
     std::vector<int> bruteForceTSP(int src) const;


    /**
     * @brief Backtracking approach to reach TSP solution - exercise [T4.1].
     * @param source Refers to the chosen starting vertex for the TSP.
     * @return Vector with the resulting path.
     * @note Time complexity: O(V!), where: <br>
     *      - V is the number of vertices; <br>
     */
    std::vector<int> backtrackingTSP(int source, double &dist, double &time) const;

    /**
     * @brief Function that uses Triangular Approximation Principle to reach TSP solution - exercise [T4.2].
     * @param src Integer representing the id of the source vertex.
     * @return Vector with the resulting path.
     * @note Time complexity: O(((V + E) log V) + V), where: <br>
     *      -  V is the number of vertices <br>
     *      -  E is the number of edges <br>
     */
    std::vector<int> triangularApproximationTSP(int src, double &dist, double &time) const;

    /**
    * @brief Function that uses Nearest-Neighbour heuristic to reach TSP solution - exercise [T4.3].
    * @param src Id of source vertex.
    * @return A double with the total path cost
    * @note Time complexity: O(n^2), where: <br>
    *      - n is the number of vertices; <br>
    */
    std::vector<int> nearestNeighbourTSP(int src, double &dist, double &time) const;

    /**
     * @brief Uses the ant colony optimization algorithm to find TSP solution - exercise [T4.4]
     * @param
     */
};

#endif /* PROJDA2_GRAPH_HPP */