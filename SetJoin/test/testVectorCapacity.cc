#include <iostream>
#include <vector>

int main()
{
  
  std::vector<int> my_vector;
  std::cout << "Initial capacity: " << my_vector.capacity() << "\n";

  my_vector.reserve(100);
  std::cout << "Capacity after reserve(100): " << my_vector.capacity() << "\n";

  my_vector.push_back(1);
  std::cout << "Size after push_back: " << my_vector.size() << "\n";
  std::cout << "Capacity after push_back: " << my_vector.capacity() << "\n";

  
  return 0;
}