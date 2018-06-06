import math
from random import randint

def main(bot):
  scannedRect = scanRect(bot, 315, 21)
  safeSpot = findSafeSpot(scannedRect)
  path = pathToSafeSpot(scannedRect, safeSpot)
  goToSafeSpot(bot, path)

def goToSafeSpot(bot, path):
  x = path[0].x
  y = path[0].y
  currentRotation = 0
  for cell in path:
    if cell.x < x:
      bot.rotate(- 90 - currentRotation)
      currentRotation = -90
      stepForward(bot)
      x -= 1
    elif cell.x > x:
      bot.rotate(90 - currentRotation)
      currentRotation = 90
      stepForward(bot)
      x += 1
    elif cell.y < y:
      stepForward(bot)
      y -= 1
    elif cell.y > y:
      bot.rotate(180 - currentRotation)
      currentRotation = 180
      stepForward(bot)
      y += 1

def findSafeSpot(grid):
  gridSize = len(grid)
  iMax = 0
  jMax = 0
  countMax = 0
  for j in range(1, gridSize - 1):
    for i in range(1, gridSize - 1):
      if grid[i][j] == True:
	continue
      count = 0
      for deltaI in range(-1, 2, 1):
	for deltaJ in range(-1, 2, 1):
	  if grid[i + deltaI][j + deltaJ] == True:
	    count += 1
      if count > countMax:
	countMax = count
	iMax = i
	jMax = j
  return (iMax, jMax)

def stepForward(bot):
  for i in range(7):
    bot.move()

# Scan walls in a rectangular area around the bot. 'step' must divide 'dist'.
def scanRect(bot, dist, step):
  theta = 0
  gridSize = 2 * dist / step + 1
  scannedRect = [[False for x in range(gridSize)] for x in range(gridSize)]
  for j in range(gridSize):
    for i in range(gridSize):
      I = (i - gridSize / 2) * step
      J = (- j + gridSize / 2) * step
      lastTheta = theta
      if J != 0:
	theta = math.atan(float(I) / float(J)) / 2
      elif I < 0:
	theta = - math.pi / 4
      elif I > 0:
	theta = math.pi / 4
      else:
	continue
      if J < 0:
	theta += math.pi / 2
      bot.rotate(radToDeg(theta) - radToDeg(lastTheta))
      h = math.sqrt(I * I + J * J)
      if bot.scan(h, 11, True, False, False)[0] > 0:
	scannedRect[i][j] = True
  bot.rotate(-radToDeg(theta))
  return scannedRect

def radToDeg(angle):
  return int(angle * 360 / math.pi)

def pathToSafeSpot(grid, destination):
  a = AStar(len(grid))
  a.init_grid(grid, destination)
  return a.process()

import heapq

class Cell(object):
    def __init__(self, x, y, reachable):
        """
        Initialize new cell

        @param x cell x coordinate
        @param y cell y coordinate
        @param reachable is cell reachable? not a wall?
        """
        self.reachable = reachable
        self.x = x
        self.y = y
        self.parent = None
        self.g = 0
        self.h = 0
        self.f = 0

class AStar(object):
    def __init__(self, gridSize):
        self.opened = []
        heapq.heapify(self.opened)
        self.closed = set()
        self.cells = []
        self.grid_height = gridSize
        self.grid_width = gridSize

    def init_grid(self, grid, destination):
        for x in range(self.grid_width):
            for y in range(self.grid_height):
                reachable = not grid[x][y]
                self.cells.append(Cell(x, y, reachable))
        self.start = self.get_cell(self.grid_width / 2, self.grid_height / 2)
        self.end = self.get_cell(destination[0], destination[1])

    def get_heuristic(self, cell):
        """
        Compute the heuristic value H for a cell: distance between
        this cell and the ending cell multiply by 10.

        @param cell
        @returns heuristic value H
        """
        return 10 * (abs(cell.x - self.end.x) + abs(cell.y - self.end.y))

    def get_cell(self, x, y):
        """
        Returns a cell from the cells list

        @param x cell x coordinate
        @param y cell y coordinate
        @returns cell
        """
        return self.cells[x * self.grid_height + y]

    def get_adjacent_cells(self, cell):
        """
        Returns adjacent cells to a cell. Clockwise starting
        from the one on the right.

        @param cell get adjacent cells for this cell
        @returns adjacent cells list 
        """
        cells = []
        if cell.x < self.grid_width-1:
            cells.append(self.get_cell(cell.x+1, cell.y))
        if cell.y > 0:
            cells.append(self.get_cell(cell.x, cell.y-1))
        if cell.x > 0:
            cells.append(self.get_cell(cell.x-1, cell.y))
        if cell.y < self.grid_height-1:
            cells.append(self.get_cell(cell.x, cell.y+1))
        return cells

    def reconstruct_path(self):
	path = []
        cell = self.end
        while cell.parent is not self.start:
	    path.insert(0, cell)
            cell = cell.parent
        path.insert(0, self.start)
        return path

    def compare(self, cell1, cell2):
        """
        Compare 2 cells F values

        @param cell1 1st cell
        @param cell2 2nd cell
        @returns -1, 0 or 1 if lower, equal or greater
        """
        if cell1.f < cell2.f:
            return -1
        elif cell1.f > cell2.f:
            return 1
        return 0
    
    def update_cell(self, adj, cell):
        """
        Update adjacent cell

        @param adj adjacent cell to current cell
        @param cell current cell being processed
        """
        adj.g = cell.g + 10
        adj.h = self.get_heuristic(adj)
        adj.parent = cell
        adj.f = adj.h + adj.g

    def process(self):
        # add starting cell to open heap queue
        heapq.heappush(self.opened, (self.start.f, self.start))
        while len(self.opened):
            # pop cell from heap queue 
            f, cell = heapq.heappop(self.opened)
            # add cell to closed list so we don't process it twice
            self.closed.add(cell)
            # if ending cell, display found path
            if cell is self.end:
                return self.reconstruct_path()
            # get adjacent cells for cell
            adj_cells = self.get_adjacent_cells(cell)
            for adj_cell in adj_cells:
                if adj_cell.reachable and adj_cell not in self.closed:
                    if (adj_cell.f, adj_cell) in self.opened:
                        # if adj cell in open list, check if current path is
                        # better than the one previously found
                        # for this adj cell.
                        if adj_cell.g > cell.g + 10:
                            self.update_cell(adj_cell, cell)
                    else:
                        self.update_cell(adj_cell, cell)
                        # add adj cell to open list
                        heapq.heappush(self.opened, (adj_cell.f, adj_cell))
