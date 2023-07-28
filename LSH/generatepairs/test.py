import time
import math
from multiprocessing import Pool, cpu_count

# This is our CPU-bound task
def factorial(n):
    return math.factorial(n)

def test_factorial_with_processes(num_processes):
    # The number we'll be calculating the factorial of
    n = 50000

    # Repeat the task multiple times
    tasks = [n]*150

    # Create a pool of processes
    with Pool(processes=num_processes) as pool:
        # Start the timer
        start_time = time.time()

        # Use the pool to execute the tasks concurrently
        pool.map(factorial, tasks)

        # Stop the timer and print the time taken
        end_time = time.time()
        print(f"Time taken with {num_processes} processes: {end_time - start_time} seconds")

if __name__ == "__main__":
    # Test with 1 to cpu_count() processes
    for num_processes in range(1, cpu_count() + 1):
        test_factorial_with_processes(num_processes)
