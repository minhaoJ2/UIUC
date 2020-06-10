# search.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Michael Abir (abir2@illinois.edu) on 08/28/2018

"""
This is the main entry point for MP1. You should only modify code
within this file -- the unrevised staff files will be used for all other
files and classes when code is run, so be careful to not modify anything else.
"""
# Search should return the path.
# The path should be a list of tuples in the form (row, col) that correspond
# to the positions of the path taken by your search algorithm.
# maze is a Maze object based on the maze from the file specified by input filename
# searchMethod is the search method specified by --method flag (bfs,dfs,astar,astar_multi,extra)
import heapq
from heapq import heappush, heappop
import copy

def search(maze, searchMethod):
    return {
        "bfs": bfs,
        "astar": astar,
        "astar_corner": astar_corner,
        "astar_multi": astar_multi,
        "extra": extra,
    }.get(searchMethod)(maze)


def bfs(maze):
    """
    Runs BFS for part 1 of the assignment.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    # TODO: Write your code here
    queue = []
    visited = []
    startPoint = [maze.getStart()]
    queue.append(startPoint)
    objective = maze.getObjectives()
    while queue != []:
        """pop(0), get the most recent path"""
        path = queue.pop(0) 
        currentPosition = path[len(path)-1]
        if currentPosition not in visited:
            visited.append(currentPosition)
            if currentPosition == objective[0]:
                return path
            for neighbors in maze.getNeighbors(currentPosition[0], currentPosition[1]):
                if neighbors not in visited:
                    """next possible steps"""
                    neighbors = [neighbors]
                    queue.append(path + neighbors)
    return []

def admissible_h(start, end):
    return abs(start[0] - end[0]) + abs(start[1] - end[1])

def astarhepler(maze, start, end):
    # Reference: https://www.youtube.com/watch?v=eSOJ3ARN5FM&t=875s A* pathfinding algorithm
    explored = []
    frontier = {}
    """store node with its f_value = h + g from distance"""
    result = []
    row, col = start
    """store the steps from starting"""
    distance = {(row, col): 0}
    """store the predecessor of this node"""
    predecessor = {} 
    # goal = maze.getObjectives()[0]
    currentPosition = (row, col)
    while currentPosition != end:
        explored.append(currentPosition)
        for neighbors in maze.getNeighbors(currentPosition[0], currentPosition[1]):
            if  neighbors in explored:
                continue
            distance[neighbors] = distance[currentPosition] + 1
            frontier[neighbors] = distance[neighbors] + admissible_h(neighbors, end)
            predecessor[neighbors] = currentPosition
        least = min(frontier, key=lambda k: frontier[k])
        currentPosition = least
        del frontier[least]
    #read the path back from target 
    while currentPosition != (row, col):
        result.append(currentPosition)
        currentPosition = predecessor[currentPosition]
    result.append((row, col))
    return result[::-1]

def astar(maze):
    """
    Runs A star for part 1 of the assignment.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    # TODO: Write your code here
    return astarhepler(maze, maze.getStart(), maze.getObjectives()[0])

def astar_corner(maze):
    """
    Runs A star for part 2 of the assignment in the case where there are four corner objectives.
        
    @param maze: The maze to execute the search on.
        
    @return path: a list of tuples containing the coordinates of each state in the computed path
        """
    # TODO: Write your code here
    objectives = maze.getObjectives()
    distance1 = []
    distance = {}
    result = []
    for i in range(len(objectives)):
        distance1.append(astarhepler(maze, maze.getStart(), objectives[i]))
    for i in range(len(objectives) - 1):
        for j in range(i+1, len(objectives)):
            distance[(i,j)] = astarhepler(maze, objectives[i], objectives[j])
            distance[(j,i)] = distance[(i, j)][::-1][1:]
            distance[(i,j)] = distance[(i,j)][1:]
            
    for i in range(len(objectives)):
        for j in range(len(objectives)):
            if i != j:
                for k in range(len(objectives)):
                    if k != i and k != j:
                        for m in range(len(objectives)):
                            if m != i and m != j and m != k:
                                result.append(distance1[i]+distance[(i,j)]+distance[(j,k)]+distance[(k,m)])
    path = result[0]
    for i in range(1, len(result)):
        if (len(result[i]) < len(path)):
            path = result[i]
    return path

def astar_multi(maze):
    """
    Runs A star for part 3 of the assignment in the case where there are
    multiple objectives.

    @param maze: The maze to execute the search on.
    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    # TODO: Write your code here
    row, col = maze.getDimensions()
    objectives = maze.getObjectives()
    distance = {}
    startingPoint = maze.getStart()
    """find the pairwise distance between objectives and store them in a dict"""
    for i in objectives:
        distance[(startingPoint, i)] = len(astarhepler(maze, startingPoint, i))
    for i in range(len(objectives) - 1):
        for j in range(i+1, len(objectives)):
            """save states explored"""
            distance[(objectives[i],objectives[j])] = len(astarhepler(maze, objectives[i], objectives[j])) - 1
            distance[(objectives[j],objectives[i])] = distance[(objectives[i],objectives[j])]
    """calculate the mst with given objectives set"""
    def getMST(objectives):
        if len(objectives) == 0:
            return 0
        edge_weight = []
        first = objectives[len(objectives) - 1]
        retval = 0
        heappush(edge_weight, (0, first))
        while len(edge_weight) != 0:
            w, obj = heappop(edge_weight)
            if obj in objectives:
                objectives.remove(obj)
                retval += w
                for objs in objectives:
                    heappush(edge_weight, (distance[(obj, objs)], objs))  
        return retval
    """get the nearest node with given current point and objectives set."""
    def nearestNode(objectives, current):
        retval = (current[0], current[1])
        minimun = 100000
        for i, j in objectives:
            distance = admissible_h(current, (i, j))
            if distance < minimun:
                minimun = distance
                retval = (i, j)
        return retval
    """define the state representation class as said on Piazza
    costSoFar is the cost of all paths from startingpoint to this this point, for comparison of heuristic
    level to track the height from the root, for tracking back to check.
    level1 is current level, level2 is for pushing to the next level. 
    explored to keep track of all explored nodes
    predecessor stores the parent of certain node, and (xcoord, ycoord, costSoFar)"""
    class State:
        def __init__(self, start, objectives, level1, level2, costSoFar):
            self.level1 = level1
            self.predecessor = {}
            for i in range(row):
                for j in range(col):
                    self.predecessor[(i, j)] = ((-1, -1), -1)
            self.start = start
            self.predecessor[(start[0],start[1])] =(start, costSoFar)
            l = copy.deepcopy(objectives)
            self.mst = getMST(l)
            self.objectives = objectives
            self.level2 = level2
            self.costSoFar = costSoFar
            """putting explored set outside of class would not work
            will need to update the explored set"""
            self.explored = {}
            for i in range (row):
                for j in range(col):
                    self.explored[(i, j)] = False
        def getMST(self):
            return self.mst
    """the new admissible heuristic in multi-dots search, as suggested on Piazza"""
    def get_heuristic(state, nextNode, current):
        return state.getMST() + admissible_h(nextNode, current)

    path = []
    costSoFar = 0
    level1 = 0
    level2 = 0
    """a priority queue to store the current distance with a tuple consisting the coordinate, level and path of this node"""
    node_queue = []
    states = {}
    states[level1] = State(startingPoint, objectives, level1, level1, costSoFar)
    heappush(node_queue, (0, (startingPoint, level1, [startingPoint])))
    """start search"""
    while len(objectives) != 0:
        currentDistance, point = heappop(node_queue)
        level1 = point[1]
        currentState = states[level1]
        objectives = currentState.objectives
        x, y = point[0]
        if currentState.explored[(x, y)] == False:
            currentState.explored[(x, y)] = True
            if (x, y) in objectives:
                level2 += 1
                """do not change the original list"""
                objectives = copy.deepcopy(objectives)
                """need to recalculate mst for each visiting objective"""
                objectives.remove((x, y))
                path = point[2]
                heappush(node_queue, (currentDistance, ((x, y), level2, path)))
                costSoFar = len(path) - 1
                """the end becomes new start"""
                states[level2] = State((x, y), objectives, level1, level2, costSoFar)
                level1 = level2
                continue
            """search the neighbors of this point"""
            neighbor = maze.getNeighbors(x, y)
            costSoFar = currentState.predecessor[(x, y)][1] + 1
            for i, j in neighbor:
                if currentState.explored[(i, j)] == False:
                    nextNode = nearestNode(objectives, (i, j))
                    if currentState.predecessor[(i, j)][1] == -1 or costSoFar < currentState.predecessor[(i, j)][1]:
                        currentState.predecessor[(i, j)] = ((x, y), costSoFar)
                    hueristic = get_heuristic(currentState, nextNode, (i, j))
                    """same reason as before, do not change the path originally stored in this point."""
                    path = copy.deepcopy(point[2])
                    path.append((i,j))
                    heappush(node_queue, (costSoFar + hueristic, ((i,j), level1, path)))
        
    """print(path)"""
    return path




def extra(maze):
    """
    Runs extra credit suggestion.

    @param maze: The maze to execute the search on.

    @return path: a list of tuples containing the coordinates of each state in the computed path
    """
    # TODO: Write your code here
    return []
