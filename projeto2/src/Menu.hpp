#ifndef PROJDA2_MENU_HPP
#define PROJDA2_MENU_HPP


#include "Graph.hpp"
#include <string>
#include <cstdint>


class Menu;

/// @brief Context class (part of the State design pattern).
/// @details Stores the data structures that hold the information provided in the files.
/// @see Graph.hpp
class Current {
private:
    /// @brief Pointer to the current menu (implementation of interface Menu) being shown on screen.
    /// @see Menu
    Menu* currentMenu;
    /// @brief The graph data structure. It's where the whole supply network is stored, with Water Reservoirs, Pumping Stations, Delivery Sites (as Vertexes) and Pipelines (as Edges);
    /// @see Graph
    Graph network;
    /**
     * @brief Unsigned 8-bit integer where the 3 LSbits (Least Significant bits) correspond to one which type of dataset was parsed.
     * @details Works as a small boolean array. The 3 types of datasets are: <br>
     *         - Bit 0: Toy-Graphs (3 small graphs - up to 13 vertices); <br>
     *         - Bit 1: Extra Fully-Connected Graphs - (12 medium graphs - between 25 and 900 vertices); <br>
     *         - Bit 2: Real-World Graphs - (3 big graphs - between 1k and 10k vertices);
     */
     uint8_t datasetParsed;

     /// @brief Stores the distance of the Triangular Approximation Algorithm
     /// @details For comparisons with the Other Heuristic Algorithm
     double triApproxDist;
    /// @brief Stores the time taken for running the Triangular Approximation Algorithm
    /// @details For comparisons with the Other Heuristic Algorithm
     double triApproxTime;

    /**
     * @brief Input type checker
     * @param input Can be made up of only an int value (menu option). <br>
     *              Or string of characters (specific input).
     * @returns false if the input string is empty (to be evaluated as an invalid string by Menu::handleInput()). <br>
     *          false if the input string is made up of characters. <br>
     *          true if the input string is made up of an int.
     * @note Time complexity: O(n), where n is the size of the input.
     */
    static bool isInt(const std::string &input);
public:
    /**
     * @brief Constructor of menus' interface.
     * @details Initializes the interface to run the menus (starting from the main menu). Processes all the files into the respective data structures.
     * @see Menu
     * @see Graph::Graph()
     * @note Time complexity: O(1)
     */
    Current();

    /**
     * @brief Runs the tool.
     * @details Starts the main menu and keeps the menus running until the 'Exit' option is selected.
     * @see Menu::print()
     * @see Menu::handleInput()
     * @note Time complexity: O(n*(p + q)), where: <br>
     *      - n is the number of inputs inserted by the user;
     *      - p is the complexity of the isInt() function;
     *      - q is the complexity of the handleInput() function;
     */
    void run();

    /**
     * @brief Destructor of menus' interface.
     * @details Deletes the allocated memory by the currentMenu member variable at the end of the Current object's lifetime.
     * @note Time complexity: O(1)
     */
    ~Current();


    /**
     * @brief Read-only overloaded getter for the Graph member variable.
     * @returns Copy of the Graph object.
     */
    Graph getNetwork() const;
    /**
     * @brief Read-write overloaded getter for the Graph member variable.
     * @returns Modifiable reference to the Graph object.
     */
    Graph& getNetwork();
    /**
     * @brief Getter for the filesParsed member variable.
     * @returns Unsigned 8-bit integer value where each of the 3 LSbits correspond to one of the 3 types of datasets.
     */
    uint8_t getParsed() const;
    /**
     * @brief
     * @returns true if any of the 3 types of datasets were parsed.
     */
    bool isParsed() const;
    /**
     * @brief Getter for the triApproxDist member variable.
     * @returns Distance of the Triangular Approximation Algorithm.
     */
    double getTriApproxDist() const;
    /**
     * @brief Getter for the triApproxTime member variable.
     * @returns Time taken for running the Triangular Approximation Algorithm.
     */
    double getTriApproxTime() const;


    /**
     * @brief Function to reset the Network and delete all loaded data.
     * @details It is called when 4 files have already been parsed and the user wishes to parse other files, to avoid mixing data.
     */
    void resetNetwork();
    /**
     * @brief Function to reset the member variable who tracks the parsed dataset.
     */
    void resetParsed();
    /**
     * @brief Function to reset the member variables who track the distance and time of the Triangular Approximation Algorithm.
     */
    void resetCompares();


    /**
     * @brief Setter for the datasetParsed member variable.
     * @param pos Unsigned integer value with dataset's respective bit position in the datasetParsed member variable: <br>
     *        - Bit 0 for Toy-Graphs; <br>
     *        - Bit 1 for Extra Fully-Connected Graphs; <br>
     *        - Bit 2 for Real-World Graphs;
     */
    void setParsed(unsigned int pos);
    /**
     * @brief Setter for the triApproxDist member variable.
     * @param dist Distance of the Triangular Approximation Algorithm.
     */
    void setTriApproxDist(double dist);
    /**
     * @brief Setter for the triApproxTime member variable.
     * @param time Time taken for running the Triangular Approximation Algorithm.
     */
    void setTriApproxTime(double time);
};



/// @brief Interface class (Abstract Class) for all the menus (part of the State design pattern).
/// @details Provides an interface for all the menus to implement. Those menus can be accessed throughout the tool's runtime.
class Menu {
public:

    /**
     * @brief Printer Interface
     * @details Pure virtual function to print the menus that are shown on screen while running the tool. <br>
     *          It needs to be implemented by the derived classes, according to what needs to be printed.
     * @note This function is marked as virtual to enable dynamic polymorphism. <br>
     *       Additionally, its signature ending with '= 0' forces the derived classes to implement it, and makes this class an abstract class.
     */
    virtual void print() = 0;

    /**
     * @brief Input Handler Interface
     * @details Pure virtual function to manage the inputs. <br>
     *          Its implementations by the derived classes will check for the validity of the input. If valid, they will call all the necessary functions, and then choose the next menu, according to the input provided.
     * @param menu Pointer to the Current class, to get access to the application state, mainly information in the member variables.
     * @param option User's int input, -1 is passed when the user inputs a valid string.
     * @param input User's string input, that will be split into arguments if needed by the derived classes. "" is passed when the input isn't a string.
     * @return Pointer to the next Menu's subclass object to be printed to the screen.
     * @note Time complexity of the functions called inside each of the handleInput() implementations will vary depending on the derived class, and will be omitted for brevity. These complexities can be found in the implementations of the respective functions.
     */
    virtual Menu* handleInput(Current *menu, int option, std::string input) = 0;

    /**
     * @brief Virtual destructor for the Menu class.
     * @details When an object of a derived class is deleted through a pointer to the Menu class, this virtual destructor ensures that the proper derived class destructor is called.
     * @note The destructor is marked as virtual to enable dynamic polymorphism.
     */
    virtual ~Menu() = default;
};


/// @brief Main Menu (Concrete Class)
/// @details This menu, as well as all the other derived classes, handle the inputs, checking their validity, call the necessary functions, select the next menu and print it to the screen.
/// @note This documentation will be omitted from further derived classes, as their only differences are related to user display and interaction.
class MainMenu : public Menu {
private:
    /// @brief Boolean corresponding to the success of the file(s) parsing functions that were called during this execution.
    uint8_t datasetParsed;
public:
    /**
     * @brief Parametrized constructor of the MainMenu class.
     * @details Initializes the member variable with its respective parameter.
     * @param datasetParsed Boolean corresponding to the success of the file(s) parsing functions during this execution.
     * @note Time complexity: O(1).
     */
    explicit MainMenu(uint8_t datasetParsed);

    /// @brief MainMenu's implementation of the Menu::print() function
    void print() override;
    /// @brief MainMenu's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief MenuParseDataset (Concrete Class)
class MenuParseDataset : public Menu {
public:
    /// @brief MenuParseDataset's implementation of the Menu::print() function
    void print() override;
    /// @brief MenuParseDataset's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Menu5 (Concrete Class)
class Menu5 : public Menu {
public:
    /// @brief Menu5's implementation of the Menu::print() function
    void print() override;
    /// @brief Menu5's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Footer Menu (Concrete Class)
class FooterMenu : public Menu {
public:
    /// @brief FooterMenu's implementation of the Menu::print() function
    void print() override;
    /// @brief FooterMenu's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief ParseSuccess Menu (Concrete Class)
class ParseSuccessMenu : public Menu {
private:
    /// @brief Boolean corresponding to the success of the file(s) parsing functions that were called during this execution.
    bool datasetParsed;
public:
    /**
     * @brief Parametrized constructor of the ParseSuccess class.
     * @details Initializes the member variable with its respective parameter.
     * @param datasetSuccess Boolean corresponding to the success of the file(s) parsing functions during this execution.
     * @note Time complexity: O(1).
     */
    ParseSuccessMenu(bool datasetSuccess);

    /// @brief ParseSuccess's implementation of the Menu::print() function
    void print() override;
    /// @brief ParseSuccess's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Footer Menu (Concrete Class)
class ResetMenu : public Menu {
public:
    /// @brief FooterMenu's implementation of the Menu::print() function
    void print() override;
    /// @brief FooterMenu's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};


#endif // PROJDA2_MENU_HPP