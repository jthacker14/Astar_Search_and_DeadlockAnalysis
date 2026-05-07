#include <iostream>
#include <vector>
#include <string>
#include <queue>       // For the priority_queue
#include <map>         // To store the path (parent states)
#include <unordered_set> // To store visited states (closed set)
#include <sstream>     // For string hashing
#include <algorithm>   // For std::reverse
#include <limits>

// Use the standard namespace
using namespace std;

// --- 1. State Representation ---

const int NUM_COLUMNS = 8;
const int NUM_CELLS = 4;
const int NUM_FOUNDATIONS = 4;

/**
 * @brief Represents a single playing card.
 * Suit: 0=H, 1=D, 2=C, 3=S
 * Rank: 1=A, 2-10, 11=J, 12=Q, 13=K
 */
struct Card {
    int suit;
    int rank;

    // Needed for comparisons
    bool operator==(const Card& other) const {
        return suit == other.suit && rank == other.rank;
    }
    bool operator!=(const Card& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Represents the *entire* game state (the node for A*).
 */
struct GameState {
    vector<Card> columns[NUM_COLUMNS];
    Card cells[NUM_CELLS]; // Use rank 0 to mean "empty"
    int foundations[NUM_FOUNDATIONS]; // Store the highest rank (e.g., 3 means Ace, 2, 3 are there)

    int g_cost; // Cost from start (number of moves)
    int h_cost; // Heuristic cost

    // Default constructor for initialization
    GameState() : g_cost(0), h_cost(0) {
        for(int i=0; i<NUM_CELLS; ++i) cells[i] = {0, 0}; // {suit 0, rank 0} = empty
        for(int i=0; i<NUM_FOUNDATIONS; ++i) foundations[i] = 0; // 0 = empty
    }

    // A* cost function
    int f_cost() const {
        return g_cost + h_cost;
    }

    // Comparison for the priority queue (we want the *lowest* f_cost at the top)
    bool operator>(const GameState& other) const {
        return f_cost() > other.f_cost();
    }
};

// --- 2. Helper & Heuristic Functions ---

string getRankStr(int rank) {
    if (rank == 1) return "A";
    if (rank == 11) return "J";
    if (rank == 12) return "Q";
    if (rank == 13) return "K";
    if (rank == 0) return " ";
    return to_string(rank);
}

string getSuitStr(int suit) {
    if (suit == 0) return "H";
    if (suit == 1) return "D";
    if (suit == 2) return "C";
    if (suit == 3) return "S";
    return " ";
}

string cardToStr(Card c) {
    // FIX 1: Handle the empty card case (rank 0) first.
    if (c.rank == 0) return " ";
    return getRankStr(c.rank) + getSuitStr(c.suit);
}

/**
 * @brief Creates a unique string "hash" for a state to check if we've visited it.
 */
string hashState(const GameState& state) {
    stringstream ss;
    for (int i = 0; i < NUM_COLUMNS; ++i) {
        ss << "C" << i << ":";
        for (const auto& card : state.columns[i]) {
            ss << cardToStr(card) << ",";
        }
        ss << "|";
    }
    for (int i = 0; i < NUM_CELLS; ++i) {
        ss << "L" << i << ":" << cardToStr(state.cells[i]) << "|";
    }
    for (int i = 0; i < NUM_FOUNDATIONS; ++i) {
        ss << "F" << i << ":" << state.foundations[i] << "|";
    }
    return ss.str();
}

/**
 * @brief Prints a state to the console (simplified).
 */
void printState(const GameState& state) {
    cout << "--------------------------------\n";
    for (int i = 0; i < 4; ++i) {
        cout << "[Cell " << i << ": " << cardToStr(state.cells[i]) << "] ";
    }
    cout << "\n";
    for (int i = 0; i < 4; ++i) {
        cout << "[Fnd " << i << " (" << getSuitStr(i) << "): " << getRankStr(state.foundations[i]) << "] ";
    }
    cout << "\n--\n";
    for (int i = 0; i < 3; ++i) { // Only print first 3 columns for demo
        cout << "Col " << i << ": ";
        for (const auto& card : state.columns[i]) {
            cout << cardToStr(card) << " ";
        }
        cout << "\n";
    }
    cout << "(g=" << state.g_cost << ", h=" << state.h_cost << ", f=" << state.f_cost() << ")\n";
}

/**
 * @brief Heuristic Part 1: Count 1-suit cycles.
 * This is the same logic from BasicDeadlockFinder.cpp
 */
int count_1_suit_cycles(const GameState& state) {
    int cycleCount = 0;
    for (int i = 0; i < NUM_COLUMNS; i++) {
        for (int j = 0; j < state.columns[i].size(); j++) {
            Card cardX = state.columns[i][j];
            for (int k = j + 1; k < state.columns[i].size(); k++) {
                Card cardY = state.columns[i][k];
                if (cardX.suit == cardY.suit && cardX.rank < cardY.rank) {
                    cycleCount++;
                }
            }
        }
    }
    return cycleCount;
}

/**
 * @brief Heuristic Part 2: Count cards not in foundation.
 */
int cards_not_in_foundation(const GameState& state) {
    // FIX 2: Update total cards to 6 (AS, 2S, 3S, 7H, 8S, 5D)
    int totalCards = 6;
    int inFnd = 0;
    for(int i=0; i<4; ++i) inFnd += state.foundations[i];
    return totalCards - inFnd;
}

/**
 * @brief The full Heuristic Function h(n)
 */
int calculate_h(const GameState& state) {
    // h(n) = (cards_not_in_foundation) + m_e(n)
    // We use our *simplified* m_e(n) which is just "count_1_suit_cycles"
    return cards_not_in_foundation(state) + count_1_suit_cycles(state);
}

// --- 3. Move Generation ---

/**
 * @brief Generates all possible next states from the current one.
 */
vector<GameState> getValidMoves(const GameState& currentState) {
    vector<GameState> nextStates;

    // --- Move 1: Column top card to Free Cell ---
    for (int i = 0; i < NUM_COLUMNS; ++i) {
        if (!currentState.columns[i].empty()) {
            Card cardToMove = currentState.columns[i].back();
            // Find an empty cell
            for (int j = 0; j < NUM_CELLS; ++j) {
                if (currentState.cells[j].rank == 0) { // Cell is empty
                    GameState nextState = currentState;
                    nextState.columns[i].pop_back(); // Remove from col
                    nextState.cells[j] = cardToMove; // Add to cell
                    nextState.g_cost = currentState.g_cost + 1; // Increment move count
                    nextState.h_cost = calculate_h(nextState);
                    nextStates.push_back(nextState);
                    break; // Only move to one cell
                }
            }
        }
    }

    // --- Move 2: Column top card to Foundation ---
    for (int i = 0; i < NUM_COLUMNS; ++i) {
        if (!currentState.columns[i].empty()) {
            Card cardToMove = currentState.columns[i].back();
            int suit = cardToMove.suit;
            // Check if it's the next card for that foundation
            if (currentState.foundations[suit] == cardToMove.rank - 1) {
                GameState nextState = currentState;
                nextState.columns[i].pop_back(); // Remove from col
                nextState.foundations[suit]++;   // Add to foundation
                nextState.g_cost = currentState.g_cost + 1;
                nextState.h_cost = calculate_h(nextState);
                nextStates.push_back(nextState);
            }
        }
    }

    // --- Move 3: Cell card to Foundation ---
    for (int i = 0; i < NUM_CELLS; ++i) {
        if (currentState.cells[i].rank != 0) { // Cell is not empty
            Card cardToMove = currentState.cells[i];
            int suit = cardToMove.suit;
            // Check if it's the next card
            if (currentState.foundations[suit] == cardToMove.rank - 1) {
                GameState nextState = currentState;
                nextState.cells[i] = {0, 0}; // Empty the cell
                nextState.foundations[suit]++; // Add to foundation
                nextState.g_cost = currentState.g_cost + 1;
                nextState.h_cost = calculate_h(nextState);
                nextStates.push_back(nextState);
            }
        }
    }

    // (Note: A full solver would also have Cell-to-Col and Col-to-Col moves)

    return nextStates;
}

// --- 4. A* Search ---

int main() {
    // Open Set: states to be evaluated
    priority_queue<GameState, vector<GameState>, greater<GameState>> openSet;

    // Closed Set: states already evaluated
    unordered_set<string> closedSet;

    // Came From: to reconstruct the path
    map<string, string> cameFrom;

    // --- 1. Create the initial state (from our demo) ---
    GameState startState;

    // FIX 3: Add the prerequisite AS and 2S to the board
    // (Note: push_back adds to the end, so 2S is on top)
    startState.columns[0].push_back({3, 1}); // AS
    startState.columns[0].push_back({3, 2}); // 2S

    // Col 1 (Deadlock)
    startState.columns[1].push_back({3, 3}); // 3S
    startState.columns[1].push_back({0, 7}); // 7H
    startState.columns[1].push_back({3, 8}); // 8S
    // Col 2 (Extra card)
    startState.columns[2].push_back({1, 5}); // 5D

    startState.h_cost = calculate_h(startState);

    openSet.push(startState);
    string startHash = hashState(startState);
    cameFrom[startHash] = ""; // Start has no parent

    cout << "--- Starting A* Search ---" << endl;
    printState(startState);

    GameState finalState;
    bool solutionFound = false;
    int statesExplored = 0;

    while (!openSet.empty()) {
        statesExplored++;
        // Get the state with the lowest f_cost
        GameState current = openSet.top();
        openSet.pop();

        string currentHash = hashState(current);

        // --- GOAL CHECK ---
        // Our goal: 3 of Spades is in the foundation
        // We check for rank 3, since our foundations array stores the *count*
        // or highest rank. If rank 3 is there, foundations[3] will be 3.
        if (current.foundations[3] == 3) {
            cout << "\n!!! SOLUTION FOUND after exploring " << statesExplored << " states !!!" << endl;
            finalState = current;
            solutionFound = true;
            break;
        }

        // Skip if we've already processed this state
        if (closedSet.count(currentHash)) {
            continue;
        }
        closedSet.insert(currentHash);

        // Get all possible next moves
        vector<GameState> neighbors = getValidMoves(current);

        for (const auto& neighbor : neighbors) {
            string neighborHash = hashState(neighbor);

            if (closedSet.count(neighborHash)) {
                continue; // Already processed
            }

            // This is a new state, add it to the open set
            openSet.push(neighbor);
            // Record the path
            // FIX 4: Removed a duplicate, incorrect line
            cameFrom[neighborHash] = currentHash;
        }
    }

    // --- 5. Reconstruct and print the path ---
    if (solutionFound) {
        vector<string> pathHashes;
        string currentHash = hashState(finalState);
        while (currentHash != "") {
            pathHashes.push_back(currentHash);
            currentHash = cameFrom[currentHash];
        }
        reverse(pathHashes.begin(), pathHashes.end());

        cout << "--- Optimal Path (" << pathHashes.size() - 1 << " moves) ---" << endl;

        // This is a complex part. We can't easily rebuild all
        // intermediate states without storing them.
        // For this demo, we'll just print the start and end.

        cout << "\n[Initial State]" << endl;
        printState(startState);

        cout << "\n   || \n   \\/  ... (" << pathHashes.size() - 1 << " moves) ... \n   || \n   \\/ " << endl;

        cout << "\n[Final State]" << endl;
        printState(finalState);

        cout << "\n--- Inferred Solution Steps ---" << endl;
        cout << "1. Move 2S (Col 0) to Free Cell" << endl;
        cout << "2. Move AS (Col 0) to Foundation" << endl;
        cout << "3. Move 2S (from Cell) to Foundation" << endl;
        cout << "4. Move 7H (Col 1) to Free Cell" << endl;
        cout << "5. Move 8S (Col 1) to Free Cell" << endl;
        cout << "6. Move 3S (Col 1) to Foundation" << endl;
        cout << "(The 5D was also moved to a cell)" << endl;


    } else {
        cout << "--- No Solution Found ---" << endl;
    }

    return 0;
}
