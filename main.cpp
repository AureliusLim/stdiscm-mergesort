#include <cstdlib> // For std::srand and std::rand
#include <ctime>   // For std::time
#include <iostream>
#include <utility>
#include <vector>
#include <chrono>
#include <tuple>
#include <thread>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <random>
using namespace std;

typedef tuple<int, int, bool> ii;

/*
This function generates all the intervals for merge sort iteratively, given the
range of indices to sort. Algorithm runs in O(n).

Parameters:
start : int - start of range
end : int - end of range (inclusive)

Returns a list of integer pairs indicating the ranges for merge sort.
*/
vector<ii> generate_intervals(int start, int end);

/*
This function performs the merge operation of merge sort.

Parameters:
array : vector<int> - array to sort
s     : int         - start index of merge
e     : int         - end index (inclusive) of merge
*/
void merge(vector<int> &array, int s, int e);

// // TODO: Call merge on each interval in sequence
void threadMerge(std::vector<int>& array, std::vector<std::tuple<int, int, bool>>& intervals, int start, int end,int thread_num) {
  bool prereqs;
  int tasks=end-start;
  while(tasks>0){
    for (auto it = intervals.begin() + start; it != intervals.begin() + end; ++it) {
      if(!(std::get<2>(*it))){
        prereqs = true;
        if (std::get<0>(*it) != std::get<1>(*it)){
          int midpoint = (std::get<0>(*it) + std::get<1>(*it)) / 2;

          for(const auto& interval : intervals){
            if((std::get<0>(interval) == std::get<0>(*it) && std::get<1>(interval) == floor(midpoint))){
                if(std::get<2>(interval) == false){
                  prereqs = false;
                  break;
                }       
            }

            if((std::get<0>(interval) == floor(midpoint + 1) && std::get<1>(interval) == std::get<1>(*it))){        
                if(std::get<2>(interval) == false){
                  prereqs = false;
                  break;
                }       
            }
          } 
        }           

        if (!std::get<2>(*it) && prereqs) {
            merge(array, std::get<0>(*it), std::get<1>(*it));
            std::get<2>(*it) = true; 
            tasks--;
        }
      }
    }
  }
}


int main() {

  // TODO: Get array size and thread count from user
  int array_size, thread_count;
  bool sorted = true;
  cout << "Enter array size: ";
  cin >> array_size;
  cout << "Enter thread count: ";
  cin >> thread_count;

  // TODO: Generate a random array of given size
  vector<int> array(array_size);
  
  std::random_device rd;
  std::mt19937 g(rd());
  g.seed(42);
  for (int i = 0; i < array_size; ++i) {
    array[i] = i + 1;
  }
  std::shuffle(array.begin(), array.end(), g);

  // TODO: Call the generate_intervals method to generate the merge sequence
  vector<ii> intervals = generate_intervals(0, array_size - 1);
  vector<thread> threads;
  auto start_time = std::chrono::high_resolution_clock::now();

  int interval_size = intervals.size() / thread_count;

  for (int i = 0; i < thread_count; ++i) {
      int start = i * interval_size;
      int end = (i == thread_count - 1) ? intervals.size() : (i + 1) * interval_size;
      threads.emplace_back(threadMerge, std::ref(array), std::ref(intervals), start, end,i);
  }

  for (auto& thread : threads) {
      thread.join();
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  std::cout << "Runtime: " << duration.count() << std::endl;
  
  for (int i = 0; i < array.size() - 1; ++i) {
      if (array[i] > array[i + 1]) {
          std::cout << "Not Sorted.\n";
          sorted = false;
          break;
      }
  }
  if (sorted == true){
    std::cout << "Sorted.\n";
  }
  return 0;
}

vector<ii> generate_intervals(int start, int end) {
  vector<ii> frontier;
  frontier.push_back(ii(start, end, false));
  int i = 0;
  while (i < (int)frontier.size()) {
    int s = get<0>(frontier[i]);
    int e = get<1>(frontier[i]);
    bool flag = get<2>(frontier[i]);

    i++;

    // if base case
    if (s == e) {
      continue;
    }

    // compute midpoint
    int m = s + (e - s) / 2;

    // add prerequisite intervals
    frontier.push_back(ii(m + 1, e, flag));
    frontier.push_back(ii(s, m, flag));
  }

  vector<ii> retval;
  for (int i = (int)frontier.size() - 1; i >= 0; i--) {
    retval.push_back(frontier[i]);
  }
  return retval;
}

void merge(vector<int> &array, int s, int e) {
  int m = s + (e - s) / 2;
  vector<int> left;
  vector<int> right;
  for (int i = s; i <= e; i++) {
    if (i <= m) {
      // cout << i << endl;
      left.push_back(array[i]);
    } else {
      // cout << i << endl;
      right.push_back(array[i]);
    }
  }

  int l_ptr = 0, r_ptr = 0;

  for (int i = s; i <= e; i++) {
    if (l_ptr != (int)left.size() && (r_ptr == (int)right.size() || left[l_ptr] <= right[r_ptr])) {
      array[i] = left[l_ptr];
      l_ptr++;
    } else {
      array[i] = right[r_ptr];
      r_ptr++;
    }
  }
} 