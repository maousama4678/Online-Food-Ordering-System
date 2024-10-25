#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>
#include <ctime>
using namespace std;

// Structure to store menu items
struct MenuItem {
    string name;
    double price;
};

// Structure to store user data
struct User {
    string username;
    string password;
    vector<string> orderHistory;
};

// Class to manage the Online Food Ordering System
class FoodOrderingSystem {
private:
    map<int, MenuItem> menu;
    map<string, User> users;  // Storing users' data
    vector<int> order;        // Stores item IDs ordered by the customer
    double totalPrice;
    string loggedInUser;

public:
    FoodOrderingSystem() {
        totalPrice = 0.0;
        loadMenuFromFile();    // Load the menu from a file
        loadUsersFromFile();   // Load user data from a file
    }

    // Function to load the menu from file
    void loadMenuFromFile() {
        ifstream file("menu.txt");
        if (!file) {
            cerr << "Error opening menu file!" << endl;
            return;
        }

        int id;
        string name;
        double price;
        while (file >> id >> ws && getline(file, name) && file >> price) {
            menu[id] = {name, price};
        }
        file.close();
    }

    // Function to load users from file
    void loadUsersFromFile() {
        ifstream file("users.txt");
        if (!file) {
            cerr << "Error opening users file!" << endl;
            return;
        }

        string username, password;
        while (file >> username >> password) {
            users[username] = {username, password};
        }
        file.close();
    }

    // Function to save users to file
    void saveUsersToFile() {
        ofstream file("users.txt");
        if (!file) {
            cerr << "Error saving users to file!" << endl;
            return;
        }

        for (const auto& user : users) {
            file << user.second.username << " " << user.second.password << endl;
        }
        file.close();
    }

    // Function to display the menu
    void displayMenu() {
        cout << "\n--------- MENU ---------" << endl;
        for (const auto& item : menu) {
            cout << item.first << ". " << item.second.name << " - Rs. " << item.second.price << endl;
        }
    }

    // Function for user registration
    void userRegistration() {
        string username, password;
        cout << "\n--- User Registration ---\n";
        cout << "Enter a username: ";
        cin >> username;
        cout << "Enter a password: ";
        cin >> password;

        if (users.find(username) != users.end()) {
            cout << "Username already exists. Try logging in.\n";
        } else {
            users[username] = {username, password};
            saveUsersToFile();
            cout << "Registration successful!\n";
        }
    }

    // Function for user login
    bool userLogin() {
        string username, password;
        cout << "\n--- User Login ---\n";
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        if (users.find(username) != users.end() && users[username].password == password) {
            loggedInUser = username;
            cout << "Login successful! Welcome, " << username << endl;
            return true;
        } else {
            cout << "Invalid username or password.\n";
            return false;
        }
    }

    // Function to take orders from the user
    void takeOrder() {
        int itemId;
        char choice;
        do {
            displayMenu();
            cout << "\nEnter the item number to order: ";
            cin >> itemId;

            if (menu.find(itemId) != menu.end()) {
                order.push_back(itemId);
                totalPrice += menu[itemId].price;
                cout << menu[itemId].name << " added to the order.\n";
            } else {
                cout << "Invalid item ID. Please try again.\n";
            }

            cout << "Do you want to order more items? (y/n): ";
            cin >> choice;
        } while (choice == 'y' || choice == 'Y');
    }

    // Function to generate a receipt with formatted output
    void generateReceipt() {
        cout << "\n\n********** RECEIPT **********" << endl;
        cout << "User: " << loggedInUser << endl;
        cout << "-----------------------------" << endl;

        cout << left << setw(25) << "Item" << right << setw(10) << "Price" << endl;
        cout << "-----------------------------" << endl;

        string orderSummary = "";
        for (int itemId : order) {
            cout << left << setw(25) << menu[itemId].name << right << setw(10) << menu[itemId].price << endl;
            orderSummary += menu[itemId].name + " Rs. " + to_string(menu[itemId].price) + "\n";
        }

        cout << "-----------------------------" << endl;
        cout << left << setw(25) << "Total Amount" << right << setw(10) << totalPrice << endl;
        cout << "*****************************\n\n";

        // Save the order to the user's order history
        time_t now = time(0);
        string dt = ctime(&now); // Get current time
        users[loggedInUser].orderHistory.push_back(dt + orderSummary + "Total: Rs. " + to_string(totalPrice) + "\n");
    }

    // Function to display the user's order history
    void displayOrderHistory() {
        cout << "\n--- Order History ---\n";
                if (users[loggedInUser].orderHistory.empty()) {
            cout << "No previous orders found.\n";
        } else {
            for (const auto& order : users[loggedInUser].orderHistory) {
                cout << order << endl;
            }
        }
    }

    // Function to start the ordering process
    void processOrder() {
        takeOrder();
        generateReceipt();
        displayOrderHistory();
    }

    // Function to display admin options for managing the menu
    void adminPanel() {
        char choice;
        cout << "\n--- Admin Panel ---\n";
        do {
            cout << "1. Add Item to Menu\n";
            cout << "2. Remove Item from Menu\n";
            cout << "3. Exit Admin Panel\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case '1': addItemToMenu(); break;
                case '2': removeItemFromMenu(); break;
                case '3': cout << "Exiting Admin Panel...\n"; break;
                default: cout << "Invalid choice! Try again.\n";
            }
        } while (choice != '3');
    }

    // Function to add a new item to the menu
    void addItemToMenu() {
        int itemId;
        string itemName;
        double itemPrice;
        cout << "Enter new item ID: ";
        cin >> itemId;
        cout << "Enter item name: ";
        cin.ignore();
        getline(cin, itemName);
        cout << "Enter item price: ";
        cin >> itemPrice;

        menu[itemId] = {itemName, itemPrice};
        saveMenuToFile();
        cout << "Item added to the menu successfully!\n";
    }

    // Function to remove an item from the menu
    void removeItemFromMenu() {
        int itemId;
        cout << "Enter the item ID to remove: ";
        cin >> itemId;

        if (menu.erase(itemId)) {
            saveMenuToFile();
            cout << "Item removed from the menu successfully!\n";
        } else {
            cout << "Item not found!\n";
        }
    }

    // Function to save the updated menu to a file
    void saveMenuToFile() {
        ofstream file("menu.txt");
        if (!file) {
            cerr << "Error saving menu to file!" << endl;
            return;
        }

        for (const auto& item : menu) {
            file << item.first << "\n" << item.second.name << "\n" << item.second.price << "\n";
        }
        file.close();
    }
};

int main() {
    FoodOrderingSystem system;
    char role;

    cout << "Welcome to the Online Food Ordering System!\n";
    cout << "Are you an admin (a) or a customer (c)? ";
    cin >> role;

    if (role == 'a') {
        string password;
        cout << "Enter admin password: ";
        cin >> password;

        if (password == "admin123") {
            system.adminPanel();
        } else {
            cout << "Invalid password!\n";
        }
    } else if (role == 'c') {
        char option;
        do {
            cout << "1. Register\n2. Login\n3. Exit\nEnter your option: ";
            cin >> option;

            if (option == '1') {
                system.userRegistration();
            } else if (option == '2') {
                if (system.userLogin()) {
                    system.processOrder();
                }
            }
        } while (option != '3');
    } else {
        cout << "Invalid role! Exiting...\n";
    }

    return 0;
}

