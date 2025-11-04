#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <string>
#include <algorithm>
#include <climits>
#include <set>
#include <limits>

using namespace std;

// --- Data Structures for Order and Graph Elements ---

// Structure representing a food order
struct Order {
    int id;               // Unique order identifier
    string restaurant;    // Starting point (Graph Node)
    string destination;   // Ending point (Graph Node)
    double price;         // Order price
};

// Structure representing a graph node (Location)
struct Location {
    string name;
};

// Structure representing a graph edge (Route)
struct Route {
    string start;
    string end;
    int distance; // Edge weight (distance in km)
};

// --- Core System Class ---

class FoodDeliverySystem {
private:
    // Queue: Manages incoming orders (FIFO: First-In, First-Out)
    queue<Order> incomingOrders;
    
    // Stack: Manages recently completed deliveries (LIFO: Last-In, First-Out)
    stack<Order> completedOrders;
    
    // Graph: Adjacency Map to store locations and routes for optimization.
    // map<StartLocation, map<EndLocation, Distance>>
    map<string, map<string, int>> deliveryGraph;
    
    // Map to quickly look up any order by its ID, regardless of its status.
    map<int, Order> allOrders;
    
    // Counter for generating unique order IDs
    int nextOrderId = 1001;

    /**
     * @brief Finds the shortest path between two locations using Dijkstra's Algorithm.
     * @param start The starting location (e.g., Depot or Restaurant).
     * @param end The destination location (e.g., Restaurant or Customer).
     * @return A vector of strings representing the path nodes.
     */
    vector<string> findShortestPath(const string& start, const string& end) {
        // Distances map: stores the shortest distance found so far from 'start' to every other node.
        map<string, int> distances;
        // Predecessors map: stores the previous node in the shortest path.
        map<string, string> predecessors;
        // Visited set: tracks nodes that have been finalized.
        set<string> visited;
        // Priority Queue: Min-heap to efficiently select the unvisited node with the smallest distance.
        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

        // Initialize distances to infinity for all known locations.
        for (const auto& pair : deliveryGraph) {
            distances[pair.first] = INT_MAX;
        }

        if (distances.find(start) == distances.end()) return {}; 

        // Start node distance is 0.
        distances[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            string current = pq.top().second;
            pq.pop();

            if (visited.count(current)) continue;
            visited.insert(current);

            // Explore neighbors
            if (deliveryGraph.count(current)) {
                for (const auto& edge : deliveryGraph[current]) {
                    string neighbor = edge.first;
                    int weight = edge.second;
                    // Relaxation step: check if a shorter path to the neighbor is found through 'current'.
                    if (distances[current] != INT_MAX && distances[current] + weight < distances[neighbor]) {
                        distances[neighbor] = distances[current] + weight;
                        predecessors[neighbor] = current;
                        pq.push({distances[neighbor], neighbor});
                    }
                }
            }
        }

        // Reconstruct path from the 'end' node back to 'start' using predecessors.
        vector<string> path;
        string current = end;
        if (distances.find(end) == distances.end() || distances[end] == INT_MAX) return {};

        while (predecessors.count(current)) {
            path.push_back(current);
            current = predecessors[current];
            if (current == start) {
                path.push_back(start);
                break;
            }
        }
        reverse(path.begin(), path.end());
        
        if (!path.empty() && path.front() == start) return path;
        return {}; 
    }

public:
    // Feature 1: Add a new graph node (Location)
    void addLocation(const string& name) {
        if (deliveryGraph.find(name) == deliveryGraph.end()) {
            deliveryGraph[name] = {};
            cout << "Location added: " << name << endl;
        } else {
            cout << "Location " << name << " already exists." << endl;
        }
    }

    // Feature 2: Add a new graph edge (Route) with distance (weight)
    void addRoute(const string& start, const string& end, int distance) {
        if (deliveryGraph.count(start) && deliveryGraph.count(end)) {
            // Add route for both directions (assuming roads are bidirectional)
            deliveryGraph[start][end] = distance;
            deliveryGraph[end][start] = distance;
            cout << "Route added: " << start << " <-> " << end << " (" << distance << " km)" << endl;
        } else {
            cout << "Error: One or both locations do not exist. Add them first." << endl;
        }
    }

    // Feature 3: Place a new order (Queue Push)
    void placeOrder(const string& restaurant, const string& destination, double price) {
        if (deliveryGraph.count(restaurant) && deliveryGraph.count(destination)) {
            Order newOrder = {nextOrderId++, restaurant, destination, price};
            incomingOrders.push(newOrder); // Add to the back of the queue
            allOrders[newOrder.id] = newOrder;
            cout << "New Order Placed (ID: " << newOrder.id << "): " << newOrder.restaurant << " -> " << newOrder.destination << endl;
        } else {
            cout << "Error: Restaurant or destination location does not exist in the map." << endl;
        }
    }

    // Feature 4: Process the next order (Queue Pop & Stack Push)
    void processNextOrder() {
        if (incomingOrders.empty()) {
            cout << "No pending orders in the queue." << endl;
            return;
        }
        Order orderToProcess = incomingOrders.front(); // Get the oldest order (FIFO)
        incomingOrders.pop();                          // Remove from the queue
        cout << "Processing Order ID " << orderToProcess.id << "..." << endl;
        
        cout << "Order ID " << orderToProcess.id << " delivered successfully!" << endl;
        completedOrders.push(orderToProcess); // Add to the top of the completed stack
    }

    // Feature 5: Track the last delivery (Stack Peek)
    void trackLastDelivery() {
        if (completedOrders.empty()) {
            cout << "No deliveries completed yet." << endl;
            return;
        }
        // Access the top element (most recent delivery) without removing it (LIFO)
        Order lastOrder = completedOrders.top();
        cout << "Last Completed Delivery (ID: " << lastOrder.id << "): " << lastOrder.restaurant << " to " << lastOrder.destination << endl;
    }

    // Feature 6: List all pending orders (Queue View)
    void listPendingOrders() {
        if (incomingOrders.empty()) {
            cout << "The order queue is empty." << endl;
            return;
        }
        queue<Order> tempQueue = incomingOrders; // Create a copy to iterate without modifying the original queue
        cout << "--- Pending Orders Queue ---" << endl;
        int count = 1;
        while (!tempQueue.empty()) {
            Order order = tempQueue.front();
            tempQueue.pop();
            cout << count++ << ". ID: " << order.id << " | From: " << order.restaurant << " | To: " << order.destination << " | Price: $" << order.price << endl;
        }
        cout << "--------------------------" << endl;
    }

    // Feature 7: Optimize Delivery Route (Graph Dijkstra)
    void optimizeDeliveryRoute(int orderId) {
        if (allOrders.find(orderId) == allOrders.end()) {
            cout << "Order ID " << orderId << " not found." << endl;
            return;
        }

        Order order = allOrders[orderId];
        string restaurant = order.restaurant;
        string destination = order.destination;
        string depot = "Depot"; // Assuming the agent starts at a Depot

        if (deliveryGraph.find(depot) == deliveryGraph.end()) {
            cout << "Error: 'Depot' location is missing for optimization." << endl;
            return;
        }

        // Path 1: Agent travel from Depot to Restaurant
        vector<string> path1 = findShortestPath(depot, restaurant);
        int distance1 = calculatePathDistance(path1);

        // Path 2: Delivery travel from Restaurant to Destination
        vector<string> path2 = findShortestPath(restaurant, destination);
        int distance2 = calculatePathDistance(path2);

        cout << "--- Route Optimization for Order " << orderId << " ---" << endl;
        
        cout << "1. Agent Path (Depot to Restaurant) - Total Distance: ";
        if (distance1 != INT_MAX) cout << distance1 << " km" << endl;
        else cout << "N/A (Route not found)" << endl;

        if (distance1 != INT_MAX && !path1.empty()) {
            for (const string& loc : path1) {
                cout << loc << (loc == path1.back() ? "" : " -> ");
            }
            cout << endl;
        }

        cout << "2. Delivery Path (Restaurant to Destination) - Total Distance: ";
        if (distance2 != INT_MAX) cout << distance2 << " km" << endl;
        else cout << "N/A (Route not found)" << endl;

        if (distance2 != INT_MAX && !path2.empty()) {
            for (const string& loc : path2) {
                cout << loc << (loc == path2.back() ? "" : " -> ");
            }
            cout << endl;
        }

        int totalDistance = (distance1 == INT_MAX || distance2 == INT_MAX) ? -1 : distance1 + distance2;
        if (totalDistance != -1) {
             cout << "Total Estimated Delivery Distance: " << totalDistance << " km" << endl;
        } else {
             cout << "Total Estimated Delivery Distance: Cannot be calculated (Missing route)." << endl;
        }
    }
    
    // Utility function to sum up the weights (distances) along a given path
    int calculatePathDistance(const vector<string>& path) {
        if (path.size() < 2) return 0;
        int totalDistance = 0;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            string start = path[i];
            string end = path[i+1];
            // Check if the edge exists and add its distance
            if (deliveryGraph.count(start) && deliveryGraph[start].count(end)) {
                totalDistance += deliveryGraph[start][end];
            } else {
                return INT_MAX; // Indicate a broken path
            }
        }
        return totalDistance;
    }

    // Feature 8: Revert the last delivery (Service Recovery: Stack Pop & Queue Push)
    void revertLastDelivery() {
        if (completedOrders.empty()) {
            cout << "No completed deliveries to revert." << endl;
            return;
        }
        Order revertedOrder = completedOrders.top(); // Get the most recent completed order
        completedOrders.pop();                       // Remove from the completed stack
        incomingOrders.push(revertedOrder);          // Place it back into the pending queue for re-processing
        cout << "Reverted delivery ID " << revertedOrder.id << " and placed back in the pending queue." << endl;
    }

    // Feature 9: View system statistics and content summary
    void systemStatus() {
        cout << "\n--- System Status ---" << endl;
        cout << "Pending Orders (Queue Size): " << incomingOrders.size() << endl;
        cout << "Completed Deliveries (Stack Size): " << completedOrders.size() << endl;
        cout << "Next Order ID to use: " << nextOrderId << endl;
        cout << "Total Locations in Graph: " << deliveryGraph.size() << endl;
        cout << "Locations available: ";
        for (const auto& pair : deliveryGraph) {
            cout << pair.first << ", ";
        }
        // Backspace characters to remove the final comma and space
        cout << "\b\b \n---------------------\n" << endl;
    }
};

// --- Input Handling Functions ---

// Displays the main interactive menu options
void showMenu() {
    cout << "\n=============== Food Delivery System Menu ===============" << endl;
    cout << "1. Add Location (Graph Node)" << endl;
    cout << "2. Add Route (Graph Edge)" << endl;
    cout << "3. Place New Order (Queue Push)" << endl;
    cout << "4. Process Next Order (Queue Pop & Stack Push)" << endl;
    cout << "5. Track Last Completed Delivery (Stack Peek)" << endl;
    cout << "6. List All Pending Orders (Queue View)" << endl;
    cout << "7. Optimize Delivery Route (Graph Dijkstra)" << endl;
    cout << "8. Revert Last Delivery (Stack Pop & Queue Push)" << endl;
    cout << "9. View System Status" << endl;
    cout << "0. Exit" << endl;
    cout << "=========================================================" << endl;
    cout << "Enter your choice: ";
}

// Robust input function for integers
int getIntInput() {
    int value;
    while (!(cin >> value)) {
        cout << "Invalid input. Please enter an integer: ";
        cin.clear(); // Clear the error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard bad input
    }
    // Consume the remaining newline character after successful read
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Robust input function for doubles (prices)
double getDoubleInput() {
    double value;
    while (!(cin >> value)) {
        cout << "Invalid input. Please enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Input function for strings (locations)
string getStringInput() {
    string value;
    getline(cin, value);
    return value;
}

// --- Main Execution ---

int main() {
    FoodDeliverySystem fds;
    int choice;

    cout << "--- Initial Setup ---" << endl;
    cout << "Setting up central Depot location..." << endl;
    fds.addLocation("Depot");

    do {
        showMenu();
        choice = getIntInput();

        switch (choice) {
            case 1: {
                cout << "Enter new location name: ";
                string locName = getStringInput();
                fds.addLocation(locName);
                break;
            }
            case 2: {
                string start, end;
                int dist;
                cout << "Enter starting location: ";
                start = getStringInput();
                cout << "Enter ending location: ";
                end = getStringInput();
                cout << "Enter distance (km): ";
                dist = getIntInput();
                fds.addRoute(start, end, dist);
                break;
            }
            case 3: {
                string rest, dest;
                double price;
                cout << "Enter Restaurant Location: ";
                rest = getStringInput();
                cout << "Enter Customer Destination: ";
                dest = getStringInput();
                cout << "Enter Order Price ($): ";
                price = getDoubleInput();
                fds.placeOrder(rest, dest, price);
                break;
            }
            case 4: {
                fds.processNextOrder();
                break;
            }
            case 5: {
                fds.trackLastDelivery();
                break;
            }
            case 6: {
                fds.listPendingOrders();
                break;
            }
            case 7: {
                int orderId;
                cout << "Enter Order ID to optimize route for: ";
                orderId = getIntInput();
                fds.optimizeDeliveryRoute(orderId);
                break;
            }
            case 8: {
                fds.revertLastDelivery();
                break;
            }
            case 9: {
                fds.systemStatus();
                break;
            }
            case 0: {
                cout << "Exiting Food Delivery System. Goodbye! 👋" << endl;
                break;
            }
            default: {
                cout << "Invalid choice. Please try again." << endl;
                break;
            }
        }
    } while (choice != 0);

    return 0;
}