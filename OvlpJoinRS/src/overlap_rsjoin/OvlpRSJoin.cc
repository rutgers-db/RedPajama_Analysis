#include <bits/stdc++.h>
#include "OvlpRSJoin.h"
// #include "Combination1.h"
#include "Combination2.h"
using namespace std;

struct combination;

int c; // threshold
unsigned short total_eles; // the number of tokens (or the size of vocabulary)
// vector<pair<int, int>> buck; // counter
// vector<int> bucket;

// int n1, n2; // R S sizes
vector<vector<unsigned short>> dataset1, dataset2; // datasets
// vector<vector<int>> records1, records2; // two sets
// vector<pair<int, int>> idmap1, idmap2; // id mapping
vector<vector<pair<int, int>>> ele_lists1, ele_lists2; // inverted lists
vector<int> heap1, heap2; // heap
vector<combination1> combs1;
vector<combination2> combs2; // comb

int64_t result_num;
int64_t candidate_num;

bool comp_int(const int a, const int b);
bool comp_pair(const pair<int, int> &p1, const int val);
bool comp_comb1(const int a, const int b);
bool comp_comb2(const int a, const int b);
bool is_equal(const combination1 & c1, const combination1 & c2);
bool is_equal(const combination2 & c1, const combination2 & c2);
char compare(const combination1 & c1, const combination2 & c2);

// build heap for combination1
bool build_heap(const vector<pair<int,int>> &vec, const vector<vector<unsigned short>> &dataset,
    int L, vector<int> &heap, vector<combination1> &combs, int &heap_size) {

  // get the number of large sets in the inverted list
  // int size = distance(vec.begin(), lower_bound(vec.begin(), vec.end(), L, comp_pair));
  // cause they are all treat as small sets
  int size = 0;

  // return false if there is no small set
  if (vec.size() < size + 1) return false;  

  // initialize heap and combinations
  heap.clear();
  combs.clear();
  for (auto i = size; i < vec.size(); i++) {
    // remove if there are not enough (>= c) elements left
    if ((int)(dataset[vec[i].first].size()) - 1 - vec[i].second < c) continue;
    heap.push_back(heap_size++);
    combs.push_back(combination1(vec[i].first, vec[i].second));
  }

  if (heap_size == 0) return false;

  make_heap(heap.begin(), heap.end(), comp_comb1);

  return true;
}

// build heap for combination1
bool build_heap(const vector<pair<int,int>> &vec, const vector<vector<unsigned short>> &dataset,
    int L, vector<int> &heap, vector<combination2> &combs, int &heap_size) {

  // get the number of large sets in the inverted list
  // int size = distance(vec.begin(), lower_bound(vec.begin(), vec.end(), L, comp_pair));
  // cause they are all treat as small sets
  int size = 0;

  // return false if there is no small set
  if (vec.size() < size + 1) return false;  

  // initialize heap and combinations
  heap.clear();
  combs.clear();
  for (auto i = size; i < vec.size(); i++) {
    // remove if there are not enough (>= c) elements left
    if ((int)(dataset[vec[i].first].size()) - 1 - vec[i].second < c) continue;
    heap.push_back(heap_size++);
    combs.push_back(combination2(vec[i].first, vec[i].second));
  }

  if (heap_size == 0) return false;

  make_heap(heap.begin(), heap.end(), comp_comb2);
  
  return true;
}

void OvlpRSJoin::small_case(int L1, int R1, int L2, int R2) {
  if (L1 >= R1) return;
  if (L2 >= R2) return;
  --c;

  timeval beg, mid, mid1, end;
  gettimeofday(&beg, NULL);

  cout << " number of small sets: " << R1 - L1 << " and " << R2 - L2 << endl;

  vector<vector<int>> res_lists1;
  vector<vector<int>> res_lists2;

  gettimeofday(&mid, NULL);

  for (auto idx = total_eles - 1; idx >= 0; idx--) {

    int heap_size1 = 0;
    int heap_size2 = 0;

    if (!build_heap(ele_lists1[idx], dataset1, L1, heap1, combs1, heap_size1)) continue;
    if (!build_heap(ele_lists2[idx], dataset2, L2, heap2, combs2, heap_size2)) continue;
    // heap_cost += (3 * c * (heap_size1 + heap_size2));
    // cout << heap_size << " initial: " << heap_cost << endl;

    bool pop1 = true;
    bool pop2 = true;

    // pop heaps
    vector<int> inv_list1;
    vector<int> inv_list2;
    do {
      if (pop1) pop_heap(heap1.begin(), heap1.begin() + heap_size1--, comp_comb1);
      else  pop1 = true;
      if (pop2) pop_heap(heap2.begin(), heap2.begin() + heap_size2--, comp_comb2);
      else  pop2 = true;

      switch (compare(combs1[heap1[heap_size1]], combs2[heap2[heap_size2]])) {
        case 1 :
          combs1[heap1[heap_size1]].binary(combs2[heap2[heap_size2]]);
          if (combs1[heap1[heap_size1]].completed) {
            heap1.pop_back();
          } else {
            push_heap(heap1.begin(), heap1.end(), comp_comb1);
            ++heap_size1;
          }
          pop2 = false;
          break;

        case -1 :
          combs2[heap2[heap_size2]].binary(combs1[heap1[heap_size1]]);
          if (combs2[heap2[heap_size2]].completed) {
            heap2.pop_back();
          } else {
            push_heap(heap2.begin(), heap2.end(), comp_comb2);
            ++heap_size2;
          }
          pop1 = false;
          break;

        case 0 :
          inv_list1.clear();
          inv_list2.clear();

          inv_list1.push_back(combs1[heap1[heap_size1]].id);

          while (heap_size1 > 0 && is_equal(combs1[heap1[heap_size1]], combs1[heap1.front()])) {
            inv_list1.push_back(combs1[heap1.front()].id);
            pop_heap(heap1.begin(), heap1.begin() + heap_size1, comp_comb1);
            --heap_size1;
          } 

          inv_list2.push_back(combs2[heap2[heap_size2]].id);

          while (heap_size2 > 0 && is_equal(combs2[heap2[heap_size2]], combs2[heap2.front()])) {
            inv_list2.push_back(combs2[heap2.front()].id);
            pop_heap(heap2.begin(), heap2.begin() + heap_size2, comp_comb2);
            --heap_size2;
          } 

          res_lists1.push_back(std::move(inv_list1));
          res_lists2.push_back(std::move(inv_list2));

          if (heap_size1 == 0 && heap_size2 == 0) break;

          for (auto i = heap_size1; i < heap1.size(); ++i) {
            combs1[heap1[i]].binary(combs2[heap2.front()]);
            // binary_cost += (c * log2(dataset2[combs2[heap2.front()].id].size()));
          }

          for (auto i = heap_size2; i < heap2.size(); ++i) {
            combs2[heap2[i]].binary(combs1[heap1.front()]);
            // binary_cost += (c * log2(dataset1[combs1[heap1.front()].id].size()));
          }

          int comp_num1 = 0;
          for (auto i = heap_size1; i < heap1.size(); ++i) {
            if (combs1[heap1[i]].completed)
              ++comp_num1;
            else if (comp_num1 > 0)
              heap1[i - comp_num1] = heap1[i];
          }

          int comp_num2 = 0;
          for (auto i = heap_size2; i < heap2.size(); ++i) {
            if (combs2[heap2[i]].completed)
              ++comp_num2;
            else if (comp_num2 > 0)
              heap2[i - comp_num2] = heap2[i];
          }

          for (auto i = heap_size1; i < (int)heap1.size() - comp_num1; i++) {
            push_heap(heap1.begin(), heap1.begin() + i + 1, comp_comb1);
          }

          for (auto i = heap_size2; i < (int)heap2.size() - comp_num2; i++) {
            push_heap(heap2.begin(), heap2.begin() + i + 1, comp_comb2);
          }

          while (comp_num1-- > 0)
            heap1.pop_back();
          heap_size1 = heap1.size();

          while (comp_num2-- > 0)
            heap2.pop_back();
          heap_size2 = heap2.size();
          break;
      }
    } while (heap_size1 > 0 && heap_size2 > 0);
  }

  cout << "Res lists num: " << res_lists1.size() << " " << res_lists2.size() << endl;

  gettimeofday(&mid1, NULL);

  vector<vector<int>> id_lists(n1);
  for (auto i = 0; i < res_lists1.size(); i++) {
    for (auto j = 0; j < res_lists1[i].size(); j++)
      id_lists[res_lists1[i][j]].push_back(i);
  }

  vector<int> results(n2, -1);
  for (auto i = n1 - 1; i >= 0; i--) {
    if (id_lists[i].empty()) continue;
    for (auto j = 0; j < id_lists[i].size(); j++) {
      for (auto k = 0; k < res_lists2[id_lists[i][j]].size(); k++) {
        if (results[res_lists2[id_lists[i][j]][k]] != i) {
          // cout << idmap[i].first << " " << idmap[res_lists2[id_lists[i][j]][k]].first << endl;
          result_pairs.emplace_back(idmap_records1[i].first, idmap_records2[res_lists2[id_lists[i][j]][k]].first);
          results[res_lists2[id_lists[i][j]][k]] = i;
          ++result_num;
        }
      }
    }
  }
  ++c;
  gettimeofday(&end, NULL);
  cout << " small p1 : " << mid.tv_sec - beg.tv_sec + (mid.tv_usec - beg.tv_usec) / 1e6 << endl;
  cout << " small p2 : " << mid1.tv_sec - mid.tv_sec + (mid1.tv_usec - mid.tv_usec) / 1e6 << endl;
  cout << " small p3 : " << end.tv_sec - mid1.tv_sec + (end.tv_usec - mid1.tv_usec) / 1e6 << endl;
}

void removeShort(const vector<vector<unsigned short>> &records, unordered_map<unsigned short, vector<int>> &ele) {
  for (int i = 0; i < records.size(); i++) {
    if (records[i].size() < c) continue;
    for (unsigned short j = 0; j < records[i].size(); j++)
      ele[records[i][j]].push_back(i);
  }
}

// Remove "widows" from a hash map based on another hash map.
// This function removes key-value pairs from the unordered_map 'ele' 
// if the key doesn't exist in another unordered_map 'ele_other'.
void removeWidow(unordered_map<unsigned short, vector<int>> &ele, const unordered_map<unsigned short, vector<int>> &ele_other) {
  
  // Start an iterator at the beginning of 'ele'
  auto eit = ele.begin();
  
  // Iterate over the entire 'ele' unordered_map
  while (eit != ele.end()) {
    
    // If the current key doesn't exist in 'ele_other'
    if (ele_other.find(eit->first) == ele_other.end())
      
      // If the key is not found in 'ele_other', erase it from 'ele'
      eit = ele.erase(eit);
    else
      // If the key is found in 'ele_other', continue to the next key-value pair in 'ele'
      ++eit;
  }
}


void transform(unordered_map<unsigned short, vector<int>> &ele, const vector<pair<int, int>> &eles, 
               vector<pair<int, int>> &idmap, vector<vector<pair<int, int>>> &ele_lists,
               vector<vector<unsigned short>> &dataset, const unsigned short total_eles, const int n) {

  dataset.resize(n);

  // the numbers in dataset is from large to small
  // the frequency in dataset is from small to large
  for (unsigned short i = 0; i < eles.size(); ++i) {
    for (auto j = ele[eles[i].first].begin(); j != ele[eles[i].first].end(); j++)
      dataset[*j].push_back(total_eles - i - 1);
  }

  for (auto i = 0; i < n; i++)
    if (dataset[i].size() < c) dataset[i].clear();

  for (auto i = 0; i < n; i++)
    idmap.push_back(make_pair(i, dataset[i].size()));

  sort(idmap.begin(), idmap.end(), [] (const pair<int, int> & a, const pair<int, int> & b) {
      return a.second > b.second;
  });
  sort(dataset.begin(), dataset.end(), [] (const vector<unsigned short>& a, const vector<unsigned short>& b) {
      return a.size() > b.size();
  });
  cout << " largest set: " << dataset.front().size() << " smallest set: " << dataset.back().size() << endl;

  // build index
  ele_lists.resize(total_eles);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < dataset[i].size(); j++)
      ele_lists[dataset[i][j]].push_back(make_pair(i,j));
}

void OvlpRSJoin::overlapjoin(int overlap_threshold){

  srand(time(NULL));
  
  timeval starting, ending, s1, t1, s2, t2;
  timeval time1, time2, time3, time4;

  gettimeofday(&starting, NULL);

  c = overlap_threshold;           // get threshold
  n1 = records1.size();
  n2 = records2.size();
  
  unordered_map<unsigned short, vector<int>> ele1;
  unordered_map<unsigned short, vector<int>> ele2;

  gettimeofday(&time1, NULL);
  removeShort(records1, ele1);
  removeShort(records2, ele2);

  // removeWidow(ele1, ele2); we dont need to remove widow in ele1 cause they all from dataset2
  removeWidow(ele2, ele1);

  vector<pair<int, int>> eles;
  for (auto it = ele1.begin(); it != ele1.end(); it++)
    eles.push_back(make_pair(it->first, it->second.size() + ele2[it->first].size()));

  // frequency increasing order
  sort(eles.begin(), eles.end(), [](const pair<int, int> &p1, const pair<int, int> &p2) {
    return p1.second < p2.second;
  });

  // the number of elements (tokens) cannot exceed the maximum of unsigned short
  assert(eles.size() <= 65535 );
  total_eles = eles.size();

  transform(ele1, eles, idmap_records1, ele_lists1, dataset1, total_eles, n1);
  transform(ele2, eles, idmap_records2, ele_lists2, dataset2, total_eles, n2);

  gettimeofday(&time3, NULL);
  cout << "Transform Time: " << time3.tv_sec - time1.tv_sec + (time3.tv_usec - time1.tv_usec) / 1e6 << endl;

  gettimeofday(&time4, NULL);

  // ****** conduct joining ******  
  result_num = 0;
  candidate_num= 0;

  gettimeofday(&s1, NULL);

  gettimeofday(&s2, NULL);
  small_case(0, n1, 0, n2);
  gettimeofday(&t2, NULL);

  gettimeofday(&ending, NULL);
  cout << "Join Time: " << ending.tv_sec - time4.tv_sec + (ending.tv_usec - time4.tv_usec) / 1e6 << endl;
  cout << "  small Time: " << t2.tv_sec - s2.tv_sec + (t2.tv_usec - s2.tv_usec) / 1e6 << endl;
  cout << "All Time: " << ending.tv_sec - starting.tv_sec + (ending.tv_usec - starting.tv_usec) / 1e6 << endl;
  cout << "Result Num: " << result_num << endl;
}

bool comp_pair(const pair<int, int> &p1, const int val) {   // binary search inverted list
  return p1.first < val;
}

bool comp_int(const int a, const int b) {
  return a > b;
}

bool comp_comb1(const int a, const int b) {
  auto & c1 = combs1[a];
  auto & c2 = combs1[b];
  for (unsigned short i = 0; i < c; i++) {
    if (dataset1[c1.id][c1.curr[i]] > dataset1[c2.id][c2.curr[i]])
      return false;
    else if (dataset1[c1.id][c1.curr[i]] < dataset1[c2.id][c2.curr[i]])
      return true;
  }
  return c1.id > c2.id;
}

bool comp_comb2(const int a, const int b) {
  auto & c1 = combs2[a];
  auto & c2 = combs2[b];
  for (unsigned short i = 0; i < c; i++) {
    if (dataset2[c1.id][c1.curr[i]] > dataset2[c2.id][c2.curr[i]])
      return false;
    else if (dataset2[c1.id][c1.curr[i]] < dataset2[c2.id][c2.curr[i]])
      return true;
  }
  return c1.id > c2.id;
}

char compare(const combination1 & c1, const combination2 & c2) {
  for (int i = 0; i < c; i++) {
    if (dataset1[c1.id][c1.curr[i]] > dataset2[c2.id][c2.curr[i]])
      return 1;
    else if (dataset1[c1.id][c1.curr[i]] < dataset2[c2.id][c2.curr[i]])
      return -1;
  }
  return 0;
}

bool is_equal(const combination1 & c1, const combination1 & c2) {
  for (int i = 0; i < c; i++) {
    if (dataset1[c1.id][c1.curr[i]] != dataset1[c2.id][c2.curr[i]])
      return false;
  }
  return true;
}

bool is_equal(const combination2 & c1, const combination2 & c2) {
  for (int i = 0; i < c; i++) {
    if (dataset2[c1.id][c1.curr[i]] != dataset2[c2.id][c2.curr[i]])
      return false;
  }
  return true;
}