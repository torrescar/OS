# pacmanMdp.py
# IA UC3M 2017
# IA UC3M 2016
# -----------------------
##
from game import GameStateData
from game import Game
from game import Actions
from util import nearestPoint
import util, layout
import sys, types, time, random, os

import mdp
import pickle

from featureExtractors import *

class PacmanMdp(mdp.MarkovDecisionProcess):
    """
      pacman MDP
    """
    def __init__(self, extractor='FullStateExtractor',transitionTableFile=None):
        # We set this to true to print the MDP in the first call of the ValueIteration
        self.printMdpTable=True
        self.transitionTableFile=transitionTableFile
        
        # Feature extractor
        self.featExtractor = util.lookup(extractor, globals())()


        # Uncomment to get the full vector of state features
        #    self.stateFeatures=['posX','posY','IncFoodX','IncFoodY','IncGhostX','IncGhostY','foodX','foodY','wallsW','wallsH','GhostDist','ClosestGhostX','ClosestGhostY','ClosestGhostDist','GhostPos']

        # Default State features
        self.stateFeatures=['posX','posY','IncFoodX','IncFoodY','IncGhostX','IncGhostY']

        # Transition function (data structure required for the transition function)
        #*** YOUR CODE STARTS HERE ***"
        
        # This variable MUST be used to reference your transition table so
        # it can be saved and loaded from file

        self.transitionTable=None
        
        #"*** YOUR CODE FINISHES HERE ***"


        # Dictionary with examples of a Low state for each High state: it serves to get possible actions
        # and to check terminal states (though it is not required if the high level representation
        # capture them)
        self.states = util.Counter()

        # Reward for each state at the high level representation
        self.reward = util.Counter()

    def isTerminalFeatures (self, featuresTuple, checkIndices=(0,2)):
        """
          This function has to be called with the proper indices to check if a state
          is terminal, based on the situation. Current rule is that: if either X position for the
          pacman agent is "None", or X position for the closest food is "None", we are in a terminal
          state. This function is used in getTransitionStatesAndProbabilities.
          
          We assume that the featuresTuple contains that information in positions 0 and 2
        """
        countNone = [i for i,x in enumerate(featuresTuple) if i in checkIndices and x==None]
        return (len(countNone)>0)
    
    def stateToHigh(self, stateMap):
        """
          Returns the high level representation of an state
          This function gets the full state given as a map as a parameter.
          First extracts all the calculated features and then filters them
          keeping only the ones provided as parameter. Also sorts features
          so they are retrieved in the provided order.
          
          Features non existent (for instance if ghosts are not present) are skipped
          Note that isTerminalFeatures expects to have "posX" and "FoodX" in particular positions
          
          Returns two tuples: tuple of features and tuple of the features names.

          Features must be loaded in the self.stateFeatures variable

          List of Features:
              'posX','posY': Position of Pac-man
              'foodX','foodY': Relative position of closest food
              'wallsW','wallsH': Width and Height of the map (including walls)
              'IncGhostX','IncGhostY': Relative position of closest ghost (Not counting walls)
              'GhostDist': Distance of closest ghost (calculated not counting walls)
              'ClosestGhostX','ClosestGhostY': Absolute position of closest ghost (Counting walls)
              'ClosestGhostDist': Distance of closest ghost (True distance, counting walls)
              'GhostPos': Positions of all ghosts (tuple of tuples)
        """

        # Gets all features
        fullState_values,fullState_names = self.featExtractor.getFeatures(stateMap)
        
        # Skip features not present in the complete list even if selected
        state_names=tuple (n for n in self.stateFeatures if n in fullState_names)
        state=tuple (fullState_values[fullState_names.index(n)] for n in state_names)

        
        return state,state_names
    
    def addStateLow(self, stateH, stateMap):
        """
          Adds a new pair stateH stateL to the dictionary of states

        """
        # print "Added", stateH
        if not stateH in self.states.keys():
            self.states[stateH] = stateMap
            self.reward[stateH] = [1, [stateMap.getScore()]]
        else:
            self.reward[stateH][0] += 1
            self.reward[stateH][1].append(stateMap.getScore())
            

    def updateTransitionFunction(self, initialMap, action, nextMap):    
        """
          Updates the transition function with a new case stateL, action, nextStateL
          
          The states received as parameters have a low level representation. The transition function
          should be stored over the high level (simplified) representation

        """
        # Change the representation to the simplified one
        
        # Some of these features will not be present in the state if there are no ghosts
        state,state_names=self.stateToHigh(initialMap)
        nextstate,nextstate_names=self.stateToHigh(nextMap)
        
        # Set the start state in the first call
        if len(self.states.keys())== 0:
            self.setStartState(state)
        # Add the received states to self.states
        self.addStateLow(state, initialMap)
        self.addStateLow(nextstate, nextMap)


        #"*** YOUR CODE STARTS HERE ***"

        util.raiseNotDefined()
        
        #"*** YOUR CODE FINISHES HERE ***"

    def getPossibleActions(self, state):
        """
        Returns list of valid actions for 'state'.

        Note that you can request moves into walls and
        that "exit" states transition to the terminal
        state under the special action "done".
        """
        if not state in self.states.keys():
            return []
        return (self.states[state]).getLegalActions(0)

    def getStates(self):
        """
        Return list of all states.
        """
        return self.states.keys()

    def isKnownState(self, state):
        """
        True if the state is in the dict of states.
        """
        return state in self.states.keys()

    def getAverageReward(self, state):
        """
        Return average rewards of the known low level states represented by a high level state 
        """
        return sum(i for i in  self.reward[state][1])/self.reward[state][0]

    def getReward(self, state, action, nextState):
        """
        Get reward for state, action, nextState transition.

        """
        return self.getAverageReward(nextState) - self.getAverageReward(state)

    def setStartState(self, state):
        """
        set for start state
        """
        self.startState = state

    def getStartState(self):
        """
        get for start state
        """
        return self.startState

    def isTerminal(self, state):
        """
        Pacman terminal states
        """
        if not state in self.states.keys():
            return isTerminalFeatures(state)
        else:
            return self.states[state].isLose() or self.states[state].isWin()

    def printMdp( self ):
        """
        Shows the transition function of the MDP
        """
        for state in sorted(self.states.keys()):
            for action in self.getPossibleActions(state):
                print state, action, self.getTransitionStatesAndProbabilities(state, action)
        
    
    def setTransitionTableFile ( self , filename):
        self.transitionTableFile = filename
    
    def getTransitionTable (self ):
        return self.transitionTable

    def saveTransitionTable(self):
        """
            Saves all the objects constructed during training
        """
        if self.transitionTableFile is not None:
            trainInfo = {'states' : self.states,
                         'reward' : self.reward,
                         'transitionTable' : self.transitionTable}
    
    
            pickle.dump(trainInfo,open(self.transitionTableFile,'wb'))
            print " MDP transition table saved to file ", self.transitionTableFile

    def loadTransitionTable(self):
        if self.transitionTableFile is not None:
            if (os.path.isfile(self.transitionTableFile)):
                trainInfo= pickle.load(open(self.transitionTableFile,'rb'))
                self.states= trainInfo['states']
                self.reward= trainInfo['reward']
                self.transitionTable= trainInfo['transitionTable']
                print " MDP transition table loaded from file ", self.transitionTableFile
#        else:
#            print " WARNING: MDP transition table file not found. Creating a new table in " , self.transitionTableFile
    
    
    def getTransitionStatesAndProbabilities(self, state, action):
        """
        Returns list of (nextState, prob) pairs
        representing the states reachable
        from 'state' by taking 'action' along
        with their transition probabilities.
        """
            
        
        if self.printMdpTable:
            self.printMdpTable=False
            print
            print " Executing MDP with transition table:"
            self.printMdp()
            print " End of MDP transition table"
            print
            
        if action not in self.getPossibleActions(state):
            raise "Illegal action!"

        if self.isTerminal(state):
            return []

        successors = []

        #"*** YOUR CODE STARTS HERE ***"

        util.raiseNotDefined()

        #"*** YOUR CODE FINISHES HERE ***"

        return successors



