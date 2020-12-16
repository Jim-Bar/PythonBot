import heapq
import math


def main(bot):
    scanned_rect = scan_rect(bot, 315, 21)
    safe_spot = find_safe_spot(scanned_rect)
    path = path_to_safe_spot(scanned_rect, safe_spot)
    go_to_safe_spot(bot, path)


def go_to_safe_spot(bot, path):
    x = path[0].x
    y = path[0].y
    current_rotation = 0
    for cell in path:
        if cell.x < x:
            bot.rotate(- 90 - current_rotation)
            current_rotation = -90
            step_forward(bot)
            x -= 1
        elif cell.x > x:
            bot.rotate(90 - current_rotation)
            current_rotation = 90
            step_forward(bot)
            x += 1
        elif cell.y < y:
            step_forward(bot)
            y -= 1
        elif cell.y > y:
            bot.rotate(180 - current_rotation)
            current_rotation = 180
            step_forward(bot)
            y += 1


def find_safe_spot(grid):
    grid_size = len(grid)
    i_max = 0
    j_max = 0
    count_max = 0
    for j in range(1, grid_size - 1):
        for i in range(1, grid_size - 1):
            if grid[i][j]:
                continue
            count = 0
            for deltaI in range(-1, 2, 1):
                for deltaJ in range(-1, 2, 1):
                    if grid[i + deltaI][j + deltaJ]:
                        count += 1
            if count > count_max:
                count_max = count
                i_max = i
                j_max = j
    return i_max, j_max


def step_forward(bot):
    for i in range(7):
        bot.move()


# Scan walls in a rectangular area around the bot. 'step' must divide 'dist'.
def scan_rect(bot, dist, step):
    theta = 0
    grid_size = 2 * dist // step + 1
    scanned_rect = [[False for _ in range(grid_size)] for _ in range(grid_size)]
    for j in range(grid_size):
        for i in range(grid_size):
            big_i = (i - grid_size / 2) * step
            big_j = (- j + grid_size / 2) * step
            last_theta = theta
            if big_j != 0:
                theta = math.atan(float(big_i) / float(big_j)) / 2
            elif big_i < 0:
                theta = - math.pi / 4
            elif big_i > 0:
                theta = math.pi / 4
            else:
                continue
            if big_j < 0:
                theta += math.pi / 2
            bot.rotate(rad_to_deg(theta) - rad_to_deg(last_theta))
            h = math.sqrt(big_i * big_i + big_j * big_j)
            if bot.scan(h, 11, True, False, False)[0] > 0:
                scanned_rect[i][j] = True
    bot.rotate(-rad_to_deg(theta))
    return scanned_rect


def rad_to_deg(angle):
    return int(angle * 360 / math.pi)


def path_to_safe_spot(grid, destination):
    a = AStar(len(grid))
    a.init_grid(grid, destination)
    return a.process()


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

    def __lt__(self, other):
        return Cell.compare(self, other)

    @staticmethod
    def compare(cell1, cell2):
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


class AStar(object):

    def __init__(self, grid_size):
        self.opened = []
        heapq.heapify(self.opened)
        self.closed = set()
        self.cells = []
        self.grid_height = grid_size
        self.grid_width = grid_size
        self.start = None
        self.end = None

    def init_grid(self, grid, destination):
        for x in range(self.grid_width):
            for y in range(self.grid_height):
                reachable = not grid[x][y]
                self.cells.append(Cell(x, y, reachable))
        self.start = self.get_cell(self.grid_width // 2, self.grid_height // 2)
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
