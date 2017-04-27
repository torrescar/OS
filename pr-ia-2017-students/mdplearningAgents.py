# MdpModelAgent.py
# IA UC3M 2017
# IA UC3M 2016
# -----------------------
##

from game import Directions, Agent, Actions
from game import *
import random,util,time
from pacmanMdp import *

from valueIterationAgents import ValueIterationAgent

import random,util,math


class MdpModelAgent(ValueIterationAgent):
  """
      What you need to know:
		  - The environment will call
		    observeTransition(state,action,nextState,deltaReward),
		    which will call update(state, action, nextState, deltaReward)
		    which you should override.
      - Use self.getLegalActions(state) to know which actions
		    are available in a state
  """
  ####################################
  #    Override These Functions      #
  ####################################

  def update(self, state, action, nextState, reward):
    """
	    This class will call this function after
	    observing a transition
    """
    abstract

  ####################################
  #    Read These Functions          #
  ####################################

  def getLegalActions(self,state):
    """
      Get the actions available for a given
      state. This is what you should use to
      obtain legal actions for a state
    """
    return self.actionFn(state)

  def observeTransition(self, state,action,nextState,deltaReward):
    """
    	Called by environment to inform agent that a transition has
    	been observed. This will result in a call to self.update
    	on the same arguments

    	NOTE: Do *not* override or call this function
    """
    self.episodeRewards += deltaReward
    self.update(state,action,nextState,deltaReward)

  def startEpisode(self):
    """
      Called by environment when new episode is starting
    """
    self.lastState = None
    self.lastAction = None
    self.episodeRewards = 0.0

  def stopEpisode(self):
    """
      Called by environment when episode is done
    """
    if self.episodesSoFar < self.numTraining:
		  self.accumTrainRewards += self.episodeRewards
    else:
		  self.accumTestRewards += self.episodeRewards
    self.episodesSoFar += 1
    if self.episodesSoFar >= self.numTraining:
        print "Training finished"
        self.epsilon = 0.0    # no exploration

  def isInTraining(self):
      return self.episodesSoFar < self.numTraining

  def isInTesting(self):
      return not self.isInTraining()

  def doAction(self,state,action):
    """
        Called by inherited class when
        an action is taken in a state
    """
    self.lastState = state
    self.lastAction = action


  def __init__(self, actionFn = None, numTraining=100, epsilon=0.5, **args):
    """
    actionFn: Function which takes a state and returns the list of legal actions

    epsilon  - exploration rate
    numTraining - number of training episodes, i.e. no learning after these many episodes
    """
    if actionFn == None:
        actionFn = lambda state: state.getLegalActions()
    self.actionFn = actionFn
    self.episodesSoFar = 0
    self.accumTrainRewards = 0.0
    self.accumTestRewards = 0.0
    self.numTraining = int(numTraining)
    self.epsilon = float(epsilon)
    ValueIterationAgent.__init__(self, **args)

  ###################
  # Pacman Specific #
  ###################
  def observationFunction(self, state):
    """
        This is where we ended up after our last action.
        The simulation should somehow ensure this is called
    """
    if not self.lastState is None:
        reward = state.getScore() - self.lastState.getScore()
        self.observeTransition(self.lastState, self.lastAction, state, reward)
    return state

  def registerInitialState(self, state):
    self.startEpisode()
    if self.episodesSoFar == 0:
        print 'Beginning %d episodes of Training' % (self.numTraining)

  def final(self, state):
    """
      Called by Pacman game at the terminal state
    """
    deltaReward = state.getScore() - self.lastState.getScore()
    self.observeTransition(self.lastState, self.lastAction, state, deltaReward)
    self.stopEpisode()

    # Make sure we have this var
    if not 'episodeStartTime' in self.__dict__:
        self.episodeStartTime = time.time()
    if not 'lastWindowAccumRewards' in self.__dict__:
        self.lastWindowAccumRewards = 0.0
    self.lastWindowAccumRewards += state.getScore()

    NUM_EPS_UPDATE = 100
    if self.episodesSoFar % NUM_EPS_UPDATE == 0:
        print 'Learning Status:'
        windowAvg = self.lastWindowAccumRewards / float(NUM_EPS_UPDATE)
        if self.episodesSoFar <= self.numTraining:
            trainAvg = self.accumTrainRewards / float(self.episodesSoFar)
            print '\tCompleted %d out of %d training episodes' % (
                   self.episodesSoFar,self.numTraining)
            print '\tAverage Rewards over all training: %.2f' % (
                    trainAvg)
        else:
            testAvg = float(self.accumTestRewards) / (self.episodesSoFar - self.numTraining)
            print '\tCompleted %d test episodes' % (self.episodesSoFar - self.numTraining)
            print '\tAverage Rewards over testing: %.2f' % testAvg
        print '\tAverage Rewards for last %d episodes: %.2f'  % (
                NUM_EPS_UPDATE,windowAvg)
        print '\tEpisode took %.2f seconds' % (time.time() - self.episodeStartTime)
        self.lastWindowAccumRewards = 0.0
        self.episodeStartTime = time.time()

    if self.episodesSoFar == self.numTraining:
        msg = 'Training Done (turning off epsilon and alpha)'
        print '%s\n%s' % (msg,'-' * len(msg))


class PacmanMdpModelAgent(MdpModelAgent):
  """
    Functions:
      - getAction

  """
  def __init__(self, extractor='FullStateExtractor', **args):
    tableFileName=args.pop('table',None)
    MdpModelAgent.__init__(self, **args)
    self.mdp = PacmanMdp()
    if not tableFileName is None:
        self.mdp.setTransitionTableFile(tableFileName)


  def getAction(self, state):
    """
      Compute the action to take in the current state. 
      when self.epsilon > 0 use random action. Otherwise use
      policy action. Epsilon value is not actually used in this
      implementation

    """

    # Pick Action
    legalActions = self.getLegalActions(state)
    #isHeads = util.flipCoin(self.epsilon)
    
    if len(legalActions) is 0:
        return None

    #if isHeads:
    if self.epsilon > 0.0:
        #print "Taking the random choice"
        return random.choice(legalActions)
    else:
        #print "Taking the known policy"
        return self.getPartialPolicy(state)


class EstimatePacmanMdpAgent(PacmanMdpModelAgent):
  """
     Agent that estimates the transition function of the MDP for Pacman
  """
    
  def __init__(self, extractor='FullStateExtractor', **args):
    self.index = 0  # This is always Pacman
    PacmanMdpModelAgent.__init__(self, **args) 
    if 'discount' in args.keys(): self.setDiscount(float(args['discount']))
    if 'iterations' in args.keys(): self.setIterations(int(args['iterations']))
    self.mdp.loadTransitionTable()



  def getAction(self, state):
    """
    Simply calls the getAction method of PacmanMdpModelAgent and then
    informs parent of action for Pacman.  Do not change or remove this
    method.
    """
    action = PacmanMdpModelAgent.getAction(self, state)
    self.doAction(state,action)
    return action

  def update(self, state, action, nextState, reward):
    """
       Should update your weights based on transition
    """
    if self.epsilon > 0.0:
        self.mdp.updateTransitionFunction(state, action, nextState)

  def final(self, state):
    "Called at the end of each game."
    # call the super-class final method
    PacmanMdpModelAgent.final(self, state)

    # did we finish training?
    if self.episodesSoFar == self.numTraining:
      # execute value iteration
      print "Executing Value Iteration" 
      self.doValueIteration()
      print "---------------------------------------------"
      # self.mdp.printMdp()
      # This stores the transition table to file
      self.mdp.saveTransitionTable()
      # self.showPolicy()
      pass


