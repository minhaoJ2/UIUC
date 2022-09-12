import numpy as np
def cardNameToInt(startingHand):
    x=0
    y=0
    if (startingHand[1] == 'c'):
        y=0
    if (startingHand[1] == 'd'):
        y=1
    if (startingHand[1] == 'h'):
        y=2
    if (startingHand[1] == 's'):
        y=3
    if (startingHand[0] == str(2)):
        x=0
    if (startingHand[0] == str(3)):
        x=1
    if (startingHand[0] == str(4)):
        x=2
    if (startingHand[0] == str(5)):
        x=3
    if (startingHand[0] == str(6)):
        x=4
    if (startingHand[0] == str(7)):
        x=5
    if (startingHand[0] == str(8)):
        x=6
    if (startingHand[0] == str(9)):
        x=7
    if (startingHand[0] == 'T'):
        x=8
    if (startingHand[0] == 'J'):
        x=9
    if (startingHand[0] == 'Q'):
        x = 10
    if (startingHand[0] == 'K'):
        x = 11
    if (startingHand[0] == 'A'):
        x = 12
    return 4 * x + y
start_cards = np.arange(2)
start_cards[0] = cardNameToInt(starting_hand[0])
start_cards[1] = cardNameToInt(starting_hand[1])











import numpy as np
import random as ra

def generatePlayersCards(nPlayers,availableDeck):
    listavailableDeck = list(availableDeck)
    playerscardnumber = list(range(2*nPlayers))
    for i in range(2*(nPlayers)):
        playerscardnumber[i] = ra.choice(listavailableDeck)
        listavailableDeck.remove(playerscardnumber[i])
    updatedCardDeck = np.zeros(len(listavailableDeck), dtype = np.int_)
    for i in range(len(updatedCardDeck)):
        updatedCardDeck[i] = int(listavailableDeck[i])
    playersCards = np.zeros((nPlayers,2), dtype = np.int_)
    for i in range(nPlayers):
        playersCards[i][0] = int(playerscardnumber[2*i])
        playersCards[i][1] = int(playerscardnumber[2*i+1])
    return(playersCards,updatedCardDeck)

def generateDealerCards(availableDeck):
    listavailableDeck = list(availableDeck)
    community = list(range(25))
    for i in range(5):
        community[i] = ra.choice(listavailableDeck)
        listavailableDeck.remove(community[i])
    dealerCards = np.zeros(5, dtype = np.int_)
    for i in range(5):
        dealerCards[i] = community[i]
    updatedCardDeck = np.zeros(len(listavailableDeck), dtype = np.int_)
    for i in range(len(updatedCardDeck)):
        updatedCardDeck[i] = int(listavailableDeck[i])
    return(dealerCards,updatedCardDeck)







import numpy as np

startCards = np.arange(2)
startCards[0] = cardNameToInt(starting_hand[0])
startCards[1] = cardNameToInt(starting_hand[1])
def whoWin(startCards, playersCards, dealerCards):
    result = compareTwoPlayers(startCards, playersCards[0], dealerCards)
    if (result == 0):
        gameResult = np.array([1,0,0])
    if (result == 1):
        gameResult = np.array([0,1,0])
    if (result == 2):
        gameResult = np.array([0,0,1])
    return gameResult

game_result = whoWin(startCards, players_cards, dealer_cards)










import numpy as np
startCards = np.arange(2)
startCards[0] = cardNameToInt(starting_hand[0])
startCards[1] = cardNameToInt(starting_hand[1])
available = list(range(52))
available.remove(startCards[0])
available.remove(startCards[1])
updatedCardDeck = np.zeros(len(available), dtype = np.int_)
for i in range(len(updatedCardDeck)):
    updatedCardDeck[i] = int(available[i])
win = 0.0
lose = 0.0
tie = 0.0
for i in range(N):
    playersCards, updatedCardDeck1 = generatePlayersCards(n_players, updatedCardDeck)
    cards = updatedCardDeck1
    for j in range(n_players):
        community, updateCards = generateDealerCards(cards)
        result = compareTwoPlayers(startCards, playersCards[j],community)
        if (result == 0):
            win += 1
        if (result == 1):
            lose += 1
        if (result == 2):
            tie += 1
win_probability = win/(N*n_players)
lose_probability = lose/(N*n_players)
tie_probability = tie/(N*n_players)











import numpy as np
def monteCarloSimulation(M,N,n_players,starting_hand):
    startCards = np.arange(2)
    startCards[0] = cardNameToInt(starting_hand[0])
    startCards[1] = cardNameToInt(starting_hand[1])
    available = list(range(52))
    available.remove(startCards[0])
    available.remove(startCards[1])
    updatedCardDeck = np.zeros(len(available), dtype = np.int_)
    for i in range(len(updatedCardDeck)):
        updatedCardDeck[i] = int(available[i])
    win_prob = np.zeros(M)
    for k in range(M):
        win = 0.0
        for i in range(N):
            playersCards, updatedCardDeck1 = generatePlayersCards(n_players, updatedCardDeck)
            cards = updatedCardDeck1
            community, cards = generateDealerCards(cards)
            result = whoWin(startCards,playersCards, community)
            win += result[0]
        win_prob[k] = win/(N*n_players)
    return win_prob







import numpy as np
import matplotlib.pyplot as plt
win_prob_20 = monteCarloSimulation(100, 20, 1, starting_hand)
win_prob_50 = monteCarloSimulation(100, 50, 1, starting_hand)
mean_20 = np.mean(win_prob_20)
mean_50 = np.mean(win_prob_50)
std_20 = np.std(win_prob_20)
std_50 = np.std(win_prob_50)
plt.hist(win_prob_20,alpha=0.5, label='20 games')
plt.hist(win_prob_50,alpha=0.5, label='50 games')
plt.title('Starting hand ' + str(starting_hand))
plt.legend(loc='upper left')
plt.xlabel('Probability of winning')
plt.ylabel('# of occurances')
plt.show()
