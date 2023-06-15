#include <algorithm>
#include <vector>
#include <iostream>

int main() {
    // Example data.
    std::vector<int> original_array = {5, 9, 3, 8, 2, 10, 1, 7, 4, 6};
    std::vector<int> sorted_positions = {6, 4, 2, 8, 0, 7, 3, 1, 9, 5};  // Positions of sorted elements in the original array.

    // The element we're searching for.
    int search_element = 10;

    // Find the position in the sorted positions array.
    auto it = std::lower_bound(sorted_positions.begin(), sorted_positions.end(), search_element,
                               [&original_array](int pos, int value) {
                                   return original_array[pos] < value;
                               });

    // If the element was found in the sorted positions array...
    if(it != sorted_positions.end() && original_array[*it] == search_element) {
        // Find the position in the original array.
        int original_position = *it;

        // Print the position and the element in the original array.
        std::cout << "Element " << search_element << " found at position " << original_position << " in the original array: " << original_array[original_position] << '\n';
    }
    else {
        // The element wasn't found in the sorted array.
        std::cout << "Element " << search_element << " not found.\n";
    }

    return 0;
}
