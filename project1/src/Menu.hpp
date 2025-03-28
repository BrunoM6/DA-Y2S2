#ifndef PROJDA1_MENU_HPP
#define PROJDA1_MENU_HPP


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
     * @brief Unsigned 8-bit integer where each of the the 4 LSbits (Least Significant bits) correspond to one of the 4 files that need to be parsed.
     * @details Works as a boolean array. <br>
     * @note Other functions related to menu execution frequently use the condition filesParsed == 0x0F. If the member variable has that value, it means that the 4 LSbits are set (00001111 == 0x0F), which means that all files have been read.
     */
     uint8_t filesParsed;

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
     * @returns Unsigned 8-bit integer value where each of the 4 LSbits correspond to one of the 4 files that need to be parsed.
     */
    uint8_t getFilesParsed() const;


    /**
     * @brief Function to reset the Network and delete all loaded data.
     * @details It is called when 4 files have already been parsed and the user wishes to parse other files, to avoid mixing data.
     */
    void resetNetwork();
    /**
     * @brief Function to reset the member variable who tracks the parsed files.
     */
    void resetParsed();


    /**
     * @brief Setter for the filesParsed member variable.
     * @param pos Unsigned integer value with file's respective bit position in the filesParsed member variable: <br>
     *        - Bit 0 for file with Water Reservoir information; <br>
     *        - Bit 1 for file with Pumping Station information; <br>
     *        - Bit 2 for file with Delivery Site (City) information; <br>
     *        - Bit 3 for file with Pipeline information;
     */
    void setParsed(unsigned int pos);
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
public:
    /// @brief MainMenu's implementation of the Menu::print() function
    void print() override;
    /// @brief MainMenu's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Menu1Choose (Concrete Class)
class Menu1Choose : public Menu {
public:
    /// @brief Menu1Choose's implementation of the Menu::print() function
    void print() override;
    /// @brief Menu1Choose's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Menu1 (Concrete Class)
class Menu1 : public Menu {
private:
    /// @brief Type of file option received from input during the execution of Menu1Choose
    /// @details Stored for further use during this class's execution
    unsigned int fileType;
public:
    /**
     * @brief Parametrized constructor of the Menu1 class.
     * @details Initializes the member variable with its respective parameter.
     * @param file_type Unsigned integer value corresponding to the type of file chosen during Menu1Choose's execution.
     * @note Time complexity: O(1).
     */
    Menu1(unsigned int file_type);

    /// @brief Menu1's implementation of the Menu::print() function
    void print() override;
    /// @brief Menu1's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Menu3 (Concrete Class)
class Menu3 : public Menu {
public:
    /// @brief Menu3's implementation of the Menu::print() function
    void print() override;
    /// @brief Menu3's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Menu6 (Concrete Class)
class Menu6 : public Menu {
public:
    /// @brief Menu6's implementation of the Menu::print() function
    void print() override;
    /// @brief Menu6's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Menu7 (Concrete Class)
class Menu7 : public Menu {
public:
    /// @brief Menu7's implementation of the Menu::print() function
    void print() override;
    /// @brief Menu7's implementation of the Menu::handleInput() function
    Menu* handleInput(Current *menu, int option, std::string input) override;
};

/// @brief Menu9 (Concrete Class)
class Menu9 : public Menu {
public:
    /// @brief Menu9's implementation of the Menu::print() function
    void print() override;
    /// @brief Menu9's implementation of the Menu::handleInput() function
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
    /// @brief Boolean received from read_file functions, indicating if the file parsing was successful
    /// @details Stored from printing the appropriate menu
    bool currFiles;
    /// @brief Boolean received from context class, indicating if the current state of parsing of all the files
    /// @details Stored from printing the appropriate menu
    bool allFiles;
public:
    /**
     * @brief Parametrized constructor of the ParseSuccess class.
     * @details Initializes the member variables with their respective parameters.
     * @param cfSuccess Boolean corresponding to the success of the file(s) parsing functions that were called during this execution.
     * @param afSuccess Boolean corresponding to the overall parsing success of all the 4 necessary files for proper tool execution
     * @note Time complexity: O(1).
     */
    ParseSuccessMenu(bool cfSuccess, bool afSuccess);

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


#endif // PROJDA1_MENU_HPP