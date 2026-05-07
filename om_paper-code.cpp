#include <iostream>

using namespace std;

// Use a fixed maximum size for arrays, as requested.
// A Freecell column will rarely exceed 20, but 52 is safest.
const int MAX_CARDS_IN_COLUMN = 52;
const int NUM_COLUMNS = 8;

// --- Structures ---

/**
 * @brief Represents a single playing card.
 * Suit: 0=Hearts, 1=Diamonds, 2=Clubs, 3=Spades
 * Rank: 1=Ace, 2-10, 11=Jack, 12=Queen, 13=King
 */
struct Card {
    int suit;
    int rank;
};

/**
 * @brief Represents one of the 8 tableau columns.
 * We use a C-style array and a 'count' to track its size.
 */
struct TableauColumn {
    Card cards[MAX_CARDS_IN_COLUMN];
    int numCards;
};

/**
 * @brief Represents the entire game board state.
 */
struct GameState {
    TableauColumn columns[NUM_COLUMNS];
    // In a full game, we'd also have freecells and foundations
};

// --- Helper Functions ---

/**
 * @brief Prints a single card to the console (e.g., "5S" or "KH").
 */
void printCard(Card c) {
    // Print Rank
    if (c.rank == 1) cout << "A";
    else if (c.rank == 11) cout << "J";
    else if (c.rank == 12) cout << "Q";
    else if (c.rank == 13) cout << "K";
    else cout << c.rank;

    // Print Suit
    if (c.suit == 0) cout << "H";
    else if (c.suit == 1) cout << "D";
    else if (c.suit == 2) cout << "C";
    else if (c.suit == 3) cout << "S";

    // Add spacing
    cout << " ";
}

/**
 * @brief Prints the entire game board state to the console.
 */
void printState(GameState state) {
    cout << "--- Game Board State ---" << endl;
    for (int i = 0; i < NUM_COLUMNS; i++) {
        cout << "Col " << i << ": ";
        for (int j = 0; j < state.columns[i].numCards; j++) {
            printCard(state.columns[i].cards[j]);
        }
        cout << endl;
    }
    cout << "------------------------" << endl;
}

// --- Core Logic from the Paper ---

/**
 * @brief Identifies and prints 1-suit cycles, as described in the paper.
 * This function follows the logic from Figure 4 (Appendix B).
 *
 * A 1-suit cycle (deadlock) occurs when:
 * 1. Two cards are in the same column.
 * 2. They have the same suit.
 * 3. The card "higher up" (cardX, lower index) has a LOWER rank
 * than the card "lower down" (cardY, higher index).
 *
 * Example: [3S ... 8S]. The 3S is blocked by the 8S.
 * The 8S cannot move to the foundation until the 3S is played.
 * The 3S cannot be played until the 8S is moved. This is a cycle.
 */
void identify_1_suit_cycles(GameState state) {
    cout << "\n--- Identifying 1-Suit Cycles ---" << endl;
    int cycleCount = 0;

    // for (all tableau columns, c)
    for (int i = 0; i < NUM_COLUMNS; i++) {
        TableauColumn c = state.columns[i];

        // for (all cards, cardX, in column c)
        for (int j = 0; j < c.numCards; j++) {
            Card cardX = c.cards[j];

            // for (all cards, cardY, below cardX)
            for (int k = j + 1; k < c.numCards; k++) {
                Card cardY = c.cards[k];

                // This is the core logic for finding a cycle:
                // Check if they are the same suit AND
                // if the higher card (X) has a lower rank than the lower card (Y).
                if (cardX.suit == cardY.suit && cardX.rank < cardY.rank) {
                    // Cycle found!
                    cycleCount++;
                    cout << "Cycle " << cycleCount << " found in Column " << i << ": ";
                    printCard(cardX);
                    cout << "(at index " << j << ") is blocked by ";
                    printCard(cardY);
                    cout << "(at index " << k << ")." << endl;
                }
            }
        }
    }

    if (cycleCount == 0) {
        cout << "No 1-suit cycles found." << endl;
    }
    cout << "---------------------------------" << endl;
}

// --- Main Function ---

int main() {
    // 1. Create a sample game state
    GameState myGame;

    // Initialize all columns to be empty
    for (int i = 0; i < NUM_COLUMNS; i++) {
        myGame.columns[i].numCards = 0;
    }

    // --- Setup Column 0 (NO cycle) ---
    // This is a valid sequence
    myGame.columns[0].cards[0] = {3, 8}; // 8S
    myGame.columns[0].cards[1] = {0, 7}; // 7H
    myGame.columns[0].cards[2] = {3, 3}; // 3S
    myGame.columns[0].numCards = 3;

    // --- Setup Column 1 (Contains a 1-suit cycle) ---
    // The 3S is "higher" than the 8S. This is a deadlock.
    myGame.columns[1].cards[0] = {3, 3}; // 3S
    myGame.columns[1].cards[1] = {0, 7}; // 7H
    myGame.columns[1].cards[2] = {3, 8}; // 8S
    myGame.columns[1].numCards = 3;

    // --- Setup Column 2 (Contains multiple cycles) ---
    // 5D is blocked by 9D
    // 5D is also blocked by 6D
    // 9D is not blocked by 6D (ranks are wrong way)
    myGame.columns[2].cards[0] = {1, 5}; // 5D
    myGame.columns[2].cards[1] = {2, 11}; // JC
    myGame.columns[2].cards[2] = {1, 9}; // 9D
    myGame.columns[2].cards[3] = {1, 6}; // 6D (this is a 2nd deadlock with 9D)
    myGame.columns[2].numCards = 4;


    // 2. Print the initial state
    printState(myGame);

    // 3. Run the cycle identification
    identify_1_suit_cycles(myGame);

    return 0;
}
