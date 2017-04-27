# featureExtractors.py
# --------------------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html

"Feature extractors for Pacman game states"

from game import Directions, Actions
import util


class FeatureExtractor:  
  def getFeatures(self, state, action):    
    """
      Returns a dict from features to counts
      Usually, the count will just be 1.0 fors
      indicator functions.  
    """
    util.raiseNotDefined()

  def getFeatures(self, state):    
    """
      Returns a dict from features to counts
      Usually, the count will just be 1.0 for
      indicator functions.  
    """
    util.raiseNotDefined()


class IdentityExtractor(FeatureExtractor):
  def getFeatures(self, state, action):
    feats = util.Counter()
    feats[(state,action)] = 1.0
    return feats

def closestFood(pos, food, walls):
  """
  closestFood -- this is similar to the function that we have
  worked on in the search project; here its all in one place
  """
  fringe = [(pos[0], pos[1], 0)]
  expanded = set()
  while fringe:
    pos_x, pos_y, dist = fringe.pop(0)
    if (pos_x, pos_y) in expanded:
      continue
    expanded.add((pos_x, pos_y))
    # if we find a food at this location then exit
    if food[pos_x][pos_y]:
      return dist
    # otherwise spread out from the location to its neighbours
    nbrs = Actions.getLegalNeighbors((pos_x, pos_y), walls)
    for nbr_x, nbr_y in nbrs:
      fringe.append((nbr_x, nbr_y, dist+1))
  # no food found
  return None

def closestFoodPos(pos, food, walls):
  """
  closestFood -- this is similar to the function that we have
  worked on in the search project; here its all in one place
  """
  fringe = [(pos[0], pos[1], 0)]
  expanded = set()
  while fringe:
    pos_x, pos_y, dist = fringe.pop(0)
    if (pos_x, pos_y) in expanded:
      continue
    expanded.add((pos_x, pos_y))
    # if we find a food at this location then exit
    if food[pos_x][pos_y]:
      return (pos_x, pos_y)
    # otherwise spread out from the location to its neighbours
    nbrs = Actions.getLegalNeighbors((pos_x, pos_y), walls)
    for nbr_x, nbr_y in nbrs:
      fringe.append((nbr_x, nbr_y, dist+1))
  # no food found
  return (0,0)

def closestElement(pos, element, walls):
  """
  closestItem -- this is similar to the function that we have
  worked on in the search project; here its all in one place
  """
  fringe = [(pos[0], pos[1], 0)]
  expanded = set()
  while fringe:
    pos_x, pos_y, dist = fringe.pop(0)
    if (pos_x, pos_y) in expanded:
      continue
    expanded.add((pos_x, pos_y))
    # if we find a food at this location then exit
    if element[pos_x][pos_y]:
      return ((pos_x, pos_y),dist)
    # otherwise spread out from the location to its neighbours
    nbrs = Actions.getLegalNeighbors((pos_x, pos_y), walls)
    for nbr_x, nbr_y in nbrs:
      fringe.append((nbr_x, nbr_y, dist+1))
  # no food found
  return ((0,0),None)


class SimpleExtractor(FeatureExtractor):
  """
  Returns simple features for a basic reflex Pacman:
  - whether food will be eaten
  - how far away the next food is
  - whether a ghost collision is imminent
  - whether a ghost is one step away
  """
  
  def getFeatures(self, state, action):
    # extract the grid of food and wall locations and get the ghost locations
    food = state.getFood()
    walls = state.getWalls()
    ghosts = state.getGhostPositions()

    features = util.Counter()
    
    features["bias"] = 1.0
    
    # compute the location of pacman after he takes the action
    x, y = state.getPacmanPosition()
    dx, dy = Actions.directionToVector(action)
    next_x, next_y = int(x + dx), int(y + dy)
    
    # count the number of ghosts 1-step away
    features["#-of-ghosts-1-step-away"] = sum((next_x, next_y) in Actions.getLegalNeighbors(g, walls) for g in ghosts)

    # if there is no danger of ghosts then add the food feature
    if not features["#-of-ghosts-1-step-away"] and food[next_x][next_y]:
      features["eats-food"] = 1.0
    
    dist = closestFood((next_x, next_y), food, walls)
    if dist is not None:
      # make the distance a number less than one otherwise the update
      # will diverge wildly
      features["closest-food"] = float(dist) / (walls.width * walls.height) 
    dist = closestFood((x, y), food, walls)
    if dist is not None:
      # make the distance a number less than one otherwise the update
      # will diverge wildly
      features["closest-food"] = float(dist) / (walls.width * walls.height) 
    features.divideAll(10.0)
    return features




## IA UC3M 2017
## You should understand the implementation of this class

import copy
from game import Grid

class FullStateExtractor(FeatureExtractor):
  """
  Returns features for a basic reflex Pacman
  """
  def isTerminalFeatures (self, featuresTuple):
    # No pacman (lose) or no food (win)
 
    # tuple example: ('IncGhostX': 4, 'IncGhostY': 0, 'posX': 5, 'posY': 1, 'FoodY': 4, 'FoodX': -3}
    # Condition: features["posX"]== None or features["#Food"] == 0
    return featuresTuple[0]  == None or featuresTuple[2] == None
    
  def getFeatures(self, state):
    """
        Returns the following features: 
            wall size (width and height) ("wallW", "wallH"), 
            pac-man position ("posX", "posY"), 
            relative position of to the closest food ("FoodX", "FoodY")
            relative position of the closest ghost not including walls ("IncGhostX", "IncGhostY")
            relative position of the closest ghost ("ClosestGhostX", "ClosestGhostY")
            ... see below for more ...
    """

    # extract the grid of food and wall locations and get the ghost locations
    food = state.getFood()
    walls = state.getWalls()
    ghosts = state.getGhostPositions()
    
    ghostMap=Grid(food.width, food.height, initialValue=False)
    for g in ghosts:
        ghostMap[int(g[0])][int(g[1])]=True

#    print "Ghost data:" , ghosts
#    print "Food data:" , food
#    print "Walls data:" , walls
#    print "Ghost data map:" , ghostMap
    
    features = util.Counter()
   
    # compute the location of pacman
    pacmanPosition = state.getPacmanPosition()
    
    features["wallsW"]=walls.width
    features["wallsH"]=walls.height
    
    if pacmanPosition != None:
        x = pacmanPosition[0]
        y = pacmanPosition[1]
        features["posX"] = x
        features["posY"] = y


        # Position and distance to all ghosts
        # Distance calculated using search and Manhattan distance
        if (state.getNumAgents() > 1):
            distancesToPacman = [(pos, util.manhattanDistance( pos, pacmanPosition )) for pos in ghosts]
            closestGhost_x, closestGhost_y  = min(distancesToPacman, key=lambda x: x[1])[0]
            features["IncGhostX"] = int (closestGhost_x - x)
            features["IncGhostY"] = int (closestGhost_y - y)
            features["GhostDist"] = int (abs(x - closestGhost_x) + abs(y - closestGhost_y))
    
            # Now we calculate features based on actual distance
#            (closestGhost2_x, closestGhost2_y) = closestFoodPos((x, y), ghostMap, walls)
            closestGhost2_pos,closestGhost2_dist = closestElement((x, y), ghostMap, walls)
            features["ClosestGhostX"] = int (closestGhost2_pos[0])
            features["ClosestGhostY"] = int (closestGhost2_pos[1])
#            closestGhost2_dist = closestFood((x, y), ghostMap, walls)
            features["ClosestGhostDist"] = int (closestGhost2_dist)
    
            # We can also use the positions of all ghosts
            features["GhostPos"] = tuple(ghosts)
          
        # closest food
        if  state.getNumFood() != 0:
            (food_x, food_y) = closestFoodPos((x, y), food, walls)
            features["IncFoodX"] =  int (food_x - x)    
            features["IncFoodY"] =  int (food_y - y)
            features["foodX"] =  int (food_x)    
            features["foodY"] =  int (food_y)
    #        features["#Food"] =  True
    #        features["#Food"] =  state.getNumFood()
        else:  
            features["IncFoodX"] =  None    
            features["IncFoodY"] =  None
            features["foodX"] =  None
            features["foodY"] =  None
    #        features["#Food"] =  False
    #        features["#Food"] =  0

    else:
        features["posX"] = None
        features["posY"] = None
        features["IncFoodX"] =  None    
        features["IncFoodY"] =  None
        features["foodX"] =  None
        features["foodY"] =  None

    return tuple(features.values()),tuple(features.keys())