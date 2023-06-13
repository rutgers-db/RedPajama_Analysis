#include <iostream>
#include <vector>

int main()
{
  using namespace std;
  
  vector<int> normal;
  cout << normal.capacity() << endl;
  
  std::cout << "Size of pair: " << sizeof(std::pair<unsigned int, unsigned short>) << " bytes\n";

  
  return 0;
}