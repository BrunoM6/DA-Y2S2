# FEUP - Algorithm Design - 2023/2024
> Curricular Unit: DA - [Desenho de Algoritmos](https://sigarra.up.pt/feup/pt/ucurr_geral.ficha_uc_view?pv_ocorrencia_id=520321)

## 2nd Year - 2nd Semester - 2nd Group Project

### Brief description:

The goal of this project is to design efficient algorithms to find optimal routes for vehicles in generic shipping and delivery scenarios, from urban deliveries to ocean shipping. 
The problem it tackles corresponds to the Traveling Salesperson Problem (TSP). As the TSP is intractable, there are no known efficient algorithms to solve it.
Therefore, the project will have the backtracking algorithm to find optimal solution for miniscule graphs and focus on designing and implementing heuristic algorithms to find good solutions for the TSP of big graphs.


### Developed by:

1. Bruno Moreira - E-mail: up202107143@edu.fe.up.pt
2. Guilherme Silva - E-mail: up202205298@edu.fe.up.pt
3. Valentina Cadime - E-mail: up202206262@edu.fe.up.pt

### Notes:
The **Brute-Force** approach has a factorial complexity level, it grows in operations at an absurd rate when we increase the amount of locations in the **Travelling Salesperson Problem**, since we need to compute all Hamiltonian paths. 

It’s completely unfeasible for solving this problem for any dataset that isn’t miniscule. For it’s complexity, it does guarantee the best result, that is if the computer is able to calculate it in a realistic amount of time:
 - Tested it with the smallest dataset of them all, tourism.csv, with 4 locations. It was so fast it approximated to 0 milliseconds. 
 - With stadiums.csv, 9 locations, it rapidly increased to 37181 ms. 
 - With any other larger dataset, it rapidly increases the time it takes and doesn’t finish in a practical timeframe. 
This approach was just used as a starting point and term of comparison moving forward, it also justifies the usage of heuristics to solve this problem.
-- --
We can use the **Backtracking approach** as an improvement upon the **Brute-Force approach** that still allows us to reach a solution. 
It does have lower time complexity but it is still absurd for a regular computer at **O(n^2*2^n)**.

Reaching the actual result for the TSP is impractical for larger and larger datasets, but we can use certain algorithms to reach a lower bound of the value. 
That is, a value that we know is, for any graph, smaller than or equal to value of the solution, it serves as a good comparison point to the solutions we reach to allow us to compare our heuristics having into consideration a hard bottom. 
To compute this bottom, we get the **Minimum Spanning Tree** for the graph without one vertex and connect said vertex to the graph through its two edges with the least weight, and get the sum of the weights. This is called a 1-tree, because it has 1 cycle and is a lower bound of the TSP cost.
-- --
The **Nearest Neighbour Heuristic** presently created serves to evaluate the usage of a greedy approach to solving the problem. 
It is similar to Dijkstra’s Algorithm in its approach, since it picks the edge to the next location as the minimum weighted edge to a location that hasn’t been visited, and finishes by connecting the last edge reached to the starting edge. 
It guarantees the smallest path up to the last vertex, but doesn’t guarantee that the choice of path made will also yield an ideal result for the last connection to close the cycle. 
It has O(n^2) complexity, very efficient, and its solutions are a large improvement on just blinding picking an Hamiltonian path. It proves itself quick and capable, even in very large datasets, and it's results, while not extremely accurate, are still relatively close to the results reached in the 1-Tree, never more than 2 times bigger.